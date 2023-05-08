/* rc3wood.inc version 2.0
 * Persistence of Vision Raytracer include file
 * A proposed POV-Ray Object Collection module
 *
 * Wood textures.
 *
 * Copyright (C) 20?? Richard Callwood III.  Some rights reserved.
 * This file is licensed under the terms of the CC-LGPL
 * a.k.a. the GNU Lesser General Public License version 2.1.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  Please
 * visit https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html for
 * the text of the GNU Lesser General Public License version 2.1.
 *
 * Vers.  Date         Comments
 * -----  ----         --------
 * 1.0    2006-Dec-10  Created as rc3_wood.inc.
 * 1.1    2006-Dec-17  The module is given its present name.
 * 1.1.1  2008-Mar-19  A macro with a roughness argument is added.
 *        2009-Apr-21  Last update prior to Object Collection conversion.
 * 2.0    20??-???-??  The module is rewritten for the Object Collection.
 *
 * Implementation note: due to POV-Ray namespace scope leakage, local variables
 * and function parameters (other than x, y, and z) *must* be prefixed.
 */
#ifndef (RC3Wood_Inc_Temp) #declare RC3Wood_Inc_Temp = version;
#version max (3.5, min (3.8, version)); // Bracket the POV version.

#declare RC3WOOD_VERSION = 2.0;
#ifdef (View_POV_Include_Stack)
  #debug "#include \"rc3wood.inc\" // version 2.0\n"
#end

#include "functions.inc"

//============================= PRIVATE FEATURES ===============================
// Tools in this section are not intended be referenced directly by the user.

// A wood grain normal.
// (This function needs A LOT of work!)
#declare RC3W__fn_Grain = function (x, y, z, rc3w_P0_Taper)
{ f_noise_generator (x, y, z / 100, 2)
}

// A cross-section normal for the growth rings.
#declare RC3W__fn_Rings = function (x, y, z, rc3w_P0_Taper)
{ mod
  ( select
    ( rc3w_P0_Taper,
      z / rc3w_P0_Taper, 0, z / rc3w_P0_Taper
    ) + sqrt (x*x + y*y),
    1
  )
}

// Creates a color map for the growth rings.
#macro RC3W__Color_map (Rings_map, c_Light, c_Dark)
  color_map
  { #local rc3w_U = 0;
    #while (rc3w_U < dimension_size (Rings_map, 1))
      #local rc3w_Map = <0, 0> + Rings_map [rc3w_U];
      [ rc3w_Map.u
        color c_Light * (1 - rc3w_Map.v) + c_Dark * rc3w_Map.v
      ]
      #local rc3w_U = rc3w_U + 1;
    #end
  }
#end

// Creates a slope map for the growth rings.
#macro RC3W__Slope_map (Rings_map)
  
#end

//============================= PUBLIC FEATURES ================================

#macro RC3Wood_Solid_t
( Taper, Rings_map, c_Light, c_Dark,
  Grain_size, Roughness, Gnarl, Stretch, v_Random
)
  #local RC3W_GRAINY = 1.5;
  #local RC3W_OMEGA = 0.4;
  #local RC3W_CROSS_BUMP = 2;
  #local rc3w_Bump_sum = 1 + Grain_size * RC3W_CROSS_BUMP;
  #local rc3w_Gnarl = <1, 1, 1 / Stretch> * Gnarl;
  #local rc3w_n_Grain = normal
  { function { RC3W__fn_Grain (x, y, z, Taper * RC3W_CROSS_BUMP) }
    Roughness * rc3w_Bump_sum
    slope_map
    { [0.00 <0, RC3W_GRAINY>]
      [0.25 <1, 0>]
      [0.50 <0, -RC3W_GRAINY>]
      [0.50 <0, RC3W_GRAINY>]
      [0.75 <1, 0>]
      [1.00 <0, -RC3W_GRAINY>]
    }
    scale Grain_size * <1, 1, RC3W_CROSS_BUMP / Stretch>
    warp { turbulence Gnarl omega RC3W_OMEGA }
  }
  #local rc3w_n_Rings = normal
  { function { RC3W__fn_Rings (x, y, z, Taper / Stretch) }
    Roughness * rc3w_Bump_sum
    //slope_map { RC3W__Slope_map (Rings_map) }
    warp { turbulence Gnarl omega RC3W_OMEGA }
  };
  #local rc3w_t_Rings = texture
  { pigment
    { function { RC3W__fn_Rings (x, y, z, Taper / Stretch) }
      RC3W__Color_map (Rings_map, c_Light, c_Dark)
      warp { turbulence Gnarl omega RC3W_OMEGA }
    }
    normal
    { average normal_map
      { [Grain_size * RC3W_CROSS_BUMP rc3w_n_Rings]
        [1 rc3w_n_Grain]
      }
    }
    scale <1, 1, Stretch>
  };
  rc3w_t_Rings
#end

#declare RC3W_Rings_Thick = array[4]
{ <0.0, 0.0>, <0.4, 0.0>, <0.7, 1.0>, <1.0, 1.0>
}

#declare RC3W_Rings_Thin = array[3]
{ <0.0, 0.0>, <0.75, 0.0>, <1.0, 1.0>
}

#declare RC3W_Rings_V111 = array[6]
{ <0.0, 0>, <0.2, 0>, <0.6, 1>, <0.8, 1>, <0.8, 0>, <1.0, 0>
}

// Sources vary and are never consistent:
#declare RC3W_i_Acrylic_Varnish = interior { ior 1.491 }
#declare RC3W_i_Polyu_Varnish_High = interior { ior 1.63 } // Prof. Google
#declare RC3W_i_Polyu_Varnish_Low = interior { ior 1.53 } // Prof. Google
#declare RC3W_i_Shellac = interior { ior 1.525 }

#version RC3Wood_Inc_Temp;
#end
// end of rc3wood.inc
