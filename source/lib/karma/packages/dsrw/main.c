/*LINTLIBRARY*/
/*  main.c

    This code provides data structure IO routines.

    Copyright (C) 1992,1993,1994,1995  Richard Gooch

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

    This file contains the various utility routines for writing and reading
    the general data structure supported in Karma to and from channel objects.


    Written by      Richard Gooch   13-SEP-1992

    Updated by      Richard Gooch   9-NOV-1992

    Updated by      Richard Gooch   31-DEC-1992: Added fast local copy of
  arrays when using local connections.

    Updated by      Richard Gooch   2-JAN-1992: Added memory mapped array
  support.

    Updated by      Richard Gooch   3-JAN-1992: Added tiled array support and
  obsoleted  ARRAYP  .

    Updated by      Richard Gooch   6-JAN-1993: Changed from  #include "*.h"
  to  #include <*.h>

    Updated by      Richard Gooch   13-FEB-1993: Added support for old ARRAYP
  in  dsrw_write_element  and changed to 64 bit dimension lengths and list
  lengths.

    Updated by      Richard Gooch   23-FEB-1993: Removed diagnostics when
  transferring arrays between nodes on VX/MVX. Hardware problem fixed.

    Updated by      Richard Gooch   1-MAR-1993: Fixed bug in  dsrw_read_element
  changed  K_BYTE_SWAPPER  back to  BYTE_SWAPPER  .Problem with sed scripts to
  change from  BYTE  to  K_BYTE  .

    Updated by      Richard Gooch   21-MAR-1993: Added clearing of padding
  array in  dsrw_write_array  to guarantee identical files for identical
  structures.

    Updated by      Richard Gooch   25-MAR-1993: Took account of changes to
  list_header  structure.

    Updated by      Richard Gooch   11-APR-1993: Fixed bug in  dsrw_write_list

    Updated by      Richard Gooch   16-MAY-1993: Moved reading and writing of
  arraynames strings after all structures. Should not affect anyone as existing
  files have only needed one general data structure per file.

    Updated by      Richard Gooch   17-MAY-1993: Added support for string data
  types.

    Updated by      Richard Gooch   5-AUG-1993: Added test for arrays not
  aligned on a 16 byte boundary when transfering between VX/MVX nodes.

    Updated by      Richard Gooch   27-AUG-1993: Fixed bug in  dsrw_read_list
  which could cause problems when reading in a zero length list on machines
  with network data format.

    Updated by      Richard Gooch   14-SEP-1993: Fixed bug in
  dsrw_write_element  which did not write out K_VSTRINGs correctly.

    Updated by      Richard Gooch   1-DEC-1993: Fixed bug in
  dsrw_read_array  which caused reading of memory mapped channels to fail on
  byte swapper machines for byte-type arrays.

    Updated by      Richard Gooch   3-APR-1994: Added more diagnostics when
  reading strings.

    Updated by      Richard Gooch   16-APR-1994: Chopped one line of code for
  the VX/MVX because of some obscure bug (not mine!).

    Updated by      Richard Gooch   21-MAY-1994: Added  #include <karma_ch.h>

    Updated by      Richard Gooch   7-JUL-1994: Fixed bug in  dsrw_read_array
  which used sizeof (int) to find size of pointers. Problem on Alpha.
  
    Updated by      Richard Gooch   6-AUG-1994: Added support for more data
  types in  dsrw_read_element  for non matching sizes platforms.

    Updated by      Richard Gooch   3-NOV-1994: Switched to OS_ and MACHINE_
  macros for machine determination.

    Updated by      Richard Gooch   26-NOV-1994: Moved to  packages/dsrw/main.c

    Updated by      Richard Gooch   7-DEC-1994: Stripped declaration of  errno
  and added #include <errno.h>

    Updated by      Richard Gooch   23-FEB-1995: Made use of
  <ds_can_transfer_packet_as_block> routine.

    Updated by      Richard Gooch   9-APR-1995: Fixed bug in transferring
  non-byte complex values on little-endian machines: whole value was byte
  swapped rather than separately byte-swapping the real and imaginary
  components.

    Updated by      Richard Gooch   5-MAY-1995: Placate SGI compiler.

    Last updated by Richard Gooch   22-AUG-1995: Made <dsrw_write_element> and
  <dsrw_read_element> more aware of alignment problems (hopefully fully aware)


*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <karma.h>
#include <karma_dsrw.h>
#include <karma_pio.h>
#include <karma_ch.h>
#include <karma_ds.h>
#include <karma_m.h>
#include <karma_a.h>
#define OS_H_VARIABLES
#include <os.h>

#define MAGIC_STRING_LENGTH (unsigned int) 16
#define VERSION_NUMBER (unsigned long) 0
#define ARRAY_BOUNDARY (unsigned int) 16
#define VXMVX_REMOTE_MEMCPY_BOUNDARY 16

#define ARRAYP        6    /*Obsolete*/

/*  The following constants define the various control values used when
    transmitting atomic arrays over local connections.
*/
#define FA_NONE 0
#define FA_VX 1

/*  The following describes what is written by the transmitter when an atomic
    array is sent over a local connection:

    FA_NONE       :    the array data
    FA_VX         :    sender's task ID, sender's parent process ID,
                       starting virtual address of array


    The following describes what response is sent from the receiving process:

    FA_NONE       :    no response is given
    FA_VX         :    TRUE if receiver is happy, FALSE if data must be sent.

*/

/*  Determine what fast array mode the platform supports  */
#ifdef OS_VXMVX
#define FA_SUPPORTED FA_VX
/*#define FA_SUPPORTED FA_NONE*/
#else
#define FA_SUPPORTED FA_NONE
#endif


/*  Private functions  */
static void transmit_array_local ();
static flag receive_array_local ();
static flag read_array_padding ();

static char magic_string[] = "KarmaRHD Version";


/*  Public functions follow  */

/*PUBLIC_FUNCTION*/
void dsrw_write_multi (channel, multi_desc)
/*  This routine will write a binary multi_array descriptor to the channel
    object given by  channel  .All descriptors and data, as well as history
    strings are written.
    The descriptor that will be sent to the channel object must be pointed to
    by  multi_desc  .
    The routine returns nothing.
*/
Channel channel;
multi_array *multi_desc;
{
    history *entry;
    unsigned int array_count;
    char *array_name;
    extern char magic_string[];
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_write_multi";

    if (channel == NULL)
    {
	return;
    }
    if (multi_desc == NULL)
    {
	(void) fprintf (stderr, "NULL descriptor pointer passed\n");
	a_prog_bug (function_name);
    }
    /*  Write magic string and version number  */
    if (ch_write (channel, magic_string, MAGIC_STRING_LENGTH)
	< MAGIC_STRING_LENGTH)
    {
	(void) fprintf (stderr, "Error writing magic string\t%s\n",
			sys_errlist[errno]);
	exit (RV_WRITE_ERROR);
    }
    if (pio_write32 (channel, VERSION_NUMBER) != TRUE)
    {
	exit (RV_WRITE_ERROR);
    }
    /*  Write number of data structures  */
    if (pio_write32 (channel, (unsigned long) multi_desc->num_arrays)
	!= TRUE)
    {
	exit (RV_WRITE_ERROR);
    }
    if ( multi_desc->num_arrays < 1 )
    {
	(void) fprintf (stderr, "No arrays in multi-array header\n");
	a_prog_bug (function_name);
    }
    /*  Write packet descriptors and data for each data structure  */
    for (array_count = 0; array_count < multi_desc->num_arrays; 
	 ++array_count)
    {
	dsrw_write_packet_desc (channel, multi_desc->headers[array_count]);
	dsrw_write_packet (channel, multi_desc->headers[array_count],
			   multi_desc->data[array_count]);
    
    }
    /*  May need to write arraynames  */
    if ( multi_desc->num_arrays > 1 )
    {
	if ( multi_desc->array_names == NULL )
        {
	    (void) fprintf (stderr,
			    "Multiple arrays and no array of array names\n");
            a_prog_bug (function_name);
        }
        /*  Write array names   */
        for (array_count = 0; array_count < multi_desc->num_arrays;
	     ++array_count)
        {
	    array_name = multi_desc->array_names[array_count];
            if ( (array_name == NULL) || (*array_name == '\0') )
            {
		(void) fprintf (stderr,
				"Array name must not be a null string\n");
                a_prog_bug (function_name);
            }
	    if (pio_write_string (channel, array_name) != TRUE)
	    {
		exit (RV_WRITE_ERROR);
	    }
        }
    }
    /*  Write history  */
    for (entry = multi_desc->first_hist; entry != NULL;
	 entry = entry->next)
    {
	if (pio_write_string (channel, entry->string) != TRUE)
	{
	    (void) fprintf (stderr, "Error writing history string: \"%s\"\n",
			    entry->string);
	    exit (RV_WRITE_ERROR);
	}
    }
    if (pio_write_string (channel, (char *) NULL) != TRUE)
    {
	(void) fprintf (stderr, "Error writing NULL history string\n");
	exit (RV_WRITE_ERROR);
    }
}   /*  End Function dsrw_write_multi   */

/*PUBLIC_FUNCTION*/
void dsrw_write_packet_desc (channel, pack_desc)
/*  This routine will write the binary format of the packet descriptor pointed
    to by  pack_desc  to the channel object given by  channel  .
    The routine will also write all lower levels of array or linked list
    descriptors.
    The routine returns nothing.
*/
Channel channel;
packet_desc *pack_desc;
{
    unsigned int element_count = 0;
    static char function_name[] = "dsrw_write_packet_desc";

    if (channel == NULL)
    {
	return;
    }
    if (pack_desc == NULL)
    {
	(void) fprintf (stderr, "NULL descriptor pointer passed\n");
	a_prog_bug (function_name);
    }
    /*  Write number of elements    */
    if (pio_write32 (channel, (unsigned long) pack_desc->num_elements )
	!= TRUE)
    {
	return;
    }
    if ( pack_desc->num_elements < 1 )
    {
	(void) fprintf (stderr, "No elements in packet descriptor\n");
	a_prog_bug (function_name);
    }
    if ( pack_desc->element_types == NULL )
    {
	(void) fprintf (stderr,
			"Packet descriptor has no array of element types\n");
        a_prog_bug (function_name);
    }
    if ( pack_desc->element_desc == NULL )
    {
	(void) fprintf (stderr,
			"Packet descriptor has no array of element descriptor pointers\n");
        a_prog_bug (function_name);
    }
    /*  Write element descriptors   */
    while (element_count < pack_desc->num_elements)
    {
	/*  Write an element descriptor */
        dsrw_write_element_desc (channel,
				 pack_desc->element_types[element_count],
				 pack_desc->element_desc[element_count]);
        ++element_count;
    }
}   /*  End Function dsrw_write_packet_desc  */

