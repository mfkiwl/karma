/*LINTLIBRARY*/
/*  copy.c

    This code provides data structure copying routines.

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

/*

    This file contains the various utility routines for copying the general
    data structure supported in Karma.


    Written by      Richard Gooch   18-SEP-1992: Copied from copy.c in

    Updated by      Richard Gooch   26-NOV-1992

    Updated by      Richard Gooch   1-JAN-1993: Took acccount of change to
  ds_alloc_contiguous_list

    Updated by      Richard Gooch   3-JAN-1993: Added support for K_ARRAY

    Updated by      Richard Gooch   6-JAN-1993: Changed from  #include "*.h"
  to  #include <*.h>

    Updated by      Richard Gooch   11-JAN-1993: Added support for tiling and
  address offset information in array descriptors.

    Updated by      Richard Gooch   30-JAN-1993: Added support for tiling and
  address offset information in  ds_copy_array  .

    Updated by      Richard Gooch   25-MAR-1993: Took account of changes to
  list_header  structure.

    Updated by      Richard Gooch   4-JAN-1994: Added support for K_VSTRING
  and K_FSTRING data types.

    Updated by      Richard Gooch   26-NOV-1994: Moved to  packages/ds/copy.c

    Updated by      Richard Gooch   19-APR-1995: Cleaned some code.

    Updated by      Richard Gooch   9-APR-1996: Changed to new documentation
  format.

    Updated by      Richard Gooch   24-MAY-1996: Added some code to deal with
  misaligned data accesses in <ds_copy_data>.

    Updated by      Richard Gooch   30-MAY-1996: Fixed bug with copying VSTRING
  type in <ds_copy_data> indroduced by last change.

    Updated by      Richard Gooch   3-JUN-1996: Took account of new fields in
  dimension descriptor for first and last co-ordinate.

    Last updated by Richard Gooch   29-SEP-1996: Added  CONST  declarations
  where appropriate.


*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <karma.h>
#include <karma_ds.h>
#include <karma_st.h>
#include <karma_m.h>
#include <karma_a.h>
#include <os.h>


/*  Private routines  */
static flag copy_tiled_data (/* arr_desc_inp, inp_data, arr_desc_out,
				out_data */);


/*  Public routines follow  */

/*PUBLIC_FUNCTION*/
flag ds_copy_packet (CONST packet_desc *pack_desc, char *dest_packet,
		     CONST char *source_packet)
/*  [SUMMARY] Copy a packet.
    <pack_desc> The packet descriptor.
    <dest_packet> The destination packet data will be written here.
    <source_packet> The source packet data.
    [NOTE] The routine will copy only the data and pointers to arrays and
    linked lists in the packet, not the arrays and linked lists which may be
    pointed to.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    if ( (pack_desc == NULL) || (dest_packet == NULL) ||
        (source_packet == NULL) )
    {
	return (FALSE);
    }
    if (dest_packet == source_packet)
    {
	return (TRUE);
    }
    m_copy ( dest_packet, source_packet, ds_get_packet_size (pack_desc) );
    return (TRUE);
}   /*  End Funtion ds_copy_packet  */

