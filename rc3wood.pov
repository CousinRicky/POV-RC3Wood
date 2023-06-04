/* rc3wood.pov version 2.0
 * Persistence of Vision Raytracer include file
 * A proposed POV-Ray Object Collection demo
 *
 * Demonstrates several RC3Wood wood textures.
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
 * 2.0    2023-Jun-04  Initial upload.
 */
// +W800 +H600 +A0.05 +R5
#version max (3.5, min (3.8, version));

#ifndef (Rad) #declare Rad = yes; #end
#ifndef (Sotf) #declare Soft = yes; #end

#include "logo.inc"
#include "shapes.inc"
#include "transforms.inc"
#include "rc3wood.inc"

#declare PV_PLACEMENT = <0, 30, 40>;

global_settings
{ assumed_gamma 1
  #if (Rad)
    radiosity
    { count 100
      error_bound 0.5
      pretrace_start 64 / image_width
      pretrace_end 2 / image_width
      recursion_limit 3
    }
  #end
}
#declare C_AMBIENT = rgb (Rad? 0: <0.10, 0.085, 0.08>);
#default
{ finish
  { diffuse 0.75 ambient C_AMBIENT * 0.75
    #if (version >= 3.7) emission 0 #end
  }
}

light_source
{ <0, 84, 0>, rgb 3000
  fade_distance 1
  fade_power 2
  spotlight point_at 0
  radius -90 falloff 90 tightness 1
  #if (Soft)
    area_light 7 * x, 7 * z, 5, 5
    circular adaptive 0 jitter
  #end
}

camera
{ location PV_PLACEMENT + <20, 20, -35>
  look_at PV_PLACEMENT + <0, 3.5, 0>
  angle 22
}

box
{ -<72, 0, 72>, <72, 96, 72>
  hollow
  pigment { rgb 0.8 }
}

//====================== TABLE TOP =========================

union
{ #local Tx = -4;
  #while (Tx < 4)
    intersection
    { cylinder { 0, -1.5 * y, 30 }
      box { <Tx * 7.5, -2, -30>, <(Tx + 1) * 7.5, 1, 30> }
     // A layered texture with a smooth gloss:
      #local t_Table = RC3Wood_Solid_t
      ( 15 * (mod (Tx, 2) = 0? 1: -1), RC3W_Rings_Medium,
        rgb <0.32, 0.2, 0.14>, rgb <0.2, 0.1, 0.05>,
        0.1, 0.5, 0.3, <0.5, 0.5, 2>, 0
      );
      texture
      { t_Table
        scale 1/8
        translate <(Tx + 0.5) * 8, Tx * 2, 0>
      }
      texture
      { pigment { rgbf 1 }
        RC3Wood_Gloss_f (1, 0.002, RC3W_nPOLYU_VARNISH_HIGH)
      }
    }
    #local Tx = Tx + 1;
  #end
  interior { RC3W_i_Polyu_Varnish_High } // Activate the gloss
  rotate 120 * y
  translate PV_PLACEMENT
}

//========================= BOX ============================

// A finished wood whose texture shows through the gloss;
// this material includes the interior block:
#declare m_Box = RC3Wood_Varnished_m
( 12, RC3W_Rings_Heavy, <0.3, 0.06, 0.04>, <0.125, 0.025, 0.015>,
  0.1, 1, 0.4, <1, 1, 3>, 100, 1, 0.01, RC3W_nSHELLAC
)

#declare Rnd = seed (101);
#macro Rand_pos (Scale)
  scale Scale
  translate (<rand (Rnd), rand (Rnd), rand (Rnd)> - 0.5) * <10, 10, 100>
#end

#declare THICKNESS = 0.5;
#declare HALF_WIDTH = 4;
#declare HALF_DEPTH = 3;
#declare HEIGHT = 3;