/*PUBLIC_FUNCTION*/
void dsrw_write_element_desc (channel, type, desc)
/*  This routine will write the binary format of the element of type  type  and
    descriptor pointed to by  desc  to the channel object given by
    channel  .
    If the element is an array pointer or a linked list pointer, that
    descriptor will also be written.
    The routine returns nothing.
*/
Channel channel;
unsigned int type;
char *desc;
{
    static char function_name[] = "dsrw_write_element_desc";

    if (channel == NULL)
    {
	return;
    }
    if (desc == NULL)
    {
	(void) fprintf (stderr, "NULL descriptor pointer passed\n");
	a_prog_bug (function_name);
    }
    if (type == ARRAYP)
    {
	(void) fprintf (stderr, "Should not be writing type ARRAYP\n");
	a_prog_bug (function_name);
    }
    /*  Write element type  */
#ifdef RETYPE_NEEDED
    /*  Retyping needed  */
    if (pio_write32 (channel, (unsigned long) host_to_net_retype[type])
	!= TRUE)
    {
	return;
    }
#else
    /*  No retyping needed  */
    if (pio_write32 (channel, (unsigned long) type) != TRUE)
    {
	return;
    }
#endif

    if ( ds_element_is_named (type) )
    {
	if ( (desc == NULL) || (*desc == '\0') )
	{
	    (void) fprintf (stderr,"Element name must not be a null string\n");
	    a_prog_bug (function_name);
	}
	/*  Write element name  */
	if (pio_write_string (channel, desc) != TRUE)
	{
	    exit (RV_WRITE_ERROR);
	}
    }
    else
    {
	switch (type)
	{
	  case K_ARRAY:
	    dsrw_write_array_desc (channel, (array_desc *) desc);
	    return;
/*
            break;
*/
	  case LISTP:
	    dsrw_write_packet_desc (channel, (packet_desc *) desc);
	    return;
/*
            break;
*/
	}
    }
}   /*  End Function dsrw_write_element_desc */

/*PUBLIC_FUNCTION*/
void dsrw_write_array_desc (channel, arr_desc)
/*  This routine will write the binary representation of the array descriptor
    pointed to by  arr_desc  to the channel object given by  channel  .
    The packet descriptor for the array will also be written (this is a
    recursive process).
*/
Channel channel;
array_desc *arr_desc;
{
    unsigned int dim_count;
    unsigned int level_count;
    unsigned int product;
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_write_array_desc";

    if (channel == NULL)
    {
	return;
    }
    if (arr_desc == NULL)
    {
	(void) fprintf (stderr, "NULL descriptor pointer passed\n");
	a_prog_bug (function_name);
    }
    /*  Write number of dimensions  */
    if (pio_write32 (channel, (unsigned long) arr_desc->num_dimensions)
	!= TRUE)
    {
	return;
    }
    if ( arr_desc->num_dimensions < 1 )
    {
	(void) fprintf (stderr, "No dimensions in array descriptor\n");
	a_prog_bug (function_name);
    }
    /*  Write tiling information  */
    if ( arr_desc->num_levels > 0 )
    {
	(void) fprintf (stderr, "Writing tiled array\n");
    }
    if (pio_write32 (channel, (unsigned long) arr_desc->num_levels)
	!= TRUE)
    {
	(void) fprintf (stderr, "Error writing number of tiling levels\n");
	return;
    }
    if ( arr_desc->dimensions == NULL )
    {
	(void) fprintf (stderr,
			"Array descriptor has no array of dimension descriptor pointers\n");
        a_prog_bug (function_name);
    }
    /*  Write dimension descriptors */
    for (dim_count = 0; dim_count < arr_desc->num_dimensions; ++dim_count)
    {
	dsrw_write_dim_desc (channel, arr_desc->dimensions[dim_count]);
	/*  Write tile lengths if appropriate  */
	for (level_count = 0, product = 1;
	     level_count < arr_desc->num_levels;
	     ++level_count)
	{
	    if (pio_write32 (channel, (unsigned long)
			     arr_desc->tile_lengths[dim_count][level_count])
		!= TRUE)
	    {
		(void) fprintf (stderr, "Error writing tile length[%u][%u]\n",
				dim_count, level_count);
		return;
	    }
	    product *= arr_desc->tile_lengths[dim_count][level_count];
	}
	/*  Check if tile lengths appropriate  */
	if (product * arr_desc->lengths[dim_count] !=
	    arr_desc->dimensions[dim_count]->length)
	{
	    (void) fprintf (stderr,
			    "Tile product * bottom length (%u * %lu): %lu not equal to length: %lu\n",
			    product, arr_desc->lengths[dim_count],
			    product * arr_desc->lengths[dim_count],
			    arr_desc->dimensions[dim_count]->length);
	    a_prog_bug (function_name);
	}
    }
    /*  Write array packet descriptor   */
    dsrw_write_packet_desc (channel, arr_desc->packet);
}   /*  End Function dsrw_write_array_desc   */

/*PUBLIC_FUNCTION*/
void dsrw_write_dim_desc (channel, dimension)
/*  This routine will write the binary representation of the dimension
    descriptor pointed to by  dimension  to the channel object given by
    channel  .
    The routine returns nothing.
*/
Channel channel;
dim_desc *dimension;
{
    unsigned int coord_count = 0;
    static char function_name[] = "dsrw_write_dim_desc";

    if (channel == NULL)
    {
	return;
    }
    if (dimension == NULL)
    {
	(void) fprintf (stderr, "NULL descriptor pointer passed\n");
	a_prog_bug (function_name);
    }
    if ( ( dimension->name == NULL ) || ( *( dimension->name ) == '\0' ) )
    {
	(void) fprintf (stderr, "Dimension name must not be a null string\n");
        a_prog_bug (function_name);
    }
    /*  Write dimension name    */
    if (pio_write_string (channel, dimension->name) != TRUE)
    {
	exit (RV_WRITE_ERROR);
    }
    if ( dimension->length < 1 )
    {
	(void) fprintf (stderr, "Dimension: \"%s\" has zero length\n",
			dimension->name);
	a_prog_bug (function_name);
    }
    /*  Write dimension length  */
    if (pio_write64 (channel, (unsigned long) dimension->length) != TRUE)
    {
	return;
    }
    if ( dimension->minimum > dimension->maximum )
    {
	(void) fprintf (stderr,
			"Dimension: \"%s\" minimum: %e is greater than maximum: %e\n",
			dimension->name,
			dimension->minimum, dimension->maximum);
	a_prog_bug (function_name);
    }
    if ( ( dimension->minimum == dimension->maximum ) &&
	( dimension->length != 1 ) )
    {
	(void) fprintf (stderr,
			"Dimension: \"%s\" minimum is equal to maximum: %e\n",
			dimension->name, dimension->maximum);
	(void) fprintf (stderr,
			"and length: %lu is not 1\n", dimension->length);
	a_prog_bug (function_name);
    }
    /*  Write REGULAR flag  */
    if ( dimension->coordinates == NULL )
    {
	dsrw_write_flag (channel, (flag) TRUE);
    }
    else
    {
	dsrw_write_flag (channel, (flag) FALSE);
    }
    if ( dimension->coordinates == NULL )
    {
	/*  Write minimum and maximum   */
	if (pio_write_double (channel, dimension->minimum) != TRUE)
	{
	    return;
	}
	if (pio_write_double (channel, dimension->maximum) != TRUE)
	{
	    return;
	}
    }
    else
    {
	/*  Write co-ordinates  */
        while (coord_count < dimension->length)
        {
	    if (pio_write_double (channel,
				  dimension->coordinates[coord_count])
		!= TRUE)
	    {
		return;
	    }
	    ++coord_count;
        }
    }
}   /*  End Function dsrw_write_dim_desc  */

/*PUBLIC_FUNCTION*/
void dsrw_write_packet (channel, pack_desc, packet)
/*  This routine will write the binary representation of the data pointed to
    by  packet  with a descriptor pointed to by  pack_desc  to the channel
    object given by  channel  .
    The routine will recursively write sub arrays and linked lists of packets.
    The routine returns nothing.
*/
Channel channel;
packet_desc *pack_desc;
char *packet;
{
    unsigned int element_count = 0;
    unsigned int bytes_to_write;
    unsigned int type;
    extern char host_type_sizes[NUMTYPES];
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_write_packet";

    if (channel == NULL)
    {
	return;
    }
    if (pack_desc == NULL)
    {
	(void) fprintf (stderr, "NULL descriptor pointer passed\n");
	a_prog_bug (function_name);
    }
    if (packet == NULL)
    {
	(void) fprintf (stderr, "NULL data pointer passed\n");
	a_prog_bug (function_name);
    }
    if ( ds_can_transfer_packet_as_block (pack_desc) )
    {
	/*  Block write  */
	bytes_to_write = ds_get_packet_size (pack_desc);
	if (ch_write (channel, packet, bytes_to_write) < bytes_to_write)
	{
	    (void) fprintf (stderr, "Error writing packet\t%s\n",
			    sys_errlist[errno]);
	    exit (RV_WRITE_ERROR);
	}
	return;
    }
    /*  Write packet data   */
    while (element_count < pack_desc->num_elements)
    {
	/*  Write element   */
        type = pack_desc->element_types[element_count];
        dsrw_write_element (channel, type,
			    pack_desc->element_desc[element_count],
			    packet);
        packet += host_type_sizes[type];
        ++element_count;
    }
}   /*  End Function dsrw_write_packet   */