/*PUBLIC_FUNCTION*/
packet_desc *ds_copy_desc_until (CONST packet_desc *inp_desc, CONST char *name)
/*  [SUMMARY] Recursively copy a packet descriptor.
    [PURPOSE] This routine will recursively copy a packet descriptor until
    an element with a specified name is found, at which time the copying
    process stops. The routine will trap such errors as multiple occurences of
    the element name.
    This routine is useful to duplicate a data structure above a certain
    element or dimension. This simplifies the processing of data which is
    sitting near the bottom of a complex data structure.
    If an array or linked list pointer is one of the elements, and it is not
    successfully copied, the routine will set the output element type to NONE
    and the descriptor pointer for that element will be NULL.
    <inp_desc> The input packet descriptor.
    <name> The name of the element at which the copying process should stop.
    If this is NULL, then the entire data structure descriptor is copied.
    [RETURNS] A pointer to the new packet descriptor on success, else NULL.
*/
{
    unsigned int name_type;
    unsigned int elem_count;
    unsigned int elem_type;
    char *inp_name;
    char *out_name = NULL;
    packet_desc *return_value = NULL;
    static char function_name[] = "ds_copy_desc_until";

    if ( (inp_desc == NULL) || (inp_desc->num_elements < 1) )
    {
	return (NULL);
    }
    if (ds_f_elem_in_packet (inp_desc, name) < inp_desc->num_elements)
    {
	/*  Stop since  name  occurred in this packet   */
        return (NULL);
    }
    name_type = ds_f_name_in_packet (inp_desc, name, (char **) NULL,
				     (unsigned int *) NULL);
    switch (name_type)
    {
      case IDENT_NOT_FOUND:
	break;
      case IDENT_GEN_STRUCT:
	(void) fprintf (stderr,"Name: \"%s\" is name of a ", name);
	(void) fprintf (stderr,
			"general array structure in a packet descriptor\n");
	a_prog_bug (function_name);
	break;
      case IDENT_DIMENSION:
      case IDENT_ELEMENT:
	break;
      case IDENT_MULTIPLE:
	(void) fprintf (stderr,
			"Function: %s\tmultiple occurences of: \"%s\"\n",
			function_name, name);
	return (NULL);
/*
	break;
*/
      default:
	(void) fprintf (stderr, 
			"Function: ds_f_name_in_packet returned illegal value: %u\n",
			name_type);
	a_prog_bug (function_name);
	break;
    }
    elem_count = 0;
    if ( ( return_value = ds_alloc_packet_desc (inp_desc->num_elements) )
        == NULL )
    {
	m_error_notify (function_name, "packet descriptor");
        return (NULL);
    }
    while (elem_count < inp_desc->num_elements)
    {
	elem_type = inp_desc->element_types[elem_count];
        return_value->element_types[elem_count] = elem_type;
        switch (elem_type)
        {
	  case K_ARRAY:
	    /*  Array pointer   */
	    if ( ( return_value->element_desc[elem_count] =
		  (char *) ds_copy_array_desc_until
		  ( (array_desc *) inp_desc->element_desc[elem_count],
		   name) )
		== NULL )
	    {
		return_value->element_types[elem_count] = NONE;
	    }
	    break;
	  case LISTP:
	    /*  Linked list pointer */
	    if ( ( return_value->element_desc[elem_count] =
		  (char *) ds_copy_desc_until
		  ( (packet_desc *) inp_desc->element_desc[elem_count]
		   , name) )
		== NULL )
	    {
		return_value->element_types[elem_count] = NONE;
	    }
	    break;
	  default:
	    if ( ds_element_is_named (elem_type) )
	    {
		/*  Named data type    */
		inp_name = inp_desc->element_desc[elem_count];
		if ( (name != NULL) && (strcmp (inp_name, name) == 0) )
		{
		    /*  This is the element to stop at:
			should have been trapped    */
		    (void) fprintf (stderr, "Inconsistency in memory\n");
		    a_prog_bug (function_name);
		}
		if (inp_name != NULL)
		{
		    if ( ( out_name =
			  m_alloc ( (unsigned int) strlen (inp_name) +
				   (unsigned int) 1 )
			  ) == NULL )
		    {
			m_error_notify (function_name, "element name");
			return (NULL);
		    }
		    (void) strcpy (out_name, inp_name);
		}
		return_value->element_desc[elem_count] = out_name;
	    }
	    else
	    {
		/*  Bad data type   */
		(void) fprintf (stderr,
				"Bad data type: %u in packet\n", elem_type);
		a_prog_bug (function_name);
	    }
	    break;
        }
        ++elem_count;
    }
    return (return_value);
}   /*  End Function ds_copy_desc_until  */

/*PUBLIC_FUNCTION*/
array_desc *ds_copy_array_desc_until (CONST array_desc *inp_desc,
				      CONST char *name)
