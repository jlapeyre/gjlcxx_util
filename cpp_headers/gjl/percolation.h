// -*-c++-*-
#ifndef GJL_PERCOLATION_H
#define GJL_PERCOLATION_H

/*****************************************************
* Copyright 2014 John Lapeyre                        *
*                                                    *
* You can redistribute this software under the terms *
* of the GNU General Public License version 3        *
* or a later version.                                *
******************************************************/

namespace perc {
  namespace site {
    namespace square {
      constexpr double pc = 0.592746010; // arXiv:1401.7847
    }
  }
  namespace bond {
    namespace square {
      constexpr double pc = 0.5;
    }
  }
} /*** END namespace perc */


#endif