/*PUBLIC_FUNCTION*/
void dsrw_write_element (Channel channel, unsigned int type, char *desc,
			 char *element)
/*  [PURPOSE] This routine will write the binary representation of an element
    to a channel object.
    <channel> The channel object.
    <type> The type of the element.
    <desc> The descriptor for the element.
    <element> A pointer to the element data.
    [RETURNS] Nothing.
*/
{
    FString *fstring;
    unsigned int size;
    unsigned int fstring_len;
    char *st;
#ifdef MACHINE_LITTLE_ENDIAN
    int byte_count;
    extern char network_type_bytes[NUMTYPES];
#endif
#if defined(NEED_ALIGNED_DATA) || defined(MACHINE_LITTLE_ENDIAN)
    char buffer[64];
#endif
    extern char *sys_errlist[];
    extern char host_type_sizes[NUMTYPES];
    static char function_name[] = "dsrw_write_element";

    if (channel == NULL)
    {
	return;
    }
    if (element == NULL)
    {
	(void) fprintf (stderr, "No element data to write\n");
        a_prog_bug (function_name);
    }
    size = host_type_sizes[type];
    if ( ds_can_transfer_element_as_block (type) )
    {
	if (ch_write (channel, element, size) != size)
	{
	    (void) fprintf (stderr,
			    "Error writing atomic element to channel\t%s\n",
			    sys_errlist[errno]);
	    exit (RV_WRITE_ERROR);
	}
	return;
    }
#if defined(NEED_ALIGNED_DATA) || defined(MACHINE_LITTLE_ENDIAN)
    if (size > 64)
    {
	(void) fprintf (stderr, "Data type size: %u too large for buffer\n",
			size);
	a_prog_bug (function_name);
    }
#endif
#ifdef MACHINE_LITTLE_ENDIAN
    switch (type)
    {
      case K_FLOAT:
      case K_DOUBLE:
      case K_INT:
      case K_SHORT:
      case K_LONG:
      case K_UINT:
      case K_USHORT:
      case K_ULONG:
	/*  Swap bytes (cannot do this for complex types)  */
	if (host_type_sizes[type] == network_type_bytes[type])
	{
	    for (byte_count = 0; byte_count < (int) size; ++byte_count)
	    {
		buffer[byte_count] = element[size - byte_count - 1];
	    }
	    if (ch_write (channel, buffer, size) != size)
	    {
		(void) fprintf(stderr,
			       "Error writing atomic element to channel\t%s\n",
			       sys_errlist[errno]);
		exit (RV_WRITE_ERROR);
	    }
	    return;
	}
        break;
    }
#endif
    /*  Now we read from the element using non-byte C types. We have to be
	careful about data alignment.  */
    /*  Data will need to be converted  */
#ifdef NEED_ALIGNED_DATA
    m_copy (buffer, element, size);
    element = buffer;
#endif
    switch (type)
    {
      case K_FLOAT:
	(void) pio_write_float (channel, *(float *) element);
        break;
      case K_DOUBLE:
	(void) pio_write_double (channel, *(double *) element);
        break;
      case K_COMPLEX:
	if ( !pio_write_float (channel, *(float *) element) ) return;
	(void) pio_write_float ( channel, *( (float *) element + 1 ) );
        break;
      case K_DCOMPLEX:
	if ( !pio_write_double (channel, *(double *) element) ) return;
	(void) pio_write_double ( channel, *( (double *) element + 1 ) );
        break;
      case K_INT:
	(void) pio_write32s (channel, *(int *) element);
	break;
      case K_SHORT:
	(void) pio_write16s (channel, *(short *) element);
	break;
      case K_ICOMPLEX:
	if ( !pio_write32s (channel, *(int *) element) ) return;
	(void) pio_write32s ( channel, *( (int *) element + 1 ) );
	break;
      case K_SCOMPLEX:
	if ( !pio_write16s (channel, *(short *) element) ) return;
	(void) pio_write16s ( channel, *( (short *) element + 1 ) );
	break;
      case K_LONG:
	(void) fprintf (stderr, "64 bit quantity not yet supported\n");
	a_prog_bug (function_name);
	break;
      case K_LCOMPLEX:
	(void) fprintf (stderr, "64 bit quantity not yet supported\n");
	a_prog_bug (function_name);
	break;
      case K_UINT:
	(void) pio_write32 (channel, *(unsigned int *) element);
	break;
      case K_USHORT:
	(void) pio_write16 (channel, *(unsigned short *) element);
	break;
      case K_ULONG:
	(void) pio_write64 (channel, *(unsigned long *) element);
	break;
      case K_UICOMPLEX:
	if ( !pio_write32 (channel, *(unsigned int *) element) ) return;
	(void) pio_write32 ( channel, *( (unsigned int *) element + 1 ) );
	break;
      case K_USCOMPLEX:
	if ( !pio_write16 (channel, *(unsigned short *) element) ) return;
	(void) pio_write16 ( channel, *( (unsigned short *) element +1 ) );
	break;
      case K_ULCOMPLEX:
	if ( !pio_write64 (channel, *(unsigned long *) element) ) return;
	(void) pio_write64 ( channel, *( (unsigned long *) element + 1 ) );
	break;
      case ARRAYP:
	(void) fprintf (stderr, "Writing unpadded array\n");
	dsrw_write_array (channel, (array_desc *) desc, element, FALSE);
	break;
      case K_ARRAY:
	dsrw_write_array (channel, (array_desc *) desc, element, TRUE);
	break;
      case LISTP:
	dsrw_write_list (channel, (packet_desc *) desc,
			 *(list_header **) element);
	break;
      case K_VSTRING:
	if ( !pio_write_string (channel, *(char **) element) ) return;
	break;
      case K_FSTRING:
	fstring = (FString *) element;
	if ( !pio_write32 (channel, fstring->max_len) ) return;
	/*  Clear tail end of string  */
	fstring_len = strlen (fstring->string);
	m_clear (fstring_len + fstring->string,
		 fstring->max_len - fstring_len);
	if (ch_write (channel, fstring->string, fstring->max_len) <
	    fstring->max_len)
	{
	    (void) fprintf (stderr,
			    "Error writing string to channel\t%s\n",
			    sys_errlist[errno]);
	    exit (RV_WRITE_ERROR);
	}
        break;
      default:
	(void) fprintf (stderr, "Bad element type: %u\n", type);
	a_prog_bug (function_name);
	break;
    }
}   /*  End Function dsrw_write_element  */

/*PUBLIC_FUNCTION*/
void dsrw_write_array (channel, arr_desc, element, pad)
/*  This routine will write the binary representation of an array to a channel.
    The channel must be given by  channel  .
    The array descriptor must be pointed to by  arr_desc  .
    The element to write the array pointer to must be pointed to by  element  .
    The array will be padded on a selected boundary if the value of  pad  is
    TRUE (this is the case when writing .kf files).
    The routine will recursively write any sub arrays or linked lists of
    packets.
    The routine returns nothing.
*/
Channel channel;
array_desc *arr_desc;
char *element;
flag pad;
{
    flag block_transfer;
    flag local;
    unsigned int bytes_to_write;
    unsigned int bytes_written;
    unsigned int array_size;
    unsigned int array_count = 0;
    unsigned int packet_size;
    unsigned int bytes_to_pad;
    unsigned long read_pos;
    unsigned long write_pos;
    char *data;
    char *array;
    packet_desc *pack_desc;
    char padding[ARRAY_BOUNDARY];
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_write_array";

    FLAG_VERIFY (pad);
    if (channel == NULL)
    {
	return;
    }
    if (arr_desc == NULL)
    {
	(void) fprintf (stderr, "NULL descriptor pointer passed\n");
	a_prog_bug (function_name);
    }
    if ( (pack_desc = arr_desc->packet) == NULL )
    {
	(void) fprintf (stderr,
			"NULL pointer to packet in array descriptor\n");
	a_prog_bug (function_name);
    }
    if (element == NULL)
    {
	(void) fprintf (stderr, "NULL element pointer passed\n");
	a_prog_bug (function_name);
    }
    array = *(char **) element;
    if (array == NULL)
    {
	(void) fprintf (stderr, "NULL data pointer passed\n");
	a_prog_bug (function_name);
    }
    array_size = ds_get_array_size (arr_desc);
    packet_size = ds_get_packet_size ( arr_desc->packet );
    block_transfer = ds_can_transfer_packet_as_block (pack_desc);
    local = ch_test_for_local_connection (channel);
    bytes_to_write = packet_size * array_size;
    /*  Pad array if needed  */
    if (pad)
    {
	/*  Clear padding array (to avoid random file differences)  */
	m_clear (padding, ARRAY_BOUNDARY);
	if (ch_tell (channel, &read_pos, &write_pos) != TRUE)
	{
	    (void) fprintf (stderr, "Error getting channel position\n");
	    exit (RV_UNDEF_ERROR);
	}
	/*  Add 4 bytes for pad size  */
	write_pos += 4;
	bytes_to_pad = ARRAY_BOUNDARY - write_pos % ARRAY_BOUNDARY;
	if (pio_write32 (channel, (unsigned long) bytes_to_pad) != TRUE)
	{
	    (void) fprintf (stderr, "Error writing pad size\n");
	    exit (RV_WRITE_ERROR);
	}
	if (bytes_to_pad > 0)
	{
	    if (ch_write (channel, padding, bytes_to_pad) < bytes_to_pad)
	    {
		(void) fprintf (stderr, "Error padding: %u bytes\t%s\n",
				bytes_to_pad, sys_errlist[errno]);
		exit (RV_WRITE_ERROR);
	    }
	}
    }
    if (block_transfer && local)
    {
	transmit_array_local (channel, array, bytes_to_write);
	return;
    }
    if (block_transfer)
    {
	/*  Do fast save  */
	if ( ( bytes_written = ch_write (channel, array, bytes_to_write) )
	    < bytes_to_write)
	{
	    (void) fprintf (stderr, "Error writing array to channel\t%s\n",
			    sys_errlist[errno]);
	    (void) fprintf (stderr,
			    "Wanted: %u bytes, wrote: %u bytes\n",
			    bytes_to_write, bytes_written);
	    exit (RV_WRITE_ERROR);
	}
	return;
    }
    /*  Do recursive save   */
    data = array;
    while (array_count++ < array_size)
    {
	/*  Write a packet in the array */
	dsrw_write_packet (channel, arr_desc->packet, data);
	data += packet_size;
    }
}   /*  End Function dsrw_write_array  */

