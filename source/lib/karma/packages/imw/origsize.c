/*LINTLIBRARY*/
/*  origsize.c

    This code provides routines to compute a raw image from a data structure.

    Copyright (C) 1994,1995  Richard Gooch

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

    This file contains various utility routines for drawing into a raw
    image.


    Written by      Richard Gooch   28-JUL-1994

    Updated by      Richard Gooch   7-AUG-1994

    Updated by      Richard Gooch   15-SEP-1994: Trapped  i_min  >=  i_max  .

    Updated by      Richard Gooch   9-NOV-1994: Moved conversion of  i_min
  and  i_max  into switch to ensure image data type is integer.

    Updated by      Richard Gooch   26-NOV-1994: Moved to
  packages/imw/origsize.c

    Updated by      Richard Gooch   2-JAN-1995: Changed function interface to
  drop input stride and intensity scaling parameters.

    Updated by      Richard Gooch   23-JAN-1995: Added a cast to keep IRIX
  compiler happy.

    Updated by      Richard Gooch   9-APR-1995: Made use of
  <ds_complex_to_real_1D> routine.

    Updated by      Richard Gooch   19-APR-1995: Cleaned some code.

    Updated by      Richard Gooch   20-APR-1995: Fixed bug with multiplying
  huge (int) values.

    Updated by      Richard Gooch   7-MAY-1995: Placate gcc -Wall

    Last updated by Richard Gooch   29-JUN-1995: Changed
  <setup_ubyte_lookup_table> and <setup_byte_lookup_table> to use floating
  point calculations with offset.


*/

#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <karma.h>
#include <karma_imw.h>
#include <karma_ds.h>
#include <karma_m.h>
#include <karma_a.h>

#define UBYTE_TABLE_LENGTH 256
#define BYTE_TABLE_LENGTH 256


/*  Private routines  */
STATIC_FUNCTION (void setup_ubyte_lookup_table,
		 (int min, int max,
		  unsigned int num_pixels, CONST unsigned char *pixel_values,
		  unsigned char min_sat_pixel, unsigned char max_sat_pixel) );
STATIC_FUNCTION (void setup_byte_lookup_table,
		 (int min, int max,
		  unsigned int num_pixels, CONST unsigned char *pixel_values,
		  unsigned char blank_pixel,
		  unsigned char min_sat_pixel, unsigned char max_sat_pixel) );
STATIC_FUNCTION (double *alloc_values_buffer, (unsigned int num_values) );


/*  Private data  */
static unsigned char ubyte_lookup_table[UBYTE_TABLE_LENGTH];
static unsigned char byte_lookup_table[UBYTE_TABLE_LENGTH];


/*  Public routines follow  */

/*OBSOLETE_FUNCTION*/
flag imw_to8_o (unsigned char *out_image, iaddr out_hstride, iaddr out_vstride,
		int width, int height, CONST char *inp_image,
		CONST iaddr *inp_hoffsets, CONST iaddr *inp_voffsets,
		unsigned int inp_type, unsigned int conv_type,
		unsigned int num_pixels, CONST unsigned char *pixel_values,
		unsigned char blank_pixel,
		unsigned char min_sat_pixel, unsigned char max_sat_pixel,
		double i_min, double i_max)
/*  OBSOLETE: use <imw_to8_oi> instead.
*/
{
    return ( imw_to8_oi (out_image, out_hstride, out_vstride, width, height,
			 inp_image, inp_hoffsets, inp_voffsets, inp_type,
			 conv_type, num_pixels, pixel_values,
			 blank_pixel, min_sat_pixel, max_sat_pixel,
			 i_min,  i_max, ( flag (*) () ) NULL, NULL) );
}   /*  End Function imw_to8_o  */


