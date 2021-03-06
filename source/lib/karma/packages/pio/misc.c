/*LINTLIBRARY*/
/*  misc.c

    This code provides portable data IO routines.

    Copyright (C) 1992-1996  Richard Gooch

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

/*  This file contains all routines needed for the portable transfer of data to
  and from channel objects.


    Written by      Richard Gooch   13-SEP-1992

    Updated by      Richard Gooch   8-OCT-1992

    Updated by      Richard Gooch   6-JAN-1993: Changed from  #include "*.h"
  to  #include <*.h>

    Updated by      Richard Gooch   13-FEB-1993: Added  pio_write64  and
  pio_read64

    Updated by      Richard Gooch   15-APR-1994: Improved documentation for
  string reading and writing routines.

    Updated by      Richard Gooch   21-MAY-1994: Added  #include <karma_ch.h>

    Updated by      Richard Gooch   7-JUL-1994: Added support for float and
  double read and write for Alpha.

    Updated by      Richard Gooch   6-AUG-1994: Moved to full ASCII function
  declaration syntax. Had to do this because of the confused Dec Alpha compiler

    Updated by      Richard Gooch   3-NOV-1994: Switched to OS_ and MACHINE_
  macros for machine determination.

    Updated by      Richard Gooch   26-NOV-1994: Moved to  packages/pio/misc.c

    Updated by      Richard Gooch   7-DEC-1994: Stripped declaration of  errno
  and added #include <errno.h>

    Updated by      Richard Gooch   27-FEB-1995: Changed to HAS_IEEE macro.

    Updated by      Richard Gooch   5-MAY-1995: Placate SGI compiler.

    Updated by      Richard Gooch   8-JUN-1995: Updated support for crayPVP.

    Updated by      Richard Gooch   31-JUL-1995: Created
  <pio_read_float_nantrap> and <pio_read_double_nantrap>.

    Updated by      Richard Gooch   22-AUG-1995: Extended NaN list to include
  all 1s filled.

    Updated by      Richard Gooch   12-APR-1996: Changed to new documentation
  format.

    Updated by      Richard Gooch   5-SEP-1996: Created <pio_read_floats>.

    Last updated by Richard Gooch   7-SEP-1996: Created <pio_read_doubles>.


*/
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <karma.h>
#include <karma_pio.h>
#include <karma_ds.h>
#include <karma_ch.h>
#include <karma_m.h>
#include <karma_p.h>
#include <karma_a.h>
#include <os.h>


#define BUF_SIZE 65536


/*  Public routines follow  */

/*PUBLIC_FUNCTION*/
flag pio_write64 (Channel channel, unsigned long data)
/*  [SUMMARY] Write 64 bits of data to a channel object.
    <channel> The channel object.
    <data> The data. This must be in host natural byte order. The data will be
    converted to network byte order prior to writing.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[8];
    extern char *sys_errlist[];

    if (p_write_buf64 (buffer, data) != TRUE)
    {
	return (FALSE);
    }
    if (ch_write (channel, buffer, 8) < 8)
    {
	fprintf (stderr, "Error writing 8 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function pio_write64  */

/*PUBLIC_FUNCTION*/
flag pio_read64 (Channel channel, unsigned long *data)
/*  [SUMMARY] Read 64 bits of data from a channel object.
    <channel> The channel object.
    <data> The data will be written here. This will be in host natural byte
    order. The data will be converted from network byte order after reading.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[8];
    extern char *sys_errlist[];

    if (ch_read (channel, buffer, 8) < 8)
    {
	fprintf (stderr, "Error reading 8 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return ( p_read_buf64 (buffer, data) );
}   /*  End Function pio_read64  */

/*PUBLIC_FUNCTION*/
flag pio_write32 (Channel channel, unsigned long data)
/*  [SUMMARY] Write 32 bits of data to a channel object.
    <channel> The channel object.
    <data> The data. This must be in host natural byte order. The data will be
    converted to network byte order prior to writing.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[4];
    extern char *sys_errlist[];

    if (p_write_buf32 (buffer, data) != TRUE)
    {
	return (FALSE);
    }
    if (ch_write (channel, buffer, 4) < 4)
    {
	fprintf (stderr, "Error writing 4 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function pio_write32  */

