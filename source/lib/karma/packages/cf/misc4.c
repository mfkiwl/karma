/*LINTLIBRARY*/
/*  misc4.c

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

    Last updated by Richard Gooch   26-NOV-1994: Moved to  packages/cf/misc4.c


*/

#include <stdio.h>
#include <math.h>
#include <karma_cf.h>
#include <karma_a.h>
#include <karma_n.h>
#include <karma.h>

#define MAX_INTENSITY 65535

/*  Colourmaps submitted by Tom  */
#define LUT_SIZE      256

typedef struct colour_struct
{
    float   red;
    float   green;
    float   blue;
} Colour_struct;

static void colour_table (num_cells, reds, greens, blues, stride, x, y, lut)
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
	slope = tan(1.57*y);
    }


    for (pixel_count = 1;  pixel_count <= num_cells ; pixel_count++)
    {
	xx = 1.0 / (float) (num_cells+1) * (pixel_count);
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

	m = ( (float) (LUT_SIZE-2) * yy + 1.5 );
	if (m >= LUT_SIZE) 
	m = LUT_SIZE-1;
	reds[(pixel_count-1) * stride]    = lut[m].red*MAX_INTENSITY;
	blues[(pixel_count-1) * stride]   = lut[m].blue*MAX_INTENSITY;
	greens[(pixel_count-1) * stride]  = lut[m].green*MAX_INTENSITY;
    }
}

static Colour_struct random_colour_table[LUT_SIZE] =
{
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.47059, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.62745, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 0.78431, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 1.00000, 0.00392 ,
    0.00392, 0.86275, 0.47059 ,
    0.00392, 0.86275, 0.47059 ,
    0.00392, 0.86275, 0.47059 ,
    0.00392, 0.86275, 0.47059 ,
    0.00392, 0.86275, 0.47059 ,
    0.00392, 0.86275, 0.47059 ,
    0.00392, 0.86275, 0.47059 ,
    0.00392, 0.86275, 0.47059 ,
    0.00392, 0.86275, 0.47059 ,
    0.00392, 0.86275, 0.47059 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.78431, 0.62745 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.70588, 0.78431 ,
    0.00000, 0.62745, 1.00000 ,
    0.00000, 0.62745, 1.00000 ,
    0.00000, 0.62745, 1.00000 ,
    0.00000, 0.62745, 1.00000 ,
    0.00000, 0.62745, 1.00000 ,
    0.00000, 0.62745, 1.00000 ,
    0.00000, 0.62745, 1.00000 ,
    0.00000, 0.62745, 1.00000 ,
    0.00000, 0.62745, 1.00000 ,
    0.00000, 0.62745, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.47059, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.23529, 0.00392, 1.00000 ,
    0.47059, 0.00392, 0.78431 ,
    0.47059, 0.00392, 0.78431 ,
    0.47059, 0.00392, 0.78431 ,
    0.47059, 0.00392, 0.78431 ,
    0.47059, 0.00392, 0.78431 ,
    0.47059, 0.00392, 0.78431 ,
    0.47059, 0.00392, 0.78431 ,
    0.47059, 0.00392, 0.78431 ,
    0.47059, 0.00392, 0.78431 ,
    0.47059, 0.00392, 0.78431 ,
    0.62745, 0.00392, 0.62745 ,
    0.62745, 0.00392, 0.62745 ,
    0.62745, 0.00392, 0.62745 ,
    0.62745, 0.00392, 0.62745 ,
    0.62745, 0.00392, 0.62745 ,
    0.62745, 0.00392, 0.62745 ,
    0.62745, 0.00392, 0.62745 ,
    0.62745, 0.00392, 0.62745 ,
    0.62745, 0.00392, 0.62745 ,
    0.62745, 0.00392, 0.62745 ,
    0.78431, 0.00392, 0.47059 ,
    0.78431, 0.00392, 0.47059 ,
    0.78431, 0.00392, 0.47059 ,
    0.78431, 0.00392, 0.47059 ,
    0.78431, 0.00392, 0.47059 ,
    0.78431, 0.00392, 0.47059 ,
    0.78431, 0.00392, 0.47059 ,
    0.78431, 0.00392, 0.47059 ,
    0.78431, 0.00392, 0.47059 ,
    0.78431, 0.00392, 0.47059 ,
    0.90196, 0.11765, 0.23529 ,
    0.90196, 0.11765, 0.23529 ,
    0.90196, 0.11765, 0.23529 ,
    0.90196, 0.11765, 0.23529 ,
    0.90196, 0.11765, 0.23529 ,
    0.90196, 0.11765, 0.23529 ,
    0.90196, 0.11765, 0.23529 ,
    0.90196, 0.11765, 0.23529 ,
    0.90196, 0.11765, 0.23529 ,
    0.90196, 0.11765, 0.23529 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.23529, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    1.00000, 0.47059, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.98039, 0.47059 ,
    0.98039, 0.98039, 0.47059 ,
    0.98039, 0.98039, 0.47059 ,
    0.98039, 0.98039, 0.47059 ,
    0.98039, 0.98039, 0.47059 ,
    0.98039, 0.98039, 0.47059 ,
    0.98039, 0.98039, 0.47059 ,
    0.98039, 0.98039, 0.47059 ,
    0.98039, 0.98039, 0.47059 ,
    0.98039, 0.98039, 0.47059 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
    1.00000, 1.00000, 1.00000 ,
};

