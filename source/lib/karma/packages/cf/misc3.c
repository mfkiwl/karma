/*LINTLIBRARY*/
/*  misc3.c

    This code provides simple colourmap generation routines.

    Copyright (C) 1994-1996  Richard Gooch

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

    Updated by      Richard Gooch   26-NOV-1994: Moved to  packages/cf/misc3.c

    Updated by      Richard Gooch   31-MAR-1996: Changed documentation style.

    Last updated by Richard Gooch   28-APR-1996: Made greyscale functions cope
  with NULL colour arrays.


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
	if (reds != NULL)
	    reds[(pixel_count-1) * stride]   = lut[m].red * MAX_INTENSITY;
	if (greens != NULL)
	    greens[(pixel_count-1) * stride] = lut[m].green * MAX_INTENSITY;
	if (blues != NULL)
	    blues[(pixel_count-1) * stride]  = lut[m].blue * MAX_INTENSITY;
    }
}

static Colour_struct mono[LUT_SIZE] =
{
    {0.00000, 0.00000, 0.00000},
    {0.00392, 0.00392, 0.00392},
    {0.00784, 0.00784, 0.00784},
    {0.01176, 0.01176, 0.01176},
    {0.01569, 0.01569, 0.01569},
    {0.01961, 0.01961, 0.01961},
    {0.02353, 0.02353, 0.02353},
    {0.02745, 0.02745, 0.02745},
    {0.03137, 0.03137, 0.03137},
    {0.03529, 0.03529, 0.03529},
    {0.03922, 0.03922, 0.03922},
    {0.04314, 0.04314, 0.04314},
    {0.04706, 0.04706, 0.04706},
    {0.05098, 0.05098, 0.05098},
    {0.05490, 0.05490, 0.05490},
    {0.05882, 0.05882, 0.05882},
    {0.06275, 0.06275, 0.06275},
    {0.06667, 0.06667, 0.06667},
    {0.07059, 0.07059, 0.07059},
    {0.07451, 0.07451, 0.07451},
    {0.07843, 0.07843, 0.07843},
    {0.08235, 0.08235, 0.08235},
    {0.08627, 0.08627, 0.08627},
    {0.09020, 0.09020, 0.09020},
    {0.09412, 0.09412, 0.09412},
    {0.09804, 0.09804, 0.09804},
    {0.10196, 0.10196, 0.10196},
    {0.10588, 0.10588, 0.10588},
    {0.10980, 0.10980, 0.10980},
    {0.11373, 0.11373, 0.11373},
    {0.11765, 0.11765, 0.11765},
    {0.12157, 0.12157, 0.12157},
    {0.12549, 0.12549, 0.12549},
    {0.12941, 0.12941, 0.12941},
    {0.13333, 0.13333, 0.13333},
    {0.13725, 0.13725, 0.13725},
    {0.14118, 0.14118, 0.14118},
    {0.14510, 0.14510, 0.14510},
    {0.14902, 0.14902, 0.14902},
    {0.15294, 0.15294, 0.15294},
    {0.15686, 0.15686, 0.15686},
    {0.16078, 0.16078, 0.16078},
    {0.16471, 0.16471, 0.16471},
    {0.16863, 0.16863, 0.16863},
    {0.17255, 0.17255, 0.17255},
    {0.17647, 0.17647, 0.17647},
    {0.18039, 0.18039, 0.18039},
    {0.18431, 0.18431, 0.18431},
    {0.18824, 0.18824, 0.18824},
    {0.19216, 0.19216, 0.19216},
    {0.19608, 0.19608, 0.19608},
    {0.20000, 0.20000, 0.20000},
    {0.20392, 0.20392, 0.20392},
    {0.20784, 0.20784, 0.20784},
    {0.21176, 0.21176, 0.21176},
    {0.21569, 0.21569, 0.21569},
    {0.21961, 0.21961, 0.21961},
    {0.22353, 0.22353, 0.22353},
    {0.22745, 0.22745, 0.22745},
    {0.23137, 0.23137, 0.23137},
    {0.23529, 0.23529, 0.23529},
    {0.23922, 0.23922, 0.23922},
    {0.24314, 0.24314, 0.24314},
    {0.24706, 0.24706, 0.24706},
    {0.25098, 0.25098, 0.25098},
    {0.25490, 0.25490, 0.25490},
    {0.25882, 0.25882, 0.25882},
    {0.26275, 0.26275, 0.26275},
    {0.26667, 0.26667, 0.26667},
    {0.27059, 0.27059, 0.27059},
    {0.27451, 0.27451, 0.27451},
    {0.27843, 0.27843, 0.27843},
    {0.28235, 0.28235, 0.28235},
    {0.28627, 0.28627, 0.28627},
    {0.29020, 0.29020, 0.29020},
    {0.29412, 0.29412, 0.29412},
    {0.29804, 0.29804, 0.29804},
    {0.30196, 0.30196, 0.30196},
    {0.30588, 0.30588, 0.30588},
    {0.30980, 0.30980, 0.30980},
    {0.31373, 0.31373, 0.31373},
    {0.31765, 0.31765, 0.31765},
    {0.32157, 0.32157, 0.32157},
    {0.32549, 0.32549, 0.32549},
    {0.32941, 0.32941, 0.32941},
    {0.33333, 0.33333, 0.33333},
    {0.33725, 0.33725, 0.33725},
    {0.34118, 0.34118, 0.34118},
    {0.34510, 0.34510, 0.34510},
    {0.34902, 0.34902, 0.34902},
    {0.35294, 0.35294, 0.35294},
    {0.35686, 0.35686, 0.35686},
    {0.36078, 0.36078, 0.36078},
    {0.36471, 0.36471, 0.36471},
    {0.36863, 0.36863, 0.36863},
    {0.37255, 0.37255, 0.37255},
    {0.37647, 0.37647, 0.37647},
    {0.38039, 0.38039, 0.38039},
    {0.38431, 0.38431, 0.38431},
    {0.38824, 0.38824, 0.38824},
    {0.39216, 0.39216, 0.39216},
    {0.39608, 0.39608, 0.39608},
    {0.40000, 0.40000, 0.40000},
    {0.40392, 0.40392, 0.40392},
    {0.40784, 0.40784, 0.40784},
    {0.41176, 0.41176, 0.41176},
    {0.41569, 0.41569, 0.41569},
    {0.41961, 0.41961, 0.41961},
    {0.42353, 0.42353, 0.42353},
    {0.42745, 0.42745, 0.42745},
    {0.43137, 0.43137, 0.43137},
    {0.43529, 0.43529, 0.43529},
    {0.43922, 0.43922, 0.43922},
    {0.44314, 0.44314, 0.44314},
    {0.44706, 0.44706, 0.44706},
    {0.45098, 0.45098, 0.45098},
    {0.45490, 0.45490, 0.45490},
    {0.45882, 0.45882, 0.45882},
    {0.46275, 0.46275, 0.46275},
    {0.46667, 0.46667, 0.46667},
    {0.47059, 0.47059, 0.47059},
    {0.47451, 0.47451, 0.47451},
    {0.47843, 0.47843, 0.47843},
    {0.48235, 0.48235, 0.48235},
    {0.48627, 0.48627, 0.48627},
    {0.49020, 0.49020, 0.49020},
    {0.49412, 0.49412, 0.49412},
    {0.49804, 0.49804, 0.49804},
    {0.50196, 0.50196, 0.50196},
    {0.50588, 0.50588, 0.50588},
    {0.50980, 0.50980, 0.50980},
    {0.51373, 0.51373, 0.51373},
    {0.51765, 0.51765, 0.51765},
    {0.52157, 0.52157, 0.52157},
    {0.52549, 0.52549, 0.52549},
    {0.52941, 0.52941, 0.52941},
    {0.53333, 0.53333, 0.53333},
    {0.53725, 0.53725, 0.53725},
    {0.54118, 0.54118, 0.54118},
    {0.54510, 0.54510, 0.54510},
    {0.54902, 0.54902, 0.54902},
    {0.55294, 0.55294, 0.55294},
    {0.55686, 0.55686, 0.55686},
    {0.56078, 0.56078, 0.56078},
    {0.56471, 0.56471, 0.56471},
    {0.56863, 0.56863, 0.56863},
    {0.57255, 0.57255, 0.57255},
    {0.57647, 0.57647, 0.57647},
    {0.58039, 0.58039, 0.58039},
    {0.58431, 0.58431, 0.58431},
    {0.58824, 0.58824, 0.58824},
    {0.59216, 0.59216, 0.59216},
    {0.59608, 0.59608, 0.59608},
    {0.60000, 0.60000, 0.60000},
    {0.60392, 0.60392, 0.60392},
    {0.60784, 0.60784, 0.60784},
    {0.61176, 0.61176, 0.61176},
    {0.61569, 0.61569, 0.61569},
    {0.61961, 0.61961, 0.61961},
    {0.62353, 0.62353, 0.62353},
    {0.62745, 0.62745, 0.62745},
    {0.63137, 0.63137, 0.63137},
    {0.63529, 0.63529, 0.63529},
    {0.63922, 0.63922, 0.63922},
    {0.64314, 0.64314, 0.64314},
    {0.64706, 0.64706, 0.64706},
    {0.65098, 0.65098, 0.65098},
    {0.65490, 0.65490, 0.65490},
    {0.65882, 0.65882, 0.65882},
    {0.66275, 0.66275, 0.66275},
    {0.66667, 0.66667, 0.66667},
    {0.67059, 0.67059, 0.67059},
    {0.67451, 0.67451, 0.67451},
    {0.67843, 0.67843, 0.67843},
    {0.68235, 0.68235, 0.68235},
    {0.68627, 0.68627, 0.68627},
    {0.69020, 0.69020, 0.69020},
    {0.69412, 0.69412, 0.69412},
    {0.69804, 0.69804, 0.69804},
    {0.70196, 0.70196, 0.70196},
    {0.70588, 0.70588, 0.70588},
    {0.70980, 0.70980, 0.70980},
    {0.71373, 0.71373, 0.71373},
    {0.71765, 0.71765, 0.71765},
    {0.72157, 0.72157, 0.72157},
    {0.72549, 0.72549, 0.72549},
    {0.72941, 0.72941, 0.72941},
    {0.73333, 0.73333, 0.73333},
    {0.73725, 0.73725, 0.73725},
    {0.74118, 0.74118, 0.74118},
    {0.74510, 0.74510, 0.74510},
    {0.74902, 0.74902, 0.74902},
    {0.75294, 0.75294, 0.75294},
    {0.75686, 0.75686, 0.75686},
    {0.76078, 0.76078, 0.76078},
    {0.76471, 0.76471, 0.76471},
    {0.76863, 0.76863, 0.76863},
    {0.77255, 0.77255, 0.77255},
    {0.77647, 0.77647, 0.77647},
    {0.78039, 0.78039, 0.78039},
    {0.78431, 0.78431, 0.78431},
    {0.78824, 0.78824, 0.78824},
    {0.79216, 0.79216, 0.79216},
    {0.79608, 0.79608, 0.79608},
    {0.80000, 0.80000, 0.80000},
    {0.80392, 0.80392, 0.80392},
    {0.80784, 0.80784, 0.80784},
    {0.81176, 0.81176, 0.81176},
    {0.81569, 0.81569, 0.81569},
    {0.81961, 0.81961, 0.81961},
    {0.82353, 0.82353, 0.82353},
    {0.82745, 0.82745, 0.82745},
    {0.83137, 0.83137, 0.83137},
    {0.83529, 0.83529, 0.83529},
    {0.83922, 0.83922, 0.83922},
    {0.84314, 0.84314, 0.84314},
    {0.84706, 0.84706, 0.84706},
    {0.85098, 0.85098, 0.85098},
    {0.85490, 0.85490, 0.85490},
    {0.85882, 0.85882, 0.85882},
    {0.86275, 0.86275, 0.86275},
    {0.86667, 0.86667, 0.86667},
    {0.87059, 0.87059, 0.87059},
    {0.87451, 0.87451, 0.87451},
    {0.87843, 0.87843, 0.87843},
    {0.88235, 0.88235, 0.88235},
    {0.88627, 0.88627, 0.88627},
    {0.89020, 0.89020, 0.89020},
    {0.89412, 0.89412, 0.89412},
    {0.89804, 0.89804, 0.89804},
    {0.90196, 0.90196, 0.90196},
    {0.90588, 0.90588, 0.90588},
    {0.90980, 0.90980, 0.90980},
    {0.91373, 0.91373, 0.91373},
    {0.91765, 0.91765, 0.91765},
    {0.92157, 0.92157, 0.92157},
    {0.92549, 0.92549, 0.92549},
    {0.92941, 0.92941, 0.92941},
    {0.93333, 0.93333, 0.93333},
    {0.93725, 0.93725, 0.93725},
    {0.94118, 0.94118, 0.94118},
    {0.94510, 0.94510, 0.94510},
    {0.94902, 0.94902, 0.94902},
    {0.95294, 0.95294, 0.95294},
    {0.95686, 0.95686, 0.95686},
    {0.96078, 0.96078, 0.96078},
    {0.96471, 0.96471, 0.96471},
    {0.96863, 0.96863, 0.96863},
    {0.97255, 0.97255, 0.97255},
    {0.97647, 0.97647, 0.97647},
    {0.98039, 0.98039, 0.98039},
    {0.98431, 0.98431, 0.98431},
    {0.98824, 0.98824, 0.98824},
    {0.99216, 0.99216, 0.99216},
    {0.99608, 0.99608, 0.99608},
    {1.00000, 1.00000, 1.00000},
};

static Colour_struct mousse[LUT_SIZE] =
{
    {0.00000, 0.00000, 0.00000},
    {0.00000, 0.00000, 0.00000},
    {0.00000, 0.00000, 0.00000},
    {0.00000, 0.00000, 0.00000},
    {0.00000, 0.00000, 0.00000},
    {0.00000, 0.00000, 0.00000},
    {0.00000, 0.00000, 0.00000},
    {0.00000, 0.00000, 0.00000},
    {0.00000, 0.00000, 0.06667},
    {0.00000, 0.00000, 0.06667},
    {0.00000, 0.00000, 0.06667},
    {0.00000, 0.00000, 0.06667},
    {0.00000, 0.00000, 0.06667},
    {0.00000, 0.00000, 0.06667},
    {0.00000, 0.00000, 0.06667},
    {0.00000, 0.00000, 0.06667},
    {0.00000, 0.00000, 0.13333},
    {0.00000, 0.00000, 0.13333},
    {0.00000, 0.00000, 0.13333},
    {0.00000, 0.00000, 0.13333},
    {0.00000, 0.00000, 0.13333},
    {0.00000, 0.00000, 0.13333},
    {0.00000, 0.00000, 0.13333},
    {0.00000, 0.00000, 0.13333},
    {0.00000, 0.00000, 0.20000},
    {0.00000, 0.00000, 0.20000},
    {0.00000, 0.00000, 0.20000},
    {0.00000, 0.00000, 0.20000},
    {0.00000, 0.00000, 0.20000},
    {0.00000, 0.00000, 0.20000},
    {0.00000, 0.00000, 0.20000},
    {0.00000, 0.00000, 0.20000},
    {0.00000, 0.00000, 0.26667},
    {0.00000, 0.00000, 0.26667},
    {0.00000, 0.00000, 0.26667},
    {0.00000, 0.00000, 0.26667},
    {0.00000, 0.00000, 0.26667},
    {0.00000, 0.00000, 0.26667},
    {0.00000, 0.00000, 0.26667},
    {0.00000, 0.00000, 0.26667},
    {0.00000, 0.00000, 0.33333},
    {0.00000, 0.00000, 0.33333},
    {0.00000, 0.00000, 0.33333},
    {0.00000, 0.00000, 0.33333},
    {0.00000, 0.00000, 0.33333},
    {0.00000, 0.00000, 0.33333},
    {0.00000, 0.00000, 0.33333},
    {0.00000, 0.00000, 0.33333},
    {0.00000, 0.00000, 0.40000},
    {0.00000, 0.00000, 0.40000},
    {0.00000, 0.00000, 0.40000},
    {0.00000, 0.00000, 0.40000},
    {0.00000, 0.00000, 0.40000},
    {0.00000, 0.00000, 0.40000},
    {0.00000, 0.00000, 0.40000},
    {0.00000, 0.00000, 0.40000},
    {0.00000, 0.00000, 0.46667},
    {0.00000, 0.00000, 0.46667},
    {0.00000, 0.00000, 0.46667},
    {0.00000, 0.00000, 0.46667},
    {0.00000, 0.00000, 0.46667},
    {0.00000, 0.00000, 0.46667},
    {0.00000, 0.00000, 0.46667},
    {0.00000, 0.00000, 0.46667},
    {0.00000, 0.00000, 0.53333},
    {0.00000, 0.00000, 0.53333},
    {0.00000, 0.00000, 0.53333},
    {0.00000, 0.00000, 0.53333},
    {0.00000, 0.00000, 0.53333},
    {0.00000, 0.00000, 0.53333},
    {0.00000, 0.00000, 0.53333},
    {0.00000, 0.00000, 0.53333},
    {0.06667, 0.00000, 0.53333},
    {0.06667, 0.00000, 0.53333},
    {0.06667, 0.00000, 0.53333},
    {0.06667, 0.00000, 0.53333},
    {0.06667, 0.00000, 0.53333},
    {0.06667, 0.00000, 0.53333},
    {0.06667, 0.00000, 0.53333},
    {0.06667, 0.00000, 0.53333},
    {0.13333, 0.00000, 0.53333},
    {0.13333, 0.00000, 0.53333},
    {0.13333, 0.00000, 0.53333},
    {0.13333, 0.00000, 0.53333},
    {0.13333, 0.00000, 0.53333},
    {0.13333, 0.00000, 0.53333},
    {0.13333, 0.00000, 0.53333},
    {0.13333, 0.00000, 0.53333},
    {0.20000, 0.00000, 0.53333},
    {0.20000, 0.00000, 0.53333},
    {0.20000, 0.00000, 0.53333},
    {0.20000, 0.00000, 0.53333},
    {0.20000, 0.00000, 0.53333},
    {0.20000, 0.00000, 0.53333},
    {0.20000, 0.00000, 0.53333},
    {0.20000, 0.00000, 0.53333},
    {0.26667, 0.00000, 0.53333},
    {0.26667, 0.00000, 0.53333},
    {0.26667, 0.00000, 0.53333},
    {0.26667, 0.00000, 0.53333},
    {0.26667, 0.00000, 0.53333},
    {0.26667, 0.00000, 0.53333},
    {0.26667, 0.00000, 0.53333},
    {0.26667, 0.00000, 0.53333},
    {0.33333, 0.00000, 0.53333},
    {0.33333, 0.00000, 0.53333},
    {0.33333, 0.00000, 0.53333},
    {0.33333, 0.00000, 0.53333},
    {0.33333, 0.00000, 0.53333},
    {0.33333, 0.00000, 0.53333},
    {0.33333, 0.00000, 0.53333},
    {0.33333, 0.00000, 0.53333},
    {0.40000, 0.00000, 0.53333},
    {0.40000, 0.00000, 0.53333},
    {0.40000, 0.00000, 0.53333},
    {0.40000, 0.00000, 0.53333},
    {0.40000, 0.00000, 0.53333},
    {0.40000, 0.00000, 0.53333},
    {0.40000, 0.00000, 0.53333},
    {0.40000, 0.00000, 0.53333},
    {0.46667, 0.00000, 0.53333},
    {0.46667, 0.00000, 0.53333},
    {0.46667, 0.00000, 0.53333},
    {0.46667, 0.00000, 0.53333},
    {0.46667, 0.00000, 0.53333},
    {0.46667, 0.00000, 0.53333},
    {0.46667, 0.00000, 0.53333},
    {0.46667, 0.00000, 0.53333},
    {0.53333, 0.00000, 0.53333},
    {0.53333, 0.00000, 0.53333},
    {0.53333, 0.00000, 0.53333},
    {0.53333, 0.00000, 0.53333},
    {0.53333, 0.00000, 0.46667},
    {0.53333, 0.00000, 0.46667},
    {0.53333, 0.00000, 0.46667},
    {0.53333, 0.00000, 0.46667},
    {0.60000, 0.00000, 0.40000},
    {0.60000, 0.00000, 0.40000},
    {0.60000, 0.00000, 0.40000},
    {0.60000, 0.00000, 0.40000},
    {0.60000, 0.00000, 0.33333},
    {0.60000, 0.00000, 0.33333},
    {0.60000, 0.00000, 0.33333},
    {0.60000, 0.00000, 0.33333},
    {0.66667, 0.00000, 0.26667},
    {0.66667, 0.00000, 0.26667},
    {0.66667, 0.00000, 0.26667},
    {0.66667, 0.00000, 0.26667},
    {0.66667, 0.00000, 0.20000},
    {0.66667, 0.00000, 0.20000},
    {0.66667, 0.00000, 0.20000},
    {0.66667, 0.00000, 0.20000},
    {0.73333, 0.00000, 0.13333},
    {0.73333, 0.00000, 0.13333},
    {0.73333, 0.00000, 0.13333},
    {0.73333, 0.00000, 0.13333},
    {0.73333, 0.00000, 0.06667},
    {0.73333, 0.00000, 0.06667},
    {0.73333, 0.00000, 0.06667},
    {0.73333, 0.00000, 0.06667},
    {0.80000, 0.00000, 0.00000},
    {0.80000, 0.00000, 0.00000},
    {0.80000, 0.00000, 0.00000},
    {0.80000, 0.00000, 0.00000},
    {0.80000, 0.00000, 0.00000},
    {0.80000, 0.00000, 0.00000},
    {0.80000, 0.00000, 0.00000},
    {0.80000, 0.00000, 0.00000},
    {0.86667, 0.00000, 0.00000},
    {0.86667, 0.00000, 0.00000},
    {0.86667, 0.00000, 0.00000},
    {0.86667, 0.00000, 0.00000},
    {0.86667, 0.00000, 0.00000},
    {0.86667, 0.00000, 0.00000},
    {0.86667, 0.00000, 0.00000},
    {0.86667, 0.00000, 0.00000},
    {0.93333, 0.00000, 0.00000},
    {0.93333, 0.00000, 0.00000},
    {0.93333, 0.00000, 0.00000},
    {0.93333, 0.00000, 0.00000},
    {0.93333, 0.00000, 0.00000},
    {0.93333, 0.00000, 0.00000},
    {0.93333, 0.00000, 0.00000},
    {0.93333, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.06667, 0.00000},
    {1.00000, 0.06667, 0.00000},
    {1.00000, 0.13333, 0.00000},
    {1.00000, 0.13333, 0.00000},
    {1.00000, 0.20000, 0.00000},
    {1.00000, 0.20000, 0.00000},
    {1.00000, 0.26667, 0.00000},
    {1.00000, 0.26667, 0.00000},
    {1.00000, 0.33333, 0.00000},
    {1.00000, 0.33333, 0.00000},
    {1.00000, 0.40000, 0.00000},
    {1.00000, 0.40000, 0.00000},
    {1.00000, 0.46667, 0.00000},
    {1.00000, 0.46667, 0.00000},
    {1.00000, 0.53333, 0.00000},
    {1.00000, 0.53333, 0.00000},
    {1.00000, 0.60000, 0.00000},
    {1.00000, 0.60000, 0.00000},
    {1.00000, 0.66667, 0.00000},
    {1.00000, 0.66667, 0.00000},
    {1.00000, 0.73333, 0.00000},
    {1.00000, 0.73333, 0.00000},
    {1.00000, 0.80000, 0.00000},
    {1.00000, 0.80000, 0.00000},
    {1.00000, 0.86667, 0.00000},
    {1.00000, 0.86667, 0.00000},
    {1.00000, 0.93333, 0.00000},
    {1.00000, 0.93333, 0.00000},
    {1.00000, 1.00000, 0.00000},
    {1.00000, 1.00000, 0.00000},
    {1.00000, 1.00000, 0.00000},
    {1.00000, 1.00000, 0.00000},
    {1.00000, 1.00000, 0.06667},
    {1.00000, 1.00000, 0.06667},
    {1.00000, 1.00000, 0.13333},
    {1.00000, 1.00000, 0.13333},
    {1.00000, 1.00000, 0.20000},
    {1.00000, 1.00000, 0.20000},
    {1.00000, 1.00000, 0.26667},
    {1.00000, 1.00000, 0.26667},
    {1.00000, 1.00000, 0.33333},
    {1.00000, 1.00000, 0.33333},
    {1.00000, 1.00000, 0.40000},
    {1.00000, 1.00000, 0.40000},
    {1.00000, 1.00000, 0.46667},
    {1.00000, 1.00000, 0.46667},
    {1.00000, 1.00000, 0.53333},
    {1.00000, 1.00000, 0.53333},
    {1.00000, 1.00000, 0.60000},
    {1.00000, 1.00000, 0.60000},
    {1.00000, 1.00000, 0.66667},
    {1.00000, 1.00000, 0.66667},
    {1.00000, 1.00000, 0.73333},
    {1.00000, 1.00000, 0.73333},
    {1.00000, 1.00000, 0.80000},
    {1.00000, 1.00000, 0.80000},
    {1.00000, 1.00000, 0.86667},
    {1.00000, 1.00000, 1.00000},
};

static Colour_struct rainbow[LUT_SIZE] =
{
    {0.00000, 0.00000, 0.01176},
    {0.00000, 0.00000, 0.02745},
    {0.00000, 0.00000, 0.04314},
    {0.00000, 0.00000, 0.05882},
    {0.00000, 0.00000, 0.07451},
    {0.00000, 0.00000, 0.09020},
    {0.00000, 0.00000, 0.10588},
    {0.00000, 0.00000, 0.12157},
    {0.00000, 0.00000, 0.13725},
    {0.00000, 0.00000, 0.15294},
    {0.00000, 0.00000, 0.16863},
    {0.00000, 0.00000, 0.18431},
    {0.00000, 0.00000, 0.20000},
    {0.00000, 0.00000, 0.21176},
    {0.00000, 0.00000, 0.22745},
    {0.00000, 0.00000, 0.24314},
    {0.00000, 0.00000, 0.25882},
    {0.00000, 0.00000, 0.27451},
    {0.00000, 0.00000, 0.29020},
    {0.00000, 0.00000, 0.30588},
    {0.00000, 0.00000, 0.32157},
    {0.00000, 0.00000, 0.33725},
    {0.00000, 0.00000, 0.35294},
    {0.00000, 0.00000, 0.36863},
    {0.00000, 0.00000, 0.38431},
    {0.00000, 0.00000, 0.40000},
    {0.00000, 0.00000, 0.41176},
    {0.00000, 0.00000, 0.42745},
    {0.00000, 0.00000, 0.44314},
    {0.00000, 0.00000, 0.45882},
    {0.00000, 0.00000, 0.47451},
    {0.00000, 0.00000, 0.49020},
    {0.00000, 0.00000, 0.50588},
    {0.00000, 0.00000, 0.52157},
    {0.00000, 0.00000, 0.53725},
    {0.00000, 0.00000, 0.55294},
    {0.00000, 0.00000, 0.56863},
    {0.00000, 0.00000, 0.58431},
    {0.00000, 0.00000, 0.60000},
    {0.00000, 0.00000, 0.61176},
    {0.00000, 0.00000, 0.62745},
    {0.00000, 0.00000, 0.64314},
    {0.00000, 0.00000, 0.65882},
    {0.00000, 0.00000, 0.67451},
    {0.00000, 0.00000, 0.69020},
    {0.00000, 0.00000, 0.70588},
    {0.00000, 0.00000, 0.72157},
    {0.00000, 0.00000, 0.73725},
    {0.00000, 0.00000, 0.75294},
    {0.00000, 0.00000, 0.76863},
    {0.00000, 0.00000, 0.78431},
    {0.00000, 0.00000, 0.80000},
    {0.00000, 0.00000, 0.81176},
    {0.00000, 0.00000, 0.82745},
    {0.00000, 0.00000, 0.84314},
    {0.00000, 0.00000, 0.85882},
    {0.00000, 0.00000, 0.87451},
    {0.00000, 0.00000, 0.89020},
    {0.00000, 0.00000, 0.90588},
    {0.00000, 0.00000, 0.92157},
    {0.00000, 0.00000, 0.93725},
    {0.00000, 0.00000, 0.95294},
    {0.00000, 0.00000, 0.96863},
    {0.00000, 0.00000, 0.98431},
    {0.00000, 0.00000, 1.00000},
    {0.00000, 0.03529, 1.00000},
    {0.00000, 0.07059, 1.00000},
    {0.00000, 0.10980, 1.00000},
    {0.00000, 0.14510, 1.00000},
    {0.00000, 0.18039, 1.00000},
    {0.00000, 0.21961, 1.00000},
    {0.00000, 0.25490, 1.00000},
    {0.00000, 0.29412, 1.00000},
    {0.00000, 0.32941, 1.00000},
    {0.00000, 0.36471, 1.00000},
    {0.00000, 0.40392, 1.00000},
    {0.00000, 0.43922, 1.00000},
    {0.00000, 0.47843, 1.00000},
    {0.00000, 0.50196, 1.00000},
    {0.00000, 0.52549, 1.00000},
    {0.00000, 0.54902, 1.00000},
    {0.00000, 0.57255, 1.00000},
    {0.00000, 0.59608, 1.00000},
    {0.00000, 0.61961, 1.00000},
    {0.00000, 0.64314, 1.00000},
    {0.00000, 0.66667, 1.00000},
    {0.00000, 0.69020, 1.00000},
    {0.00000, 0.71373, 1.00000},
    {0.00000, 0.73725, 1.00000},
    {0.00000, 0.76078, 1.00000},
    {0.00000, 0.78431, 1.00000},
    {0.00000, 0.80000, 1.00000},
    {0.00000, 0.81569, 1.00000},
    {0.00000, 0.83137, 1.00000},
    {0.00000, 0.84706, 1.00000},
    {0.00000, 0.86667, 1.00000},
    {0.00000, 0.88235, 1.00000},
    {0.00000, 0.89804, 1.00000},
    {0.00000, 0.91373, 1.00000},
    {0.00000, 0.93333, 1.00000},
    {0.00000, 0.94902, 1.00000},
    {0.00000, 0.96471, 1.00000},
    {0.00000, 0.98039, 1.00000},
    {0.00000, 1.00000, 1.00000},
    {0.00000, 1.00000, 0.97647},
    {0.00000, 1.00000, 0.95294},
    {0.00000, 1.00000, 0.92941},
    {0.00000, 1.00000, 0.90588},
    {0.00000, 1.00000, 0.88627},
    {0.00000, 1.00000, 0.86275},
    {0.00000, 1.00000, 0.83922},
    {0.00000, 1.00000, 0.81569},
    {0.00000, 1.00000, 0.79608},
    {0.00000, 1.00000, 0.77255},
    {0.00000, 1.00000, 0.74902},
    {0.00000, 1.00000, 0.72549},
    {0.00000, 1.00000, 0.70588},
    {0.00000, 1.00000, 0.65098},
    {0.00000, 1.00000, 0.59608},
    {0.00000, 1.00000, 0.54118},
    {0.00000, 1.00000, 0.48627},
    {0.00000, 1.00000, 0.43137},
    {0.00000, 1.00000, 0.37647},
    {0.00000, 1.00000, 0.32549},
    {0.00000, 1.00000, 0.27059},
    {0.00000, 1.00000, 0.21569},
    {0.00000, 1.00000, 0.16078},
    {0.00000, 1.00000, 0.10588},
    {0.00000, 1.00000, 0.05098},
    {0.00000, 1.00000, 0.00000},
    {0.05098, 1.00000, 0.00000},
    {0.10588, 1.00000, 0.00000},
    {0.16078, 1.00000, 0.00000},
    {0.21569, 1.00000, 0.00000},
    {0.27059, 1.00000, 0.00000},
    {0.32549, 1.00000, 0.00000},
    {0.37647, 1.00000, 0.00000},
    {0.43137, 1.00000, 0.00000},
    {0.48627, 1.00000, 0.00000},
    {0.54118, 1.00000, 0.00000},
    {0.59608, 1.00000, 0.00000},
    {0.65098, 1.00000, 0.00000},
    {0.70588, 1.00000, 0.00000},
    {0.72549, 1.00000, 0.00000},
    {0.74902, 1.00000, 0.00000},
    {0.77255, 1.00000, 0.00000},
    {0.79608, 1.00000, 0.00000},
    {0.81569, 1.00000, 0.00000},
    {0.83922, 1.00000, 0.00000},
    {0.86275, 1.00000, 0.00000},
    {0.88627, 1.00000, 0.00000},
    {0.90588, 1.00000, 0.00000},
    {0.92941, 1.00000, 0.00000},
    {0.95294, 1.00000, 0.00000},
    {0.97647, 1.00000, 0.00000},
    {1.00000, 1.00000, 0.00000},
    {0.99608, 0.97647, 0.00000},
    {0.99608, 0.95686, 0.00000},
    {0.99608, 0.93333, 0.00000},
    {0.99608, 0.91373, 0.00000},
    {0.99216, 0.89412, 0.00000},
    {0.99216, 0.87059, 0.00000},
    {0.99216, 0.85098, 0.00000},
    {0.99216, 0.82745, 0.00000},
    {0.98824, 0.80784, 0.00000},
    {0.98824, 0.78824, 0.00000},
    {0.98824, 0.76471, 0.00000},
    {0.98824, 0.74510, 0.00000},
    {0.98824, 0.72549, 0.00000},
    {0.98824, 0.70588, 0.00000},
    {0.98824, 0.68627, 0.00000},
    {0.98824, 0.66667, 0.00000},
    {0.98824, 0.64706, 0.00000},
    {0.99216, 0.62745, 0.00000},
    {0.99216, 0.60784, 0.00000},
    {0.99216, 0.58824, 0.00000},
    {0.99216, 0.56863, 0.00000},
    {0.99608, 0.54902, 0.00000},
    {0.99608, 0.52941, 0.00000},
    {0.99608, 0.50980, 0.00000},
    {0.99608, 0.49020, 0.00000},
    {1.00000, 0.47059, 0.00000},
    {1.00000, 0.43137, 0.00000},
    {1.00000, 0.39608, 0.00000},
    {1.00000, 0.36078, 0.00000},
    {1.00000, 0.32549, 0.00000},
    {1.00000, 0.28627, 0.00000},
    {1.00000, 0.25098, 0.00000},
    {1.00000, 0.21569, 0.00000},
    {1.00000, 0.18039, 0.00000},
    {1.00000, 0.14118, 0.00000},
    {1.00000, 0.10588, 0.00000},
    {1.00000, 0.07059, 0.00000},
    {1.00000, 0.03529, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.00000},
    {1.00000, 0.00000, 0.05098},
    {1.00000, 0.00000, 0.10588},
    {1.00000, 0.00000, 0.16078},
    {1.00000, 0.00000, 0.21569},
    {1.00000, 0.00000, 0.27059},
    {1.00000, 0.00000, 0.32549},
    {1.00000, 0.00000, 0.37647},
    {1.00000, 0.00000, 0.43137},
    {1.00000, 0.00000, 0.48627},
    {1.00000, 0.00000, 0.54118},
    {1.00000, 0.00000, 0.59608},
    {1.00000, 0.00000, 0.65098},
    {1.00000, 0.00000, 0.70588},
    {1.00000, 0.00000, 0.72549},
    {1.00000, 0.00000, 0.74902},
    {1.00000, 0.00000, 0.77255},
    {1.00000, 0.00000, 0.79608},
    {1.00000, 0.00000, 0.81569},
    {1.00000, 0.00000, 0.83922},
    {1.00000, 0.00000, 0.86275},
    {1.00000, 0.00000, 0.88627},
    {1.00000, 0.00000, 0.90588},
    {1.00000, 0.00000, 0.92941},
    {1.00000, 0.00000, 0.95294},
    {1.00000, 0.00000, 0.97647},
    {1.00000, 0.00000, 1.00000},
    {1.00000, 0.03529, 1.00000},
    {1.00000, 0.07059, 1.00000},
    {1.00000, 0.10588, 1.00000},
    {1.00000, 0.14118, 1.00000},
    {1.00000, 0.18039, 1.00000},
    {1.00000, 0.21569, 1.00000},
    {1.00000, 0.25098, 1.00000},
    {1.00000, 0.28627, 1.00000},
    {1.00000, 0.32549, 1.00000},
    {1.00000, 0.36078, 1.00000},
    {1.00000, 0.39608, 1.00000},
    {1.00000, 0.43137, 1.00000},
    {1.00000, 0.47059, 1.00000},
    {1.00000, 0.48627, 1.00000},
    {1.00000, 0.50588, 1.00000},
    {1.00000, 0.52157, 1.00000},
    {1.00000, 0.54118, 1.00000},
    {1.00000, 0.56078, 1.00000},
    {1.00000, 0.57647, 1.00000},
    {1.00000, 0.59608, 1.00000},
    {1.00000, 0.61176, 1.00000},
    {1.00000, 0.63137, 1.00000},
    {1.00000, 0.65098, 1.00000},
    {1.00000, 0.66667, 1.00000},
    {1.00000, 0.68627, 1.00000},
    {1.00000, 0.70588, 1.00000},
    {1.00000, 0.74510, 1.00000},
    {1.00000, 0.78824, 1.00000},
    {1.00000, 0.83137, 1.00000},
    {1.00000, 0.87059, 1.00000},
    {1.00000, 0.91373, 1.00000},
    {1.00000, 0.95686, 1.00000},
    {1.00000, 1.00000, 1.00000},
};

/*PUBLIC_FUNCTION*/
void cf_mono (unsigned int num_cells, unsigned short *reds,
	      unsigned short *greens, unsigned short *blues,
	      unsigned int stride, double x, double y, void *var_param)
