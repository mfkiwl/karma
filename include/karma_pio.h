/*  karma_pio.h

    Header for  pio_  package.

    Copyright (C) 1992,1993  Richard Gooch

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

    This include file contains all the definitions and function declarations
  needed to interface to the pio_ routines in the Karma library.


    Written by      Richard Gooch   13-SEP-1992

    Last updated by Richard Gooch   13-FEB-1993

*/

#ifndef KARMA_PIO_H
#define KARMA_PIO_H


#ifndef EXTERN_FUNCTION
#  include <c_varieties.h>
#endif
#include <karma_ch.h>

/*  File:   portable_io.c   */
EXTERN_FUNCTION (flag pio_write64, (Channel channel, unsigned long data) );
EXTERN_FUNCTION (flag pio_read64, (Channel channel, unsigned long *data) );
EXTERN_FUNCTION (flag pio_write32, (Channel channel, unsigned long data) );
EXTERN_FUNCTION (flag pio_read32, (Channel channel, unsigned long *data) );
EXTERN_FUNCTION (flag pio_write16, (Channel channel, unsigned long data) );
EXTERN_FUNCTION (flag pio_read16, (Channel channel, unsigned long *data) );
EXTERN_FUNCTION (flag pio_write_float, (Channel channel, float data) );
EXTERN_FUNCTION (flag pio_read_float, (Channel channel, float *data) );
EXTERN_FUNCTION (flag pio_write_double, (Channel channel, double data) );
EXTERN_FUNCTION (flag pio_read_double, (Channel channel, double *data) );
EXTERN_FUNCTION (flag pio_write32s, (Channel channel, long data) );
EXTERN_FUNCTION (flag pio_read32s, (Channel channel, long *data) );
EXTERN_FUNCTION (flag pio_write16s, (Channel channel, long data) );
EXTERN_FUNCTION (flag pio_read16s, (Channel channel, long *data) );
EXTERN_FUNCTION (flag pio_write_swap, (Channel channel, char *data,
				       unsigned int length) );
EXTERN_FUNCTION (flag pio_read_swap, (Channel channel, char *data,
				      unsigned int length) );
EXTERN_FUNCTION (flag pio_write_string, (Channel channel, char *string) );
EXTERN_FUNCTION (char *pio_read_string, (Channel channel,
					 unsigned int *length) );


#endif /*  KARMA_PIO_H  */
