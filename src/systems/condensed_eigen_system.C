// The libMesh Finite Element Library.
// Copyright (C) 2002-2022 Benjamin S. Kirk, John W. Peterson, Roy H. Stogner

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "libmesh/libmesh_config.h"

// Currently, the EigenSystem should only be available
// if SLEPc support is enabled.
#if defined(LIBMESH_HAVE_SLEPC)

#include "libmesh/condensed_eigen_system.h"

#include "libmesh/dof_map.h"
#include "libmesh/equation_systems.h"
#include "libmesh/int_range.h"
#include "libmesh/libmesh_logging.h"
#include "libmesh/numeric_vector.h"
#include "libmesh/parallel.h"

namespace libMesh
{

CondensedEigenSystem::CondensedEigenSystem (EquationSystems & es,
                                            const std::string & name_in,
                                            const unsigned int number_in)
  : Parent(es, name_in, number_in),
    condensed_matrix_A(&this->add_matrix("Condensed Eigen Matrix A")),
    condensed_matrix_B(&this->add_matrix("Condensed Eigen Matrix B")),
    condensed_dofs_initialized(false)
{
}

CondensedEigenSystem::~CondensedEigenSystem() = default;

void
CondensedEigenSystem::initialize_condensed_dofs(const std::set<dof_id_type> & global_dirichlet_dofs_set)
{
  const DofMap & dof_map = this->get_dof_map();

  // First, put all unconstrained local dofs into non_dirichlet_dofs_set
  std::set<dof_id_type> local_non_condensed_dofs_set;
  for (auto i : make_range(dof_map.first_dof(), dof_map.end_dof()))
#if LIBMESH_ENABLE_CONSTRAINTS
    if (!dof_map.is_constrained_dof(i))
#endif
      local_non_condensed_dofs_set.insert(i);

  // Now erase the condensed dofs
  for (const auto & dof : global_dirichlet_dofs_set)
    if ((dof_map.first_dof() <= dof) && (dof < dof_map.end_dof()))
      local_non_condensed_dofs_set.erase(dof);

  // Finally, move local_non_condensed_dofs_set over to a vector for convenience in solve()
  this->local_non_condensed_dofs_vector.clear();

  for (const auto & dof : local_non_condensed_dofs_set)
    this->local_non_condensed_dofs_vector.push_back(dof);

  condensed_dofs_initialized = true;
}

dof_id_type CondensedEigenSystem::n_global_non_condensed_dofs() const
{
  if (!condensed_dofs_initialized)
    {
      return this->n_dofs();
    }
  else
    {
      dof_id_type n_global_non_condensed_dofs =
        cast_int<dof_id_type>(local_non_condensed_dofs_vector.size());
      this->comm().sum(n_global_non_condensed_dofs);

      return n_global_non_condensed_dofs;
    }
}


void CondensedEigenSystem::solve()
{
  LOG_SCOPE("solve()", "CondensedEigenSystem");

  // If we haven't initialized any condensed dofs,
  // just use the default eigen_system
  if (!condensed_dofs_initialized)
    {
      Parent::solve();
      return;
    }

  // A reference to the EquationSystems
  EquationSystems & es = this->get_equation_systems();

  // check that necessary parameters have been set
  libmesh_assert (es.parameters.have_parameter<unsigned int>("eigenpairs"));
  libmesh_assert (es.parameters.have_parameter<unsigned int>("basis vectors"));

  if (this->assemble_before_solve)
    {
      // Assemble the linear system
      this->assemble ();

      // And close the assembled matrices; using a non-closed matrix
      // with create_submatrix() is deprecated.
      matrix_A->close();
      if (generalized())
        matrix_B->close();
    }

  // If we reach here, then there should be some non-condensed dofs
  libmesh_assert(!local_non_condensed_dofs_vector.empty());

  // Now condense the matrices
  matrix_A->create_submatrix(*condensed_matrix_A,
                             local_non_condensed_dofs_vector,
                             local_non_condensed_dofs_vector);

  if (generalized())
    {
      matrix_B->create_submatrix(*condensed_matrix_B,
                                 local_non_condensed_dofs_vector,
                                 local_non_condensed_dofs_vector);
    }


  // Get the tolerance for the solver and the maximum
  // number of iterations. Here, we simply adopt the linear solver
  // specific parameters.
  const double tol          =
    double(es.parameters.get<Real>("linear solver tolerance"));

  const unsigned int maxits =
    es.parameters.get<unsigned int>("linear solver maximum iterations");

  const unsigned int nev    =
    es.parameters.get<unsigned int>("eigenpairs");

  const unsigned int ncv    =
    es.parameters.get<unsigned int>("basis vectors");

  std::pair<unsigned int, unsigned int> solve_data;

  // call the solver depending on the type of eigenproblem
  if (generalized())
    {
      //in case of a generalized eigenproblem
      solve_data = eigen_solver->solve_generalized
        (*condensed_matrix_A,*condensed_matrix_B, nev, ncv, tol, maxits);
    }

  else
    {
      libmesh_assert (!matrix_B);

      //in case of a standard eigenproblem
      solve_data = eigen_solver->solve_standard (*condensed_matrix_A, nev, ncv, tol, maxits);
    }

  set_n_converged(solve_data.first);
  set_n_iterations(solve_data.second);
}



std::pair<Real, Real> CondensedEigenSystem::get_eigenpair(dof_id_type i)
{
  LOG_SCOPE("get_eigenpair()", "CondensedEigenSystem");

  // If we haven't initialized any condensed dofs,
  // just use the default eigen_system
  if (!condensed_dofs_initialized)
    return Parent::get_eigenpair(i);

  // If we reach here, then there should be some non-condensed dofs
  libmesh_assert(!local_non_condensed_dofs_vector.empty());

  // This function assumes that condensed_solve has just been called.
  // If this is not the case, then we will trip an asset in get_eigenpair
  std::unique_ptr<NumericVector<Number>> temp = NumericVector<Number>::build(this->comm());
  const dof_id_type n_local =
    cast_int<dof_id_type>(local_non_condensed_dofs_vector.size());
  dof_id_type n       = n_local;
  this->comm().sum(n);

  temp->init (n, n_local, false, PARALLEL);

  std::pair<Real, Real> eval = eigen_solver->get_eigenpair (i, *temp);

  // Now map temp to solution. Loop over local entries of local_non_condensed_dofs_vector
  this->solution->zero();
  for (auto j : make_range(n_local))
    {
      const dof_id_type index = local_non_condensed_dofs_vector[j];
      solution->set(index,(*temp)(temp->first_local_index()+j));
    }

  solution->close();
  this->update();

  return eval;
}



SparseMatrix<Number> & CondensedEigenSystem::get_condensed_matrix_A() const
{
  libmesh_assert(condensed_matrix_A);
  libmesh_assert_equal_to(&get_matrix("Condensed Eigen Matrix A"), condensed_matrix_A);
  return *condensed_matrix_A;
}



SparseMatrix<Number> & CondensedEigenSystem::get_condensed_matrix_B() const
{
  libmesh_assert(condensed_matrix_B);
  libmesh_assert_equal_to(&get_matrix("Condensed Eigen Matrix B"), condensed_matrix_B);
  return *condensed_matrix_B;
}



} // namespace libMesh


#endif // LIBMESH_HAVE_SLEPC