/*PUBLIC_FUNCTION*/
flag pio_read32 (Channel channel, unsigned long *data)
/*  [SUMMARY] Read 32 bits of data from a channel object.
    <channel> The channel object.
    <data> The data will be written here. This will be in host natural byte
    order. The data will be converted from network byte order after reading.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[4];
    extern char *sys_errlist[];

    if (ch_read (channel, buffer, 4) < 4)
    {
	fprintf (stderr, "Error reading 4 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return ( p_read_buf32 (buffer, data) );
}   /*  End Function pio_read32  */

/*PUBLIC_FUNCTION*/
flag pio_write16 (Channel channel, unsigned long data)
/*  [SUMMARY] Write 16 bits of data to a channel object.
    <channel> The channel object.
    <data> The data. This must be in host natural byte order. The data will be
    converted to network byte order prior to writing.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[2];
    extern char *sys_errlist[];

    if (p_write_buf16 (buffer, data) != TRUE)
    {
	return (FALSE);
    }
    if (ch_write (channel, buffer, 2) < 2)
    {
	fprintf (stderr, "Error writing 2 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function pio_write16  */

/*PUBLIC_FUNCTION*/
flag pio_read16 (Channel channel, unsigned long *data)
/*  [SUMMARY] Read 16 bits of data from a channel object.
    <channel> The channel object.
    <data> The data will be written here. This will be in host natural byte
    order. The data will be converted from network byte order after reading.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[2];
    extern char *sys_errlist[];

    if (ch_read (channel, buffer, 2) < 2)
    {
	fprintf (stderr, "Error reading 2 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return ( p_read_buf16 (buffer, data) );
}   /*  End Function pio_read16  */

/*PUBLIC_FUNCTION*/
flag pio_write_float (Channel channel, float data)
/*  [SUMMARY] Write a floating point number to a channel object.
    <channel> The channel object.
    <data> The data. This must be in host natural format. The data will be
    converted to IEEE network format prior to writing.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[NET_FLOAT_SIZE];
    extern char *sys_errlist[];

    if ( !p_write_buf_float (buffer, data) ) return (FALSE);
    if (ch_write (channel, buffer, NET_FLOAT_SIZE) < NET_FLOAT_SIZE)
    {
	fprintf (stderr, "Error writing %d bytes of data\t%s\n",
		 NET_FLOAT_SIZE, sys_errlist[errno]);
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function pio_write_float  */

/*PUBLIC_FUNCTION*/
flag pio_read_float (Channel channel, float *data)
/*  [SUMMARY] Read a floating point number from a channel object.
    <channel> The channel object.
    <data> The data will be written here. This will be written in host natural
    format. The data will be converted from IEEE network format prior to
    writing.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[NET_FLOAT_SIZE];
    extern char *sys_errlist[];

    if (ch_read (channel, buffer, NET_FLOAT_SIZE) < NET_FLOAT_SIZE)
    {
	fprintf (stderr, "Error reading: %d bytes of data\t%s\n",
		 NET_FLOAT_SIZE, sys_errlist[errno]);
	return (FALSE);
    }
    return ( p_read_buf_float (buffer, data) );
}   /*  End Function pio_read_float  */

/*PUBLIC_FUNCTION*/
flag pio_read_float_nantrap (Channel channel, float *data, flag *was_nan)
/*  [SUMMARY] Read floating point data from a channel object, trapping NaNs.
    [PURPOSE] This routine will read a floating point number from a channel
    object and will trap IEEE Not-A-Number (NaN) values.
    <channel> The channel object.
    <data> The data will be written here. This will be written in host natural
    format. The data will be converted from IEEE network format prior to
    writing.
    <was_nan> The value TRUE will be written here if the value was NaN, else
    FALSE is written here.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    uaddr num_nan;
    char buffer[NET_FLOAT_SIZE];
    extern char *sys_errlist[];

    if (ch_read (channel, buffer, NET_FLOAT_SIZE) < NET_FLOAT_SIZE)
    {
	fprintf (stderr, "Error reading: %d bytes of data\t%s\n",
		 NET_FLOAT_SIZE, sys_errlist[errno]);
	return (FALSE);
    }
    if ( !p_read_buf_floats (buffer, 1, data, &num_nan) ) return (FALSE);
    *was_nan = (num_nan > 0) ? TRUE : FALSE;
    return (TRUE);
}   /*  End Function pio_read_float_nantrap  */

