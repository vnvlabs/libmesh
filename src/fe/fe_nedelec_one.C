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



// Local includes
#include "libmesh/dof_map.h"
#include "libmesh/fe.h"
#include "libmesh/fe_interface.h"
#include "libmesh/elem.h"
#include "libmesh/tensor_value.h"
#include "libmesh/enum_to_string.h"


namespace libMesh
{


LIBMESH_DEFAULT_VECTORIZED_FE(0,NEDELEC_ONE)
LIBMESH_DEFAULT_VECTORIZED_FE(1,NEDELEC_ONE)
LIBMESH_DEFAULT_VECTORIZED_FE(2,NEDELEC_ONE)
LIBMESH_DEFAULT_VECTORIZED_FE(3,NEDELEC_ONE)


// Anonymous namespace for local helper functions
namespace {
void nedelec_one_nodal_soln(const Elem * elem,
                            const Order order,
                            const std::vector<Number> & elem_soln,
                            const int dim,
                            std::vector<Number> & nodal_soln)
{
  const unsigned int n_nodes = elem->n_nodes();
  const ElemType elem_type   = elem->type();

  const Order totalorder = static_cast<Order>(order + elem->p_level());

  nodal_soln.resize(n_nodes*dim);

  FEType fe_type(order, NEDELEC_ONE);
  FEType p_refined_fe_type(totalorder, NEDELEC_ONE);

  switch (totalorder)
    {
    case FIRST:
      {
        switch (elem_type)
          {
          case TRI6:
          case TRI7:
            {
              libmesh_assert_equal_to (elem_soln.size(), 3);
              libmesh_assert_equal_to (nodal_soln.size(), 6*2);
              break;
            }
          case QUAD8:
          case QUAD9:
            {
              libmesh_assert_equal_to (elem_soln.size(), 4);

              if (elem_type == QUAD8)
                libmesh_assert_equal_to (nodal_soln.size(), 8*2);
              else
                libmesh_assert_equal_to (nodal_soln.size(), 9*2);
              break;
            }
          case TET10:
          case TET14:
            {
              libmesh_assert_equal_to (elem_soln.size(), 6);
              if (elem_type == TET10)
                libmesh_assert_equal_to (nodal_soln.size(), 10*3);
              else
                libmesh_assert_equal_to (nodal_soln.size(), 14*3);

              break;
            }


          case HEX20:
          case HEX27:
            {
              libmesh_assert_equal_to (elem_soln.size(), 12);

              if (elem_type == HEX20)
                libmesh_assert_equal_to (nodal_soln.size(), 20*3);
              else
                libmesh_assert_equal_to (nodal_soln.size(), 27*3);

              break;
            }

          default:
            libmesh_error_msg("ERROR: Invalid ElemType " << Utility::enum_to_string(elem_type) << " selected for NEDELEC_ONE FE family!");

          } // switch(elem_type)

        const unsigned int n_sf =
          FEInterface::n_shape_functions(fe_type, elem);

        std::vector<Point> refspace_nodes;
        FEVectorBase::get_refspace_nodes(elem_type,refspace_nodes);
        libmesh_assert_equal_to (refspace_nodes.size(), n_nodes);


        // Need to create new fe object so the shape function as the FETransformation
        // applied to it.
        std::unique_ptr<FEVectorBase> vis_fe = FEVectorBase::build(dim, p_refined_fe_type);

        const std::vector<std::vector<RealGradient>> & vis_phi = vis_fe->get_phi();

        vis_fe->reinit(elem,&refspace_nodes);

        for (unsigned int n = 0; n < n_nodes; n++)
          {
            libmesh_assert_equal_to (elem_soln.size(), n_sf);

            // Zero before summation
            for (int d = 0; d < dim; d++)
              {
                nodal_soln[dim*n+d] = 0;
              }

            // u = Sum (u_i phi_i)
            for (unsigned int i=0; i<n_sf; i++)
              {
                for (int d = 0; d < dim; d++)
                  {
                    nodal_soln[dim*n+d]   += elem_soln[i]*(vis_phi[i][n](d));
                  }
              }
          }

        return;
      } // case FIRST

    default:
      libmesh_error_msg("ERROR: Invalid total order " << Utility::enum_to_string(totalorder) << " selected for NEDELEC_ONE FE family!");

    }//switch (totalorder)

  return;
} // nedelec_one_nodal_soln


unsigned int nedelec_one_n_dofs(const ElemType t, const Order o)
{
  switch (o)
    {
    case FIRST:
      {
        switch (t)
          {
          case TRI6:
          case TRI7:
            return 3;

          case QUAD8:
          case QUAD9:
            return 4;

          case TET10:
          case TET14:
            return 6;

          case HEX20:
          case HEX27:
            return 12;

          case INVALID_ELEM:
            return 0;

          default:
            libmesh_error_msg("ERROR: Bad ElemType = " << Utility::enum_to_string(t) << " for " << Utility::enum_to_string(o) << " order approximation!");
          }
      }

    default:
      libmesh_error_msg("ERROR: Invalid Order " << Utility::enum_to_string(o) << " selected for NEDELEC_ONE FE family!");
    }
}




unsigned int nedelec_one_n_dofs_at_node(const ElemType t,
                                        const Order o,
                                        const unsigned int n)
{
  switch (o)
    {
    case FIRST:
      {
        switch (t)
          {
          case TRI6:
          case TRI7:
            {
              switch (n)
                {
                case 0:
                case 1:
                case 2:
                  return 0;
                case 3:
                case 4:
                case 5:
                  return 1;

                default:
                  libmesh_error_msg("ERROR: Invalid node ID " << n);
                }
            }
          case QUAD8:
            {
              switch (n)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                  return 0;
                case 4:
                case 5:
                case 6:
                case 7:
                  return 1;

                default:
                  libmesh_error_msg("ERROR: Invalid node ID " << n);
                }
            }
          case QUAD9:
            {
              switch (n)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                case 8:
                  return 0;
                case 4:
                case 5:
                case 6:
                case 7:
                  return 1;

                default:
                  libmesh_error_msg("ERROR: Invalid node ID " << n);
                }
            }
          case TET10:
          case TET14:
            {
              switch (n)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                  return 0;
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                  return 1;

                case 10:
                case 11:
                case 12:
                case 13:
                  return 0;

                default:
                  libmesh_error_msg("ERROR: Invalid node ID " << n);
                }
            }

          case HEX20:
            {
              switch (n)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                  return 0;
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                  return 1;

                default:
                  libmesh_error_msg("ERROR: Invalid node ID " << n);
                }
            }
          case HEX27:
            {
              switch (n)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                  return 0;
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16:
                case 17:
                case 18:
                case 19:
                  return 1;

                default:
                  libmesh_error_msg("ERROR: Invalid node ID " << n);
                }
            }

