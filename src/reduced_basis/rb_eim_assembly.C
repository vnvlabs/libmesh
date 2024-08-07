// rbOOmit: An implementation of the Certified Reduced Basis method.
// Copyright (C) 2009, 2010 David J. Knezevic

// This file is part of rbOOmit.

// rbOOmit is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// rbOOmit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// rbOOmit includes
#include "libmesh/rb_eim_assembly.h"
#include "libmesh/rb_eim_construction.h"

// libMesh includes
#include "libmesh/fem_context.h"
#include "libmesh/dof_map.h"
#include "libmesh/quadrature.h"
#include "libmesh/libmesh_logging.h"
#include "libmesh/int_range.h"

namespace libMesh
{

RBEIMAssembly::RBEIMAssembly(RBEIMConstruction & rb_eim_con,
                             unsigned int basis_function_index_in)
  :
  _rb_eim_con(rb_eim_con),
  _basis_function_index(basis_function_index_in)
{
}

RBEIMAssembly::~RBEIMAssembly() = default;

void RBEIMAssembly::evaluate_basis_function(dof_id_type elem_id,
                                            unsigned int comp,
                                            std::vector<Number> & values)
{
  get_rb_eim_construction().get_rb_eim_evaluation().get_eim_basis_function_values_at_qps(
    _basis_function_index, elem_id, comp, values);

  libmesh_error_msg_if(values.empty(), "Error: EIM basis function has no entries on this element for this processor");
}

void RBEIMAssembly::evaluate_side_basis_function(dof_id_type elem_id,
                                                 unsigned int side_index,
                                                 unsigned int comp,
                                                 std::vector<Number> & values)
{
  get_rb_eim_construction().get_rb_eim_evaluation().get_eim_basis_function_side_values_at_qps(
    _basis_function_index, elem_id, side_index, comp, values);

  libmesh_error_msg_if(values.empty(), "Error: EIM basis function has no entries on this element for this processor");
}

RBEIMConstruction & RBEIMAssembly::get_rb_eim_construction()
{
  return _rb_eim_con;
}

}
