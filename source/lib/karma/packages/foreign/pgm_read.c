/*LINTLIBRARY*/
/*  pgm_read.c

    This code provides a PGM read facility.

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

    This file contains the various utility routines for reading greyscale
    images in PGM format.


    Written by      Richard Gooch   3-DEC-1996: Copied from ppm_read.c

    Last updated by Richard Gooch   3-DEC-1996


*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <karma.h>
#include <karma_foreign.h>
#include <karma_chs.h>
#include <karma_ch.h>
#include <karma_ds.h>
#include <karma_ex.h>
#include <karma_m.h>
#include <karma_a.h>


/*  Declarations of private functions follow  */
STATIC_FUNCTION (unsigned int get_value,
		 (Channel channel, char *string, unsigned int length,
		  flag *newline) );


/*  Public functions follow  */

/*PUBLIC_FUNCTION*/
multi_array *foreign_pgm_read (Channel channel, ...)
/*  [SUMMARY] Read a colour image in PGM format from a channel.
    <channel> The channel to read from.
    [VARARGS] The optional attributes are given as pairs of attribute-key
    attribute-value pairs. This list must be terminated with FA_PGM_READ_END.
    See [<FOREIGN_ATT_PGM_READ>] for a list of defined attributes.
    [RETURNS] A pointer to the multi_array data structure on success, else NULL
*/
{
    flag binary, newline;
    int max_value;
    int width, height, y;
    int value;
    char ch;
    unsigned int att_key;
    unsigned int values_read;
    va_list argp;
    multi_array *multi_desc;
    char *array, *ptr, *p;
    char txt[STRING_LENGTH];
    uaddr lengths[2];
    extern char *sys_errlist[];
    static char function_name[] = "foreign_pgm_read";

    va_start (argp, channel);
    if ( channel == NULL)
    {
	fprintf (stderr, "NULL pointer passed\n");
	a_prog_bug (function_name);
    }
    /*  Process attributes  */
    while ( ( att_key = va_arg (argp, unsigned int) ) != FA_PGM_READ_END )
    {
	switch (att_key)
	{
	  default:
	    fprintf (stderr, "Unknown attribute key: %u\n", att_key);
	    a_prog_bug (function_name);
	    break;
	}
    }
    va_end (argp);
    /*  Read magic number  */
    if (ch_read (channel, txt, 3) < 3)
    {
	fprintf (stderr, "Error reading\t%s\n", sys_errlist[errno]);
	return (NULL);
    }
    if (strncmp (txt, "P2", 2) == 0) binary = FALSE;
    else if (strncmp (txt, "P5", 2) == 0) binary = TRUE;
    else
    {
	fprintf (stderr, "Input not of PGM format\n");
	return (NULL);
    }
    if ( !isspace (txt[2]) )
    {
	fprintf (stderr, "Input not of PGM format (whitespace missing)\n");
	return (NULL);
    }
    /*  Read width  */
    if (get_value (channel, txt, STRING_LENGTH, &newline) < 1)
    {
	fprintf (stderr, "Error reading width\t%s\n", sys_errlist[errno]);
	return (NULL);
    }
    if ( ( width = ex_int (txt, &p) ) < 1 )
    {
	fprintf (stderr, "Bad width: \"%s\"\n", txt);
	return (NULL);
    }
    /*  Read height  */
    if (get_value (channel, txt, STRING_LENGTH, &newline) < 1)
    {
	fprintf (stderr, "Error reading height\t%s\n", sys_errlist[errno]);
	return (NULL);
    }
    if ( ( height = ex_int (txt, &p) ) < 1 )
    {
	fprintf (stderr, "Bad height: \"%s\"\n", txt);
	return (NULL);
    }
    /*  Read max_value  */
    if (get_value (channel, txt, STRING_LENGTH, &newline) < 1)
    {
	fprintf (stderr, "Error reading max_value\t%s\n", sys_errlist[errno]);
	return (NULL);
    }
    if ( ( max_value = ex_int (txt, &p) ) < 1 )
    {
	fprintf (stderr, "Bad max_value: \"%s\"\n", txt);
	return (NULL);
    }
    lengths[0] = height;
    lengths[1] = width;
    if ( ( array = ds_easy_alloc_array (&multi_desc, 2, lengths,
					(CONST double *) NULL,
					(CONST double *) NULL,
					(CONST char **) NULL, K_UBYTE,
					NULL) ) == NULL )
    {
	return (NULL);
    }
    /*  Read the pixel data  */
    if (binary)
    {
	if (!newline)
	{
	    /*  Drain until the next line  */
	    ch = ' ';
	    while ( (ch != '\n') && (ch_read (channel, &ch, 1) == 1) );
	}
	for (y = height - 1; y >= 0; --y)
	{
	    /*  Read in one line  */
	    ptr = array + y * width;
	    if ( ( values_read = ch_read (channel, ptr, width) )
		 < width )
	    {
		fprintf (stderr, "Error reading: %u bytes, got: %u\t%s\n",
			 width, values_read, sys_errlist[errno]);
		ds_dealloc_multi (multi_desc);
		return (NULL);
	    }
	}
	return (multi_desc);
    }
    /*  Read ASCII  */
    for (y = height - 1; y >= 0; --y)
    {
	/*  Read in one line  */
	ptr = array + y * width;
	for (values_read = 0; values_read < width; ++values_read)
	{
	    if (get_value (channel, txt, STRING_LENGTH, &newline) < 1)
	    {
		ds_dealloc_multi (multi_desc);
		return (NULL);
	    }
	    if ( ( value = ex_int (txt, &p) ) < 0 )
	    {
		ds_dealloc_multi (multi_desc);
		return (NULL);
	    }
	    if (max_value != 255) value = value * 255 / max_value;
	    *(unsigned char *) (ptr + values_read) = value;
	}
    }
    return (multi_desc);
}   /*  End Function foreign_pgm_read  */


/*  Private functions follow  */

static unsigned int get_value (Channel channel, char *string,
			       unsigned int length, flag *newline)
/*  [PURPOSE] This routine will scan a channel object for a whitespace
    separated value. '#' is the comment character.
    <channel> The channel to read from.
    <string> The buffer to write the value to.
    <length> The size of the buffer (in bytes).
    <newline> The value TRUE is written here if a newline was read, else FALSE
    is written here.
    [RETURNS] The length of the string scanned on success, else 0.
*/
{
    unsigned int char_pos;
    char ch;
    static char function_name[] = "chs_get_value";

    char_pos = 0;
    *newline = FALSE;
    while (ch_read (channel, string + char_pos, 1) == 1)
    {
	if (string[char_pos] == '#')
	{
	    /*  Skip until next newline and count as whitespace  */
	    ch = '\0';
	    while ( (ch != '\n') && (ch_read (channel, &ch, 1) == 1) );
	    if (ch != '\n') return (char_pos);
	    string[char_pos] = '\n';
	}
	/*  Have another character  */
	if ( isspace (string[char_pos]) )
	{
	    /*  Have a whitespace character  */
	    if (char_pos > 0)
	    {
		/*  Have read in some non-whitespace already  */
		if (string[char_pos] == '\n') *newline = TRUE;
		return (char_pos);
	    }
	}
	else
	{
	    /*  Non-whitespace  */
	    if (++char_pos >= length)
	    {
		a_func_abort (function_name, "value too large for buffer");
		return (0);
	    }
	}
    }
    return (char_pos);
}   /*  End Function get_value  */