/*  [SUMMARY] Compute a greyscale colourmap.
    <num_cells> The number of colour cells to modify.
    <reds> The red intensity values. This may be NULL.
    <greens> The green intensity values. This may be NULL.
    <blues> The blue intensity values. This may be NULL.
    <stride> The stride (in unsigned shorts) between intensity values.
    <x> A parameter used to compute the colour values, ranging from 0.0 to 1.0.
    <y> A parameter used to compute the colour values, ranging from 0.0 to 1.0.
    <var_param> A parameter used to compute the colour values. Ignored.
    [RETURNS] Nothing.
*/
{
    extern Colour_struct   mono[];

    colour_table (num_cells, reds, greens, blues, stride, x, y, mono);
}   /*  End Function cf_mono  */

/*PUBLIC_FUNCTION*/
void cf_mousse (unsigned int num_cells, unsigned short *reds,
		unsigned short *greens, unsigned short *blues,
		unsigned int stride, double x, double y, void *var_param)
/*  [SUMMARY] Compute a mousse colourmap.
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
    extern Colour_struct   mousse[];

    colour_table (num_cells, reds, greens, blues, stride, x, y, mousse);
}   /*  End Function cf_mousse  */

/*PUBLIC_FUNCTION*/
void cf_rainbow (unsigned int num_cells, unsigned short *reds,
		 unsigned short *greens, unsigned short *blues,
		 unsigned int stride, double x, double y, void *var_param)
/*  [SUMMARY] Compute a rainbow colourmap.
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
    extern Colour_struct   rainbow[];

    colour_table (num_cells, reds, greens, blues, stride, x, y, rainbow);
}   /*  End Function cf_rainbow  */