          case INVALID_ELEM:
            return 0;

          default:
            libmesh_error_msg("ERROR: Bad ElemType = " << Utility::enum_to_string(t) << " for " << Utility::enum_to_string(o) << " order approximation!");
          }
      }

    default:
      libmesh_error_msg("ERROR: Invalid Order " << Utility::enum_to_string(o) << " selected for NEDELEC_ONE FE family!");
    }
}



#ifdef LIBMESH_ENABLE_AMR
void nedelec_one_compute_constraints (DofConstraints & /*constraints*/,
                                      DofMap & /*dof_map*/,
                                      const unsigned int /*variable_number*/,
                                      const Elem * libmesh_dbg_var(elem),
                                      const unsigned Dim)
{
  // Only constrain elements in 2,3D.
  if (Dim == 1)
    return;

  libmesh_assert(elem);

  libmesh_not_implemented();
} // nedelec_one_compute_constraints()
#endif // #ifdef LIBMESH_ENABLE_AMR

} // anonymous namespace

#define NEDELEC_LOW_D_ERROR_MESSAGE                                     \
  libmesh_error_msg("ERROR: This method makes no sense for low-D elements!");


// Do full-specialization for every dimension, instead
// of explicit instantiation at the end of this file.
template <>
void FE<0,NEDELEC_ONE>::nodal_soln(const Elem *,
                                   const Order,
                                   const std::vector<Number> &,
                                   std::vector<Number> &)
{ NEDELEC_LOW_D_ERROR_MESSAGE }

