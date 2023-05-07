/* rc3wood.inc version 1.1.1-20090421A
 * Persistence of Vision Raytracer include file
 *
 * Wood textures.
 *
 * Copyright © 2006 - 2009 Richard Callwood III.  Some rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Vers.  Date         Comments
 * -----  ----         --------
 * 1.0    2006-Dec-10  Created as rc3_wood.inc, based on wood5.pov.
 * 1.1    2006-Dec-17  Renamed to rc3wood.inc.
 * 1.1.1 ≤2008-mar-19  A second macro is added with a roughness argument.
 * 1.1.1  2009-Apr-21  Last update.
 *        2014-Nov-22  Administrative:  The change log is reconstructed from
 *                     available evidence.
 *        2023-May-07  The library is cleaned up for public release.
 */
#ifndef (RC3_WOOD_INC) #declare RC3_WOOD_INC = version;
#version 3.6;
#declare RC3WOOD_VERSION = 1.11;

#include "functions.inc"

#debug "---------------\n"
#debug "#include \"rc3wood.inc\" // version 1.1.1-20090421A\n"

#macro RC3_Wood2_t (Hue, Contrast, Gnarly, Stretch, Banding,
                    Grainy, Rough, Varnish)
   #local G = Grainy / 2;
   #local n_Grain = normal
   {  function { f_noise3d (x * 10, y * 10, z / 10) } 0.2 * Rough
      slope_map
      {  [0     <0.0, 0>]
         [G     <2/3, 2>]
         [G     <2/3, 0>]
         [1 - G <2/3, 0>]
         [1 - G <2/3, 1>]
         [1     <1.0, 1>]
      }
      scale <1, 1, 1 /*0.5*/ / Stretch>
      warp { turbulence Gnarly * 2/3 }
   }
   #local n_Rings = normal
   {  wood 0.2 * Rough ramp_wave
      slope_map
      {  [0.0  <0.5,  0>]
         [0.2  <0.5,  0>]
         [0.6  <1.0,  0>]
         [0.75 <1.0,  0>]
         [0.75 <1.0, -1>]
         [0.8  <0.0,  1>]
         [0.85 <0.5,  0>]
      }
      warp { turbulence Gnarly }
   };
   #local t_Rings = texture
   {  pigment
      {  wood ramp_wave color_map
         {  [0.0 color Hue]
            [0.2 color Hue]
            [0.6 color Hue * (1 - Contrast)]
            [0.8 color Hue * (1 - Contrast)]
            [0.8 color Hue]
            [1.0 color Hue]
         }
         warp { turbulence Gnarly }
      }
      normal
      {  average normal_map
         {  [1 n_Rings]
            [1 n_Grain]
         }
      }
      finish
      {  specular 0.1 + Varnish * 0.3
         roughness 0.03 / (Varnish + 1) + 0.01
      }
      scale <1, 1, Stretch>
   }
   #local t_Sheen = texture
   {  pigment { rgbf 1 }
      finish
      {  reflection { 0 0.4 * Varnish fresnel }
         specular Varnish * 0.6 roughness 0.0025
      }
   }
   #if (Banding)
      texture { t_Rings }
      texture
      {  pigment
         {  gradient z triangle_wave color_map
            {  [0.0 rgb 0 transmit 1]
               [0.2 rgb 0 transmit 1]
               [0.8 rgb 0 transmit 1 - Contrast * 0.5]
               [1.0 rgb 0 transmit 1 - Contrast * 0.5]
            }
            warp { turbulence 0.3 }
            scale Banding
         }
      }
      texture { t_Sheen }
   #else
      texture { t_Rings }
      texture { t_Sheen }
   #end
#end

#macro RC3_Wood1_t (Hue, Contrast, Gnarly, Stretch, Banding, Grainy, Varnish)
   RC3_Wood2_t (Hue, Contrast, Gnarly, Stretch, Banding, Grainy, 1, Varnish)
#end

#declare i_Wood_varnish = interior { ior 1.4 } // varnish won't work w/o this

#version RC3_WOOD_INC;
#end