/*PUBLIC_FUNCTION*/
void dsrw_write_list (channel, pack_desc, list_head)
/*  This routine will write the binary representation of the data in the linked
    list pointed to by  list_head  with a descriptor pointed to by
    pack_desc  to the channel object given by  channel  .
    The routine will recursively write any sub arrays or linked lists of
    packets.
    The routine returns nothing.
*/
Channel channel;
packet_desc *pack_desc;
list_header *list_head;
{
    flag block_transfer;
    unsigned int bytes_written;
    unsigned int bytes_to_write;
    unsigned int count;
    unsigned int pack_size;
    char *data;
    list_entry *curr_entry;
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_write_list";

    if (channel == NULL)
    {
	return;
    }
    if (pack_desc == NULL)
    {
	(void) fprintf (stderr, "NULL descriptor pointer passed\n");
	a_prog_bug (function_name);
    }
    if (list_head == NULL)
    {
	(void) fprintf (stderr, "NULL list header pointer passed\n");
	a_prog_bug (function_name);
    }
    if ( list_head->magic != MAGIC_LIST_HEADER )
    {
	(void) fprintf (stderr, "List header has bad magic number\n");
	a_prog_bug (function_name);
    }
    /*  Write list length   */
    if (pio_write64 (channel, (unsigned long) list_head->length) != TRUE)
    {
	return;
    }
    switch ( list_head->sort_type )
    {
      case SORT_INCREASING:
      case SORT_DECREASING:
      case SORT_RANDOM:
	break;
      default:
	(void) fprintf (stderr,
			"Sort type: %u not recognised\n",
			list_head->sort_type);
	a_prog_bug (function_name);
	break;
    }
    /*  Write list sort type    */
    if (pio_write32 (channel, (unsigned long) list_head->sort_type)
	!= TRUE)
    {
	return;
    }
    if ( list_head->sort_elem_num >= pack_desc->num_elements )
    {
	(void) fprintf (stderr,
			"List sort element number: %u is not less than num_elem: %u\n",
			list_head->sort_elem_num, pack_desc->num_elements);
        a_prog_bug (function_name);
    }
    /*  Write list sort element number  */
    if (pio_write32 (channel, (unsigned long) list_head->sort_elem_num)
	!= TRUE)
    {
	return;
    }
    /*  Write list packets  */
    /*  Write contiguous section of list  */
    pack_size = ds_get_packet_size (pack_desc);
    block_transfer = ds_can_transfer_packet_as_block (pack_desc);
    if (block_transfer && list_head->contiguous_length > 0)
    {
	/*  Contiguous block of packet data: do a faster write  */
	bytes_to_write = pack_size * list_head->contiguous_length;
	if ( ( bytes_written = ch_write (channel, list_head->contiguous_data,
					 bytes_to_write) )
	    < bytes_to_write)
	{
	    (void) fprintf (stderr,
			    "Error writing contiguous block of list packets\n");
	    (void) fprintf (stderr,
			    "Wanted: %u bytes, wrote: %u bytes\n",
			    bytes_to_write, bytes_written);
	    exit (RV_WRITE_ERROR);
	}
	return;
    }
    else
    {
	/*  Must write out each packet  */
	for (count = 0, data = list_head->contiguous_data;
	     count < list_head->contiguous_length;
	     ++count, data += pack_size)
	{
	    dsrw_write_packet (channel, pack_desc, data);
	}
    }
    /*  Write out fragmented section of list  */
    for (curr_entry = list_head->first_frag_entry; curr_entry != NULL;
	 curr_entry = curr_entry->next)
    {
	dsrw_write_packet (channel, pack_desc, curr_entry->data);
    }
}   /*  End Function dsrw_write_list */

/*PUBLIC_FUNCTION*/
void dsrw_write_flag (channel, logical)
/*  This routine will write the binary representation of the flag value in
    logical  to the channel object given by  channel  .
    The routine returns nothing.
*/
Channel channel;
flag logical;
{
    char value;
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_write_flag";

    if (channel == NULL)
    {
	return;
    }
    if ( (logical != TRUE) && (logical != FALSE) )
    {
	(void) fprintf (stderr, "flag value: %u is not TRUE or FALSE\n",
			(unsigned int) logical);
        a_prog_bug (function_name);
    }
    value = logical;
    if (ch_write (channel, (char *) &value, 1) != 1)
    {
	(void) fprintf (stderr, "Error writing flag to channel\t%s\n",
			sys_errlist[errno]);
        exit (RV_WRITE_ERROR);
    }
}   /*  End Function dsrw_write_flag  */

/*PUBLIC_FUNCTION*/
multi_array *dsrw_read_multi (channel)
/*  This routine will read the binary representation of the multi array general
    data structure header from the channel object given by  channel  and
    will allocate the required descriptors. The routine will then read in the
    data from the channel object and will write it into the data structure(s)
    created.
    The routine returns a pointer to the multi array header on success, else
    it displays an error message and returns NULL.
*/
Channel channel;
{
    flag array_alloc;
    unsigned int num_arrays;
    unsigned int array_count;
    unsigned int length;
    unsigned long long_data;
    unsigned long version;
    char *string;
    history *entry;
    char magic_str_buf[MAGIC_STRING_LENGTH];
    char *data;
    multi_array *multi_desc;
    packet_desc *pack_desc;
    extern char magic_string[];
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_read_multi";

    if (channel == NULL)
    {
	(void) fprintf (stderr,
			"No channel to read multi array data structure from\n");
        return (NULL);
    }
    /*  Read magic string and version number  */
    if (ch_read (channel, magic_str_buf, MAGIC_STRING_LENGTH)
	< MAGIC_STRING_LENGTH)
    {
	(void) fprintf (stderr, "Error reading magic string\n");
        return (NULL);
    }
    if (strncmp (magic_string, magic_str_buf, MAGIC_STRING_LENGTH) != 0)
    {
	(void) fprintf (stderr, "Magic string not present\n");
	return (NULL);
    }
    if (pio_read32 (channel, &version) == FALSE)
    {
	(void) fprintf (stderr, "Error reading version number\n");
        return (NULL);
    }
    if (version != VERSION_NUMBER)
    {
	(void) fprintf (stderr, "Unknown version number: %lu\n", version);
        return (NULL);
    }
    if (pio_read32 (channel, &long_data) == FALSE)
    {
	(void) fprintf (stderr, "Error reading number of arrays\n");
        return (NULL);
    }
    num_arrays = long_data;
    if (num_arrays < 1)
    {
	(void) fprintf (stderr, "Must have at least on array\n");
	return (NULL);
    }
    if ( ( multi_desc = ds_alloc_multi (num_arrays) ) == NULL )
    {
	m_error_notify (function_name, "multi_desc descriptor");
        return (NULL);
    }
    /*  Read the structures  */
    array_alloc = ch_test_for_mmap (channel) ? FALSE : TRUE;
    for (array_count = 0; array_count < num_arrays; ++array_count)
    {
	/*  Load the descriptor for one array   */
        if ( ( pack_desc = dsrw_read_packet_desc (channel) ) == NULL )
        {
	    ds_dealloc_multi (multi_desc);
            return (NULL);
        }
        multi_desc->headers[array_count] = pack_desc;
	/*  Allocate the required data space   */
	if ( ( data = ds_alloc_data ( multi_desc->headers[array_count],
				     FALSE, array_alloc ) )
	    == NULL )
        {
	    m_error_notify (function_name, "general data structure");
            ds_dealloc_multi (multi_desc);
            return (NULL);
        }
        multi_desc->data[array_count] = data;
	/*  Read the data   */
        if (dsrw_read_packet (channel, multi_desc->headers[array_count],
			      multi_desc->data[array_count]) == FALSE)
        {
	    (void) fprintf (stderr, "Error reading array number %u\n",
			    array_count);
            ds_dealloc_multi (multi_desc);
            return (NULL);
        }
    }
    /*  May need to read arraynames  */
    for (array_count = 0; (array_count < num_arrays) && (num_arrays > 1);
	 ++array_count)
    {
	/*  Load the names of the arrays    */
	if ( ( multi_desc->array_names[array_count] =
	      pio_read_string (channel, &length) ) == NULL )
        {
	    a_func_abort (function_name, "Error reading array name");
            ds_dealloc_multi (multi_desc);
            return (NULL);
        }
        if (length == 0)
        {
	    a_func_abort (function_name,
			  "Array name must not be a null string");
            ds_dealloc_multi (multi_desc);
            return (NULL);
        }
    }
    /*  Read history strings  */
    if ( ( string = pio_read_string (channel, &length) )
	== NULL )
    {
	a_func_abort (function_name, "Error reading first history string");
	ds_dealloc_multi (multi_desc);
	return (NULL);
    }
    while (length > 0)
    {
	if ( ( entry = (history *) m_alloc (sizeof *entry) ) == NULL )
	{
	    m_abort (function_name, "history entry");
	}
	entry->string = string;
	entry->next = NULL;
	if ( multi_desc->first_hist == NULL )
	{
	    /*  First entry  */
	    multi_desc->first_hist = entry;
	    multi_desc->last_hist = entry;
	}
	else
	{
	    /*  Append  */
	    multi_desc->last_hist->next = entry;
	    multi_desc->last_hist = entry;
	}
	if ( ( string = pio_read_string (channel, &length) )
	    == NULL )
	{
	    a_func_abort (function_name, "Error reading history string");
	    ds_dealloc_multi (multi_desc);
	    return (NULL);
	}
    }
    m_free (string);
    return (multi_desc);
}   /*  End Function dsrw_read_multi */

