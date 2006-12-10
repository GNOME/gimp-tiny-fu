/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __SCRIPT_FU_ENUMS_H__
#define __SCRIPT_FU_ENUMS_H__

/*  Typedefs for script-fu argument types  */

typedef enum
{
  SF_IMAGE = 0,
  SF_DRAWABLE,
  SF_LAYER,
  SF_CHANNEL,
  SF_VECTORS,
  SF_COLOR,
  SF_TOGGLE,
  SF_VALUE,
  SF_STRING,
  SF_ADJUSTMENT,
  SF_FONT,
  SF_PATTERN,
  SF_BRUSH,
  SF_GRADIENT,
  SF_FILENAME,
  SF_DIRNAME,
  SF_OPTION,
  SF_PALETTE,
  SF_TEXT,
  SF_ENUM
} SFArgType;

typedef enum
{
  SF_SLIDER = 0,
  SF_SPINNER
} SFAdjustmentType;

#endif /*  __SCRIPT_FU_ENUMS__  */
