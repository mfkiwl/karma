/*LINTLIBRARY*/
/*  ps.c

    This code provides PostScript output for Intelligent Arrays.

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

/*  This file contains all routines needed for converting Intelligent Array
  images to PostScript.


    Written by      Richard Gooch   10-MAY-1994

    Updated by      Richard Gooch   20-MAY-1994: Took account of changes to
  psw_  package.

    Updated by      Richard Gooch   22-MAY-1994: Made use of
  psw_pseudocolour_image  .

    Last updated by Richard Gooch   26-NOV-1994: Moved to  packages/iarray/ps.c


*/
#include <stdio.h>
#include <math.h>
#include <karma.h>
#include <karma_iarray.h>
#include <karma_psw.h>
#include <karma_ds.h>
#include <karma_m.h>
#include <karma_a.h>


#define VERIFY_IARRAY(array) if (array == NULL) \
{(void) fprintf (stderr, "NULL iarray passed\n"); \
 a_prog_bug (function_name); }
#ifdef dummy
if ( (*array).magic_number != MAGIC_NUMBER ) \
{(void) fprintf (stderr, "Invalid iarray\n"); \
 a_prog_bug (function_name); }
#endif


/*  Private functions  */


/*  Public routines follow  */

/*PUBLIC_FUNCTION*/
flag iarray_write_mono_ps (image, pspage, xstart, ystart, xend, yend, iscale)
/*  This routine will convert a 2-dimensional Intelligent Array to monochrome
    PostScript. The routine does NOT write PostScript headers or tails.
    The Intelligent Array must be given by  image  .
    The PostScriptPage object must be given by  pspage  .
    The x starting point (scaled from 0.0 to 1.0) must be given by  xstart  .
    The y starting point (scaled from 0.0 to 1.0) must be given by  ystart  .
    The x ending point (scaled from 0.0 to 1.0) must be given by  xend  .
    The y ending point (scaled from 0.0 to 1.0) must be given by  yend  .
    If the input Intelligent Array is of type K_UBYTE and the value of  iscale
    is  FALSE, the images values will be unscaled prior to PostScript
    conversion (0 = black, 255 = white), otherwise (min = black, max = white).
    The routine returns TRUE on success, else it returns FALSE.
*/
iarray image;
PostScriptPage pspage;
double xstart;
double ystart;
double xend;
double yend;
flag iscale;
{
    iarray ubarray;
    flag retval;
    double min, max;
    double scale[2], offset[2];
    static char function_name[] = "iarray_write_mono_ps";

    VERIFY_IARRAY (image);
    FLAG_VERIFY (iscale);
    if (iarray_num_dim (image) != 2)
    {
	(void) fprintf (stderr, "Intelligent Array must be 2-dimensional\n");
	a_prog_bug (function_name);
    }
    if ( !iscale && (iarray_type (image) == K_UBYTE) )
    {
	ubarray = image;
    }
    else
    {
	if (iarray_min_max (image, CONV1_REAL, &min, &max)
	    != TRUE) return (FALSE);
	if ( ( ubarray = iarray_create_2D (iarray_dim_length (image, 0),
					   iarray_dim_length (image, 1),
					   K_UBYTE) ) == NULL )
	{
	    m_error_notify (function_name, "unsigned byte iarray");
	    return (FALSE);
	}
	scale[0] = 255.0 / (max - min);
	scale[1] = 0.0;
	offset[0] = -255.0 * min / (max - min);
	offset[1] = 0.0;
	if (iarray_scale_and_offset (ubarray, image, scale, offset, FALSE)
	    != TRUE)
	{
	    iarray_dealloc (ubarray);
	    return (FALSE);
	}
    }
    retval = psw_mono_image (pspage, (unsigned char *) (*ubarray).data,
			     iarray_dim_length (ubarray, 1),
			     iarray_dim_length (ubarray, 0),
			     (*ubarray).offsets[1], (*ubarray).offsets[0],
			     (unsigned char *) NULL,
			     xstart, ystart, xend, yend);
    if (ubarray != image) iarray_dealloc (ubarray);
    return (retval);
}   /*  End Function iarray_write_mono_ps  */