/*PUBLIC_FUNCTION*/
packet_desc *dsrw_read_packet_desc (channel)
/*  This routine will read the channel object given by  channel  for a
    packet descriptor.
    The routine will recursively read in array and linked list descriptors
    if required.
    The routine returns a pointer to the packet descriptor. NULL is returned
    if an error occured (the routine prints error messages).
*/
Channel channel;
{
    unsigned int num_elements = 0;
    unsigned int element_count;
    unsigned long data;
    unsigned int elem_type;
    packet_desc *pack_desc;
    array_desc *arr_desc;
    packet_desc *list_desc;
    static char function_name[] = "dsrw_read_packet_desc";

    if (channel == NULL)
    {
	(void) fprintf (stderr, "No channel to read packet descriptor from\n");
        return (NULL);
    }
    /*  Get number of elements  */
    if (pio_read32 (channel, &data) == FALSE)
    {
	(void) fprintf (stderr, "Error reading number of elements\n");
        return (NULL);
    }
    num_elements = data;
    if (num_elements < 1)
    {
	(void) fprintf (stderr, "Bad number of elements: %u\n", num_elements);
        return (NULL);
    }
    /*  Allocate packet descriptor  */
    if ( ( pack_desc = ds_alloc_packet_desc (num_elements) ) == NULL )
    {
	m_error_notify (function_name, "packet descriptor");
        return (NULL);
    }
    for (element_count = 0; element_count < num_elements; ++element_count)
    {
	if (dsrw_read_type (channel, &elem_type) == FALSE)
        {
	    ds_dealloc_packet (pack_desc, (char *) NULL);
            return (NULL);
        }
	pack_desc->element_types[element_count] = elem_type;
	if ( ds_element_is_named (elem_type) )
	{
	    /*  Atomic data type    */
	    if ( ( pack_desc->element_desc[element_count] =
		  pio_read_string (channel, (unsigned int *) NULL) )
		== NULL )
	    {
		a_func_abort (function_name, "Error reading element_name");
		ds_dealloc_packet (pack_desc, (char *) NULL);
		return (NULL);
	    }
	}
	else
	{
	    switch (elem_type)
	    {
	      case ARRAYP:
		pack_desc->element_types[element_count] = K_ARRAY;
		/*  Fall through  */
	      case K_ARRAY:
		if ( ( arr_desc = dsrw_read_array_desc (channel, elem_type) )
		    == NULL )
		{
		    ds_dealloc_packet (pack_desc, (char *) NULL);
		    return (NULL);
		}
		pack_desc->element_desc[element_count] = (char *) arr_desc;
		break;
	      case LISTP:
		if ( ( list_desc = dsrw_read_packet_desc (channel) ) == NULL )
		{
		    ds_dealloc_packet (pack_desc, (char *) NULL);
		    return (NULL);
		}
		pack_desc->element_desc[element_count] = (char *) list_desc;
		break;
	      default:
		/*  This should NEVER happen!   */
		(void) fprintf (stderr, "Illegal element type: %u\n",
				elem_type);
		(void) fprintf (stderr,
				"Returned from function: dsrw_read_type\n");
		a_prog_bug (function_name);
		break;
	    }
	}
    }
    return (pack_desc);
}   /*  End Function dsrw_read_packet_desc   */

/*PUBLIC_FUNCTION*/
array_desc *dsrw_read_array_desc (channel, type)
/*  This routine will read in an array descriptor from the channel object
    given by  channel  .
    The type of the array must be given by  type  .Legal values are:
        ARRAYP   (old format)
	K_ARRAY  (new format)
    The routine will recursively read the packet descriptor for the array.
    The routine returns a pointer to the array descriptor if there were no
    errors in reading, else it displays an error message and returns NULL.
*/
Channel channel;
unsigned int type;
{
    unsigned int num_dim = 0;
    unsigned int dim_count;
    unsigned int level_count;
    unsigned int num_levels;
    unsigned int product;
    unsigned long data;
    array_desc *arr_desc;
    dim_desc *dimension;
    packet_desc *pack_desc;
    static char function_name[] = "dsrw_read_array_desc";

    if (channel == NULL)
    {
	(void) fprintf (stderr, "No channel to read array descriptor from\n");
        return (NULL);
    }
    if ( (type != ARRAYP) && (type != K_ARRAY) )
    {
	(void) fprintf (stderr, "Bad type: %u\n", type);
	a_prog_bug (function_name);
    }
    /*  Get number of dimensions    */
    if (pio_read32 (channel, &data) != TRUE)
    {
	(void) fprintf (stderr, "Error reading number of dimensions\n");
        return (NULL);
    }
    num_dim = data;
    if (num_dim < 1)
    {
	(void) fprintf (stderr, "Bad number of dimensions: %u\n", num_dim);
        return (NULL);
    }
    if (type == K_ARRAY)
    {
	/*  Get number of tiling levels  */
	if (pio_read32 (channel, &data) != TRUE)
	{
	    (void) fprintf (stderr, "Error reading number of tiling levels\n");
	    return (NULL);
	}
	if (data > 0)
	{
	    (void) fprintf (stderr, "Reading tiled array\n");
	}
	num_levels = data;
    }
    else
    {
	num_levels = 0;
    }
    if ( ( arr_desc = ds_alloc_array_desc (num_dim, num_levels) ) == NULL )
    {
	m_error_notify (function_name, "array descriptor");
        return (NULL);
    }
    if (type == ARRAYP)
    {
	arr_desc->padded = FALSE;
    }
    else
    {
	arr_desc->padded = TRUE;
    }
    /*  Get dimension descriptors   */
    for (dim_count = 0; dim_count < num_dim; ++dim_count)
    {
	if ( ( dimension = dsrw_read_dim_desc (channel) ) == NULL )
        {
	    ds_dealloc_array_desc (arr_desc);
            return (NULL);
        }
        arr_desc->dimensions[dim_count] = dimension;
	/*  Get tile lengths if appropriate  */
	for (level_count = 0, product = 1; level_count < num_levels;
	     ++level_count)
	{
	    if (pio_read32 (channel, &data) != TRUE)
	    {
		(void) fprintf (stderr, "Error reading tile length[%u][%u]\n",
				dim_count, level_count);
		ds_dealloc_array_desc (arr_desc);
		return (NULL);
	    }
	    arr_desc->tile_lengths[dim_count][level_count] = data;
	    product *= data;
	}
	/*  Check if tile lengths appropriate  */
	if ( dimension->length % product != 0 )
	{
	    (void) fprintf (stderr,
			    "Tile product: %u not a factor of length: %lu\n",
			    product, dimension->length);
	    a_func_abort (function_name, "bad data");
	    ds_dealloc_array_desc (arr_desc);
	    return (NULL);
	}
	arr_desc->lengths[dim_count] = dimension->length / product;
    }
    /*  Get packet descriptor for array */
    if ( ( pack_desc = dsrw_read_packet_desc (channel) ) == NULL )
    {
	ds_dealloc_array_desc (arr_desc);
        return (NULL);
    }
    arr_desc->packet = pack_desc;
    return (arr_desc);
}   /*  End Function dsrw_read_array_desc    */

/*PUBLIC_FUNCTION*/
dim_desc *dsrw_read_dim_desc (channel)
/*  This routine will read the channel object given by  channel  for a
    dimension descriptor.
    The routine returns a pointer to the dimension descriptor if there were no
    errors in reading, else it displays an error message and returns NULL.
*/
Channel channel;
{
    unsigned int length;
    unsigned int dim_length = 0;
    unsigned int coord_count = 0;
    unsigned long data;
    double minimum;
    double maximum;
    double coordinate;
    double last_coord = -TOOBIG;
    flag regular;
    char *dim_name;
    dim_desc *dimension;
    static char function_name[] = "dsrw_read_dim_desc";

    if (channel == NULL)
    {
	(void) fprintf (stderr,
			"No channel to read dimension descriptor from\n");
        return (NULL);
    }
    /*  Get name    */
    if ( ( dim_name = pio_read_string (channel, &length) ) == NULL )
    {
	a_func_abort (function_name, "Error reading dimension name");
        return (NULL);
    }
    if (length == 0)
    {
	a_func_abort (function_name,
		      "Dimension name must not be a null string");
	m_free (dim_name);
        return (NULL);
    }
    /*  Get length  */
    if (pio_read64 (channel, &data) != TRUE)
    {
	(void) fprintf (stderr, "Error reading dimension length\n");
	m_free (dim_name);
        return (NULL);
    }
    dim_length = data;
    if (dim_length < 1)
    {
	(void) fprintf (stderr, "Bad dimension length: %u\n", dim_length);
	m_free (dim_name);
        return (NULL);
    }
    /*  Get REGULAR flag  */
    if (dsrw_read_flag (channel, &regular) == FALSE)
    {
	(void) fprintf (stderr, "Error reading REGULAR flag\n");
	m_free (dim_name);
        return (NULL);
    }
    if (regular == TRUE)
    {
	/*  Dimension co-ordinates are regularly spaced */
        /*  Get minimum */
        if (pio_read_double (channel, &minimum) != TRUE)
        {
	    (void) fprintf (stderr, "Error reading minimum co-ordinate\n");
	    m_free (dim_name);
            return (NULL);
        }
        /*  Get maximum */
        if (pio_read_double (channel, &maximum) != TRUE)
        {
	    (void) fprintf (stderr, "Error reading maximum co-ordinate\n");
	    m_free (dim_name);
            return (NULL);
        }
	if (minimum > maximum)
	{
	    (void) fprintf (stderr,
			    "Dimension: \"%s\" minimum: %e is greater than maximum: %e\n",
			    dim_name, minimum, maximum);
	    a_prog_bug (function_name);
	}
	if ( (minimum == maximum) && (dim_length != 1) )
	{
	    (void) fprintf (stderr,
			    "Dimension: \"%s\" minimum is equal to maximum: %e\n",
			    dim_name, maximum);
	    (void) fprintf (stderr,
			    "and length: %u is not 1\n", dim_length);
	    a_prog_bug (function_name);
	}
        if ( ( dimension = ds_alloc_dim_desc (dim_name, dim_length, minimum,
					     maximum, regular) ) == NULL )
        {
	    m_error_notify (function_name, "dimension descriptor");
	    m_free (dim_name);
            return (NULL);
        }
	m_free (dim_name);
        return (dimension);
    }
    /*  Dimension co-ordinates are to be supplied   */
    if ( ( dimension = ds_alloc_dim_desc (dim_name, dim_length, minimum,
					 maximum, regular) ) == NULL )
    {
	m_error_notify (function_name, "dimension descriptor");
	m_free (dim_name);
        return (NULL);
    }
    m_free (dim_name);
    /*  Get co-ordinates and compute minimum and maximum    */
    while (coord_count < dim_length)
    {
	if (pio_read_double (channel, &coordinate) != TRUE)
        {
	    (void) fprintf (stderr, "Error reading Co-ordinate number: %u\n",
			    coord_count);
            m_free ( dimension->name );
            m_free ( (char *) dimension->coordinates );
            m_free ( (char *) dimension );
            return (NULL);
        }
	if (coordinate <= last_coord)
	{
	    /*  Co-ordinates are not increasing  */
	    (void) fprintf (stderr,
			    "Co-ordinates are not increasing: last: %e curr: %e\n",
			    last_coord, coordinate);
	    m_free ( dimension->name );
            m_free ( (char *) dimension->coordinates );
            m_free ( (char *) dimension );
            return (NULL);
        }
        dimension->coordinates[coord_count] = coordinate;
	last_coord = coordinate;
        ++coord_count;
    }
    dimension->minimum = dimension->coordinates[0];
    dimension->maximum = dimension->coordinates[dim_length - 1];
    return (dimension);
}   /*  End Function dsrw_read_dim_desc  */