/*  [SUMMARY] Recursively copy an array descriptor.
    [PURPOSE] This routine will make a copy of an array descriptor and all
    sub-descriptors until a specified name is encountered. All tiling
    information is copied, but address offset arrays are NOT copied.
    <inp_desc> The input array descriptor.
    <name> The name specifying where the copying should stop. If this is NULL,
    the entire data structure descriptor is copied.
    [RETURNS] A pointer to the array descriptor created on success, else NULL.
*/
{
    unsigned int dim_count;
    array_desc *return_value = NULL;
    dim_desc *dimension;
    static char function_name[] = "ds_copy_array_desc_until";

    if ( (inp_desc == NULL) || (inp_desc->num_dimensions < 1) )
    {
	return (NULL);
    }
    if ( ( return_value = ds_alloc_array_desc (inp_desc->num_dimensions,
					       inp_desc->num_levels) )
        == NULL )
    {
	m_error_notify (function_name, "array descriptor");
        return (NULL);
    }
    /*  Copy bottom lengths  */
    m_copy ( (char *) return_value->lengths, (char *) inp_desc->lengths,
	    sizeof *return_value->lengths * inp_desc->num_dimensions);
    for (dim_count = 0; dim_count < inp_desc->num_dimensions; ++dim_count)
    {
	dimension = inp_desc->dimensions[dim_count];
        if ( (name != NULL) && (strcmp (name, dimension->name) == 0) )
        {
	    /*  Dimension name is  name  */
            ds_dealloc_array_desc (return_value);
            return (NULL);
        }
        else
        {
	    /*  Dimension name is not  name  */
            if ( ( return_value->dimensions[dim_count] =
		  ds_copy_dim_desc (dimension) ) == NULL )
            {
		ds_dealloc_array_desc (return_value);
		a_func_abort (function_name,
			      "Error copying dimension descriptor");
                return (NULL);
            }
        }
	if (inp_desc->tile_lengths != NULL)
	{
	    /*  Copy tile lengths  */
	    m_copy ( (char *) return_value->tile_lengths[dim_count],
		    (char *) inp_desc->tile_lengths[dim_count],
		    sizeof *inp_desc->tile_lengths[dim_count] *
		    inp_desc->num_levels );
	}
    }
    /*  Must search packet descriptor for  name */
    if (ds_f_elem_in_packet (inp_desc->packet, name) >=
	inp_desc->packet->num_elements)
    {
	/*  Copy over packet descriptor */
        if ( ( return_value->packet = ds_copy_desc_until
	      (inp_desc->packet, name) ) == NULL )
        {
	    ds_dealloc_array_desc (return_value);
	    a_func_abort (function_name, "Error copying packet descriptor");
            return (NULL);
        }
    }
    return (return_value);
}   /*  End Function ds_copy_array_desc_until  */

/*PUBLIC_FUNCTION*/
dim_desc *ds_copy_dim_desc (CONST dim_desc *inp_desc)
/*  [SUMMARY] Copy a dimension descriptor.
    <inp_desc> The input 
    [RETURNS] A pointer to a freshly allocated dimension descriptor on success,
    else NULL.
*/
{
    flag regular = TRUE;
    unsigned int coord_count = 0;
    dim_desc *return_value = NULL;
    static char function_name[] = "ds_copy_dim_desc";

    if ( (inp_desc == NULL) || (inp_desc->length < 1) )
    {
	return (NULL);
    }
    if (inp_desc->coordinates != NULL)
    {
	regular = FALSE;
    }
    if ( ( return_value = ds_alloc_dim_desc (inp_desc->name,
					     inp_desc->length,
					     inp_desc->first_coord,
					     inp_desc->last_coord, regular) )
	== NULL )
    {
	m_error_notify (function_name, "dimension descriptor");
        return (NULL);
    }
    if (!regular)
    {
	while (coord_count < inp_desc->length)
        {
	    return_value->coordinates[coord_count] =
	    inp_desc->coordinates[coord_count];
            ++coord_count;
        }
    }
    return (return_value);
}   /*  End Function ds_copy_dim_desc  */

/*PUBLIC_FUNCTION*/
flag ds_copy_data (CONST packet_desc *inp_desc, CONST char *inp_data,
		   packet_desc *out_desc, char *out_data)
