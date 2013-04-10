// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <coolfluid-ufem-config.hpp>

#include "../NavierStokesSemiImplicit.hpp"
#include "../MatrixAssembly.hpp"

namespace cf3 {
namespace UFEM {

using namespace common;
using namespace solver;
using namespace solver::actions;
using namespace solver::actions::Proto;

void NavierStokesSemiImplicit::set_matrix_assembly_triag( cf3::UFEM::LSSAction& rhs_lss, cf3::UFEM::LSSAction& t_lss )
{
#ifdef CF3_UFEM_ENABLE_TRIAGS
  set_matrix_assembly< boost::mpl::vector1<mesh::LagrangeP1::Triag2D> >(rhs_lss, t_lss, "AssemblyTriags");
#endif
}

} // UFEM
} // cf3