/*PUBLIC_FUNCTION*/
flag iarray_write_pseudocolour_ps (image, pspage, xstart, ystart, xend, yend,
				   cmap, cmap_size)
/*  This routine will convert a 2-dimensional Intelligent Array to colour
    PostScript. The routine does NOT write PostScript headers or tails.
    The Intelligent Array must be given by  image  .
    The PostScriptPage object must be given by  pspage  .
    The x starting point (scaled from 0.0 to 1.0) must be given by  xstart  .
    The y starting point (scaled from 0.0 to 1.0) must be given by  ystart  .
    The x ending point (scaled from 0.0 to 1.0) must be given by  xend  .
    The y ending point (scaled from 0.0 to 1.0) must be given by  yend  .
    The colourmap must be pointed to by  cmap  .This must be the same format as
    returned by  ds_cmap_find_colourmap  .
    The size of the colourmap must be given by  cmap_size  .The maximum size is
    256.
    The routine returns TRUE on success, else it returns FALSE.
*/
iarray image;
PostScriptPage pspage;
double xstart;
double ystart;
double xend;
double yend;
unsigned short *cmap;
unsigned int cmap_size;
{
    iarray ubarray;
    flag retval;
    unsigned int count;
    unsigned int xlen, ylen;
    double min, max;
    double scale[2], offset[2];
    unsigned char imap_red[256], imap_green[256], imap_blue[256];
    static char function_name[] = "iarray_write_pseudocolour_ps";

    VERIFY_IARRAY (image);
    if (cmap == NULL)
    {
	(void) fprintf (stderr, "NULL colourmap pointer passed\n");
	a_prog_bug (function_name);
    }
    if (cmap_size < 1)
    {
	(void) fprintf (stderr, "Colourmap size of 0 passed\n");
	a_prog_bug (function_name);
    }
    if (cmap_size > 256)
    {
	(void) fprintf (stderr, "Colourmap size over 256 passed\n");
	a_prog_bug (function_name);
    }
    xlen = iarray_dim_length (image, 1);
    ylen = iarray_dim_length (image, 0);
    if (iarray_min_max (image, CONV1_REAL, &min, &max) != TRUE) return (FALSE);
    if ( ( ubarray = iarray_create_2D (ylen, xlen, K_UBYTE) ) == NULL )
    {
	m_error_notify (function_name, "unsigned byte iarray");
	return (FALSE);
    }
    scale[0] = (double) (cmap_size - 1) / (max - min);
    scale[1] = 0.0;
    offset[0] = -(double) (cmap_size - 1) * min / (max - min);
    offset[1] = 0.0;
    if (iarray_scale_and_offset (ubarray, image, scale, offset, FALSE) != TRUE)
    {
	iarray_dealloc (ubarray);
	return (FALSE);
    }
    for (count = 0; count < cmap_size; ++count)
    {
	imap_red[count] = (int) cmap[count * 3] >> 8;
	imap_green[count] = (int) cmap[count * 3 + 1] >> 8;
	imap_blue[count] = (int) cmap[count * 3 + 2] >> 8;
    }
    retval = psw_pseudocolour_image (pspage,
				     (unsigned char *) (*ubarray).data,
				     xlen, ylen,
				     (*ubarray).offsets[1],
				     (*ubarray).offsets[0],
				     imap_red, imap_green, imap_blue,
				     xstart, ystart, xend, yend);
    iarray_dealloc (ubarray);
    return (retval);
}   /*  End Function iarray_write_pseudocolour_ps  */

/*PUBLIC_FUNCTION*/
flag iarray_write_rgb_ps (image_red, image_green, image_blue, pspage,
			  xstart, ystart, xend, yend)