/*  [SUMMARY] Copy data between Karma data structures.
    [PURPOSE] This routine will copy data from one data structure to another,
    provided the two data structures have the same format. If there are any
    variations in the two formats, the copying process is stopped at that level
    If one or more elements are different, they are not copied, however, the
    other elements are copied. This also applies to array and linked list
    pointer elements. This condition only holds for packets with the same
    number of elements, otherwise no elements are copied. The ordering of
    elements must also be the same.
    The names of elements and dimensions must be the same, as well as the
    data types. For information on array and linked list copying, see
    The routine recursively copies data in arrays and linked lists.
    [<ds_copy_array>] and [<ds_copy_list>].
    <inp_desc> The input data structure descriptor.
    <inp_data> The input data.
    <out_desc> The output data structure descriptor.
    <out_data> The output data memory allocation.
    [RETURNS] TRUE if the two packet descriptors are identical, else FALSE.
*/
{
    flag return_value = TRUE;
    unsigned int inp_elem_type;
    unsigned int out_elem_type;
    unsigned int inp_elem_size;
    unsigned int out_elem_size;
    unsigned int elem_count = 0;
    array_desc *inp_array;
    array_desc *out_array;
    packet_desc *inp_packet;
    packet_desc *out_packet;
    char *inp_ptr, *out_ptr;
    FString *inp_fstring, *out_fstring;
    extern char host_type_sizes[NUMTYPES];
    static char function_name[] = "ds_copy_data";

    if ( (inp_desc == NULL) || (out_desc == NULL) || (inp_data == NULL) ||
        (out_data == NULL) )
    {
	return (FALSE);
    }
    if (inp_desc->num_elements != out_desc->num_elements)
    {
	return (FALSE);
    }
    if (inp_desc->num_elements < 1)
    {
	return (TRUE);
    }
    while (elem_count < inp_desc->num_elements)
    {
	inp_elem_type = inp_desc->element_types[elem_count];
        inp_elem_size = host_type_sizes[inp_elem_type];
	out_elem_type = out_desc->element_types[elem_count];
        out_elem_size = host_type_sizes[out_elem_type];
        if (inp_elem_type == out_elem_type)
        {
	    switch (inp_elem_type)
	    {
	      case K_FLOAT:
	      case K_DOUBLE:
	      case K_BYTE:
	      case K_INT:
	      case K_SHORT:
	      case K_COMPLEX:
	      case K_DCOMPLEX:
	      case K_BCOMPLEX:
	      case K_ICOMPLEX:
	      case K_SCOMPLEX:
	      case K_LONG:
	      case K_LCOMPLEX:
	      case K_UBYTE:
	      case K_UINT:
	      case K_USHORT:
	      case K_ULONG:
	      case K_UBCOMPLEX:
	      case K_UICOMPLEX:
	      case K_USCOMPLEX:
	      case K_ULCOMPLEX:
		/*  Atomic data type    */
		if (strcmp (inp_desc->element_desc[elem_count],
			    out_desc->element_desc[elem_count]) == 0)
		{
		    /*  Elements have same name  */
		    m_copy (out_data, inp_data, inp_elem_size);
		}
		else
		{
		    return_value = FALSE;
		}
		break;
	      case K_ARRAY:
		/*  Array pointer   */
		inp_array = (array_desc *) inp_desc->element_desc[elem_count];
		out_array = (array_desc *) out_desc->element_desc[elem_count];
#ifdef NEED_ALIGNED_DATA
		/*  The following stuffing around with <m_copy> is required in
		    order to avoid bus errors on some systems
		    */
		m_copy ( (char *) &inp_ptr, inp_data, sizeof inp_ptr );
		m_copy ( (char *) &out_ptr, out_data, sizeof out_ptr );
#else
		inp_ptr = *(char **) inp_data;
		out_ptr = *(char **) out_data;
#endif
		(void) ds_copy_array (inp_array, inp_ptr, out_array, out_ptr);
		break;
	      case LISTP:
		/*  Linked list pointer */
		inp_packet = (packet_desc *)inp_desc->element_desc[elem_count];
		out_packet = (packet_desc *)out_desc->element_desc[elem_count];
#ifdef NEED_ALIGNED_DATA
		/*  The following stuffing around with <m_copy> is required in
		    order to avoid bus errors on some systems
		    */
		m_copy ( (char *) &inp_ptr, inp_data, sizeof inp_ptr );
		m_copy ( (char *) &out_ptr, out_data, sizeof out_ptr );
#else
		inp_ptr = *(char **) inp_data;
		out_ptr = *(char **) out_data;
#endif
		(void) ds_copy_list (inp_packet, (list_header *) inp_ptr,
				     out_packet, (list_header *) out_ptr);
		break;
	      case K_VSTRING:
#ifdef NEED_ALIGNED_DATA
		/*  The following stuffing around with <m_copy> is required in
		    order to avoid bus errors on some systems
		    */
		m_copy ( (char *) &inp_ptr, inp_data, sizeof inp_ptr );
#else
		inp_ptr = *(char **) inp_data;
#endif
		if ( ( out_ptr = m_alloc (strlen (inp_ptr) + 1) ) == NULL )
		{
		    m_abort (function_name, "new variable string");
		}
		(void) strcpy (out_ptr, inp_ptr);
#ifdef NEED_ALIGNED_DATA
		/*  The following stuffing around with <m_copy> is required in
		    order to avoid bus errors on some systems
		    */
		m_copy (out_data, (char *) &out_ptr, sizeof out_ptr);
#else
		*(char **) out_data = out_ptr;
#endif
		break;
	      case K_FSTRING:
		/*  TODO: fix this so that misaligned accesses are dealt with
		    properly  */
		inp_fstring = (FString *) inp_data;
		out_fstring = (FString *) out_data;
		if ( ( out_fstring->string =
		      m_alloc (inp_fstring->max_len) ) == NULL )
		{
		    m_abort (function_name, "new fixed string");
		}
		m_copy (out_fstring->string, inp_fstring->string,
			inp_fstring->max_len);
		out_fstring->max_len = inp_fstring->max_len;
		break;
	      default:
		/*  Wrong data type */
		(void) fprintf (stderr, "Bad data type value: %u\n",
				inp_elem_type);
		a_prog_bug (function_name);
	    }
	}
	else
	{
	    return_value = FALSE;
	}
        inp_data += inp_elem_size;
        out_data += out_elem_size;
        ++elem_count;
    }
    return (return_value);
}   /*  End Function ds_copy_data  */

