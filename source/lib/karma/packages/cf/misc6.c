/*LINTLIBRARY*/
/*  misc6.c

    This code provides simple colourmap generation routines.

    Copyright (C) 1994  Richard Gooch

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Richard Gooch may be reached by email at  karma-request@atnf.csiro.au
    The postal address is:
      Richard Gooch, c/o ATNF, P. O. Box 76, Epping, N.S.W., 2121, Australia.
*/

/*

    This file contains the various utility routines for writing simple colour
    values.


    Written by      Richard Gooch   5-MAY-1994: Copied from  cmap.c

    Updated by      Richard Gooch   5-MAY-1994

    Last updated by Richard Gooch   26-NOV-1994: Moved to  packages/cf/misc6.c


*/

#include <stdio.h>
#include <math.h>
#include <karma_cf.h>
#include <karma_a.h>
#include <karma_n.h>
#include <karma.h>

#define MAX_INTENSITY 65535

/*  Colourmaps submitted by Jeanne  */
#define LUT_SIZE      256

typedef struct colour_struct
{
    float   red;
    float   green;
    float   blue;
} Colour_struct;

static Colour_struct mirp[LUT_SIZE] =
{
     0.000000, 0.000000, 1.000000,
     0.000000, 0.060295, 1.000000,
     0.000000, 0.112233, 1.000000,
     0.000000, 0.157963, 1.000000,
     0.000000, 0.198902, 1.000000,
     0.000000, 0.236033, 1.000000,
     0.000000, 0.270066, 1.000000,
     0.000000, 0.301531, 1.000000,
     0.000000, 0.330834, 1.000000,
     0.000000, 0.358294, 1.000000,
     0.000000, 0.384164, 1.000000,
     0.000000, 0.408651, 1.000000,
     0.000000, 0.431924, 1.000000,
     0.000000, 0.454124, 1.000000,
     0.000000, 0.475368, 1.000000,
     0.000000, 0.495760, 1.000000,
     0.000000, 0.515384, 1.000000,
     0.000000, 0.534315, 1.000000,
     0.000000, 0.552619, 1.000000,
     0.000000, 0.570353, 1.000000,
     0.000000, 0.587566, 1.000000,
     0.000000, 0.604304, 1.000000,
     0.000000, 0.620605, 1.000000,
     0.000000, 0.636507, 1.000000,
     0.000000, 0.652040, 1.000000,
     0.000000, 0.667234, 1.000000,
     0.000000, 0.682114, 1.000000,
     0.000000, 0.696706, 1.000000,
     0.000000, 0.711030, 1.000000,
     0.000000, 0.725108, 1.000000,
     0.000000, 0.738957, 1.000000,
     0.000000, 0.752595, 1.000000,
     0.000000, 0.766038, 1.000000,
     0.000000, 0.779300, 1.000000,
     0.000000, 0.792396, 1.000000,
     0.000000, 0.805338, 1.000000,
     0.000000, 0.818139, 1.000000,
     0.000000, 0.830811, 1.000000,
     0.000000, 0.843364, 1.000000,
     0.000000, 0.855809, 1.000000,
     0.000000, 0.868156, 1.000000,
     0.000000, 0.880414, 1.000000,
     0.000000, 0.892592, 1.000000,
     0.000000, 0.904700, 1.000000,
     0.000000, 0.916746, 1.000000,
     0.000000, 0.928738, 1.000000,
     0.000000, 0.940684, 1.000000,
     0.000000, 0.952592, 1.000000,
     0.000000, 0.964470, 1.000000,
     0.000000, 0.976326, 1.000000,
     0.000000, 0.988167, 1.000000,
     0.000000, 1.000000, 1.000000,
     0.000000, 1.000000, 0.988167,
     0.000000, 1.000000, 0.976326,
     0.000000, 1.000000, 0.964470,
     0.000000, 1.000000, 0.952592,
     0.000000, 1.000000, 0.940684,
     0.000000, 1.000000, 0.928738,
     0.000000, 1.000000, 0.916746,
     0.000000, 1.000000, 0.904700,
     0.000000, 1.000000, 0.892592,
     0.000000, 1.000000, 0.880414,
     0.000000, 1.000000, 0.868156,
     0.000000, 1.000000, 0.855809,
     0.000000, 1.000000, 0.843364,
     0.000000, 1.000000, 0.830811,
     0.000000, 1.000000, 0.818139,
     0.000000, 1.000000, 0.805338,
     0.000000, 1.000000, 0.792396,
     0.000000, 1.000000, 0.779300,
     0.000000, 1.000000, 0.766038,
     0.000000, 1.000000, 0.752595,
     0.000000, 1.000000, 0.738957,
     0.000000, 1.000000, 0.725108,
     0.000000, 1.000000, 0.711030,
     0.000000, 1.000000, 0.696706,
     0.000000, 1.000000, 0.682114,
     0.000000, 1.000000, 0.667234,
     0.000000, 1.000000, 0.652040,
     0.000000, 1.000000, 0.636507,
     0.000000, 1.000000, 0.620605,
     0.000000, 1.000000, 0.604304,
     0.000000, 1.000000, 0.587566,
     0.000000, 1.000000, 0.570353,
     0.000000, 1.000000, 0.552619,
     0.000000, 1.000000, 0.534315,
     0.000000, 1.000000, 0.515384,
     0.000000, 1.000000, 0.495760,
     0.000000, 1.000000, 0.475368,
     0.000000, 1.000000, 0.454124,
     0.000000, 1.000000, 0.431924,
     0.000000, 1.000000, 0.408651,
     0.000000, 1.000000, 0.384164,
     0.000000, 1.000000, 0.358294,
     0.000000, 1.000000, 0.330834,
     0.000000, 1.000000, 0.301531,
     0.000000, 1.000000, 0.270066,
     0.000000, 1.000000, 0.236033,
     0.000000, 1.000000, 0.198902,
     0.000000, 1.000000, 0.157963,
     0.000000, 1.000000, 0.112233,
     0.000000, 1.000000, 0.060295,
     0.000000, 1.000000, 0.000000,
     0.060295, 1.000000, 0.000000,
     0.112233, 1.000000, 0.000000,
     0.157963, 1.000000, 0.000000,
     0.198902, 1.000000, 0.000000,
     0.236033, 1.000000, 0.000000,
     0.270066, 1.000000, 0.000000,
     0.301531, 1.000000, 0.000000,
     0.330834, 1.000000, 0.000000,
     0.358294, 1.000000, 0.000000,
     0.384164, 1.000000, 0.000000,
     0.408651, 1.000000, 0.000000,
     0.431924, 1.000000, 0.000000,
     0.454124, 1.000000, 0.000000,
     0.475368, 1.000000, 0.000000,
     0.495760, 1.000000, 0.000000,
     0.515384, 1.000000, 0.000000,
     0.534315, 1.000000, 0.000000,
     0.552619, 1.000000, 0.000000,
     0.570353, 1.000000, 0.000000,
     0.587566, 1.000000, 0.000000,
     0.604304, 1.000000, 0.000000,
     0.620605, 1.000000, 0.000000,
     0.636507, 1.000000, 0.000000,
     0.652040, 1.000000, 0.000000,
     0.667234, 1.000000, 0.000000,
     0.682114, 1.000000, 0.000000,
     0.696706, 1.000000, 0.000000,
     0.711030, 1.000000, 0.000000,
     0.725108, 1.000000, 0.000000,
     0.738957, 1.000000, 0.000000,
     0.752595, 1.000000, 0.000000,
     0.766038, 1.000000, 0.000000,
     0.779300, 1.000000, 0.000000,
     0.792396, 1.000000, 0.000000,
     0.805338, 1.000000, 0.000000,
     0.818139, 1.000000, 0.000000,
     0.830811, 1.000000, 0.000000,
     0.843364, 1.000000, 0.000000,
     0.855809, 1.000000, 0.000000,
     0.868156, 1.000000, 0.000000,
     0.880414, 1.000000, 0.000000,
     0.892592, 1.000000, 0.000000,
     0.904700, 1.000000, 0.000000,
     0.916746, 1.000000, 0.000000,
     0.928738, 1.000000, 0.000000,
     0.940684, 1.000000, 0.000000,
     0.952592, 1.000000, 0.000000,
     0.964470, 1.000000, 0.000000,
     0.976326, 1.000000, 0.000000,
     0.988167, 1.000000, 0.000000,
     1.000000, 1.000000, 0.000000,
     1.000000, 0.988167, 0.000000,
     1.000000, 0.976326, 0.000000,
     1.000000, 0.964470, 0.000000,
     1.000000, 0.952592, 0.000000,
     1.000000, 0.940684, 0.000000,
     1.000000, 0.928738, 0.000000,
     1.000000, 0.916746, 0.000000,
     1.000000, 0.904700, 0.000000,
     1.000000, 0.892592, 0.000000,
     1.000000, 0.880414, 0.000000,
     1.000000, 0.868156, 0.000000,
     1.000000, 0.855809, 0.000000,
     1.000000, 0.843364, 0.000000,
     1.000000, 0.830811, 0.000000,
     1.000000, 0.818139, 0.000000,
     1.000000, 0.805338, 0.000000,
     1.000000, 0.792396, 0.000000,
     1.000000, 0.779300, 0.000000,
     1.000000, 0.766038, 0.000000,
     1.000000, 0.752595, 0.000000,
     1.000000, 0.738957, 0.000000,
     1.000000, 0.725108, 0.000000,
     1.000000, 0.711030, 0.000000,
     1.000000, 0.696706, 0.000000,
     1.000000, 0.682114, 0.000000,
     1.000000, 0.667234, 0.000000,
     1.000000, 0.652040, 0.000000,
     1.000000, 0.636507, 0.000000,
     1.000000, 0.620605, 0.000000,
     1.000000, 0.604304, 0.000000,
     1.000000, 0.587566, 0.000000,
     1.000000, 0.570353, 0.000000,
     1.000000, 0.552619, 0.000000,
     1.000000, 0.534315, 0.000000,
     1.000000, 0.515384, 0.000000,
     1.000000, 0.495760, 0.000000,
     1.000000, 0.475368, 0.000000,
     1.000000, 0.454124, 0.000000,
     1.000000, 0.431924, 0.000000,
     1.000000, 0.408651, 0.000000,
     1.000000, 0.384164, 0.000000,
     1.000000, 0.358294, 0.000000,
     1.000000, 0.330834, 0.000000,
     1.000000, 0.301531, 0.000000,
     1.000000, 0.270066, 0.000000,
     1.000000, 0.236033, 0.000000,
     1.000000, 0.198902, 0.000000,
     1.000000, 0.157963, 0.000000,
     1.000000, 0.112233, 0.000000,
     1.000000, 0.060295, 0.000000,
     1.000000, 0.000000, 0.000000,
     1.000000, 0.000000, 0.060295,
     1.000000, 0.000000, 0.112233,
     1.000000, 0.000000, 0.157963,
     1.000000, 0.000000, 0.198902,
     1.000000, 0.000000, 0.236033,
     1.000000, 0.000000, 0.270066,
     1.000000, 0.000000, 0.301531,
     1.000000, 0.000000, 0.330834,
     1.000000, 0.000000, 0.358294,
     1.000000, 0.000000, 0.384164,
     1.000000, 0.000000, 0.408651,
     1.000000, 0.000000, 0.431924,
     1.000000, 0.000000, 0.454124,
     1.000000, 0.000000, 0.475368,
     1.000000, 0.000000, 0.495760,
     1.000000, 0.000000, 0.515384,
     1.000000, 0.000000, 0.534315,
     1.000000, 0.000000, 0.552619,
     1.000000, 0.000000, 0.570353,
     1.000000, 0.000000, 0.587566,
     1.000000, 0.000000, 0.604304,
     1.000000, 0.000000, 0.620605,
     1.000000, 0.000000, 0.636507,
     1.000000, 0.000000, 0.652040,
     1.000000, 0.000000, 0.667234,
     1.000000, 0.000000, 0.682114,
     1.000000, 0.000000, 0.696706,
     1.000000, 0.000000, 0.711030,
     1.000000, 0.000000, 0.725108,
     1.000000, 0.000000, 0.738957,
     1.000000, 0.000000, 0.752595,
     1.000000, 0.000000, 0.766038,
     1.000000, 0.000000, 0.779300,
     1.000000, 0.000000, 0.792396,
     1.000000, 0.000000, 0.805338,
     1.000000, 0.000000, 0.818139,
     1.000000, 0.000000, 0.830811,
     1.000000, 0.000000, 0.843364,
     1.000000, 0.000000, 0.855809,
     1.000000, 0.000000, 0.868156,
     1.000000, 0.000000, 0.880414,
     1.000000, 0.000000, 0.892592,
     1.000000, 0.000000, 0.904700,
     1.000000, 0.000000, 0.916746,
     1.000000, 0.000000, 0.928738,
     1.000000, 0.000000, 0.940684,
     1.000000, 0.000000, 0.952592,
     1.000000, 0.000000, 0.964470,
     1.000000, 0.000000, 0.976326,
     1.000000, 0.000000, 0.988167,
     1.000000, 0.000000, 1.000000,
};

