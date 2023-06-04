/* rc3wood.inc version 2.0
 * Persistence of Vision Raytracer include file
 * A proposed POV-Ray Object Collection module
 *
 * Procedural wood texture macros.
 *
 * Copyright (C) 2023 Richard Callwood III.  Some rights reserved.
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
 * 2.0    2023-Jun-04  The module is rewritten for the Object Collection.
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

#ifndef (rc3w__Debug_slope) #declare rc3w__Debug_slope = no; #end
#declare RC3W__OMEGA = 0.4;

// A wood grain normal.
// (This function needs A LOT of work!)
#declare RC3W__fn_Grain = function (x, y, z, rc3w_P0_Taper)
{ f_noise_generator (x, y, z / 100, 2)
}

// Returns a compromise value for attenuation of highlights, for use with
// POV-Ray versions that do not have Fresnel highlighting.
// Technical note: For IORs between 1.333 and 2.417 (water and diamond),
// this formula seems to match my test renders at 45 degrees AOI better
// than Schlick's approximation does.
#declare RC3W__fn_Hilite = function (x)
{ select (x - 1.4, pow (x - 1, 1.5) * 0.1415, x * 0.142 - 0.163)
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

// Adjusts a specular highlight argument using the albedo keyword if available,
// or the equivalent formula if not.
#macro RC3W__Albedo (Hilite, Roughness)
  #if (version < 3.7)
   //See Parser::Parse_Finish() in parstxtr.cpp in the POV-Ray 3.7+ source.
    (1 / Roughness + 2) / (4 * (2 - pow (2, -0.5 / Roughness))) * Hilite
  #else
    albedo Hilite
  #end
#end

// Creates a color map for the growth rings.
#macro RC3W__Color_map (Rings_map, c_Light, c_Dark)
  color_map
  { #local rc3w_U = 0;
    #while (rc3w_U < dimension_size (Rings_map, 1))
      #local rc3w_Map = <0, 0> + Rings_map [rc3w_U];
      [rc3w_Map.u color c_Light * (1 - rc3w_Map.v) + c_Dark * rc3w_Map.v]
      #local rc3w_U = rc3w_U + 1;
    #end
  }
#end

// Checks a scale vector for zero elements, and converts any zeroes to ones.
// A scalar argument is promoted to a vector.
#macro RC3W__Scale (Scale)
  #local rc3w_V = <0,0,0> + Scale;
  <(rc3w_V.x? rc3w_V.x: 1), (rc3w_V.y? rc3w_V.y: 1), (rc3w_V.z? rc3w_V.z: 1)>
#end

// Creates a slope map for the growth rings.
#macro RC3W__Slope_map (Rings_map)
  #local rc3w_N = dimension_size (Rings_map, 1);
  #local rc3w_Domain = false; // error flag
  #local rc3w_Order = false; // error flag
  slope_map
  { #local rc3w_I = 0;
    #while (rc3w_I < rc3w_N)
      #local rc3w_Here = <0,0> + Rings_map [rc3w_I];
      #if (rc3w_Here.u < 0 | rc3w_Here.u > 1) #local rc3w_Domain = true; #end
      #if (rc3w__Debug_slope)
        #debug concat ("<", vstr (2, rc3w_Here, ", ", 0, 3), ">")
      #end
      #if (rc3w_N = 1)
        #if (rc3w__Debug_slope) RC3W__Write_slope (rc3w_Here, 0) #end
        [rc3w_Here.u, <rc3w_Here.v, 0>]
      #else
        #if (rc3w_I > 0)
          #local rc3w_Prev = <0,0> + Rings_map [rc3w_I - 1];
          #local rc3w_M = RC3W__Slope (rc3w_Prev, rc3w_Here);
          #if (rc3w__Debug_slope) RC3W__Write_slope (rc3w_Here, rc3w_M) #end
          [rc3w_Here.u, <rc3w_Here.v, rc3w_M>]
          #if (rc3w_Here.u < rc3w_Prev.u) #local rc3w_Order = true; #end
        #end
        #if (rc3w_I < rc3w_N - 1)
          #local rc3w_Next = <0,0> + Rings_map [rc3w_I + 1];
          #local rc3w_M = RC3W__Slope (rc3w_Here, rc3w_Next);
          #if (rc3w__Debug_slope) RC3W__Write_slope (rc3w_Here, rc3w_M) #end
          [rc3w_Here.u, <rc3w_Here.v, rc3w_M>]
        #end
      #end
      #if (rc3w__Debug_slope) #debug "\n" #end
      #local rc3w_I = rc3w_I + 1;
    #end
  }
  #if (rc3w_Order)
    #warning "Rings map entries are out of sequence; wood normal may not be as expected."
  #end
  #if (rc3w_Domain)
    #warning "Rings map entry position is outside 0...1; wood normal may not be as expected."
  #end
  #if (rc3w_Order | rc3w_Domain) // Data dump on error
    #debug "Rings_map argument:\n"
    #local rc3w_I = 0;
    #while (rc3w_I < rc3w_N)
      #debug concat
      ( "  <", vstr (2, <0,0> + Rings_map [rc3w_I], ", ", 0, 3), ">\n"
      )
      #local rc3w_I = rc3w_I + 1;
    #end
  #end
#end

#macro RC3W__Write_slope (uv_Entry, Slope)
  #debug concat
  ( " [", vstr (2, uv_Entry, ", <", 0, 3), ",", str (Slope, 6, 3), ">]"
  )
#end

#macro RC3W__Slope (Entry1, Entry2)
( #if (Entry1.u = Entry2.u)
    select (Entry2.v - Entry1.v, -3, 0, 3) // clip |infinity| to 3
  #else
    max (-3, min (3, (Entry2.v - Entry1.v) / (Entry2.u - Entry1.u)))
  #end
)
#end

//============================= PUBLIC FEATURES ================================
/* Implementation note: When designing these macros I had to decide whether to
 * return a block or an identifier.  An identifier would seem more convenient
 * for the user, as wood textures usually need to be transformed before being
 * applied to an object.  It is easier to type
 *   texture { RC3Wood_Solid_t (...) scale 0.1 }
 * than
 *   #declare T = RC3Wood_Solid_t (...)
 *   texture { T scale 0.1 }
 * Unfortunately, I have run into intractable parser roadblocks with the
 * identifier plan, so I decided to return blocks.  It may be for the better,
 * as the user would likely run into the same roadblocks anyway, and declaring
 * the textures often leads to cleaner code.
 */