/*PUBLIC_FUNCTION*/
flag dsrw_read_packet (channel, descriptor, packet)
/*  This routine will read the binary representation of the data packet
    from the channel object given by  channel  .
    The descriptor for the packet must be pointed to by  descriptor  and the
    data will be written to the storage pointed to by  packet  .
    The routine will recursively read in sub arrays and linked lists.
    The routine returns TRUE on success, else it displays an error message
    and returns FALSE.
*/
Channel channel;
packet_desc *descriptor;
char *packet;
{
    unsigned int elem_count = 0;
    unsigned int type;
    unsigned int bytes_to_read;
    extern char host_type_sizes[NUMTYPES];
    extern char *sys_errlist[];
/*
    static char function_name[] = "dsrw_read_packet";
*/

    if (channel == NULL)
    {
	(void) fprintf (stderr, "No channel to read packet from\n");
        return (FALSE);
    }
    if (descriptor == NULL)
    {
	(void) fprintf (stderr,
			"No packet descriptor for packet to be read\n");
        return (FALSE);
    }
    if (packet == NULL)
    {
	(void) fprintf (stderr, "No packet to write to\n");
        return (FALSE);
    }
    if ( ds_can_transfer_packet_as_block (descriptor) )
    {
	/*  Block write  */
	bytes_to_read = ds_get_packet_size (descriptor);
	if (ch_read (channel, packet, bytes_to_read) < bytes_to_read)
	{
	    (void) fprintf (stderr, "Error reading packet\t%s\n",
			    sys_errlist[errno]);
	    return (FALSE);
	}
	return (TRUE);
    }
    while (elem_count < descriptor->num_elements)
    {
	type = descriptor->element_types[elem_count];
        if (dsrw_read_element (channel, type,
			       (char *) descriptor->element_desc[elem_count],
			       packet)
            == FALSE)
        {
	    return (FALSE);
        }
        packet += host_type_sizes[type];
        ++elem_count;
    }
    return (TRUE);
}   /*  End Function dsrw_read_packet    */

/*PUBLIC_FUNCTION*/
flag dsrw_read_element (Channel channel, unsigned int type, char *desc,
			char *element)
/*  [PURPOSE] This routine will read the binary representation of an element
    from a channel object. If the element type is a pointer to an array or
    linked lists, the routine will recursively read in the array or linked
    list of packets.
    <channel> The channel object.
    <type> The type of the element.
    <desc> The element descriptor.
    <element> A pointer to the element storage. This will be written to.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    FString *fstring;
#ifdef MACHINE_LITTLE_ENDIAN
    int byte_count;
    extern char network_type_bytes[NUMTYPES];
#endif
    unsigned int size;
    unsigned long fstring_len;
    array_desc *arr_desc;
    char *ptr;
#if defined(NEED_ALIGNED_DATA) || defined(MACHINE_LITTLE_ENDIAN)
    char buffer[64];
#endif
    long data_s[2];
    unsigned long data_u[2];
    extern char host_type_sizes[NUMTYPES];
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_read_element";

    if (channel == NULL)
    {
	(void) fprintf (stderr, "No channel to read element from\n");
        return (FALSE);
    }
    if (element == NULL)
    {
	(void) fprintf (stderr, "No element to write to\n");
        return (FALSE);
    }
    size = host_type_sizes[type];
    if ( ds_can_transfer_element_as_block (type) )
    {
	if (ch_read (channel, element, size) != size)
	{
	    (void) fprintf (stderr, "%s: error reading atomic element\t%s\n",
			    function_name, sys_errlist[errno]);
	    return (FALSE);
	}
	return (TRUE);
    }
#if defined(NEED_ALIGNED_DATA) || defined(MACHINE_LITTLE_ENDIAN)
    if (size > 64)
    {
	(void) fprintf (stderr, "Data type size: %u too large for buffer\n",
			size);
	a_prog_bug (function_name);
    }
#endif
#ifdef MACHINE_LITTLE_ENDIAN
    switch (type)
    {
#  ifdef HAS_IEEE
      case K_FLOAT:
      case K_DOUBLE:
#  endif
      case K_INT:
      case K_SHORT:
      case K_LONG:
      case K_UINT:
      case K_USHORT:
      case K_ULONG:
	/*  NOTE: cannot simply byte-swap a complex value!  */
	if (size == network_type_bytes[type])
	{
	    if (ch_read (channel, buffer, size) != size)
	    {
		(void) fprintf (stderr,
				"%s: error reading atomic element from channel\t%s\n",
				function_name, sys_errlist[errno]);
		return (FALSE);
	    }
	    /*  Swap bytes  */
	    for (byte_count = 0; byte_count < (int) size; ++byte_count)
	    {
		element[byte_count] = buffer[size - byte_count - 1];
	    }
	    return (TRUE);
	}
	break;
    }
#endif
    /*  Now we write into the element using non-byte C types. We have to be
	careful about data alignment.  */
#ifdef NEED_ALIGNED_DATA
    ptr = buffer;
    m_copy (buffer, element, size);
#else
    ptr = element;
#endif
    switch (type)
    {
      case K_FLOAT:
	if ( !pio_read_float (channel, (float *) ptr) ) return (FALSE);
	break;
      case K_DOUBLE:
	if ( !pio_read_double (channel, (double *) ptr) ) return (FALSE);
	break;
      case K_INT:
	if ( !pio_read32s (channel, data_s) ) return (FALSE);
	*(int *) ptr = data_s[0];
	break;
      case K_SHORT:
	if ( !pio_read16s (channel, data_s) ) return (FALSE);
	*(short *) ptr = data_s[0];
	break;
      case K_COMPLEX:
	if ( !pio_read_float (channel, (float *) ptr) ) return(FALSE);
	if ( !pio_read_float (channel, (float *) ptr + 1) ) return (FALSE);
	break;
      case K_DCOMPLEX:
	if ( !pio_read_double (channel, (double *) ptr) ) return (FALSE);
	if ( !pio_read_double (channel, (double *) ptr + 1) ) return FALSE;
	break;
      case K_ICOMPLEX:
	if ( !pio_read32s (channel, data_s) ) return (FALSE);
	if ( !pio_read32s (channel, data_s + 1) ) return (FALSE);
	*(int *) ptr = data_s[0];
	*( (int *) ptr + 1 ) = data_s[1];
	break;
      case K_SCOMPLEX:
	if ( !pio_read16s (channel, data_s) ) return (FALSE);
	if ( !pio_read16s (channel, data_s + 1) ) return (FALSE);
	*(short *) ptr = data_s[0];
	*( (short *) ptr + 1 ) = data_s[1];
	break;
      case K_LONG:
      case K_LCOMPLEX:
	(void) fprintf (stderr,
			"Atomic data type: %u not supported on this architecture yet\n",
			type);
	a_prog_bug (function_name);
	break;
      case K_UINT:
	if ( !pio_read32 (channel, data_u) ) return (FALSE);
	*(unsigned int *) ptr = data_u[0];
	break;
      case K_USHORT:
	if ( !pio_read16 (channel, data_u) ) return (FALSE);
	*(unsigned short *) ptr = data_u[0];
	break;
      case K_ULONG:
	(void) fprintf (stderr,
			"Atomic data type: %u not supported on this architecture yet\n",
			type);
	a_prog_bug (function_name);
	break;
      case K_UICOMPLEX:
	if ( !pio_read32 (channel, data_u) ) return (FALSE);
	if ( !pio_read32 (channel, data_u + 1) ) return (FALSE);
	*(unsigned int *) ptr = data_u[0];
	*( (unsigned int *) ptr + 1 ) = data_u[1];
	break;
      case K_USCOMPLEX:
	if ( !pio_read16 (channel, data_u) ) return (FALSE);
	if ( !pio_read16 (channel, data_u + 1) ) return (FALSE);
	*(unsigned short *) ptr = data_u[0];
	*( (unsigned short *) ptr + 1 ) = data_u[1];
	break;
      case K_ULCOMPLEX:
	(void) fprintf (stderr,
			"Atomic data type: %u not supported on this architecture yet\n",
			type);
	a_prog_bug (function_name);
	break;
      case K_ARRAY:
	arr_desc = (array_desc *) desc;
	if ( !dsrw_read_array (channel, arr_desc, ptr, arr_desc->padded) )
	{
	    return (FALSE);
	}
	break;
      case LISTP:
	if ( !dsrw_read_list ( channel, (packet_desc *) desc,
			      *( (list_header **) ptr ) ) ) return (FALSE);
	break;
      case K_VSTRING:
	if (*(char **) ptr != NULL)
	{
	    (void) fprintf (stderr, "Variable string already allocated\n");
	    a_prog_bug (function_name);
	}
	if ( ( *(char **) ptr = pio_read_string (channel,
						     (unsigned int *) NULL) )
	    == NULL )
	{
	    /*  I'm hitting some obscure limit with the VX/MVX: so I've been
		chopping out the more recent additions until it works.
		Fuck I'm fustrated by this!  */
#ifndef OS_VXMVX
	    (void) fprintf (stderr,
			    "Could not read VSTRING for element: \"%s\"\n",
			    desc);
#endif
	    return (FALSE);
	}
	break;
      case K_FSTRING:
	fstring = (FString *) ptr;
	if ( (fstring->max_len > 0) || (fstring->string != NULL) )
	{
	    (void) fprintf (stderr, "Fixed string already allocated\n");
	    a_prog_bug (function_name);
	}
	if (pio_read32 (channel, &fstring_len) != TRUE)
	{
	    a_func_abort (function_name, "Error reading FSTRING length");
	    return (FALSE);
	}
	if ( ( fstring->string = m_alloc (fstring_len) ) == NULL )
	{
	    m_error_notify (function_name, "fixed string length");
	    return (FALSE);
	}
	if (ch_read (channel, fstring->string, fstring_len) < fstring_len)
	{
	    (void) fprintf (stderr, "Error reading fixed string element\n");
	    m_free ( fstring->string );
	    fstring->string = NULL;
	    return (FALSE);
	}
	fstring->max_len = fstring_len;
	break;
      default:
	(void) fprintf (stderr, "Bad data type: %u\n", type);
	a_prog_bug (function_name);
	break;
    }