/*PUBLIC_FUNCTION*/
flag ds_copy_array (CONST array_desc *inp_desc, CONST char *inp_data,
		    array_desc *out_desc, char *out_data)
/*  [SUMMARY] Recursively copy array data.
    [PURPOSE] This routine will copy data from one array to another. The two
    arrays must be the same, else the copying process will stop.
    The routine recursively copies data in the array packets.
    For information on the copying rules when the two list descriptors differ,
    see the routine [<ds_copy_data>].
    <inp_desc> The input array descriptor.
    <inp_data> The input array data.
    <out_desc> The output array descriptor.
    <out_data> The output data memory allocation.
    [RETURNS] TRUE if the two array descriptors are identical, else FALSE.
*/
{
    flag return_value = TRUE;
    unsigned int array_size;
    unsigned int packet_count;
    unsigned int inp_packet_size;
    unsigned int out_packet_size;
    unsigned int dim_count;
    unsigned int coord_count;
    dim_desc *inp_dim;
    dim_desc *out_dim;
    static char function_name[] = "ds_copy_array";

    if ( (inp_desc == NULL) || (out_desc == NULL) || (inp_data == NULL) ||
        (out_data == NULL) )
    {
	return (FALSE);
    }
    if (inp_desc->num_dimensions != out_desc->num_dimensions)
    {
	return (FALSE);
    }
    if (inp_desc->num_dimensions < 1)
    {   return (TRUE);
    }
    if ( (inp_desc->packet == NULL) || (out_desc->packet == NULL) )
    {
	return (FALSE);
    }
    /*  Test to see if dimensions are identical */
    for (dim_count = 0; dim_count < inp_desc->num_dimensions; ++dim_count)
    {
	if ( (inp_dim = inp_desc->dimensions[dim_count]) == NULL )
        {
	    (void) fprintf (stderr, "No array of input dimension pointers\n");
            a_prog_bug (function_name);
        }
        if ( (out_dim = out_desc->dimensions[dim_count]) == NULL )
        {
	    (void) fprintf (stderr, "No array of output dimension pointers\n");
            a_prog_bug (function_name);
        }
        if (inp_dim->length != out_dim->length) return (FALSE);
        if (inp_dim->first_coord != out_dim->first_coord) return (FALSE);
        if (inp_dim->last_coord != out_dim->last_coord) return (FALSE);
        if (strcmp (inp_dim->name, out_dim->name) != 0) return (FALSE);
        if (inp_dim->coordinates == NULL)
        {
	    /*  Input dimension is regularly spaced */
            if (out_dim->coordinates != NULL)
            {
		return (FALSE);
            }
        }
        else
        {
	    /*  Input dimension is not regularly spaced */
            if (out_dim->coordinates == NULL)
            {
		return (FALSE);
            }
            
            for (coord_count = 0; coord_count < inp_dim->length;
		 ++coord_count)
            {
		if (inp_dim->coordinates[coord_count] !=
                    out_dim->coordinates[coord_count])
                {
		    return (FALSE);
                }
            }
        }
    }
    /*  Test to see if array packets are similar  */
    if (inp_desc->packet->num_elements != out_desc->packet->num_elements)
    {
	return (FALSE);
    }
    /*  Copy arrays */
    if ( (inp_desc->num_levels > 0) || (out_desc->num_levels > 0) )
    {
	return ( copy_tiled_data (inp_desc, inp_data, out_desc, out_data) );
    }
    array_size = ds_get_array_size (inp_desc);
    inp_packet_size = ds_get_packet_size (inp_desc->packet);
    if ( ds_copy_data (inp_desc->packet, inp_data, out_desc->packet,out_data) )
    {
	/*  Array packets are identical  */
	if ( ds_packet_all_data (inp_desc->packet) )
	{
	    /*  Packet contains only atomics: can do fast copy  */
	    m_copy (out_data, inp_data, array_size * inp_packet_size);
	    return (TRUE);
	}
    }
    /*  Must do slow copy   */
    out_packet_size = ds_get_packet_size (out_desc->packet);
    for (packet_count = 0; packet_count < array_size; packet_count++)
    {
	if ( !ds_copy_data (inp_desc->packet, inp_data,
			    out_desc->packet, out_data) )
        {
	    return_value = FALSE;
        }
        inp_data += inp_packet_size;
        out_data += out_packet_size;
    }
    return (return_value);
}   /*  End Function ds_copy_array  */