static void colour_table_mirp (num_cells, reds, greens, blues, stride, x, y,
			       lut)
unsigned int      num_cells;
unsigned short    *reds;
unsigned short    *greens;
unsigned short    *blues;
unsigned int      stride;
double            x;
double            y;
Colour_struct      lut[];
{

    unsigned int      pixel_count;
    int               m;
    double            shift, slope, xx, yy;

    if ((x < 0.0) || (x > 1.0) || (y < 0.0) || (y > 1.0))
    {
	shift = 0.5;
	slope = 1.0;
    }
    else {
	shift = x;
	slope = tan(PI_ON_2*y);
    }


    for (pixel_count = 0;  pixel_count < num_cells ; pixel_count++)
    {
	xx = 1.0 / (float) (num_cells-1) * (pixel_count);
	yy = slope * (xx-shift) + 0.5;
	if (yy < 0.0)
	{
	    yy = 0.0;
	} else
	{
	    if (yy > 1.0)
	    {
		yy = 1.0;
	    }
	}

	m = ( (float)(LUT_SIZE-1) * yy + 0.5 );
	if (m >= LUT_SIZE) 
	m = LUT_SIZE-1;
	reds[pixel_count * stride]    = lut[m].red*MAX_INTENSITY;
	blues[pixel_count * stride]   = lut[m].blue*MAX_INTENSITY;
	greens[pixel_count * stride]  = lut[m].green*MAX_INTENSITY;
    }
}

/*PUBLIC_FUNCTION*/
void cf_mirp (num_cells, reds, greens, blues, stride, x, y, var_param)
unsigned int          num_cells;
unsigned short        *reds;
unsigned short        *greens;
unsigned short        *blues;
unsigned int          stride;
double                x;
double                y;
void                  *var_param;
{
    extern Colour_struct   mirp[];

    colour_table_mirp (num_cells, reds, greens, blues, stride, x, y, mirp);
}
