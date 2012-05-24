// Copyright (C) 2010-2011 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include "common/Builder.hpp"

#include "sdm/lineuler/SourceLighthill2D.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace cf3 {
namespace sdm {
namespace lineuler {

//////////////////////////////////////////////////////////////////////////////

common::ComponentBuilder<SourceLighthill2D,Term,LibLinEuler> SourceLighthill2D_builder;

/////////////////////////////////////////////////////////////////////////////

} // lineuler
} // sdm
} // cf3