#ifdef NEED_ALIGNED_DATA
    m_copy (element, buffer, size);
#endif
    return (TRUE);
}   /*  End Function dsrw_read_element   */

/*PUBLIC_FUNCTION*/
flag dsrw_read_array (channel, descriptor, element, pad)
/*  This routine will read in the binary representation of an array of data
    packets from the channel object given by  channel  .
    The array descriptor must be pointed to by  descriptor  .
    The element to write the array pointer to must be pointed to by  element  .
    The array will be padded on a selected boundary if the value of  pad  is
    TRUE (this is the case when reading .kf files).
    The routine will recusively read in sub arrays or linked lists.
    The routine returns TRUE on success, else it displays an error message
    and returns FALSE.
*/
Channel channel;
array_desc *descriptor;
char *element;
flag pad;
{
    flag block_transfer;
    flag local;
    unsigned int bytes_to_read;
    unsigned int array_count = 0;
    unsigned int array_size;
    unsigned int packet_size;
    unsigned int got_bytes;
    unsigned long read_pos;
    unsigned long write_pos;
    unsigned int alloc_type;
    char *addr;
    char *array;
    packet_desc *pack_desc;
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_read_array";

    FLAG_VERIFY (pad);
    if (channel == NULL)
    {
	a_func_abort (function_name, "No channel to read array from");
        return (FALSE);
    }
    if (descriptor == NULL)
    {
	(void) fprintf (stderr, "No array descriptor for array to be read\n");
        return (FALSE);
    }
    if ( (pack_desc = descriptor->packet) == NULL )
    {
	(void) fprintf (stderr, "No packet descriptor for array to be read\n");
        return (FALSE);
    }
    if (element == NULL)
    {
	(void) fprintf (stderr, "No element containing array pointer\n");
        return (FALSE);
    }
    array = *(char **) element;
    alloc_type = *(unsigned int *) (sizeof (char *) + element);
    switch (alloc_type)
    {
      case K_ARRAY_M_ALLOC:
      case K_ARRAY_MMAP:
      case K_ARRAY_UNALLOCATED:
	break;
      default:
	(void) fprintf (stderr, "Illegal array allocation type: %u\n",
			alloc_type);
	a_prog_bug (function_name);
	break;
    }
    array_size = ds_get_array_size (descriptor);
    packet_size = ds_get_packet_size ( descriptor->packet );
    block_transfer = ds_can_transfer_packet_as_block (pack_desc);
    local = ch_test_for_local_connection (channel);
    bytes_to_read = packet_size * array_size;
    if (pad)
    {
	if (read_array_padding (channel) != TRUE)
	{
	    (void) fprintf (stderr, "Error reading array padding\n");
	    return (FALSE);
	}
    }
    if (block_transfer && local)
    {
	/*  Read from local connection  */
	return ( receive_array_local (channel, array, bytes_to_read) );
    }
    if ( ch_test_for_mmap (channel) )
    {
	if (block_transfer)
	{
	    /*  Channel was memory mapped  */
	    if (array != NULL)
	    {
		/*  No data should have been allocated  */
		(void) fprintf (stderr,
				"Memory mapped array already allocated\n");
		(void) fprintf (stderr, "Top dimension of array: \"%s\"\n",
				descriptor->dimensions[0]->name);
		a_prog_bug (function_name);
	    }
	    /*  Can memory map array  */
	    if (!pad)
	    {
		(void) fprintf (stderr, "Memory mapping unpadded array\n");
	    }
	    if (ch_tell (channel, &read_pos, &write_pos) != TRUE)
	    {
		(void) fprintf (stderr, "Error getting channel positions\n");
		return (FALSE);
	    }
	    addr = ch_get_mmap_addr (channel);
	    *(char **) element = addr + read_pos;
	    *( (unsigned int *) ( element + sizeof (char *) ) ) = K_ARRAY_MMAP;
	    if (ch_seek (channel, read_pos + bytes_to_read) != TRUE)
	    {
		(void) fprintf (stderr, "Error seeking channel\n");
		return (FALSE);
	    }
	    return (TRUE);
	}
	/*  Cannot memory map array: allocate for later if not already
	    allocated  */
	if (alloc_type == K_ARRAY_UNALLOCATED)
	{
	    if (ds_alloc_array (descriptor, element, FALSE, TRUE) != TRUE)
	    {
		m_error_notify (function_name, "array data");
		return (FALSE);
	    }
	}
    }
    /*  Must get that pointer again, as things may have been allocated now  */
    array = *(char **) element;
    /*  Read from channel  */
    if (array == NULL)
    {
	(void) fprintf (stderr, "No array to write to\n");
	return (FALSE);
    }
    if (block_transfer)
    {
	/*  Can do a fast read  */
	if ( ( got_bytes = ch_read (channel, array, bytes_to_read) )
	    < bytes_to_read )
	{
	    (void) fprintf (stderr, "Error reading array\t%s\n",
			    sys_errlist[errno]);
	    (void) fprintf (stderr, "Wanted: %u bytes, got: %u bytes\n",
			    bytes_to_read, got_bytes);
	    return (FALSE);
	}
	return (TRUE);
    }
    /*  Do recursive read   */
    while (array_count++ < array_size)
    { 
	if (dsrw_read_packet (channel, descriptor->packet, array) == FALSE)
	{
	    (void) fprintf (stderr, "Error reading array packet: %u\n",
			    array_count);
	    return (FALSE);
	}
	array += packet_size;
    }
    return (TRUE);
}   /*  End Function dsrw_read_array  */

/*PUBLIC_FUNCTION*/
flag dsrw_read_list (channel, descriptor, header)
/*  This routine will read in the binary representation of a linked list of
    data from the channel object given by  channel  .
    The descriptor for the packets must be pointed to by  descriptor  and the
    linked list header must be pointed to by  header  .
    The routine will recursively read in sub arrays and linked lists.
    The linked list entries and data will be contiguous in memory.
    The routine returns TRUE on success, else it displays an error message
    and returns FALSE.
*/
Channel channel;
packet_desc *descriptor;
list_header *header;
{
    flag array_alloc;
    unsigned int length;
    unsigned long data;
    unsigned int bytes_to_read;
    unsigned int pack_size;
    unsigned int count;
    char *data_ptr;
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_read_list";

    if (channel == NULL)
    {
	(void) fprintf (stderr, "No channel to read linked list from\n");
        return (FALSE);
    }
    if (descriptor == NULL)
    {
	(void) fprintf (stderr, "No descriptor for list to be read\n");
        return (FALSE);
    }
    if (header == NULL)
    {
	(void) fprintf (stderr, "No header for list to be read\n");
    }
    if ( header->magic != MAGIC_LIST_HEADER )
    {
	(void) fprintf (stderr, "List header has bad magic number\n");
	a_prog_bug (function_name);
    }
    /*  Get linked list length  */
    if (pio_read64 (channel, &data) != TRUE)
    {
	(void) fprintf (stderr, "Error reading linked list length\n");
        return (FALSE);
    }
    length = data;
    array_alloc = ch_test_for_mmap (channel) ? FALSE : TRUE;
    if (ds_alloc_contiguous_list (descriptor, header, length, FALSE,
				  array_alloc) == FALSE)
    {
	(void) fprintf (stderr, "Error allocating memory for linked list\n");
        return (FALSE);
    }
    /*  Get sort type   */
    if (pio_read32 (channel, &data) != TRUE)
    {
	(void) fprintf (stderr, "Error reading list sort type\n");
        ds_dealloc_list_entries (descriptor, header);
        return (FALSE);
    }
    header->sort_type = data;
    switch ( header->sort_type )
    {
      case SORT_INCREASING:
      case SORT_DECREASING:
      case SORT_RANDOM:
	break;
      default:
	(void) fprintf (stderr, "Bad sort type value: %u\n",
			header->sort_type);
	ds_dealloc_list_entries (descriptor, header);
	return (FALSE);
/*
	break;
*/
    }
    /*  Get sort element number     */
    if (pio_read32 (channel, &data) != TRUE)
    {
	(void) fprintf (stderr, "Error reading list sort element number\n");
        ds_dealloc_list_entries (descriptor, header);
        return (FALSE);
    }
    header->sort_elem_num = data;
    if ( header->sort_elem_num >= descriptor->num_elements )
    {
	(void) fprintf (stderr,
			"List sort element number: %u is not less than num_elem: %u\n",
			header->sort_elem_num, descriptor->num_elements);
	(void) fprintf (stderr, "Error detected in function: %s\n",
			function_name);
        ds_dealloc_list_entries (descriptor, header);
        return (FALSE);
    }
    if (length < 1) return (TRUE);
    /*  Get list  */
    pack_size = ds_get_packet_size (descriptor);
    if ( ds_can_transfer_packet_as_block (descriptor) )
    {
	/*  Can read data in one block  */
	bytes_to_read = pack_size * length;
	if (ch_read (channel, header->contiguous_data, bytes_to_read)
	    < bytes_to_read)
	{
	    (void) fprintf (stderr,
			    "Error reading contiguous block of list packets from channel\t%s\n",
			    sys_errlist[errno]);
	    return (FALSE);
	}
	return (TRUE);
    }
    /*  Read data one packet at a time  */
    for (count = 0, data_ptr = header->contiguous_data;
	 count < header->contiguous_length; ++count, data_ptr += pack_size)
    {
	if (dsrw_read_packet (channel, descriptor, data_ptr)
	    == FALSE)
        {
	    (void) fprintf (stderr, "Error reading linked list\n");
            ds_dealloc_list_entries (descriptor, header);
            return (FALSE);
        }
    }
    return (TRUE);
}   /*  End Function dsrw_read_list  */