/*PUBLIC_FUNCTION*/
flag ds_copy_list (CONST packet_desc *inp_desc, CONST list_header *inp_head,
		   packet_desc *out_desc, list_header *out_head)
/*  [SUMMARY] Recursively copy linked lists.
    [PURPOSE] This routine will copy a linked list to another.
    The routine will recursively copy sub arrays and linked lists.
    The linked list entries and data fields will be allocated. The entries
    will be contiguous in memory.
    For information on the copying rules when the two list descriptors differ,
    see the routine [<ds_copy_data>].
    <inp_desc> The input data structure descriptor.
    <inp_head> The input linked list header.
    <out_desc> The output data structure descriptor.
    <out_head> The output linked list header.
    [RETURNS] TRUE if the two packet descriptors are identical, else FALSE.
*/
{
    flag return_value = TRUE;
    unsigned int inp_pack_size;
    unsigned int out_pack_size;
    unsigned int inp_count;
    unsigned int out_count;
    char *inp_data;
    char *out_data;
    list_entry *inp_entry;
    static char function_name[] = "ds_copy_list";

    if ( (inp_desc == NULL) || (inp_head == NULL) ||
        (out_desc == NULL) || (out_head == NULL) )
    {
	return (FALSE);
    }
    if (inp_head->magic != MAGIC_LIST_HEADER)
    {
	(void) fprintf (stderr, "Input list header has bad magic number\n");
	a_prog_bug (function_name);
    }
    if (out_head->magic != MAGIC_LIST_HEADER)
    {
	(void) fprintf (stderr, "Output list header has bad magic number\n");
	a_prog_bug (function_name);
    }
    if (inp_desc->num_elements != out_desc->num_elements)
    {
	return (FALSE);
    }
    out_head->sort_type = inp_head->sort_type;
    out_head->sort_elem_num = inp_head->sort_elem_num;
    if ( !ds_alloc_contiguous_list (out_desc, out_head, inp_head->length, TRUE,
				    TRUE) )
    {
	m_error_notify (function_name, "contiguous linked list");
        return (FALSE);
    }
    /*  Process the contiguous entries of the output list  */
    inp_data = inp_head->contiguous_data;
    out_data = out_head->contiguous_data;
    inp_pack_size = ds_get_packet_size (inp_desc);
    out_pack_size = ds_get_packet_size (out_desc);
    inp_entry = inp_head->first_frag_entry;
    for (out_count = 0, inp_count = 0;
	 out_count < out_head->contiguous_length;
	 ++out_count, out_data += out_pack_size)
    {
	if (inp_count < inp_head->contiguous_length)
	{
	    /*  Still in input list's contiguous section  */
	    if ( !ds_copy_data (inp_desc, inp_data, out_desc, out_data) )
	    {
		return_value = FALSE;
	    }
	    ++inp_count;
	    inp_data += inp_pack_size;
	}
	else
	{
	    /*  In input list's fragmented section  */
	    if ( !ds_copy_data (inp_desc, inp_entry->data,
				out_desc, out_data) )
	    {
		return_value = FALSE;
	    }
	    inp_entry = inp_entry->next;
	}
    }
    return (return_value);
}   /*  End Function ds_copy_list  */