static Colour_struct rgb[LUT_SIZE] =
{
    0.00000, 0.00000, 0.00000 ,
    0.01176, 0.00000, 0.00000 ,
    0.02745, 0.00000, 0.00000 ,
    0.04314, 0.00000, 0.00000 ,
    0.05882, 0.00000, 0.00000 ,
    0.07451, 0.00000, 0.00000 ,
    0.08627, 0.00000, 0.00000 ,
    0.10196, 0.00000, 0.00000 ,
    0.11765, 0.00000, 0.00000 ,
    0.13333, 0.00000, 0.00000 ,
    0.14902, 0.00000, 0.00000 ,
    0.16078, 0.00000, 0.00000 ,
    0.17647, 0.00000, 0.00000 ,
    0.19216, 0.00000, 0.00000 ,
    0.20784, 0.00000, 0.00000 ,
    0.22353, 0.00000, 0.00000 ,
    0.23529, 0.00000, 0.00000 ,
    0.25098, 0.00000, 0.00000 ,
    0.26667, 0.00000, 0.00000 ,
    0.28235, 0.00000, 0.00000 ,
    0.29804, 0.00000, 0.00000 ,
    0.30980, 0.00000, 0.00000 ,
    0.32549, 0.00000, 0.00000 ,
    0.34118, 0.00000, 0.00000 ,
    0.35686, 0.00000, 0.00000 ,
    0.37255, 0.00000, 0.00000 ,
    0.38431, 0.00000, 0.00000 ,
    0.40000, 0.00000, 0.00000 ,
    0.41569, 0.00000, 0.00000 ,
    0.43137, 0.00000, 0.00000 ,
    0.44706, 0.00000, 0.00000 ,
    0.45882, 0.00000, 0.00000 ,
    0.47451, 0.00000, 0.00000 ,
    0.49020, 0.00000, 0.00000 ,
    0.50588, 0.00000, 0.00000 ,
    0.52157, 0.00000, 0.00000 ,
    0.53725, 0.00000, 0.00000 ,
    0.54902, 0.00000, 0.00000 ,
    0.56471, 0.00000, 0.00000 ,
    0.58039, 0.00000, 0.00000 ,
    0.59608, 0.00000, 0.00000 ,
    0.61176, 0.00000, 0.00000 ,
    0.62353, 0.00000, 0.00000 ,
    0.63922, 0.00000, 0.00000 ,
    0.65490, 0.00000, 0.00000 ,
    0.67059, 0.00000, 0.00000 ,
    0.68627, 0.00000, 0.00000 ,
    0.69804, 0.00000, 0.00000 ,
    0.71373, 0.00000, 0.00000 ,
    0.72941, 0.00000, 0.00000 ,
    0.74510, 0.00000, 0.00000 ,
    0.76078, 0.00000, 0.00000 ,
    0.77255, 0.00000, 0.00000 ,
    0.78824, 0.00000, 0.00000 ,
    0.80392, 0.00000, 0.00000 ,
    0.81961, 0.00000, 0.00000 ,
    0.83529, 0.00000, 0.00000 ,
    0.84706, 0.00000, 0.00000 ,
    0.86275, 0.00000, 0.00000 ,
    0.87843, 0.00000, 0.00000 ,
    0.89412, 0.00000, 0.00000 ,
    0.90980, 0.00000, 0.00000 ,
    0.92157, 0.00000, 0.00000 ,
    0.93725, 0.00000, 0.00000 ,
    0.95294, 0.00000, 0.00000 ,
    0.96863, 0.01176, 0.00000 ,
    0.98431, 0.02745, 0.00000 ,
    1.00000, 0.04314, 0.00000 ,
    0.98431, 0.05882, 0.00000 ,
    0.96863, 0.07451, 0.00000 ,
    0.95294, 0.09020, 0.00000 ,
    0.93725, 0.10588, 0.00000 ,
    0.92157, 0.12157, 0.00000 ,
    0.90196, 0.13725, 0.00000 ,
    0.88627, 0.15294, 0.00000 ,
    0.87059, 0.16863, 0.00000 ,
    0.85490, 0.18431, 0.00000 ,
    0.83922, 0.20000, 0.00000 ,
    0.82353, 0.21569, 0.00000 ,
    0.80392, 0.23137, 0.00000 ,
    0.78824, 0.24706, 0.00000 ,
    0.77255, 0.26275, 0.00000 ,
    0.75686, 0.27843, 0.00000 ,
    0.74118, 0.29412, 0.00000 ,
    0.72157, 0.30980, 0.00000 ,
    0.70588, 0.32549, 0.00000 ,
    0.69020, 0.34118, 0.00000 ,
    0.67451, 0.35686, 0.00000 ,
    0.65882, 0.37255, 0.00000 ,
    0.64314, 0.38824, 0.00000 ,
    0.62353, 0.40392, 0.00000 ,
    0.60784, 0.41961, 0.00000 ,
    0.59216, 0.43529, 0.00000 ,
    0.57647, 0.45098, 0.00000 ,
    0.56078, 0.46667, 0.00000 ,
    0.54118, 0.48235, 0.00000 ,
    0.52549, 0.49804, 0.00000 ,
    0.50980, 0.51373, 0.00000 ,
    0.49412, 0.52941, 0.00000 ,
    0.47843, 0.54510, 0.00000 ,
    0.46275, 0.56078, 0.00000 ,
    0.44314, 0.57647, 0.00000 ,
    0.42745, 0.59216, 0.00000 ,
    0.41176, 0.60784, 0.00000 ,
    0.39608, 0.62353, 0.00000 ,
    0.38039, 0.63922, 0.00000 ,
    0.36078, 0.65490, 0.00000 ,
    0.34510, 0.67059, 0.00000 ,
    0.32941, 0.68627, 0.00000 ,
    0.31373, 0.70196, 0.00000 ,
    0.29804, 0.71765, 0.00000 ,
    0.28235, 0.73333, 0.00000 ,
    0.26275, 0.74902, 0.00000 ,
    0.24706, 0.76471, 0.00000 ,
    0.23137, 0.78039, 0.00000 ,
    0.21569, 0.79608, 0.00000 ,
    0.20000, 0.81176, 0.00000 ,
    0.18039, 0.82745, 0.00000 ,
    0.16471, 0.84314, 0.00000 ,
    0.14902, 0.85882, 0.00000 ,
    0.13333, 0.87451, 0.00000 ,
    0.11765, 0.89020, 0.00000 ,
    0.10196, 0.90588, 0.00000 ,
    0.08235, 0.92157, 0.00000 ,
    0.06667, 0.93725, 0.00000 ,
    0.05098, 0.95294, 0.00000 ,
    0.03529, 0.96863, 0.00000 ,
    0.01961, 0.98431, 0.01176 ,
    0.00000, 1.00000, 0.02745 ,
    0.00000, 0.98431, 0.04314 ,
    0.00000, 0.96863, 0.05882 ,
    0.00000, 0.95294, 0.07451 ,
    0.00000, 0.93725, 0.09020 ,
    0.00000, 0.92157, 0.10588 ,
    0.00000, 0.90588, 0.11765 ,
    0.00000, 0.89020, 0.13333 ,
    0.00000, 0.87451, 0.14902 ,
    0.00000, 0.85882, 0.16471 ,
    0.00000, 0.84314, 0.18039 ,
    0.00000, 0.82745, 0.19608 ,
    0.00000, 0.81176, 0.21176 ,
    0.00000, 0.79608, 0.22353 ,
    0.00000, 0.78039, 0.23922 ,
    0.00000, 0.76471, 0.25490 ,
    0.00000, 0.74902, 0.27059 ,
    0.00000, 0.73333, 0.28627 ,
    0.00000, 0.71765, 0.30196 ,
    0.00000, 0.70196, 0.31765 ,
    0.00000, 0.68627, 0.33333 ,
    0.00000, 0.66667, 0.34510 ,
    0.00000, 0.65098, 0.36078 ,
    0.00000, 0.63529, 0.37647 ,
    0.00000, 0.61961, 0.39216 ,
    0.00000, 0.60392, 0.40784 ,
    0.00000, 0.58824, 0.42353 ,
    0.00000, 0.57255, 0.43922 ,
    0.00000, 0.55686, 0.45098 ,
    0.00000, 0.54118, 0.46667 ,
    0.00000, 0.52549, 0.48235 ,
    0.00000, 0.50980, 0.49804 ,
    0.00000, 0.49412, 0.51373 ,
    0.00000, 0.47843, 0.52941 ,
    0.00000, 0.46275, 0.54510 ,
    0.00000, 0.44706, 0.55686 ,
    0.00000, 0.43137, 0.57255 ,
    0.00000, 0.41569, 0.58824 ,
    0.00000, 0.40000, 0.60392 ,
    0.00000, 0.38431, 0.61961 ,
    0.00000, 0.36863, 0.63529 ,
    0.00000, 0.35294, 0.65098 ,
    0.00000, 0.33333, 0.66667 ,
    0.00000, 0.31765, 0.67843 ,
    0.00000, 0.30196, 0.69412 ,
    0.00000, 0.28627, 0.70980 ,
    0.00000, 0.27059, 0.72549 ,
    0.00000, 0.25490, 0.74118 ,
    0.00000, 0.23922, 0.75686 ,
    0.00000, 0.22353, 0.77255 ,
    0.00000, 0.20784, 0.78431 ,
    0.00000, 0.19216, 0.80000 ,
    0.00000, 0.17647, 0.81569 ,
    0.00000, 0.16078, 0.83137 ,
    0.00000, 0.14510, 0.84706 ,
    0.00000, 0.12941, 0.86275 ,
    0.00000, 0.11373, 0.87843 ,
    0.00000, 0.09804, 0.89020 ,
    0.00000, 0.08235, 0.90588 ,
    0.00000, 0.06667, 0.92157 ,
    0.00000, 0.05098, 0.93725 ,
    0.00000, 0.03529, 0.95294 ,
    0.00000, 0.01961, 0.96863 ,
    0.00000, 0.00000, 0.98431 ,
    0.00000, 0.00000, 1.00000 ,
    0.00000, 0.00000, 0.98431 ,
    0.00000, 0.00000, 0.96863 ,
    0.00000, 0.00000, 0.95294 ,
    0.00000, 0.00000, 0.93725 ,
    0.00000, 0.00000, 0.92157 ,
    0.00000, 0.00000, 0.90588 ,
    0.00000, 0.00000, 0.89020 ,
    0.00000, 0.00000, 0.87451 ,
    0.00000, 0.00000, 0.85882 ,
    0.00000, 0.00000, 0.84314 ,
    0.00000, 0.00000, 0.82745 ,
    0.00000, 0.00000, 0.81176 ,
    0.00000, 0.00000, 0.79608 ,
    0.00000, 0.00000, 0.78039 ,
    0.00000, 0.00000, 0.76471 ,
    0.00000, 0.00000, 0.74902 ,
    0.00000, 0.00000, 0.73333 ,
    0.00000, 0.00000, 0.71765 ,
    0.00000, 0.00000, 0.70196 ,
    0.00000, 0.00000, 0.68627 ,
    0.00000, 0.00000, 0.66667 ,
    0.00000, 0.00000, 0.65098 ,
    0.00000, 0.00000, 0.63529 ,
    0.00000, 0.00000, 0.61961 ,
    0.00000, 0.00000, 0.60392 ,
    0.00000, 0.00000, 0.58824 ,
    0.00000, 0.00000, 0.57255 ,
    0.00000, 0.00000, 0.55686 ,
    0.00000, 0.00000, 0.54118 ,
    0.00000, 0.00000, 0.52549 ,
    0.00000, 0.00000, 0.50980 ,
    0.00000, 0.00000, 0.49412 ,
    0.00000, 0.00000, 0.47843 ,
    0.00000, 0.00000, 0.46275 ,
    0.00000, 0.00000, 0.44706 ,
    0.00000, 0.00000, 0.43137 ,
    0.00000, 0.00000, 0.41569 ,
    0.00000, 0.00000, 0.40000 ,
    0.00000, 0.00000, 0.38431 ,
    0.00000, 0.00000, 0.36863 ,
    0.00000, 0.00000, 0.35294 ,
    0.00000, 0.00000, 0.33333 ,
    0.00000, 0.00000, 0.31765 ,
    0.00000, 0.00000, 0.30196 ,
    0.00000, 0.00000, 0.28627 ,
    0.00000, 0.00000, 0.27059 ,
    0.00000, 0.00000, 0.25490 ,
    0.00000, 0.00000, 0.23922 ,
    0.00000, 0.00000, 0.22353 ,
    0.00000, 0.00000, 0.20784 ,
    0.00000, 0.00000, 0.19216 ,
    0.00000, 0.00000, 0.17647 ,
    0.00000, 0.00000, 0.16078 ,
    0.00000, 0.00000, 0.14510 ,
    0.00000, 0.00000, 0.12941 ,
    0.00000, 0.00000, 0.11373 ,
    0.00000, 0.00000, 0.09804 ,
    0.00000, 0.00000, 0.08235 ,
    0.00000, 0.00000, 0.06667 ,
    0.00000, 0.00000, 0.05098 ,
    0.00000, 0.00000, 0.03529 ,
    0.00000, 0.00000, 0.01961 ,
    0.00000, 0.00000, 0.00000 ,
};