/*EXPERIMENTAL_FUNCTION*/
uaddr pio_read_floats (Channel channel, uaddr num_values, float *data,
		       uaddr *num_nan)
/*  [SUMMARY] Read floating point data from a channel object, trapping NaNs.
    [PURPOSE] This routine will read many floating point numbers from a channel
    object and will optionally trap IEEE Not-A-Number (NaN) values. Trapped
    NaNs are converted to the TOOBIG value.
    <channel> The channel object.
    <num_values> The number of values to read.
    <data> The data will be written here. This will be written in host natural
    format. The data will be converted from IEEE network format prior to
    writing.
    <num_nan> The number of NaN values found will be written here. If this is
    NULL then NaN values are not trapped.
    [RETURNS] The number of values read and converted.
*/
{
    flag block_transfer, swap_transfer;
    uaddr values_converted, block_length;
    uaddr num_nan_local = 0;
    char buffer[BUF_SIZE * NET_FLOAT_SIZE];
    extern char *sys_errlist[];
    static char function_name[] = "pio_read_floats";

    if ( (channel == NULL) || (data == NULL) )
    {
	fprintf (stderr, "NULL pointer(s) passed\n");
	a_prog_bug (function_name);
    }
    block_transfer = ds_can_transfer_element_as_block (K_FLOAT);
    swap_transfer = ds_can_swaptransfer_element (K_FLOAT);
    if (num_nan == NULL)
    {
	/*  No NaN trap: try to speed things up  */
	if (block_transfer)
	{
	    if (ch_read (channel, (char *) data, num_values * NET_FLOAT_SIZE) <
		num_values * NET_FLOAT_SIZE)
	    {
		fprintf (stderr, "%s: error reading data\t%s\n",
			 function_name, sys_errlist[errno]);
		return (0);
	    }
	    return (num_values);
	}
	if (swap_transfer)
	{
	    if (ch_read_and_swap_blocks (channel, (char *) data, num_values,
					 NET_FLOAT_SIZE)
		< num_values * NET_FLOAT_SIZE)
	    {
		fprintf (stderr, "%s: error reading data\t%s\n",
			 function_name, sys_errlist[errno]);
		return (0);
	    }
	    return (num_values);
	}
    }
    /*  Must trap for NaNs or sizes are different  */
    for (values_converted = 0; values_converted < num_values;
	 values_converted += block_length, data += block_length)
    {
	block_length = num_values - values_converted;
	if (block_length > BUF_SIZE) block_length = BUF_SIZE;
	/*  Read a block of data  */
	if (ch_read (channel, buffer, block_length * NET_FLOAT_SIZE) <
	    block_length * NET_FLOAT_SIZE)
	{
	    fprintf (stderr, "%s: error reading data\t%s\n",
		     function_name, sys_errlist[errno]);
	    return (values_converted);
	}
	/*  Convert and possibly NaN trap  */
	if ( !p_read_buf_floats (buffer, block_length, data, num_nan) )
	{
	    return (values_converted);
	}
	if (num_nan != NULL) num_nan_local += *num_nan;
    }
    if (num_nan != NULL) *num_nan = num_nan_local;
    return (values_converted);
}   /*  End Function pio_read_floats  */

/*PUBLIC_FUNCTION*/
flag pio_write_double (Channel channel, double data)
/*  [SUMMARY] Write double precision floating point data to a channel object.
    <channel> The channel object.
    <data> The data. This must be in host natural format. The data will be
    converted to IEEE network format prior to writing.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[NET_DOUBLE_SIZE];
    extern char *sys_errlist[];

    if ( !p_write_buf_double (buffer, data) ) return (FALSE);
    if (ch_write (channel, buffer, NET_DOUBLE_SIZE) < NET_DOUBLE_SIZE)
    {
	fprintf (stderr, "Error writing %d bytes of data\t%s\n",
		 NET_DOUBLE_SIZE, sys_errlist[errno]);
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function pio_write_double  */