/*PUBLIC_FUNCTION*/
flag imw_to8_oi (unsigned char *out_image,
		 iaddr out_hstride, iaddr out_vstride,
		 int width, int height, CONST char *inp_image,
		 CONST iaddr *inp_hoffsets, CONST iaddr *inp_voffsets,
		 unsigned int inp_type, unsigned int conv_type,
		 unsigned int num_pixels, CONST unsigned char *pixel_values,
		 unsigned char blank_pixel,
		 unsigned char min_sat_pixel, unsigned char max_sat_pixel,
		 double i_min, double i_max,
		 flag (*iscale_func) (), void *iscale_info)
/*  [PURPOSE] This routine will convert an image from one format to an 8 bit
    image of pixels, maintaining the original image size. The output image is
    flipped vertically relative to the input image.
    <out_image> The output image will be written here.
    <out_hstride> The stride between successive horizontal pixels (in bytes).
    <out_vstride> The stride between successive vertical pixels (in bytes).
    <width> The width of the image.
    <height> The height of the image.
    <inp_image> The input image data.
    <inp_hoffsets> The array of horizontal byte offsets.
    <inp_voffsets> The array of vertical byte offsets.
    <inp_type> The type of the input data.
    <conv_type> The input conversion type (when the input is complex).
    <num_pixels> The number of pixels in the pixel array.
    <pixel_values> The array of pixel values.
    <blank_pixel> The pixel value to be used when the intensity value is an
    undefined value.
    <min_sat_pixel> The pixel value to be used when the intensity value is
    below the minimum value.
    <max_sat_pixel> The pixel value to be used when the intensity value is
    above the maximum value.
    <i_min> The minimum intensity value.
    <i_max> The maximum intensity value.
    <iscale_func> The function to be called when non-linear intensity scaling
    is required. If NULL, linear intensity scaling is used. The interface to
    this function is as follows:
    [<pre>]
    flag iscale_func (double *out, unsigned int out_stride,
                      double *inp, unsigned int inp_stride,
		      unsigned int num_values, double i_min, double i_max,
		      void *info)
    *   [PURPOSE] This routine will perform an arbitrary intensity scaling on
        an array of values. This routine may be called many times to scale an
	image.
        <out> The output array.
	<out_stride> The stride (in doubles) of the output array.
	<inp> The input array.
	<inp_stride> The stride (in doubles) of the input array.
	<num_values> The number of values to scale.
	<i_min> The minimum intensity value.
	<i_max> The maximum intensity value.
	<info> A pointer to arbitrary information.
	[RETURNS] TRUE on success, else FALSE.
    *
    [</pre>]

    <iscale_info> A pointer to arbitrary information for <<iscale_func>>.
    [MT-LEVEL] Unsafe.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    flag complex;
    flag fast_conversion = FALSE;
    int hcount, vcount;
    long l_mul;
    long l_div;
    long l_data;
    long l_min = 1;    /*  Initialised to keep compiler happy  */
    long l_max = -1;   /*  Initialised to keep compiler happy  */
    long l_blank = 0;  /*  Initialised to keep compiler happy  */
    float f_mul;
    float f_data;
    float f_min = i_min;
    float f_max = i_max;
    float f_toobig = TOOBIG;
    double d_toobig = TOOBIG;
    double d_mul;
    double d_data;
    CONST char *inp_line, *inp_ptr;
    unsigned char *out_line;
    CONST unsigned char *b_table;
    double *values, *val_ptr;
    extern unsigned char byte_lookup_table[BYTE_TABLE_LENGTH];
    extern unsigned char ubyte_lookup_table[UBYTE_TABLE_LENGTH];
    static char function_name[] = "imw_to8_oi";

    if ( (inp_hoffsets == NULL) || (inp_voffsets == NULL) )
    {
	(void) fprintf (stderr, "NULL pointer(s) passed\n");
	a_prog_bug (function_name);
    }
    if (i_min >= i_max)
    {
	(void) fprintf (stderr, "i_max: %e  is not greater than i_min: %e\n",
			i_max, i_min);
	a_prog_bug (function_name);
    }
    if ( !ds_element_is_atomic (inp_type) )
    {
	(void) fprintf (stderr, "Input image must be atomic\n");
	a_prog_bug (function_name);
    }
    if (getuid () == 465) (void) fprintf (stderr, "%s started...\n",
					  function_name);
    switch (inp_type)
    {
      case K_FLOAT:
      case K_DOUBLE:
	fast_conversion = TRUE;
	break;
      case K_COMPLEX:
      case K_DCOMPLEX:
	fast_conversion = FALSE;
	break;
      case K_UBYTE:
	l_min = i_min;
	l_max = i_max;
	/*  Unsigned bytes: can speed things up  */
	setup_ubyte_lookup_table ( (long) i_min, (long) i_max,
				  num_pixels, pixel_values,
				  min_sat_pixel, max_sat_pixel );
	fast_conversion = TRUE;
	break;
      case K_BYTE:
	l_min = i_min;
	l_max = i_max;
	/*  Signed bytes: can speed things up  */
	setup_byte_lookup_table ( (long) i_min, (long) i_max,
				 num_pixels, pixel_values,
				 blank_pixel, min_sat_pixel, max_sat_pixel );
	fast_conversion = TRUE;
	break;
      case K_SHORT:
	l_min = i_min;
	l_max = i_max;
	l_blank = -32768;
	fast_conversion = TRUE;
	break;
      default:
	fast_conversion = FALSE;
	break;
    }
    if (iscale_func != NULL) fast_conversion = FALSE;
    /*  Allocate values buffer  */
    if ( ( values = alloc_values_buffer ( (unsigned int) width ) ) == NULL )
    {
	return (FALSE);
    }
    /*  Loop through the image lines  */
    for (vcount = 0; vcount < height; ++vcount)
    {
	out_line = out_image + (height - vcount - 1) * out_vstride;
	/*  out_line  may be modified now  */
	inp_line = inp_image + inp_voffsets[vcount];
	/*  inp_line  may be modified now  */
	/*  Now have pointers to input and output image lines  */
	if (fast_conversion)
	{
	    switch (inp_type)
	    {
	      case K_FLOAT:
		f_mul = (num_pixels - 1) / (f_max - f_min);
		for (hcount = 0; hcount < width;
		     ++hcount, out_line += out_hstride)
		{
		    inp_ptr = inp_line + inp_hoffsets[hcount];
		    if ( (f_data = *(float *) inp_ptr) < f_min )
		    {
			*out_line = min_sat_pixel;
		    }
		    else if (f_data >= f_toobig)
		    {
			*out_line = blank_pixel;
		    }
		    else if (f_data > f_max)
		    {
			*out_line = max_sat_pixel;
		    }
		    else
		    {
			*out_line = pixel_values[(int) ( (f_data - f_min)
							* f_mul + 0.5 )];
		    }
		}
		/*  End  K_FLOAT  */
		break;
	      case K_DOUBLE:
		d_mul = (num_pixels - 1) / (i_max - i_min);
		for (hcount = 0; hcount < width;
		     ++hcount, out_line += out_hstride)
		{
		    inp_ptr = inp_line + inp_hoffsets[hcount];
		    if ( (d_data = *(double *) inp_ptr) < i_min )
		    {
			*out_line = min_sat_pixel;
		    }
		    else if (d_data >= d_toobig)
		    {
			*out_line = blank_pixel;
		    }
		    else if (d_data > i_max)
		    {
			*out_line = max_sat_pixel;
		    }
		    else
		    {
			*out_line = pixel_values[(int) ( (d_data - i_min)
							* d_mul + 0.5 )];
		    }
		}
		/*  End  K_DOUBLE  */
		break;
	      case K_BYTE:
		b_table = byte_lookup_table + 128;
		for (hcount = 0; hcount < width;
		     ++hcount, out_line += out_hstride)
		{
		    inp_ptr = inp_line + inp_hoffsets[hcount];
		    *out_line = *(b_table + *(char *) inp_ptr);
		}
		/*  End  K_BYTE  */
		break;
	      case K_SHORT:
		l_mul = (num_pixels - 1);
		l_div = (i_max - i_min);
		/*  Loop  */
		for (hcount = 0; hcount < width;
		     ++hcount, out_line += out_hstride)
		{
		    inp_ptr = inp_line + inp_hoffsets[hcount];
		    l_data = *(short *) inp_ptr;
		    if (l_data == l_blank)
		    {
			*out_line = blank_pixel;
		    }
		    else if (l_data < l_min)
		    {
			*out_line = min_sat_pixel;
		    }
		    else if (l_data > l_max)
		    {
			*out_line = max_sat_pixel;
		    }
		    else
		    {
			*out_line = pixel_values[(int) ( (l_data - l_min)
							* l_mul / l_div )];
		    }
		}
		/*  End  K_SHORT  */
		break;
	      case K_UBYTE:
		for (hcount = 0; hcount < width;
		     ++hcount, out_line += out_hstride)
		{
		    inp_ptr = inp_line + inp_hoffsets[hcount];
		    *out_line = *(ubyte_lookup_table +
				  *(unsigned char *) inp_ptr);
		}
		/*  End  K_UBYTE  */
		break;
	      default:
		(void) fprintf (stderr, "Data type: %u not supported\n",
				inp_type);
		a_prog_bug (function_name);
		break;
	    }
	}
	else
	{
	    /*  Slow conversion needed  */
	    /*  Convert data to generic data type  */
	    /*  Note the cast from (iaddr *) to (uaddr *) for the offset
		arrays. This is dodgy, but it should work.  */
	    if ( !ds_get_scattered_elements (inp_line, inp_type,
					     (uaddr *) inp_hoffsets,
					     values, &complex,
					     (unsigned int) width) )
	    {
		(void) fprintf (stderr, "Error converting data\n");
		return (FALSE);
	    }
	    if (complex) ds_complex_to_real_1D (values, 2, values,
						(unsigned int) width,
						conv_type);
	    if (iscale_func != NULL)
	    {
		if ( !(*iscale_func) (values, 2, values, 2,
				      (unsigned int) width, i_min, i_max,
				      iscale_info) )
		{
		    (void) fprintf (stderr,
				    "Error applying intensity scale\n");
		    return (FALSE);
		}
	    }
	    d_mul = (num_pixels - 1) / (i_max - i_min);
	    /*  Loop for each value  */
	    for (hcount = 0, val_ptr = values; hcount < width;
		 ++hcount, out_line += out_hstride, val_ptr += 2)
	    {
		if ( (d_data  = *val_ptr) < i_min ) *out_line = min_sat_pixel;
		else if (d_data >= d_toobig) *out_line = blank_pixel;
		else if (d_data > i_max) *out_line = max_sat_pixel;
		else *out_line = pixel_values[(int) ( (d_data - i_min)
						     * d_mul + 0.5 )];
	    }
	}
    }
    return (TRUE);
}   /*  End Function imw_to8_oi  */


