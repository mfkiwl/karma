/*LINTLIBRARY*/
/*  misc7.c

    This code provides simple colourmap generation routines.

    Copyright (C) 1996  Richard Gooch

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


    Written by      Richard Gooch   1-OCT-1996: Copied from <kubes>.

    Last updated by Richard Gooch   1-OCT-1996


*/

#include <stdio.h>
#include <math.h>
#include <karma_cf.h>
#include <karma_a.h>
#include <karma_n.h>
#include <karma.h>

#define MAX_INTENSITY 65535

/*  Colourmaps submitted by Tom  */
#define LUT_SIZE      126

typedef struct colour_struct
{
    float   red;
    float   green;
    float   blue;
} Colour_struct;


static void colour_table (num_cells, reds, greens, blues, stride, x, y, lut, 
                          lut_size)
unsigned int      num_cells;
unsigned short    *reds;
unsigned short    *greens;
unsigned short    *blues;
unsigned int      stride;
double            x;
double            y;
Colour_struct     lut[];
int               lut_size;
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

	m = ( (float) (lut_size-2) * yy + 1.5 );
	if (m >= lut_size) 
	m = lut_size-1;
	reds[(pixel_count-1) * stride]    = lut[m].red*MAX_INTENSITY;
	blues[(pixel_count-1) * stride]   = lut[m].blue*MAX_INTENSITY;
	greens[(pixel_count-1) * stride]  = lut[m].green*MAX_INTENSITY;
    }
}