#macro RC3Wood_Gloss_f (Glossiness, Softness, IoR)
  finish
  { #if (version >= 3.8)
      fresnel Glossiness
      #if (Softness) specular albedo Glossiness roughness Softness #end
    #else
      #if (Softness)
        specular RC3W__Albedo (Glossiness * RC3W__fn_Hilite (IoR), Softness)
        roughness Softness
      #end
    #end
    reflection { Glossiness fresnel } conserve_energy
  }
#end

//------------------------------------------------

#macro RC3Wood_Normal
( Taper, Rings_map, Grain_size, Roughness, Gnarl, Gnarl_size, v_Random
)
  #local RC3W_GRAINY = 1.5;
  #local RC3W_CROSS_BUMP = 2;
  #local rc3w_v_Gnarl = RC3W__Scale (Gnarl_size);
  #local rc3w_Stretch = rc3w_v_Gnarl.z;
  #local rc3w_Bump_sum = 1 + Grain_size * RC3W_CROSS_BUMP;
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
    translate v_Random
    scale Grain_size
     * <1 / rc3w_v_Gnarl.x, 1 / rc3w_v_Gnarl.y, RC3W_CROSS_BUMP / rc3w_Stretch>
    warp { turbulence <1, 1, 0> * Gnarl omega RC3W__OMEGA }
    scale <rc3w_v_Gnarl.x, rc3w_v_Gnarl.y, 1>
    translate -v_Random
  }
  #local rc3w_n_Rings = normal
  { function { RC3W__fn_Rings (x, y, z, Taper) }
    Roughness * rc3w_Bump_sum
    RC3W__Slope_map (Rings_map)
    translate v_Random
    scale <1 / rc3w_v_Gnarl.x, 1 / rc3w_v_Gnarl.y, 1>
    warp { turbulence <1, 1, 0> * Gnarl omega RC3W__OMEGA }
    scale <rc3w_v_Gnarl.x, rc3w_v_Gnarl.y, 1 / rc3w_Stretch>
    translate -v_Random
  };
  normal
  { average normal_map
    { [Grain_size * RC3W_CROSS_BUMP rc3w_n_Rings]
      [1 rc3w_n_Grain]
    }
    scale <1, 1, rc3w_Stretch>
  }
#end

//------------------------------------------------

#macro RC3Wood_Solid_t
( Taper, Rings_map, c_Light, c_Dark,
  Grain_size, Roughness, Gnarl, Gnarl_size, v_Random
)
  #local rc3w_v_Gnarl = RC3W__Scale (Gnarl_size);
  #local rc3w_Stretch = rc3w_v_Gnarl.z;
  texture
  { pigment
    { function { RC3W__fn_Rings (x, y, z, Taper / rc3w_Stretch) }
      RC3W__Color_map (Rings_map, c_Light, c_Dark)
      translate v_Random
      scale <1 / rc3w_v_Gnarl.x, 1 / rc3w_v_Gnarl.y, 1>
      warp { turbulence <1, 1, 0> * Gnarl omega RC3W__OMEGA }
      scale <rc3w_v_Gnarl.x, rc3w_v_Gnarl.y, 1>
      translate -v_Random
      scale <1, 1, rc3w_Stretch>
    }
    RC3Wood_Normal
    ( Taper, Rings_map, Grain_size, Roughness, Gnarl, Gnarl_size, v_Random
    )
  }