/*  Private functions follow  */

static void setup_ubyte_lookup_table (int min, int max,
				      unsigned int num_pixels,
				      CONST unsigned char *pixel_values,
				      unsigned char min_sat_pixel,
				      unsigned char max_sat_pixel)
/*  This routine will setup a lookup table for unsigned byte data which will
    convert from data values to pixel values.
    The minimum (lower intensity clipping point) value must be given by  min  .
    The maximum (upper intensity clipping point) value must be given by  max  .
    The number of pixel values to use must be given by  num_pixels  .
    The pixel values to use must be in the array pointed to by  pixel_values  .
    The pixel value to be used when the intensity value is below the minimum
    value must be given by  min_sat_pixel  .
    The pixel value to be used when the intensity value is above the maximum
    value must be given by  max_sat_pixel  .
    The routine will write the lookup table into the global array:
    ubyte_lookup_table  .
    The routine returns nothing.
*/
{
    int ubyte_val;
    float mul, val;
    float offset = 0.1;
    extern unsigned char ubyte_lookup_table[UBYTE_TABLE_LENGTH];

    /*  Precompute pixel index  */
    mul = (float) (num_pixels - 1) / (float) (max - min);
    /*  Loop through all possible ubyte values  */
    for (ubyte_val = 0; ubyte_val < UBYTE_TABLE_LENGTH; ++ubyte_val)
    {
	if (ubyte_val < min)
	{
	    ubyte_lookup_table[ubyte_val] = min_sat_pixel;
	}
	else if (ubyte_val > max)
	{
	    ubyte_lookup_table[ubyte_val] = max_sat_pixel;
	}
	else
	{
	    val = mul * (float) (ubyte_val - min) + offset;
	    ubyte_lookup_table[ubyte_val] = pixel_values[(int) val];
	}
    }
}   /*  End Function setup_ubyte_lookup_table  */