/*PUBLIC_FUNCTION*/
flag pio_read_double (Channel channel, double *data)
/*  [SUMMARY] Read double precision floating point data from a channel object.
    <channel> The channel object.
    <data> The data will be written here. This will be written in host natural
    format. The data will be converted from IEEE network format prior to
    writing.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[NET_DOUBLE_SIZE];
    extern char *sys_errlist[];

    if (ch_read (channel, buffer, NET_DOUBLE_SIZE) < NET_DOUBLE_SIZE)
    {
	fprintf (stderr, "Error reading: %d bytes of data\t%s\n",
		 NET_DOUBLE_SIZE, sys_errlist[errno]);
	return (FALSE);
    }
    return ( p_read_buf_double (buffer, data) );
}   /*  End Function pio_read_double  */

/*PUBLIC_FUNCTION*/
flag pio_read_double_nantrap (Channel channel, double *data, flag *was_nan)
/*  [SUMMARY] Read floating point data from a channel object, trapping NaNs.
    [PURPOSE] This routine will read a double precision floating point number
    from a channel object and will trap IEEE Not-A-Number (NaN) values.
    <channel> The channel object.
    <data> The data will be written here. This will be written in host natural
    format. The data will be converted from IEEE network format prior to
    writing.
    <was_nan> The value TRUE will be written here if the value was NaN, else
    FALSE is written here.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    uaddr num_nan;
    char buffer[NET_DOUBLE_SIZE];
    extern char *sys_errlist[];

    if (ch_read (channel, buffer, NET_DOUBLE_SIZE) < NET_DOUBLE_SIZE)
    {
	fprintf (stderr, "Error reading: %d bytes of data\t%s\n",
		 NET_DOUBLE_SIZE, sys_errlist[errno]);
	return (FALSE);
    }
    if ( !p_read_buf_doubles (buffer, 1, data, &num_nan) ) return (FALSE);
    *was_nan = (num_nan > 0) ? TRUE : FALSE;
    return (TRUE);
}   /*  End Function pio_read_double_nantrap  */

/*EXPERIMENTAL_FUNCTION*/
uaddr pio_read_doubles (Channel channel, uaddr num_values, double *data,
			uaddr *num_nan)
/*  [SUMMARY] Read double float data from a channel object, trapping NaNs.
    [PURPOSE] This routine will read many double precision floating point
    numbers from a channel object and will optionally trap IEEE Not-A-Number
    (NaN) values. Trapped NaNs are converted to the TOOBIG value.
    <channel> The channel object.
    <num_values> The number of values to read.
    <data> The data will be written here. This will be written in host natural
    format. The data will be converted from IEEE network format prior to
    writing.
    <num_nan> The number of NaN values found will be written here. If this is
    NULL then NaN values are not trapped.
    [RETURNS] The number of values read and converted.
*/
{
    flag block_transfer, swap_transfer;
    uaddr values_converted, block_length;
    uaddr num_nan_local = 0;
    char buffer[BUF_SIZE * NET_DOUBLE_SIZE];
    extern char *sys_errlist[];
    static char function_name[] = "pio_read_doubles";

    if ( (channel == NULL) || (data == NULL) )
    {
	fprintf (stderr, "NULL pointer(s) passed\n");
	a_prog_bug (function_name);
    }
    block_transfer = ds_can_transfer_element_as_block (K_DOUBLE);
    swap_transfer = ds_can_swaptransfer_element (K_DOUBLE);
    if (num_nan == NULL)
    {
	/*  No NaN trap: try to speed things up  */
	if (block_transfer)
	{
	    if (ch_read (channel, (char *) data, num_values * NET_DOUBLE_SIZE)
		< num_values * NET_DOUBLE_SIZE)
	    {
		fprintf (stderr, "%s: error reading data\t%s\n",
			 function_name, sys_errlist[errno]);
		return (0);
	    }
	    return (num_values);
	}
	if (swap_transfer)
	{
	    if (ch_read_and_swap_blocks (channel, (char *) data, num_values,
					 NET_DOUBLE_SIZE)
		< num_values * NET_DOUBLE_SIZE)
	    {
		fprintf (stderr, "%s: error reading data\t%s\n",
			 function_name, sys_errlist[errno]);
		return (0);
	    }
	    return (num_values);
	}
    }
    /*  Must trap for NaNs or sizes are different  */
    for (values_converted = 0; values_converted < num_values;
	 values_converted += block_length, data += block_length)
    {
	block_length = num_values - values_converted;
	if (block_length > BUF_SIZE) block_length = BUF_SIZE;
	/*  Read a block of data  */
	if (ch_read (channel, buffer, block_length * NET_DOUBLE_SIZE) <
	    block_length * NET_DOUBLE_SIZE)
	{
	    fprintf (stderr, "%s: error reading data\t%s\n",
		     function_name, sys_errlist[errno]);
	    return (values_converted);
	}
	/*  Convert and possibly NaN trap  */
	if ( !p_read_buf_doubles (buffer, block_length, data, num_nan) )
	{
	    return (values_converted);
	}
	if (num_nan != NULL) num_nan_local += *num_nan;
    }
    if (num_nan != NULL) *num_nan = num_nan_local;
    return (values_converted);
}   /*  End Function pio_read_doubles  */