#end

//------------------------------------------------

#macro RC3Wood_Varnished_t
( Taper, Rings_map, c_Light, c_Dark,
  Grain_size, Roughness, Gnarl, Gnarl_size, v_Random,
  Glossiness, Softness, IoR
)
  #local rc3w_Unfinished = RC3Wood_Solid_t
  ( Taper, Rings_map, c_Light, c_Dark,
    Grain_size, Roughness, Gnarl, Gnarl_size, v_Random
  );
  #if (version >= 3.8) // POV-Ray 3.8 can do this in one shot
    texture
    { rc3w_Unfinished
      RC3Wood_Gloss_f (Glossiness, Softness, IoR)
    }
  #else // Prior versions need a separate clear layer for proper Fresnel
    #local rc3w_Gloss = texture
    { pigment { rgbf 1 }
      RC3Wood_Normal
      ( Taper, Rings_map, Grain_size, Roughness, Gnarl, Gnarl_size, v_Random
      )
      RC3Wood_Gloss_f (Glossiness, Softness, IoR)
    };
    texture { rc3w_Unfinished }
    texture { rc3w_Gloss }
  #end
#end

//------------------------------------------------

#macro RC3Wood_Varnished_m
( Taper, Rings_map, c_Light, c_Dark,
  Grain_size, Roughness, Gnarl, Gnarl_size, v_Random,
  Glossiness, Softness, IoR
)
  material
  { RC3Wood_Varnished_t
    ( Taper, Rings_map, c_Light, c_Dark,
      Grain_size, Roughness, Gnarl, Gnarl_size, v_Random,
      Glossiness, Softness, IoR
    )
    interior { ior IoR }
  }
#end

//------------------------------------------------

#declare RC3W_Rings_Heavy = array[4]
{ <0.25, 0.0>, <0.4, 0.8>, <0.8, 0.8>, <1.0, 1.0>
}
#declare RC3W_Rings_Light = array[5]
{ <0.0, 0.0>, <0.3, 0.0>, <0.8, 0.2>, <0.9, 0.4>, <1.0, 1.0>
}
#declare RC3W_Rings_Light2 = array[8]
{ <0.0, 0.0>, <0.15, 0.0>, <0.35, 0.3>, <0.5, 0.3>, <0.7, 0.0>,
  <0.75, 0.0>, <0.9, 0.3>, <1.0, 1.0>
}
#declare RC3W_Rings_Medium = array[4]
{ <0.0, 0.0>, <0.4, 0.25>, <0.75, 1.0>, <1.0, 1.0>
}
#declare RC3W_Rings_Medium2 = array[7]
{ <0.0, 0.0>, <0.43, 0.25>, <0.53, 0.9>, <0.55, 0.9>, <0.65, 0.25>,
  <0.8, 1.0>, <1.0, 1.0>
}
#declare RC3W_Rings_Ramp = array[2]
{ <0.0, 0.0>, <1.0, 1.0>
}
#declare RC3W_Rings_Triangle = array[3]
{ <0.0, 0.0>, <0.5, 1.0>, <1.0, 0.0>
}

// Although I'm making a clean start with v2.0, this quasi-legacy
// map serves as a stress test for RC3W__Slope_map().
#declare RC3W_Rings_V111 = array[6]
{ <0.0, 0>, <0.2, 0>, <0.6, 1>, <0.8, 1>, <0.8, 0>, <1.0, 0>
}

// Varnishes/lacquers.  IOR sources vary and are never consistent.
#declare RC3W_nACRYLIC_VARNISH = 1.491;
#declare RC3W_nPOLYU_VARNISH_HIGH = 1.63; // Professor Google
#declare RC3W_nPOLYU_VARNISH_LOW = 1.53; // Professor Google
#declare RC3W_nSHELLAC = 1.525;
#declare RC3W_i_Acrylic_Varnish = interior { ior RC3W_nACRYLIC_VARNISH }
#declare RC3W_i_Polyu_Varnish_High = interior { ior RC3W_nPOLYU_VARNISH_HIGH }
#declare RC3W_i_Polyu_Varnish_Low = interior { ior RC3W_nPOLYU_VARNISH_LOW }
#declare RC3W_i_Shellac = interior { ior RC3W_nSHELLAC }

#version RC3Wood_Inc_Temp;
#end
// end of rc3wood.inc