/*PUBLIC_FUNCTION*/
flag dsrw_read_flag (channel, logical)
/*  This routine will read in the binary representation of the the boolean
    flag from the channel object given by  channel  and will write it to
    the flag pointed to by  logical  .
    The routine returns TRUE if the flag was read in without error, else
    it returns FALSE.
*/
Channel channel;
flag *logical;
{
    char value;
    extern char *sys_errlist[];
    static char function_name[] = "dsrw_read_flag";

    if (channel == NULL)
    {
	(void) fprintf (stderr, "No channel to read flag from\n");
        return (FALSE);
    }
    if (ch_read (channel, (char *) &value, 1) != 1)
    {
	(void) fprintf (stderr, "Error reading flag from channel\t%s\n",
			sys_errlist[errno]);
        return (FALSE);
    }
    *logical = value;
    if (*logical == TRUE)
    {
	return (TRUE);
    }
    if (*logical == FALSE)
    {
	return (TRUE);
    }
    (void) fprintf (stderr, "Error reading flag, bad value: %u\n",
		    (int) *logical);
    (void) fprintf (stderr, "Function: %s error\n", function_name);
    return (FALSE);
}   /*  End Function dsrw_read_flag  */

/*PUBLIC_FUNCTION*/
flag dsrw_read_type (channel, type)
/*  This routine will read in the binary representation of the data type from
    the channel object given by  channel  and will write the type value
    into the storage pointed to by  type  .
    The routine returns TRUE if the type was read without error, else it
    returns FALSE.
*/
Channel channel;
unsigned int *type;
{
    unsigned long data;
    static char function_name[] = "dsrw_read_type";

    if (channel == NULL)
    {
	a_func_abort (function_name, "No channel to read type from");
        return (FALSE);
    }
    if (pio_read32 (channel, &data) == FALSE)
    {
	return (FALSE);
    }
    if (ds_element_is_legal (data) != TRUE)
    {
	(void) fprintf (stderr, "Function: %s\tbad data type value: %lu\n",
			function_name, data);
	return (FALSE);
    }
#ifdef RETYPE_NEEDED
    /*  Retyping needed  */
    *type = net_to_host_retype[data];
#else
    /*  No retyping needed  */
    *type = data;
#endif
    return (TRUE);
}   /*  End Function dsrw_read_type  */


/*  Private functions follow  */

static void transmit_array_local (channel, array, length)
/*  This routine will transmit an atomic array over a local connection, using
    the most efficient transport available.
    The channel must be given by  channel  .
    The array data must be pointed to by  array  .
    The length (in bytes) of the array must be given by  length  .
    The routine returns nothing.
*/
Channel channel;
char *array;
unsigned int length;
{
#if FA_SUPPORTED == FA_VX
    flag ok;
#endif
    unsigned int bytes_written;
    unsigned int control;
    extern char *sys_errlist[];
    static char function_name[] = "transmit_array_local";

    control = FA_SUPPORTED;
#if FA_SUPPORTED == FA_VX
    if ( (int) array % VXMVX_REMOTE_MEMCPY_BOUNDARY != 0 )
    {
	(void) fprintf (stderr,
			"%s: array: %p not on a %d byte boundary: slow transfer\n",
			array, VXMVX_REMOTE_MEMCPY_BOUNDARY);
	control = FA_NONE;
    }
#endif  /*  FA_SUPPORTED == FA_VX  */
    /*  Write control value  */
    if (pio_write32 (channel, control) != TRUE)
    {
	(void) fprintf (stderr, "Error writing control value\n");
	exit (RV_WRITE_ERROR);
    }
#if FA_SUPPORTED == FA_VX
    /*  Try to use  remote_memcpy  */
    if (pio_write32s ( channel, (long) task_get_id () ) != TRUE)
    {
	(void) fprintf (stderr, "Error writing task ID\n");
	exit (RV_WRITE_ERROR);
    }
    if (pio_write32s ( channel, (long) r_getppid () ) != TRUE)
    {
	(void) fprintf (stderr, "Error writing parent process ID\n");
	exit (RV_WRITE_ERROR);
    }
    if (pio_write32 ( channel, (unsigned long) array ) != TRUE)
    {
	(void) fprintf (stderr, "Error writing array address ID\n");
	exit (RV_WRITE_ERROR);
    }
    if (ch_flush (channel) != TRUE)
    {
	(void) fprintf (stderr, "Error flushing channel\t%s\n",
			sys_errlist[errno]);
	exit (RV_WRITE_ERROR);
    }
    /*  Get response  */
    if (dsrw_read_flag (channel, &ok) != TRUE)
    {
	(void) fprintf (stderr, "Error reading response flag\n");
	exit (RV_READ_ERROR);
    }
    if (ok) return;
#endif  /*  FA_SUPPORTED == FA_VX  */
    /*  Simple transfer  */
    if ( ( bytes_written = ch_write (channel, array, length) ) < length)
    {
	(void) fprintf (stderr, "Error writing array to channel\t%s\n",
			sys_errlist[errno]);
	(void) fprintf (stderr, "%s: wanted: %u bytes, wrote: %u bytes\n",
			function_name, length, bytes_written);
	exit (RV_WRITE_ERROR);
    }
}   /*  End Function transmit_array_local  */

static flag receive_array_local (channel, array, length)
/*  This routine will receive an atomic array over a local connection, using
    the most efficient transport available.
    The channel must be given by  channel  .
    The array data must be pointed to by  array  .
    The length (in bytes) of the array must be given by  length  .
    The routine returns TRUE on success, else it returns FALSE.
*/
Channel channel;
char *array;
unsigned int length;
{
    flag happy = FALSE;
    long sender_id;
    long sender_ppid;
    unsigned int bytes_read;
    unsigned long control;
    unsigned long sender_addr;
    extern char *sys_errlist[];
    static char function_name[] = "receive_array_local";

    /*  Read control value  */
    if (pio_read32 (channel, &control) != TRUE)
    {
	(void) fprintf (stderr, "Error reading control value\n");
	return (FALSE);
    }
    /*  Read special information depending on transfer type  */
    switch (control)
    {
      case FA_NONE:
	/*  Do nothing  */
	break;
      case FA_VX:
	/*  Try to use  remote_memcpy  */
	if (pio_read32s (channel, &sender_id) != TRUE)
	{
	    (void) fprintf (stderr, "Error reading task ID\n");
	    return (FALSE);
	}
	if (pio_read32s (channel, &sender_ppid) != TRUE)
	{
	    (void) fprintf (stderr, "Error reading parent process ID\n");
	    return (FALSE);
	}
	if (pio_read32 (channel, &sender_addr) != TRUE)
	{
	    (void) fprintf (stderr, "Error reading array address ID\n");
	    return (FALSE);
	}
#if FA_SUPPORTED == FA_VX
	if (r_getppid () == sender_ppid)
	{
	    if ( (int) array % VXMVX_REMOTE_MEMCPY_BOUNDARY == 0 )
	    {
		(void) remote_memcpy (task_get_id (), array,
				      sender_id, (char *) sender_addr,
				      length);
		happy = TRUE;
	    }
	    else
	    {
		(void) fprintf (stderr,
				"%s: array: %p not on a %d byte boundary: slow transfer\n",
				array, VXMVX_REMOTE_MEMCPY_BOUNDARY);
		happy = FALSE;
	    }
	}
#endif  /*  FA_SUPPORTED == FA_VX  */
	break;
      default:
	(void) fprintf (stderr, "Illegal control value: %lu\n", control);
	return (FALSE);
/*
        break;
*/
    }
    if (control != FA_NONE)
    {
	/*  Tell transmitter if we like it or not  */
	dsrw_write_flag (channel, happy);
	if (ch_flush (channel) != TRUE)
	{
	    (void) fprintf (stderr, "Error flushing channel\t%s\n",
			    sys_errlist[errno]);
	    return (FALSE);
	}
	if (happy)
	{
	    return (TRUE);
	}
    }
    /*  Simple transfer  */
    if ( ( bytes_read = ch_read (channel, array, length) ) < length)
    {
	(void) fprintf (stderr, "Error reading array from channel\t%s\n",
			sys_errlist[errno]);
	(void) fprintf (stderr, "%s: wanted: %u bytes, got: %u bytes\n",
			function_name, length, bytes_read);
	return (FALSE);
    }
    return (TRUE);
}   /*  End Function receive_array_local  */

static flag read_array_padding (channel)
/*  This routine will read array padding data from a channel.
    The channel must be given by  channel  .
    The routine returns TRUE on success, else it returns FALSE.
*/
Channel channel;
{
    char buf;
    unsigned long pad_bytes;
    extern char *sys_errlist[];

    if (pio_read32 (channel, &pad_bytes) != TRUE)
    {
	(void) fprintf (stderr, "Error reading pad size\n");
	return (FALSE);
    }
    while (pad_bytes-- > 0)
    {
	if (ch_read (channel, &buf, 1) < 1)
	{
	    (void) fprintf (stderr, "Error reading byte\t%s\n",
			    sys_errlist[errno]);
	    return (FALSE);
	}
    }
    return (TRUE);
}   /*  End Function read_array_padding  */