/*PUBLIC_FUNCTION*/
flag pio_write32s (Channel channel, long data)
/*  [SUMMARY] Write 32 bits of signed data to a channel object.
    <channel> The channel object.
    <data> The data. This must be in host natural byte order. The data will be
    converted to network byte order prior to writing.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[4];
    extern char *sys_errlist[];
/*
    static char function_name[] = "pio_write32s";
*/

    if (p_write_buf32s (buffer, data) != TRUE)
    {
	return (FALSE);
    }
    if (ch_write (channel, buffer, 4) < 4)
    {
	fprintf (stderr, "Error writing 4 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function pio_write32s  */

/*PUBLIC_FUNCTION*/
flag pio_read32s (Channel channel, long *data)
/*  [SUMMARY] Read 32 bits of signed data from a channel object.
    <channel> The channel object.
    <data> The data will be written here. This will be in host natural byte
    order. The data will be converted from network byte order after reading.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[4];
    extern char *sys_errlist[];
/*
    static char function_name[] = "pio_read32s";
*/

    if (ch_read (channel, buffer, 4) < 4)
    {
	fprintf (stderr, "Error reading 4 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return ( p_read_buf32s (buffer, data) );
}   /*  End Function pio_read32s  */

/*PUBLIC_FUNCTION*/
flag pio_write16s (Channel channel, long data)
/*  [SUMMARY] Write 16 bits of signed data to a channel object.
    <channel> The channel object.
    <data> The data. This must be in host natural byte order. The data will be
    converted to network byte order prior to writing.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[2];
    extern char *sys_errlist[];
/*
    static char function_name[] = "pio_write16s";
*/

    if (p_write_buf16s (buffer, data) != TRUE)
    {
	return (FALSE);
    }
    if (ch_write (channel, buffer, 2) < 2)
    {
	fprintf (stderr, "Error writing 2 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function pio_write16s  */

/*PUBLIC_FUNCTION*/
flag pio_read16s (Channel channel, long *data)
/*  [SUMMARY] Read 16 bits of signed data from a channel object.
    <channel> The channel object.
    <data> The data will be written here. This will be in host natural byte
    order. The data will be converted from network byte order after reading.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    char buffer[2];
    extern char *sys_errlist[];
/*
    static char function_name[] = "pio_read16s";
*/

    if (ch_read (channel, buffer, 2) < 2)
    {
	fprintf (stderr, "Error reading 2 bytes of data\t%s\n",
		 sys_errlist[errno]);
	return (FALSE);
    }
    return ( p_read_buf16s (buffer, data) );
}   /*  End Function pio_read16s  */

/*PUBLIC_FUNCTION*/
flag pio_write_swap (Channel channel, CONST char *data, unsigned int length)
/*  [SUMMARY] Swap and write bytes to a channel object.
    <channel> The channel object.
    <data> A pointer to the data bytes.
    <length> The number of bytes to swap and then write.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    unsigned int byte_count;
    extern char *sys_errlist[];
    static unsigned int buf_len = 0;
    static char *buffer = NULL;
    static char function_name[] = "pio_write_swap";

    /*  Test to see if old buffer is big enough  */
    if (length > buf_len)
    {
	/*  Old buffer too small  */
	if (buffer != NULL)
	{
	    /*  Deallocate old buffer  */
	    m_free (buffer);
	}
	if ( ( buffer = m_alloc (length) ) == NULL )
	{
	    m_error_notify (function_name, "swap buffer");
	    return (FALSE);
	}
	buf_len = length;
    }
    /*  Copy and swap bytes  */
    for (byte_count = 0, --data; byte_count < length; ++byte_count)
    {
	buffer[byte_count] = data[length - byte_count];
    }
    /*  Write buffer  */
    if (ch_write (channel, buffer, length) < length)
    {
	fprintf (stderr, "Error writing: %u bytes of data\t%s\n",
		 length, sys_errlist[errno]);
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function pio_write_swap  */

/*PUBLIC_FUNCTION*/
flag pio_read_swap (Channel channel, char *data, unsigned int length)
/*  [SUMMARY] Read from a channel object and swap bytes.
    <channel> The channel object.
    <data> The data bytes will be written here.
    <length> The number of bytes to read and then swap.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    unsigned int byte_count;
    extern char *sys_errlist[];
    static unsigned int buf_len = 0;
    static char *buffer = NULL;
    static char function_name[] = "pio_read_swap";

    /*  Test to see if old buffer is big enough  */
    if (length > buf_len)
    {
	/*  Old buffer too small  */
	if (buffer != NULL)
	{
	    /*  Deallocate old buffer  */
	    m_free (buffer);
	}
	if ( ( buffer = m_alloc (length) ) == NULL )
	{
	    m_error_notify (function_name, "swap buffer");
	    return (FALSE);
	}
	buf_len = length;
    }
    /*  Read buffer  */
    if (ch_read (channel, buffer, length) < length)
    {
	fprintf (stderr, "Error reading: %u bytes of data\t%s\n",
		 length, sys_errlist[errno]);
	return (FALSE);
    }
    /*  Copy and swap bytes  */
    for (byte_count = 0, --data; byte_count < length; ++byte_count)
    {
	data[length - byte_count] = buffer[byte_count];
    }
    return (TRUE);
}   /*  End Function pio_read_swap  */

/*PUBLIC_FUNCTION*/
flag pio_write_string (Channel channel, CONST char *string)
/*  [SUMMARY] Write a string to a channel object.
    <channel> The channel object.
    <string> The string. If this is NULL, a zero length string is written.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    unsigned long length;
/*
    static char function_name[] = "pio_write_string";
*/

    if (string == NULL) length = 0;
    else length = strlen (string);
    if ( !pio_write32 (channel, length) ) return (FALSE);
    if (length < 1) return (TRUE);
    if (ch_write (channel, string, (unsigned int) length) < length)
    {
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function pio_write_string  */

/*PUBLIC_FUNCTION*/
char *pio_read_string (Channel channel, unsigned int *length)
/*  [SUMMARY] Read a string from a channel object.
    <channel> The channel object.
    <length> The length of the string will be written here. If this is NULL,
    nothing is written here.
    [RETURNS] A pointer to a dynamically allocated string on success,
    else NULL. If a zero length string was read, the returned buffer is one
    character long and contains the '\0' character.
*/
{
    unsigned long len;
    char *string;
    extern char *sys_errlist[];
    static char function_name[] = "pio_read_string";

    if ( !pio_read32 (channel, &len) )
    {
	a_func_abort (function_name, "Error reading string length");
	return (NULL);
    }
    if ( ( string = m_alloc (len + 1) ) == NULL )
    {
	m_error_notify (function_name, "string");
	return (NULL);
    }
    if (len < 1)
    {
	/*  Empty string  */
	string[0] = '\0';
	if (length != NULL)
	{
	    *length = len;
	}
	return (string);
    }
    /*  Read string  */
    if (ch_read (channel, string, (unsigned int) len) < len)
    {
	fprintf (stderr, "Error reading: %u bytes\t%s\n",
		 (unsigned int) len, sys_errlist[errno]);
	a_func_abort (function_name, "could not read string");
	m_free (string);
	return (NULL);
    }
    string[len] = '\0';
    if (length != NULL)
    {
	*length = len;
    }
    return (string);
}   /*  End Function pio_read_string  */