template <>
void FE<1,NEDELEC_ONE>::nodal_soln(const Elem *,
                                   const Order,
                                   const std::vector<Number> &,
                                   std::vector<Number> &)
{ NEDELEC_LOW_D_ERROR_MESSAGE }

template <>
void FE<2,NEDELEC_ONE>::nodal_soln(const Elem * elem,
                                   const Order order,
                                   const std::vector<Number> & elem_soln,
                                   std::vector<Number> & nodal_soln)
{ nedelec_one_nodal_soln(elem, order, elem_soln, 2 /*dim*/, nodal_soln); }

template <>
void FE<3,NEDELEC_ONE>::nodal_soln(const Elem * elem,
                                   const Order order,
                                   const std::vector<Number> & elem_soln,
                                   std::vector<Number> & nodal_soln)
{ nedelec_one_nodal_soln(elem, order, elem_soln, 3 /*dim*/, nodal_soln); }


// Do full-specialization for every dimension, instead
// of explicit instantiation at the end of this function.
// This could be macro-ified.
template <> unsigned int FE<0,NEDELEC_ONE>::n_dofs(const ElemType, const Order) { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> unsigned int FE<1,NEDELEC_ONE>::n_dofs(const ElemType, const Order) { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> unsigned int FE<2,NEDELEC_ONE>::n_dofs(const ElemType t, const Order o) { return nedelec_one_n_dofs(t, o); }
template <> unsigned int FE<3,NEDELEC_ONE>::n_dofs(const ElemType t, const Order o) { return nedelec_one_n_dofs(t, o); }


// Do full-specialization for every dimension, instead
// of explicit instantiation at the end of this function.
template <> unsigned int FE<0,NEDELEC_ONE>::n_dofs_at_node(const ElemType, const Order, const unsigned int) { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> unsigned int FE<1,NEDELEC_ONE>::n_dofs_at_node(const ElemType, const Order, const unsigned int) { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> unsigned int FE<2,NEDELEC_ONE>::n_dofs_at_node(const ElemType t, const Order o, const unsigned int n) { return nedelec_one_n_dofs_at_node(t, o, n); }
template <> unsigned int FE<3,NEDELEC_ONE>::n_dofs_at_node(const ElemType t, const Order o, const unsigned int n) { return nedelec_one_n_dofs_at_node(t, o, n); }


// Nedelec first type elements have no dofs per element
// FIXME: Only for first order!
template <> unsigned int FE<0,NEDELEC_ONE>::n_dofs_per_elem(const ElemType, const Order) { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> unsigned int FE<1,NEDELEC_ONE>::n_dofs_per_elem(const ElemType, const Order) { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> unsigned int FE<2,NEDELEC_ONE>::n_dofs_per_elem(const ElemType, const Order) { return 0; }
template <> unsigned int FE<3,NEDELEC_ONE>::n_dofs_per_elem(const ElemType, const Order) { return 0; }

// Nedelec first type FEMs are always tangentially continuous
template <> FEContinuity FE<0,NEDELEC_ONE>::get_continuity() const { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> FEContinuity FE<1,NEDELEC_ONE>::get_continuity() const { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> FEContinuity FE<2,NEDELEC_ONE>::get_continuity() const { return H_CURL; }
template <> FEContinuity FE<3,NEDELEC_ONE>::get_continuity() const { return H_CURL; }

// Nedelec first type FEMs are not hierarchic
template <> bool FE<0,NEDELEC_ONE>::is_hierarchic() const { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> bool FE<1,NEDELEC_ONE>::is_hierarchic() const { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> bool FE<2,NEDELEC_ONE>::is_hierarchic() const { return false; }
template <> bool FE<3,NEDELEC_ONE>::is_hierarchic() const { return false; }

// Nedelec first type FEM shapes always need to be reinit'ed (because of orientation dependence)
template <> bool FE<0,NEDELEC_ONE>::shapes_need_reinit() const { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> bool FE<1,NEDELEC_ONE>::shapes_need_reinit() const { NEDELEC_LOW_D_ERROR_MESSAGE }
template <> bool FE<2,NEDELEC_ONE>::shapes_need_reinit() const { return true; }
template <> bool FE<3,NEDELEC_ONE>::shapes_need_reinit() const { return true; }

#ifdef LIBMESH_ENABLE_AMR
template <>
void FE<0,NEDELEC_ONE>::compute_constraints (DofConstraints &,
                                             DofMap &,
                                             const unsigned int,
                                             const Elem *)
{ NEDELEC_LOW_D_ERROR_MESSAGE }

template <>
void FE<1,NEDELEC_ONE>::compute_constraints (DofConstraints &,
                                             DofMap &,
                                             const unsigned int,
                                             const Elem *)
{ NEDELEC_LOW_D_ERROR_MESSAGE }

template <>
void FE<2,NEDELEC_ONE>::compute_constraints (DofConstraints & constraints,
                                             DofMap & dof_map,
                                             const unsigned int variable_number,
                                             const Elem * elem)
{ nedelec_one_compute_constraints(constraints, dof_map, variable_number, elem, /*Dim=*/2); }

template <>
void FE<3,NEDELEC_ONE>::compute_constraints (DofConstraints & constraints,
                                             DofMap & dof_map,
                                             const unsigned int variable_number,
                                             const Elem * elem)
{ nedelec_one_compute_constraints(constraints, dof_map, variable_number, elem, /*Dim=*/3); }
#endif // LIBMESH_ENABLE_AMR

// Specialize useless shape function methods
template <>
RealGradient FE<0,NEDELEC_ONE>::shape(const ElemType, const Order,const unsigned int,const Point &)
{ NEDELEC_LOW_D_ERROR_MESSAGE }
template <>
RealGradient FE<0,NEDELEC_ONE>::shape(const Elem *,const Order,const unsigned int,const Point &,const bool)
{ NEDELEC_LOW_D_ERROR_MESSAGE }
template <>
RealGradient FE<0,NEDELEC_ONE>::shape_deriv(const ElemType, const Order,const unsigned int,
                                            const unsigned int,const Point &)
{ NEDELEC_LOW_D_ERROR_MESSAGE }
template <>
RealGradient FE<0,NEDELEC_ONE>::shape_deriv(const Elem *,const Order,const unsigned int,
                                            const unsigned int,const Point &,const bool)
{ NEDELEC_LOW_D_ERROR_MESSAGE }

#ifdef LIBMESH_ENABLE_SECOND_DERIVATIVES
template <>
RealGradient FE<0,NEDELEC_ONE>::shape_second_deriv(const ElemType, const Order,const unsigned int,
                                                   const unsigned int,const Point &)
{ NEDELEC_LOW_D_ERROR_MESSAGE }
template <>
RealGradient FE<0,NEDELEC_ONE>::shape_second_deriv(const Elem *,const Order,const unsigned int,
                                                   const unsigned int,const Point &,const bool)
{ NEDELEC_LOW_D_ERROR_MESSAGE }

#endif

template <>
RealGradient FE<1,NEDELEC_ONE>::shape(const ElemType, const Order,const unsigned int,const Point &)
{ NEDELEC_LOW_D_ERROR_MESSAGE }
template <>
RealGradient FE<1,NEDELEC_ONE>::shape(const Elem *,const Order,const unsigned int,const Point &,const bool)
{ NEDELEC_LOW_D_ERROR_MESSAGE }
template <>
RealGradient FE<1,NEDELEC_ONE>::shape_deriv(const ElemType, const Order,const unsigned int,
                                            const unsigned int,const Point &)
{ NEDELEC_LOW_D_ERROR_MESSAGE }
template <>
RealGradient FE<1,NEDELEC_ONE>::shape_deriv(const Elem *,const Order,const unsigned int,
                                            const unsigned int,const Point &,const bool)
{ NEDELEC_LOW_D_ERROR_MESSAGE }

#ifdef LIBMESH_ENABLE_SECOND_DERIVATIVES
template <>
RealGradient FE<1,NEDELEC_ONE>::shape_second_deriv(const ElemType, const Order,const unsigned int,
                                                   const unsigned int,const Point &)
{ NEDELEC_LOW_D_ERROR_MESSAGE }
template <>
RealGradient FE<1,NEDELEC_ONE>::shape_second_deriv(const Elem *,const Order,const unsigned int,
                                                   const unsigned int,const Point &,const bool)
{ NEDELEC_LOW_D_ERROR_MESSAGE }

#endif

} // namespace libMesh
