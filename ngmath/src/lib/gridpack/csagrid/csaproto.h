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

#include <stdio.h>
 
/*
 *  Specify all of the function prototypes.
 */
float *c_csa1s(int, float [], float [], int, int, float [], int *);
float *c_csa1xs(int, float [], float [], float [], int,
             float, int, int, float [], int *);
float *c_csa2s(int, float [], float [], float [], int [],
               int, int, float [], float [], int *);
float *c_csa2xs(int, float [], float [], float [], float [], int [], float, 
                int [], int, int, float [], float [], int *);
float *c_csa2ls(int, float [], float [], float [], int [],
                int, float [], float [], int *);
float *c_csa2lxs(int, float [], float [], float [], float [], int [], 
                 float, int [], int, float [], float [], int *);
float *c_csa3s(int, float [], float [], float [], float [], int [], int, int, 
               int, float [], float [], float [], int *);
float *c_csa3xs(int, float [], float [], float [], float [], float [], 
                int [], float, int [], int, int, int, float [], 
                float [], float [], int *);
float *c_csa3ls(int, float [], float [], float [], float [],
                int [], int, float [], float [], float[], int *);
float *c_csa3lxs(int, float [], float [], float [], float [],
                 float [], int [], float, int [],
                 int, float [], float [], float [], int *);

/*
 *  Fortran function macro.  This macro is used to provide the appropriate
 *  system-specific C function name for it to be Fortran callable.
 */
#ifndef NGCALLF
 
#ifdef  UNICOS
#define NGCALLF(reg,caps)       caps
 
#elif   defined(RS6000) || defined(__hpux)
#define NGCALLF(reg,caps)       reg
 
#else
#ifdef  __STDC__
#define NGCALLF(reg,caps)       reg##_
#else
#define NGCALLF(reg,caps)       reg/**/_
 
#endif  /* __STDC__ */
#endif  /* UNICOS else ... */
#endif  /* NGCALLF */

