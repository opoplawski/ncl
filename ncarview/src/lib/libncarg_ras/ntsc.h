/*
 *	$Id: ntsc.h,v 1.5 2000-08-22 03:30:24 haley Exp $
 */
/************************************************************************
*                                                                       *
*                Copyright (C)  2000                                    *
*        University Corporation for Atmospheric Research                *
*                All Rights Reserved                                    *
*                                                                       *
* This file is free software; you can redistribute it and/or modify     *
* it under the terms of the GNU General Public License as published     *
* by the Free Software Foundation; either version 2 of the License, or  *
* (at your option) any later version.                                   *
*                                                                       *
* This software is distributed in the hope that it will be useful, but  *
* WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
* General Public License for more details.                              *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this software; if not, write to the Free Software          *
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307   *
* USA.                                                                  *
*                                                                       *
************************************************************************/

#define MAX_PIXELS	256
#define GAMMA		2.2

/*
These values are not general - they are for a Parallax VideoView
frame buffer. Different video devices can have different maximum
composite and chroma magnitudes. For the Parallax, black
starts at 0 IRE, not 7.5 IRE setup.
*/

/* Values are for Parallax frame buffer. Units are in IRE */

static float	ChromaMax =  50.0; /* Chroma amplitude */
static float	LumaMax   =  95.0; /* Luminance level */
static float	Pedestal  =   0.0; /* Where we start from */
/*static float	IREMax    = 110.0;*/ /* Level not to exceed */

/* Equations to convert from RGB to YIQ and back */

#define NTSC_Y(r,g,b) (.2989 * (r) + .5866 * (g) + .1144 * (b))
#define NTSC_I(r,g,b) (.5959 * (r) - .2741 * (g) - .3218 * (b))
#define NTSC_Q(r,g,b) (.2113 * (r) - .5227 * (g) + .3113 * (b))

#define NTSC_R(y,i,q) ((y) + 0.9562 * (i) + 0.6210 * (q))
#define NTSC_G(y,i,q) ((y) - 0.2717 * (i) - 0.6485 * (q))
#define NTSC_B(y,i,q) ((y) - 1.1053 * (i) + 1.7020 * (q))