#declare Front = box
{ -HALF_WIDTH * x, <HALF_WIDTH, THICKNESS + HEIGHT, -THICKNESS>
  translate -HALF_DEPTH * z
}
#declare Side = union
{ box { -HALF_DEPTH * z, <THICKNESS, THICKNESS + HEIGHT, HALF_DEPTH> }
  intersection
  { cylinder { 0, (THICKNESS + HEIGHT) * y, THICKNESS }
    plane { -x, 0 }
    translate HALF_DEPTH * z
  }
  intersection
  { cylinder { 0, (THICKNESS + HEIGHT) * y, THICKNESS }
    plane { -x, 0 }
    translate -HALF_DEPTH * z
  }
  intersection
  { cylinder { -HALF_DEPTH * z, HALF_DEPTH * z, THICKNESS }
    plane { -x, 0 }
    translate (THICKNESS + HEIGHT) * y
  }
  intersection
  { sphere { 0, THICKNESS }
    box { 0, 1 }
    translate <0, THICKNESS + HEIGHT, HALF_DEPTH>
  }
  intersection
  { sphere { 0, THICKNESS }
    box { 0, 1 }
    translate <0, THICKNESS + HEIGHT, HALF_DEPTH>
    scale <1, 1, -1>
  }
  translate HALF_WIDTH * x
}
#declare Lid = union
{ box { <-HALF_WIDTH, THICKNESS, -2 * HALF_DEPTH>, HALF_WIDTH * x }
  intersection
  { cylinder
    { -HALF_WIDTH * x, HALF_WIDTH * x, THICKNESS / 2
      translate <0, THICKNESS, THICKNESS> / 2
    }
    cylinder { -x, x, THICKNESS scale <HALF_WIDTH + 0.1, 1, 1> }
  }
  intersection
  { box { -HALF_WIDTH * x, <HALF_WIDTH, THICKNESS, THICKNESS / 2> }
    cylinder { -x, x, THICKNESS scale <HALF_WIDTH + 0.1, 1, 1> }
  }
  // Use a blob for a grip notch with rounded edges.
  // The magic numbers for the blob strengths were calculated to match the box
  // dimensions using function RE_fn_Blob_strength() from RoundEdge, which is
  // also available from the Object Collection.
  intersection
  { blob
    { cylinder { -HALF_WIDTH * x, HALF_WIDTH * x, THICKNESS * 9/8, 22.7024 }
      cylinder
      { -1.25 * x, 1.25 * x, THICKNESS * 4/8, -5.2245
        scale <1, 1, 2>
        translate -THICKNESS * z
      }
    }
    box { -HALF_WIDTH * x, <HALF_WIDTH, THICKNESS, -THICKNESS> }
    translate -2 * HALF_DEPTH * z
  }
  // Move to the pivot location:
  translate <0, -THICKNESS / 2, -THICKNESS / 2>
}

#declare Box_posn = transform
{ rotate -75 * y
  translate PV_PLACEMENT + <-3.5, 0, 1>
}

union
{ box
  { -<HALF_WIDTH, 0, HALF_DEPTH>, <HALF_WIDTH, THICKNESS, HALF_DEPTH>
    material { m_Box Rand_pos (0.1) rotate 90 * y }
  }
  object
  { Front
    material { m_Box Rand_pos (0.1) rotate 90 * y }
  }
  object
  { Front
    material { m_Box Rand_pos (0.1) rotate 90 * y }
    rotate 180 * y
  }
  object
  { Side
    material { m_Box Rand_pos (0.1) }
  }
  object
  { Side
    material { m_Box Rand_pos (0.1) }
    rotate 180 * y
  }
  object
  { Lid
    material { m_Box Rand_pos (0.1) rotate 90 * y }
    rotate x * 42.21
    translate <0, HEIGHT + 1.5 * THICKNESS, HALF_DEPTH + THICKNESS / 2>
  }
  // No interior block required.
  transform { Box_posn }
}

//================== OBJECT INSIDE BOX =====================

// An unfinished wood:
#declare t_Thing = RC3Wood_Solid_t
( 30, RC3W_Rings_Light2, <0.8, 0.6, 0.4>, <0.4, 0.3, 0.2>,
  0.04, 1, 0.2, <1, 1, 3>, 20
)

object
{ Round_Cylinder_Union (0, 6 * y, 1.5, 0.3)
  texture { t_Thing rotate -90 * x scale 0.3 translate -0.5 }
  translate <0, THICKNESS, 1.5 - HALF_DEPTH>
  transform { Box_posn }
}

//========================= LOGO ===========================

// A finished wood whose texture shows through the gloss:
#declare t_Logo = RC3Wood_Varnished_t
( 30, RC3W_Rings_Light, <0.60, 0.40, 0.10>, <0.36, 0.15, 0.08>,
  0.04, 0.5, 0.2, <1, 1, 3>, 40, 1, 0.01, RC3W_nSHELLAC
)

#declare PV_SPHERE = vtransform
( 2 * y,
  transform { rotate <0, 0, -25> translate <-0.5, 3.65, 0> } // See logo.inc
);
union
{ object
  { Povray_Logo
    translate y
    scale 4
    texture
    { object
      { sphere { PV_SPHERE, 1.1 }
        texture
        { t_Logo
          scale 0.25
          rotate <-90, 0, -30>
          translate -2 * x
        }
        texture
        { t_Logo
          scale 0.25
          rotate 90 * x
          translate PV_SPHERE - x
        }
      }
    }
    interior { RC3W_i_Shellac } // Activate the gloss
  }
  cylinder // rod to hold sphere in place
  { 2 * y, -2 * y, 1/16
    texture
    { pigment { rgb <0.53, 0.51, 0.49> }
      finish
      { diffuse 0.1 ambient 0.1 * C_AMBIENT
        reflection { 0.8 metallic }
        specular 100 metallic roughness 0.001
      }
    }
    rotate -25 * z
    translate PV_SPHERE
  }
  rotate <78.23, 0, -2.47>
  rotate -15 * y
  translate PV_PLACEMENT + <5, -0.098, -2>
}

// end of rc3wood.pov