/*PUBLIC_FUNCTION*/
multi_array *ds_select_arrays (char **array_list, unsigned int num_in_list,
			       multi_array *multi_desc, flag save_unproc,
			       unsigned int **index_list)
/*  [SUMMARY] Select data structures in a multi_array descriptor.
    [PURPOSE] This routine will create a multi array descriptor which contains
    a selected number of array names.
    <array_list> The array of array names that are to be copied.
    <num_in_list> The number of array names in <<array_list>>. If this is 0,
    all array names are copied.
    <multi_desc> The input data structure.
    <save_unproc> If TRUE, all array names are copied.
    <index_list> An array of unsigned ints will be allocated by the routine and
    the pointer written here. The length of this array is equal to the number
    of arrays in the input multi_array descriptor. This array contains the
    index of the array name in the input multi array descriptor for each array
    name in the output multi_array descriptor. If there is no name match for an
    array, the index value is equal to the number of arrays in the input
    multi_array descriptor.
    [RETURNS] A pointer to the created multi array descriptor on success,
    else NULL.
*/
{
    unsigned int num_arrays = 0;
    unsigned int array_count;
    char *old_name = NULL;  /*  Initialised to keep compiler happy  */
    char *new_name;
    multi_array *return_value;
    static char function_name[] = "ds_select_arrays";

    if (multi_desc == NULL)
    {
	return (NULL);
    }
    if (multi_desc->num_arrays < 1)
    {
	(void) fprintf (stderr, "Multi array descriptor has no arrays\n");
        a_prog_bug (function_name);
    }
    if ( (num_in_list > 0) && (array_list == NULL) )
    {
	(void) fprintf (stderr,
			"Array name list length: %u but no name list given\n",
			num_in_list);
        a_prog_bug (function_name);
    }

    /*  Create array of indices */
    if ( ( *index_list = (unsigned int *) m_alloc (sizeof (**index_list) *
						   multi_desc->num_arrays)
	  ) == NULL )
    {
	m_error_notify (function_name, "array indices list");
        return (NULL);
    }
    /*  Initialize array of indices */
    array_count = 0;
    while (array_count < multi_desc->num_arrays)
    (*index_list)[array_count++] = multi_desc->num_arrays;

    if (multi_desc->num_arrays < 2)
    {
	/*  Only one array: do special case */
        if ( (num_in_list > 0) && !save_unproc )
        {
	    /*  No name match and no copying of array   */
	    a_func_abort (function_name, "No output arrays in function");
            m_free ( (char *) *index_list );
            return (NULL);
        }
        if ( ( return_value = ds_alloc_multi ( (unsigned int) 1 ) ) == NULL )
        {
	    m_error_notify (function_name, "multi array descriptor");
            m_free ( (char *) *index_list );
            return (NULL);
        }
        if (num_in_list < 1)
        {
	    (*index_list)[0] = 0;
        }
        else
        {   (*index_list)[0] = 1;
        }
        return (return_value);
    }

    if (num_in_list < 1)
    {
	/*  Name match for all  */
        array_count = 0;
        while (array_count < multi_desc->num_arrays)
        {
	    (*index_list)[array_count] = array_count;
            ++array_count;
        }
    }

    /*  Build index list  */
    if (num_in_list > 0)
    {
	array_count = 0;
	while (array_count < multi_desc->num_arrays)
	{
	    if (st_find ( (CONST char **) array_list, num_in_list,
			 multi_desc->array_names[array_count], strcmp )
		< num_in_list)
	    {
		/*  Name matched    */
		(*index_list)[num_arrays++] = array_count;
	    }
	    ++array_count;
        }
    }

    /*  Determine number of output arrays   */
    if ( (num_in_list < 1) || save_unproc )
    {
	/*  Allocate for all arrays */
        num_arrays = multi_desc->num_arrays;
    }

    /*  Create new multi array descriptor   */
    if ( ( return_value = ds_alloc_multi (num_arrays) ) == NULL )
    {
	m_error_notify (function_name, "multi array descriptor");
        m_free ( (char *) *index_list );
        return (NULL);
    }
    if (num_arrays < 2)
    {
	/*  No need to copy names of arrays  */
	return (return_value);
    }
    /*  Copy over array names   */
    array_count = 0;
    while (array_count < num_arrays)
    {
	if ( (num_in_list < 1) || save_unproc )
	{
	    /*  Copy over name always  */
	    old_name = multi_desc->array_names[array_count];
	}
	else
	{
	    /*  Copy over name only if selected (in index list)  */
	    if ( (*index_list)[array_count] < multi_desc->num_arrays )
	    {
		old_name = multi_desc->array_names[ (*index_list)[array_count] ];
	    }
	}
        if ( ( new_name = m_alloc ( (unsigned int) strlen (old_name) +
				   (unsigned) 1) ) == NULL )
        {
	    m_error_notify (function_name, "array names");
            m_free ( (char *) *index_list );
            ds_dealloc_multi (return_value);
            return (NULL);
        }
        (void) strcpy (new_name, old_name);
        return_value->array_names[array_count] = new_name;
        ++array_count;
    }
    return (return_value);
}   /*  End Function ds_select_arrays  */