static Colour_struct smooth[LUT_SIZE] =
{
    0.00000, 0.00000, 1.00000 ,
    0.01569, 0.00000, 0.98431 ,
    0.03529, 0.00000, 0.96471 ,
    0.05098, 0.00000, 0.94902 ,
    0.06667, 0.00000, 0.93333 ,
    0.08627, 0.00000, 0.91373 ,
    0.10196, 0.00000, 0.89804 ,
    0.11765, 0.00000, 0.88235 ,
    0.13725, 0.00000, 0.86275 ,
    0.15294, 0.00000, 0.84706 ,
    0.16863, 0.00000, 0.83137 ,
    0.18824, 0.00000, 0.81176 ,
    0.20392, 0.00000, 0.79608 ,
    0.21961, 0.00000, 0.78039 ,
    0.23922, 0.00000, 0.76078 ,
    0.25490, 0.00000, 0.74510 ,
    0.27059, 0.00000, 0.72941 ,
    0.28627, 0.00000, 0.71373 ,
    0.30588, 0.00000, 0.69412 ,
    0.32157, 0.00000, 0.67843 ,
    0.33725, 0.00000, 0.66275 ,
    0.35686, 0.00000, 0.64314 ,
    0.37255, 0.00000, 0.62745 ,
    0.38824, 0.00000, 0.61176 ,
    0.40784, 0.00000, 0.59216 ,
    0.42353, 0.00000, 0.57647 ,
    0.43922, 0.00000, 0.56078 ,
    0.45882, 0.00000, 0.54118 ,
    0.47451, 0.00000, 0.52549 ,
    0.49020, 0.00000, 0.50980 ,
    0.50980, 0.00000, 0.49020 ,
    0.52549, 0.00000, 0.47451 ,
    0.54118, 0.00000, 0.45882 ,
    0.56078, 0.00000, 0.43922 ,
    0.57647, 0.00000, 0.42353 ,
    0.59216, 0.00000, 0.40784 ,
    0.61176, 0.00000, 0.38824 ,
    0.62745, 0.00000, 0.37255 ,
    0.64314, 0.00000, 0.35686 ,
    0.66275, 0.00000, 0.33725 ,
    0.67843, 0.00000, 0.32157 ,
    0.69412, 0.00000, 0.30588 ,
    0.71373, 0.00000, 0.28627 ,
    0.72941, 0.00000, 0.27059 ,
    0.74510, 0.00000, 0.25490 ,
    0.76078, 0.00000, 0.23922 ,
    0.78039, 0.00000, 0.21961 ,
    0.79608, 0.00000, 0.20392 ,
    0.81176, 0.00000, 0.18824 ,
    0.83137, 0.00000, 0.16863 ,
    0.84706, 0.00000, 0.15294 ,
    0.86275, 0.00000, 0.13725 ,
    0.88235, 0.00000, 0.11765 ,
    0.89804, 0.00000, 0.10196 ,
    0.91373, 0.00000, 0.08627 ,
    0.93333, 0.00000, 0.06667 ,
    0.94902, 0.00000, 0.05098 ,
    0.96471, 0.00000, 0.03529 ,
    0.98431, 0.00000, 0.01569 ,
    1.00000, 0.00000, 0.00000 ,
    1.00000, 0.00000, 0.00000 ,
    1.00000, 0.01176, 0.00000 ,
    1.00000, 0.01961, 0.00000 ,
    1.00000, 0.03137, 0.00000 ,
    1.00000, 0.03922, 0.00000 ,
    1.00000, 0.05098, 0.00000 ,
    1.00000, 0.05882, 0.00000 ,
    1.00000, 0.07059, 0.00000 ,
    1.00000, 0.08235, 0.00000 ,
    1.00000, 0.09020, 0.00000 ,
    1.00000, 0.10196, 0.00000 ,
    1.00000, 0.10980, 0.00000 ,
    1.00000, 0.12157, 0.00000 ,
    1.00000, 0.12941, 0.00000 ,
    1.00000, 0.14118, 0.00000 ,
    0.99608, 0.15294, 0.00000 ,
    0.99608, 0.16078, 0.00000 ,
    0.99608, 0.17255, 0.00000 ,
    0.99608, 0.18039, 0.00000 ,
    0.99608, 0.19216, 0.00000 ,
    0.99608, 0.20392, 0.00000 ,
    0.99608, 0.21176, 0.00000 ,
    0.99608, 0.22353, 0.00000 ,
    0.99608, 0.23137, 0.00000 ,
    0.99608, 0.24314, 0.00000 ,
    0.99608, 0.25098, 0.00000 ,
    0.99608, 0.26275, 0.00000 ,
    0.99608, 0.27451, 0.00000 ,
    0.99608, 0.28235, 0.00000 ,
    0.99608, 0.29412, 0.00000 ,
    0.99608, 0.30196, 0.00000 ,
    0.99608, 0.31373, 0.00000 ,
    0.99608, 0.32157, 0.00000 ,
    0.99608, 0.33333, 0.00000 ,
    0.99608, 0.34510, 0.00000 ,
    0.99608, 0.35294, 0.00000 ,
    0.99608, 0.36471, 0.00000 ,
    0.99608, 0.37255, 0.00000 ,
    0.99608, 0.38431, 0.00000 ,
    0.99608, 0.39216, 0.00000 ,
    0.99608, 0.40392, 0.00000 ,
    0.99608, 0.41569, 0.00000 ,
    0.99608, 0.42353, 0.00000 ,
    0.99608, 0.43529, 0.00000 ,
    0.99608, 0.44314, 0.00000 ,
    0.99216, 0.45490, 0.00000 ,
    0.99216, 0.46667, 0.00000 ,
    0.99216, 0.47451, 0.00000 ,
    0.99216, 0.48627, 0.00000 ,
    0.99216, 0.49412, 0.00000 ,
    0.99216, 0.50588, 0.00000 ,
    0.99216, 0.51373, 0.00000 ,
    0.99216, 0.52549, 0.00000 ,
    0.99216, 0.53725, 0.00000 ,
    0.99216, 0.54510, 0.00000 ,
    0.99216, 0.55686, 0.00000 ,
    0.99216, 0.56471, 0.00000 ,
    0.99216, 0.57647, 0.00000 ,
    0.99216, 0.58431, 0.00000 ,
    0.99216, 0.59608, 0.00000 ,
    0.99216, 0.60000, 0.00000 ,
    0.99216, 0.60784, 0.00000 ,
    0.99216, 0.61176, 0.00000 ,
    0.99216, 0.61569, 0.00000 ,
    0.99216, 0.61961, 0.00000 ,
    0.99216, 0.62745, 0.00000 ,
    0.99216, 0.63137, 0.00000 ,
    0.99216, 0.63529, 0.00000 ,
    0.99216, 0.64314, 0.00000 ,
    0.98824, 0.64706, 0.00000 ,
    0.98824, 0.65098, 0.00000 ,
    0.98824, 0.65882, 0.00000 ,
    0.98824, 0.66275, 0.00000 ,
    0.98824, 0.66667, 0.00000 ,
    0.98824, 0.67451, 0.00000 ,
    0.98824, 0.67843, 0.00000 ,
    0.98824, 0.68235, 0.00000 ,
    0.98824, 0.68627, 0.00000 ,
    0.98824, 0.69412, 0.00000 ,
    0.98824, 0.69804, 0.00000 ,
    0.98824, 0.70196, 0.00000 ,
    0.98824, 0.70980, 0.00000 ,
    0.98824, 0.71373, 0.00000 ,
    0.98824, 0.71765, 0.00000 ,
    0.98824, 0.72549, 0.00000 ,
    0.98824, 0.72941, 0.00000 ,
    0.98824, 0.73333, 0.00000 ,
    0.98824, 0.73725, 0.00000 ,
    0.98824, 0.74510, 0.00000 ,
    0.98824, 0.74902, 0.00000 ,
    0.98431, 0.75294, 0.00000 ,
    0.98431, 0.76078, 0.00000 ,
    0.98431, 0.76471, 0.00000 ,
    0.98431, 0.76863, 0.00000 ,
    0.98431, 0.77255, 0.00000 ,
    0.98431, 0.78039, 0.00000 ,
    0.98431, 0.78431, 0.00000 ,
    0.98431, 0.78824, 0.00000 ,
    0.98431, 0.79608, 0.00000 ,
    0.98431, 0.80000, 0.00000 ,
    0.98431, 0.80392, 0.00000 ,
    0.98431, 0.81176, 0.00000 ,
    0.98431, 0.81569, 0.00000 ,
    0.98431, 0.81961, 0.00000 ,
    0.98431, 0.82745, 0.00000 ,
    0.98431, 0.83137, 0.00000 ,
    0.98431, 0.83529, 0.00000 ,
    0.98431, 0.83922, 0.00000 ,
    0.98431, 0.84706, 0.00000 ,
    0.98431, 0.85098, 0.00000 ,
    0.98039, 0.85490, 0.00000 ,
    0.98039, 0.86275, 0.00000 ,
    0.98039, 0.86667, 0.00000 ,
    0.98039, 0.87059, 0.00000 ,
    0.98039, 0.87843, 0.00000 ,
    0.98039, 0.88235, 0.00000 ,
    0.98039, 0.88627, 0.00000 ,
    0.98039, 0.89020, 0.00000 ,
    0.98039, 0.89804, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.98039, 0.90196, 0.00000 ,
    0.96471, 0.88627, 0.00000 ,
    0.94902, 0.87059, 0.00000 ,
    0.92941, 0.85490, 0.00000 ,
    0.91373, 0.83922, 0.00000 ,
    0.89804, 0.82745, 0.00000 ,
    0.88235, 0.81176, 0.00000 ,
    0.86275, 0.79608, 0.00000 ,
    0.84706, 0.78039, 0.00000 ,
    0.83137, 0.76471, 0.00000 ,
    0.81569, 0.74902, 0.00000 ,
    0.79608, 0.73333, 0.00000 ,
    0.78039, 0.71765, 0.00000 ,
    0.76471, 0.70196, 0.00000 ,
    0.74902, 0.68627, 0.00000 ,
    0.72941, 0.67451, 0.00000 ,
    0.71373, 0.65882, 0.00000 ,
    0.69804, 0.64314, 0.00000 ,
    0.68235, 0.62745, 0.00000 ,
    0.66275, 0.61176, 0.00000 ,
    0.64706, 0.59608, 0.00000 ,
    0.63137, 0.58039, 0.00000 ,
    0.61569, 0.56471, 0.00000 ,
    0.60000, 0.54902, 0.00000 ,
    0.58039, 0.53333, 0.00000 ,
    0.56471, 0.52157, 0.00000 ,
    0.54902, 0.50588, 0.00000 ,
    0.53333, 0.49020, 0.00000 ,
    0.51373, 0.47451, 0.00000 ,
    0.49804, 0.45882, 0.00000 ,
    0.48235, 0.44314, 0.00000 ,
    0.46667, 0.42745, 0.00000 ,
    0.44706, 0.41176, 0.00000 ,
    0.43137, 0.39608, 0.00000 ,
    0.41569, 0.38039, 0.00000 ,
    0.40000, 0.36863, 0.00000 ,
    0.38039, 0.35294, 0.00000 ,
    0.36471, 0.33725, 0.00000 ,
    0.34902, 0.32157, 0.00000 ,
    0.33333, 0.30588, 0.00000 ,
    0.31765, 0.29020, 0.00000 ,
    0.29804, 0.27451, 0.00000 ,
    0.28235, 0.25882, 0.00000 ,
    0.26667, 0.24314, 0.00000 ,
    0.25098, 0.22745, 0.00000 ,
    0.23137, 0.21569, 0.00000 ,
    0.21569, 0.20000, 0.00000 ,
    0.20000, 0.18431, 0.00000 ,
    0.18431, 0.16863, 0.00000 ,
    0.16471, 0.15294, 0.00000 ,
    0.14902, 0.13725, 0.00000 ,
    0.13333, 0.12157, 0.00000 ,
    0.11765, 0.10588, 0.00000 ,
    0.09804, 0.09020, 0.00000 ,
    0.08235, 0.07451, 0.00000 ,
    0.06667, 0.06275, 0.00000 ,
    0.05098, 0.04706, 0.00000 ,
    0.03137, 0.03137, 0.00000 ,
    0.01569, 0.01569, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
    0.00000, 0.00000, 0.00000 ,
};