static Colour_struct rgb2[LUT_SIZE] =
{
    {0.00000, 0.00000, 1.00000},
    {0.00000, 0.00000, 0.98431},
    {0.00000, 0.01961, 0.96863},
    {0.00000, 0.03529, 0.95294},
    {0.00000, 0.05098, 0.93725},
    {0.00000, 0.06667, 0.92157},
    {0.00000, 0.08235, 0.90588},
    {0.00000, 0.09804, 0.89020},
    {0.00000, 0.11373, 0.87843},
    {0.00000, 0.12941, 0.86275},
    {0.00000, 0.14510, 0.84706},
    {0.00000, 0.16078, 0.83137},
    {0.00000, 0.17647, 0.81569},
    {0.00000, 0.19216, 0.80000},
    {0.00000, 0.20784, 0.78431},
    {0.00000, 0.22353, 0.77255},
    {0.00000, 0.23922, 0.75686},
    {0.00000, 0.25490, 0.74118},
    {0.00000, 0.27059, 0.72549},
    {0.00000, 0.28627, 0.70980},
    {0.00000, 0.30196, 0.69412},
    {0.00000, 0.31765, 0.67843},
    {0.00000, 0.33333, 0.66667},
    {0.00000, 0.35294, 0.65098},
    {0.00000, 0.36863, 0.63529},
    {0.00000, 0.38431, 0.61961},
    {0.00000, 0.40000, 0.60392},
    {0.00000, 0.41569, 0.58824},
    {0.00000, 0.43137, 0.57255},
    {0.00000, 0.44706, 0.55686},
    {0.00000, 0.46275, 0.54510},
    {0.00000, 0.47843, 0.52941},
    {0.00000, 0.49412, 0.51373},
    {0.00000, 0.50980, 0.49804},
    {0.00000, 0.52549, 0.48235},
    {0.00000, 0.54118, 0.46667},
    {0.00000, 0.55686, 0.45098},
    {0.00000, 0.57255, 0.43922},
    {0.00000, 0.58824, 0.42353},
    {0.00000, 0.60392, 0.40784},
    {0.00000, 0.61961, 0.39216},
    {0.00000, 0.63529, 0.37647},
    {0.00000, 0.65098, 0.36078},
    {0.00000, 0.66667, 0.34510},
    {0.00000, 0.68627, 0.33333},
    {0.00000, 0.70196, 0.31765},
    {0.00000, 0.71765, 0.30196},
    {0.00000, 0.73333, 0.28627},
    {0.00000, 0.74902, 0.27059},
    {0.00000, 0.76471, 0.25490},
    {0.00000, 0.78039, 0.23922},
    {0.00000, 0.79608, 0.22353},
    {0.00000, 0.81176, 0.21176},
    {0.00000, 0.82745, 0.19608},
    {0.00000, 0.84314, 0.18039},
    {0.00000, 0.85882, 0.16471},
    {0.00000, 0.87451, 0.14902},
    {0.00000, 0.89020, 0.13333},
    {0.00000, 0.90588, 0.11765},
    {0.00000, 0.92157, 0.10588},
    {0.00000, 0.93725, 0.09020},
    {0.00000, 0.95294, 0.07451},
    {0.00000, 0.96863, 0.05882},
    {0.00000, 0.98431, 0.04314},
    {0.00000, 1.00000, 0.02745},
    {0.01961, 0.98431, 0.01176},
    {0.03529, 0.96863, 0.00000},
    {0.05098, 0.95294, 0.00000},
    {0.06667, 0.93725, 0.00000},
    {0.08235, 0.92157, 0.00000},
    {0.10196, 0.90588, 0.00000},
    {0.11765, 0.89020, 0.00000},
    {0.13333, 0.87451, 0.00000},
    {0.14902, 0.85882, 0.00000},
    {0.16471, 0.84314, 0.00000},
    {0.18039, 0.82745, 0.00000},
    {0.20000, 0.81176, 0.00000},
    {0.21569, 0.79608, 0.00000},
    {0.23137, 0.78039, 0.00000},
    {0.24706, 0.76471, 0.00000},
    {0.26275, 0.74902, 0.00000},
    {0.28235, 0.73333, 0.00000},
    {0.29804, 0.71765, 0.00000},
    {0.31373, 0.70196, 0.00000},
    {0.32941, 0.68627, 0.00000},
    {0.34510, 0.67059, 0.00000},
    {0.36078, 0.65490, 0.00000},
    {0.38039, 0.63922, 0.00000},
    {0.39608, 0.62353, 0.00000},
    {0.41176, 0.60784, 0.00000},
    {0.42745, 0.59216, 0.00000},
    {0.44314, 0.57647, 0.00000},
    {0.46275, 0.56078, 0.00000},
    {0.47843, 0.54510, 0.00000},
    {0.49412, 0.52941, 0.00000},
    {0.50980, 0.51373, 0.00000},
    {0.52549, 0.49804, 0.00000},
    {0.54118, 0.48235, 0.00000},
    {0.56078, 0.46667, 0.00000},
    {0.57647, 0.45098, 0.00000},
    {0.59216, 0.43529, 0.00000},
    {0.60784, 0.41961, 0.00000},
    {0.62353, 0.40392, 0.00000},
    {0.64314, 0.38824, 0.00000},
    {0.65882, 0.37255, 0.00000},
    {0.67451, 0.35686, 0.00000},
    {0.69020, 0.34118, 0.00000},
    {0.70588, 0.32549, 0.00000},
    {0.72157, 0.30980, 0.00000},
    {0.74118, 0.29412, 0.00000},
    {0.75686, 0.27843, 0.00000},
    {0.77255, 0.26275, 0.00000},
    {0.78824, 0.24706, 0.00000},
    {0.80392, 0.23137, 0.00000},
    {0.82353, 0.21569, 0.00000},
    {0.83922, 0.20000, 0.00000},
    {0.85490, 0.18431, 0.00000},
    {0.87059, 0.16863, 0.00000},
    {0.88627, 0.15294, 0.00000},
    {0.90196, 0.13725, 0.00000},
    {0.92157, 0.12157, 0.00000},
    {0.93725, 0.10588, 0.00000},
    {0.95294, 0.09020, 0.00000},
    {0.96863, 0.07451, 0.00000},
    {0.98431, 0.05882, 0.00000},
    {1.00000, 0.04314, 0.00000},
};

/*PUBLIC_FUNCTION*/
void cf_rgb2 (unsigned int num_cells, unsigned short *reds,
	      unsigned short *greens, unsigned short *blues,
	      unsigned int stride, double x, double y, void *var_param)
/*  [SUMMARY] Compute a MIRP colourmap.
    <num_cells> The number of colour cells to modify.
    <reds> The red intensity values.
    <greens> The green intensity values.
    <blues> The blue intensity values.
    <stride> The stride (in unsigned shorts) between intensity values.
    <x> A parameter used to compute the colour values, ranging from 0.0 to 1.0.
    <y> A parameter used to compute the colour values, ranging from 0.0 to 1.0.
    <var_param> A parameter used to compute the colour values. Ignored.
    [RETURNS] Nothing.
*/
{
    extern Colour_struct rgb2[];

    colour_table (num_cells, reds, greens, blues, stride, x, y, rgb2,LUT_SIZE);
    reds[0]    = 0;
    blues[0]   = 0;
    greens[0]  = 0;
}   /*  End Function cf_rbg2  */