/*  This routine will convert a 2-dimensional Intelligent Array to colour
    PostScript. The routine does NOT write PostScript headers or tails.
    The red component Intelligent Array must be given by  image_red  .
    The green component Intelligent Array must be given by  image_green  .
    The blue component Intelligent Array must be given by  image_blue  .
    Each of these Intelligent Arrays must be of type K_UBYTE.
    The PostScriptPage object must be given by  pspage  .
    The x starting point (scaled from 0.0 to 1.0) must be given by  xstart  .
    The y starting point (scaled from 0.0 to 1.0) must be given by  ystart  .
    The x ending point (scaled from 0.0 to 1.0) must be given by  xend  .
    The y ending point (scaled from 0.0 to 1.0) must be given by  yend  .
    The routine returns TRUE on success, else it returns FALSE.
*/
iarray image_red;
iarray image_green;
iarray image_blue;
PostScriptPage pspage;
double xstart;
double ystart;
double xend;
double yend;
{
    unsigned int xlen, ylen;
    static char function_name[] = "iarray_write_rgb_ps";

    VERIFY_IARRAY (image_red);
    VERIFY_IARRAY (image_green);
    VERIFY_IARRAY (image_blue);
    if (iarray_num_dim (image_red) != 2)
    {
	(void) fprintf (stderr, "Red image is not 2-dimensional\n");
	a_prog_bug (function_name);
    }
    if (iarray_num_dim (image_green) != 2)
    {
	(void) fprintf (stderr, "Green image is not 2-dimensional\n");
	a_prog_bug (function_name);
    }
    if (iarray_num_dim (image_blue) != 2)
    {
	(void) fprintf (stderr, "Blue image is not 2-dimensional\n");
	a_prog_bug (function_name);
    }
    if (iarray_type (image_red) != K_UBYTE)
    {
	(void) fprintf (stderr, "Red image is not of type K_UBYTE\n");
	a_prog_bug (function_name);
    }
    if (iarray_type (image_green) != K_UBYTE)
    {
	(void) fprintf (stderr, "Green image is not of type K_UBYTE\n");
	a_prog_bug (function_name);
    }
    if (iarray_type (image_blue) != K_UBYTE)
    {
	(void) fprintf (stderr, "Blue image is not of type K_UBYTE\n");
	a_prog_bug (function_name);
    }
    xlen = iarray_dim_length (image_red, 1);
    ylen = iarray_dim_length (image_red, 0);
    if (iarray_dim_length (image_green, 1) != xlen)
    {
	(void) fprintf (stderr,
			"Green xlen: %u is not equal to red xlen: %u\n",
			iarray_dim_length (image_green, 1), xlen);
	a_prog_bug (function_name);
    }
    if (iarray_dim_length (image_green, 0) != xlen)
    {
	(void) fprintf (stderr,
			"Green ylen: %u is not equal to red ylen: %u\n",
			iarray_dim_length (image_green, 0), ylen);
	a_prog_bug (function_name);
    }
    if (iarray_dim_length (image_blue, 1) != xlen)
    {
	(void) fprintf (stderr,
			"Blue xlen: %u is not equal to red xlen: %u\n",
			iarray_dim_length (image_blue, 1), xlen);
	a_prog_bug (function_name);
    }
    if (iarray_dim_length (image_blue, 0) != xlen)
    {
	(void) fprintf (stderr,
			"Blue ylen: %u is not equal to red ylen: %u\n",
			iarray_dim_length (image_blue, 0), ylen);
	a_prog_bug (function_name);
    }
    return ( psw_rgb_image (pspage,
			    (unsigned char *) (*image_red).data,
			    (unsigned char *) (*image_green).data,
			    (unsigned char *) (*image_blue).data,
			    xlen, ylen,
			    (*image_red).offsets[1],
			    (*image_red).offsets[0],
			    (*image_green).offsets[1],
			    (*image_green).offsets[0],
			    (*image_blue).offsets[1],
			    (*image_blue).offsets[0],
			    0, xstart, ystart, xend, yend) );
}   /*  End Function iarray_write_rgb_ps  */