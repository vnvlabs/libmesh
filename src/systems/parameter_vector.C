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



// Local Includes
#include "libmesh/parameter_pointer.h"
#include "libmesh/parameter_vector.h"

namespace libMesh
{

ParameterVector::ParameterVector(const std::vector<Number *> &params)
  : _is_shallow_copy(false)
{
  _params.reserve(params.size());

  for (auto p : params)
    _params.push_back(new ParameterPointer<Number>(p));
}



void ParameterVector::deep_copy(ParameterVector & target) const
{
  const std::size_t Np = this->_params.size();
  target.clear();
  target._params.resize(Np);
  target._my_data.resize(Np);
  for (std::size_t i=0; i != Np; ++i)
    {
      target._params[i] =
        new ParameterPointer<Number>(&target._my_data[i]);
      target._my_data[i] = *(*this)[i];
    }
}



void ParameterVector::shallow_copy(ParameterVector & target) const
{
  target._my_data.clear();
  target._params = this->_params;
  target._is_shallow_copy = true;
}



void ParameterVector::value_copy(ParameterVector & target) const
{
  const std::size_t Np = this->_params.size();
  libmesh_assert_equal_to (target._params.size(), Np);

  for (std::size_t i=0; i != Np; ++i)
    *target[i] = *(*this)[i];
}



void ParameterVector::resize(std::size_t s)
{
  libmesh_assert(!_is_shallow_copy);

  const std::size_t old_size = this->_params.size();

  // If we're shrinking the vector, we don't want to leak memory.
  // Note that we're using < in these for loops, not !=
  // We don't know a priori if we're shrinking or growing
  for (std::size_t i=s; i < old_size; ++i)
    delete _params[i];

  this->_params.resize(s);

  for (std::size_t i=old_size; i < s; ++i)
    this->_params[i] =
      new ParameterPointer<Number>(nullptr);
}



void ParameterVector::deep_resize(std::size_t s)
{
  libmesh_assert(!_is_shallow_copy);

  this->_params.resize(s);
  this->_my_data.resize(s);
  for (std::size_t i=0; i != s; ++i)
    this->_params[i] =
      new ParameterPointer<Number>(&this->_my_data[i]);
}



ParameterVector & ParameterVector::operator *= (const Number a)
{
  const std::size_t Np = this->_params.size();
  for (std::size_t i=0; i != Np; ++i)
    *(*this)[i] *= a;
  return *this;
}



ParameterVector & ParameterVector::operator += (const ParameterVector & a)
{
  const std::size_t Np = this->_params.size();
  libmesh_assert_equal_to (a._params.size(), Np);
  for (std::size_t i=0; i != Np; ++i)
    *(*this)[i] += *a[i];
  return *this;
}


} // namespace libMesh