static void setup_byte_lookup_table (int min, int max,
				     unsigned int num_pixels,
				     CONST unsigned char *pixel_values,
				     unsigned char blank_pixel,
				     unsigned char min_sat_pixel,
				     unsigned char max_sat_pixel)
/*  This routine will setup a lookup table for signed byte data which will
    convert from data values to pixel values.
    The minimum (lower intensity clipping point) value must be given by  min  .
    The maximum (upper intensity clipping point) value must be given by  max  .
    The number of pixel values to use must be given by  num_pixels  .
    The pixel values to use must be in the array pointed to by  pixel_values  .
    The pixel value to be used when the intensity value is an undefined value
    value must be given by  blank_pixel  .
    The pixel value to be used when the intensity value is below the minimum
    value must be given by  min_sat_pixel  .
    The pixel value to be used when the intensity value is above the maximum
    value must be given by  max_sat_pixel  .
    The routine will write the lookup table into the global array:
    byte_lookup_table  .
    The routine returns nothing.
*/
{
    int byte_val;
    float mul, val;
    float offset = 0.1;
    unsigned char *table;
    extern unsigned char byte_lookup_table[BYTE_TABLE_LENGTH];

    /*  Precompute pixel index  */
    table = byte_lookup_table + 128;
    *(table - 128) = blank_pixel;
    mul = (float) (num_pixels - 1) / (float) (max - min);
    /*  Loop through all possible byte values  */
    for (byte_val = -127; byte_val < 128; ++byte_val)
    {
	if (byte_val < min)
	{
	    *(table + byte_val) = min_sat_pixel;
	}
	else if (byte_val > max)
	{
	    *(table + byte_val) = max_sat_pixel;
	}
	else
	{
	    val = mul * (float) (byte_val - min) + offset;
	    *(table + byte_val) = pixel_values[(int) val];
	}
    }
}   /*  End Function setup_byte_lookup_table  */

static double *alloc_values_buffer (unsigned int num_values)
/*  This routine will allocate a buffer space for generic data values.
    The number of (DCOMPLEX) values to allocate must be given by  num_values  .
    The routine will return a pointer to the buffer space on success,
    else it returns NULL. The buffer is global and must NOT be deallocated.
*/
{
    static unsigned int value_buf_len = 0;
    static double *values = NULL;
    static char function_name[] = "alloc_values_buffer";

    /*  Make sure value buffer is big enough  */
    if (value_buf_len < num_values)
    {
	/*  Buffer too small  */
	if (values != NULL)
	{
	    m_free ( (char *) values );
	}
	value_buf_len = 0;
	if ( ( values = (double *) m_alloc (sizeof *values * 2 * num_values)
	      ) == NULL )
	{
	    m_error_notify (function_name, "values buffer");
	    return (NULL);
	}
	value_buf_len = num_values;
    }
    return (values);
}   /*  End Function alloc_values_buffer  */