/*PUBLIC_FUNCTION*/
void cf_random (num_cells, reds, greens, blues, stride, x, y, var_param)
unsigned int          num_cells;
unsigned short        *reds;
unsigned short        *greens;
unsigned short        *blues;
unsigned int          stride;
double                x;
double                y;
void                  *var_param;
{
    extern Colour_struct   random_colour_table[];

    colour_table (num_cells, reds, greens, blues, stride, x, y,
		  random_colour_table);
}

/*PUBLIC_FUNCTION*/
void cf_rgb (num_cells, reds, greens, blues, stride, x, y, var_param)
unsigned int          num_cells;
unsigned short        *reds;
unsigned short        *greens;
unsigned short        *blues;
unsigned int          stride;
double                x;
double                y;
void                  *var_param;
{
    extern Colour_struct   rgb[];

    colour_table (num_cells, reds, greens, blues, stride, x, y, rgb);
}

/*PUBLIC_FUNCTION*/
void cf_smooth (num_cells, reds, greens, blues, stride, x, y, var_param)
unsigned int          num_cells;
unsigned short        *reds;
unsigned short        *greens;
unsigned short        *blues;
unsigned int          stride;
double                x;
double                y;
void                  *var_param;
{
    extern Colour_struct   smooth[];

    colour_table (num_cells, reds, greens, blues, stride, x, y, smooth);
}
