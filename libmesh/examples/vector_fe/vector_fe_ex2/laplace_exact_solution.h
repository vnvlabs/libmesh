/* The Next Great Finite Element Library. */
/* Copyright (C) 2003  Benjamin S. Kirk */

/* This library is free software; you can redistribute it and/or */
/* modify it under the terms of the GNU Lesser General Public */
/* License as published by the Free Software Foundation; either */
/* version 2.1 of the License, or (at your option) any later version. */

/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU */
/* Lesser General Public License for more details. */

/* You should have received a copy of the GNU Lesser General Public */
/* License along with this library; if not, write to the Free Software */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include "libmesh_common.h"

#ifndef __laplace_exact_solution_h__
#define __laplace_exact_solution_h__

class LaplaceExactSolution
{
public:
  LaplaceExactSolution(){}

  ~LaplaceExactSolution(){}
  
  Real operator()( unsigned int component, 
		   Real x, Real y, Real z = 0.0)
  {
    switch(component)
    {
    case 0:
      return cos(.5*pi*x)*sin(.5*pi*y)*cos(.5*pi*z);

    case 1:
      return sin(.5*pi*x)*cos(.5*pi*y)*cos(.5*pi*z);

    case 2:
      return sin(.5*pi*x)*cos(.5*pi*y)*sin(.5*pi*z);

    default:
      libmesh_error();
    }
  }
};

#endif // __laplace_exact_solution_h__