/*  Private routines follow  */
static flag copy_tiled_data (arr_desc_inp, inp_data, arr_desc_out, out_data)
/*  This routine will copy data from one array to another, taking account of
    any tiling information.
    The input array descriptor must be pointed to by  arr_desc_inp  .
    The input array data must be pointed to by  inp_data  .
    The output array descriptor must be pointed to by  arr_desc_out  .
    The output array data must be pointed to by  out_data  .
    The routine returns TRUE if the arrays were identical, else it
    returns FALSE.

*/
array_desc *arr_desc_inp;
char *inp_data;
array_desc *arr_desc_out;
char *out_data;
{
    flag return_value = TRUE;
    flag more;
    int dim_count;
    unsigned int inp_offset;
    unsigned int out_offset;
    unsigned int *coords;
    static char function_name[] = "copy_tiled_data";

    if (arr_desc_inp->offsets == NULL)
    {
	if ( !ds_compute_array_offsets (arr_desc_inp) )
	{
	    m_abort (function_name, "input array offsets");
	}
    }
    if (arr_desc_out->offsets == NULL)
    {
	if ( !ds_compute_array_offsets (arr_desc_out) )
	{
	    m_abort (function_name, "output array offsets");
	}
    }
    if ( ( coords = (unsigned int *)
	  m_alloc (sizeof *coords * arr_desc_inp->num_dimensions) )
	== NULL )
    {
	m_abort (function_name, "co-ordinate array");
    }
    m_clear ( (char *) coords,
	     sizeof *coords * arr_desc_inp->num_dimensions );
    /*  Must do slow copy  */
    while (TRUE)
    {
	/*  Compute offsets  */
	inp_offset = 0;
	out_offset = 0;
	for (dim_count = 0; dim_count < arr_desc_inp->num_dimensions;
	     ++dim_count)
	{
	    inp_offset += arr_desc_inp->offsets[dim_count][ coords[dim_count] ];
	    out_offset += arr_desc_out->offsets[dim_count][ coords[dim_count] ];
	}
	if ( !ds_copy_data (arr_desc_inp->packet, inp_data + inp_offset,
			    arr_desc_out->packet, out_data + out_offset) )
        {
	    return_value = FALSE;
        }
	/*  Increment co-ordinate counters  */
	dim_count = arr_desc_inp->num_dimensions - 1;
	more = TRUE;
	while (more)
	{
	    if (++coords[dim_count] <
		arr_desc_inp->dimensions[dim_count]->length)
	    {
		/*  OK  */
		more = FALSE;
		continue;
	    }
	    coords[dim_count] = 0;
	    if (--dim_count < 0)
	    {
		m_free ( (char *) coords );
		return (return_value);
	    }
	}
    }
}   /*  End Function copy_tiled_data  */
