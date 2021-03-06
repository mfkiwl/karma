/*LINTLIBRARY*/
/*  dealloc.c

    This code provides data structure deallocation routines.

    Copyright (C) 1993-1996  Richard Gooch

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

    This file contains the various utility routines for deallocating data
  structures for the general data structure supported in Karma.


    Written by      Richard Gooch   2-JAN-1993: Extracted deallocation routines
  from  ds_alloc.c

    Updated by      Richard Gooch   3-JAN-1993

    Updated by      Richard Gooch   6-JAN-1993: Changed from  #include "*.h"
  to  #include <*.h>

    Updated by      Richard Gooch   25-MAR-1993: Took account of changes to
  list_header  structure and added  ds_dealloc_packet_subdata  .

    Updated by      Richard Gooch   17-MAY-1993: Added support for string data
  types.

    Updated by      Richard Gooch   7-AUG-1993: Documented effect of attachment
  count when deallocating  multi_array  data structures.

    Updated by      Richard Gooch   24-AUG-1993: Fixed bugs in
  ds_dealloc_packet_subdata  and  ds_dealloc_packet  when deallocating
  non-legal elements.

    Updated by      Richard Gooch   5-SEP-1993: Fixed bug in
  ds_dealloc_packet_subdata  which prevented arrays and linked list to be
  deallocated and fixed bug in  ds_dealloc_array_desc  which did not deallocate
  the offset array for each dimension.

    Updated by      Richard Gooch   6-DEC-1993: Tested for NULL  K_VSTRING
  in  ds_dealloc_packet_subdata  .

    Updated by      Richard Gooch   21-JUL-1993: Fixed bug in  ds_dealloc_array
  which deallocated unallocated memory if array packets not atomic.

    Updated by      Richard Gooch   31-JUN-1994: Tested for magic number in
  ds_dealloc_multi  .

    Updated by      Richard Gooch   26-NOV-1994: Moved to
  packages/ds/dealloc.c

    Updated by      Richard Gooch   17-JAN-1995: Changed to  destroy_callbacks
  field for  multi_array  structure.

    Updated by      Richard Gooch   19-APR-1995: Cleaned some code.

    Updated by      Richard Gooch   22-AUG-1995: Made
  <ds_dealloc_packet_subdata> more aware of alignment problems (hopefully
  fully aware).

    Updated by      Richard Gooch   9-APR-1996: Changed to new documentation
  format.

    Last updated by Richard Gooch   28-JUN-1996: Changed more pointers to
  CONST.


*/

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <karma.h>
#include <os.h>
#include <karma_ds.h>
#include <karma_m.h>
#include <karma_a.h>
#include <karma_c.h>


/*PUBLIC_FUNCTION*/
void ds_dealloc_multi (multi_array *multi_desc)
/*  [SUMMARY] Deallocate a multi_array data structure.
    [PURPOSE] This routine will deallocate all memory associated with a
    multi_array header. This includes all the descriptors and data arrays and
    lists in the hierarchy below. The routine will only deallocate the data
    structure when it's attachment count is zero, else it decrements the
    attachment count and returns. The routine is quite robust, deallocating in
    the correct order and cleanly bypassing missing sections in the hierarchy.
    <multi_desc> The descriptor.
    [RETURNS] Nothing.
*/
{
    unsigned int array_count = 0;
    history *entry;
    history *next;
    static char function_name[] = "ds_dealloc_multi";

    if (multi_desc == NULL) return;
    if (multi_desc->magic_number != MAGIC_MULTI_ARRAY)
    {
	(void) fprintf (stderr, "Bad magic number\n");
	a_prog_bug (function_name);
    }
    if (multi_desc->attachments > 0)
    {
	--multi_desc->attachments;
	return;
    }
    if (multi_desc->destroy_callbacks != NULL)
    {
	c_call_callbacks (multi_desc->destroy_callbacks, multi_desc);
	c_destroy_list (multi_desc->destroy_callbacks);
    }
    while (array_count < multi_desc->num_arrays)
    {
	if ( (multi_desc->array_names != NULL) &&
            (multi_desc->array_names[array_count] != NULL) )
        {
	    m_free (multi_desc->array_names[array_count]);
        }
        ds_dealloc_packet (multi_desc->headers[array_count],
			   multi_desc->data[array_count]);
        ++array_count;
    }
    if (multi_desc->array_names != NULL)
    {
	m_free ( ( char *) multi_desc->array_names );
    }
    if (multi_desc->headers != NULL)
    {
	m_free ( (char *) multi_desc->headers );
    }
    if (multi_desc->data != NULL)
    {
	m_free ( (char *) multi_desc->data );
    }
    for (entry = multi_desc->first_hist; entry != NULL; entry = next)
    {
	next = entry->next;
	m_free (entry->string);
	m_free ( (char *) entry );
    }
    multi_desc->magic_number = 0;
    m_clear ( (char *) multi_desc, sizeof *multi_desc );
    m_free ( (char *) multi_desc);
}   /*  End Function ds_dealloc_multi  */

/*PUBLIC_FUNCTION*/
void ds_dealloc_packet (packet_desc *pack_desc, char *data)
/*  [SUMMARY] Recursively deallocate a packet.
    [PURPOSE] This routine will deallocate all memory associated with a data
    packet. This includes all the descriptors and data arrays and lists
    associated with the packet.
    The routine is quite robust, deallocating in the correct order and cleanly
    bypassing missing sections in the hierarchy.
    <pack_desc> The descriptor for the packet.
    <data> The data for the packet.
    [RETURNS] Nothing.
*/
{
    unsigned int element_count;
    unsigned int type;

    if (pack_desc == NULL)
    {
	/*  No descriptor for packet: deallocate blindly  */
        if (data != NULL)
        {
	    m_free (data);
        }
        return;
    }
    if (data != NULL)
    {
	/*  Deallocate all the data (in all lower levels too)  */
        ds_dealloc_data (pack_desc, data);
    }
    for (element_count = 0; element_count < pack_desc->num_elements;
	 ++element_count)
    {
	type = pack_desc->element_types[element_count];
	if (ds_element_is_legal (type) != TRUE) continue;
	if ( ds_element_is_named (type) )
	{
	    /*  Named data type  */
	    m_free ( pack_desc->element_desc[element_count] );
	}
	else
	{
	    switch (type)
	    {
	      case K_ARRAY:
		/*  Array: go down  */
		ds_dealloc_array_desc ( (array_desc *)
				       pack_desc->element_desc[element_count]);
		break;
	      case LISTP:
		/*  Linked list: go down  */
		ds_dealloc_packet ( (packet_desc *)
				   pack_desc->element_desc[element_count],
				   (char *) NULL );
		break;
	      default:
		break;
	    }
	}
    }
    m_free ( (char *) pack_desc->element_types );
    m_free ( (char *) pack_desc->element_desc );
    m_free ( (char *) pack_desc);
}   /*  End Function ds_dealloc_packet  */

/*PUBLIC_FUNCTION*/
void ds_dealloc_data (packet_desc *pack_desc, char *packet)
/*  [SUMMARY] Recursively deallocate packet data.
    [PURPOSE] This routine will deallocate all memory associated with the
    storage of data in a packet. Any sub-arrays or linked lists are recursively
    deallocated.
    The routine is quite robust, deallocating in the correct order and cleanly
    bypassing missing sections in the hierarchy.
    <pack_desc> The descriptor for the packet.
    <data> The data for the packet.
    [RETURNS] Nothing.
*/
{
    if (packet == NULL)
    {
	/*  No data to deallocate  */
        return;
    }
    if (pack_desc == NULL)
    {
	/*  No descriptor for packet: deallocate blindly  */
        m_free (packet);
        return;
    }
    ds_dealloc_packet_subdata (pack_desc, packet);
    m_free (packet);
}   /*  End Function ds_dealloc_data  */

/*PUBLIC_FUNCTION*/
void ds_dealloc_packet_subdata (CONST packet_desc *pack_desc, char *packet)
/*  [SUMMARY] Recursively deallocate packet data.
    [PURPOSE] This routine will recursively deallocate all memory associated
    with the storage of sub-arrays or linked lists. The packet itself is not
    deallocated.
    The routine is quite robust, deallocating in the correct order and cleanly
    bypassing missing sections in the hierarchy.
    <pack_desc> The descriptor for the packet.
    <data> The data for the packet.
    [RETURNS] Nothing.
*/
{
    FString fstring;
    unsigned int elem_count;
    unsigned int type;
    char *element, *ptr;
    extern char host_type_sizes[NUMTYPES];

    if (packet == NULL)
    {
	/*  No data to deallocate  */
        return;
    }
    for (elem_count = 0, element = packet;
	 elem_count < pack_desc->num_elements;
	 ++elem_count, element += host_type_sizes[type])
    {
	type = pack_desc->element_types[elem_count];
	if ( !ds_element_is_legal (type) ) continue;
	if ( ds_element_is_atomic (type) ) continue;
	/*  Must specially deallocate element  */
	switch (type)
	{
	  case K_ARRAY:
	    /*  Array: go down  */
	    ds_dealloc_array ( (array_desc *)
			      pack_desc->element_desc[elem_count],
			      element );
	    break;
	  case LISTP:
	    /*  Linked list: go down  */
	    ds_dealloc_list ( (packet_desc *)
			     pack_desc->element_desc[elem_count],
			     *(list_header **) (element) );
	    break;
	  case K_VSTRING:
#ifdef NEED_ALIGNED_DATA
	    ptr = (char *) &fstring;
	    m_copy (ptr, element, sizeof ptr);
#else
	    ptr = element;
#endif
	    if (*(char **) ptr != NULL) m_free (*(char **) ptr);
	    break;
	  case K_FSTRING:
	    m_copy ( (char *) &fstring, element, sizeof fstring);
	    if (fstring.string != NULL)
	    {
		m_free (fstring.string);
	    }
	    m_clear (element, sizeof fstring);
	    break;
	  default:
	    break;
	}
    }
}   /*  End Function ds_dealloc_packet_subdata  */

/*PUBLIC_FUNCTION*/
void ds_dealloc_array_desc (array_desc *arr_desc)
/*  [SUMMARY] Recursively deallocate an array.
    [PURPOSE] This routine will deallocate all descriptor information
    associated with a tiled array. This includes all descriptors for sub-arrays
    and linked lists.
    The routine is quite robust, deallocating in the correct order and cleanly
    bypassing missing sections in the hierarchy.
    <arr_desc> The array descriptor.
    [RETURNS] Nothing.
*/
{
    unsigned int dim_count;
    packet_desc *pack_desc;
    dim_desc *dimension;

    if (arr_desc == NULL)
    {
	/*  No array descriptor to deallocate  */
        return;
    }
    /*  Deallocate dimension descriptors  */
    for (dim_count = 0; dim_count < arr_desc->num_dimensions; ++dim_count)
    {
	if ( (dimension = arr_desc->dimensions[dim_count]) != NULL )
        {
	    /*  Deallocate dimension name  */
            if ( dimension->name != NULL )
            {
		m_free ( dimension->name );
            }
            /*  Deallocate dimension co-ordinates  */
            if ( dimension->coordinates != NULL )
            {
		m_free ( (char *) dimension->coordinates );
            }
            /*  Deallocate dimension descriptor */
            m_free ( (char *) dimension);
        }
	if ( arr_desc->offsets != NULL )
	{
	    if ( arr_desc->offsets[dim_count] != NULL )
	    {
		m_free ( (char *) arr_desc->offsets[dim_count] );
	    }
	}
	if ( arr_desc->tile_lengths != NULL )
	{
	    if ( arr_desc->tile_lengths[dim_count] != NULL )
	    {
		m_free ( (char *) arr_desc->tile_lengths[dim_count] );
	    }
	}
    }
    m_free ( (char *) arr_desc->dimensions );
    if ( arr_desc->tile_lengths != NULL )
    {
	m_free ( (char *) arr_desc->tile_lengths );
    }
    if ( arr_desc->offsets != NULL )
    {
	m_free ( (char *) arr_desc->offsets );
    }
    if ( arr_desc->lengths != NULL )
    {
	m_free ( (char *) arr_desc->lengths );
    }
    if ( (pack_desc = arr_desc->packet) == NULL )
    {
	/*  No descriptor for packets in the array  */
        m_free ( (char *) arr_desc);
        return;
    }
    ds_dealloc_packet (pack_desc, (char *) NULL);
    m_free ( (char *) arr_desc);
}   /*  End Function ds_dealloc_array_desc  */

/*PUBLIC_FUNCTION*/
void ds_dealloc_list (packet_desc *list_desc, list_header *list_head)
/*  [SUMMARY] Recursively deallocate a linked list.
    [PURPOSE] This routine will deallocate all memory associated with the
    storage of data in a linked list.
    Any sub-arrays or linked lists are recursively deallocated.
    The routine is quite robust, deallocating in the correct order and cleanly
    bypassing missing sections in the hierarchy.
    The list header is also deallocated.
    <list_desc> The descriptor for the list.
    <list_head> The linked list header.
    [RETURNS] Nothing.
*/
{
    static char function_name[] = "ds_dealloc_list";

    if (list_head == NULL)
    {
	/*  No linked list to deallocate  */
        return;
    }
    if ( list_head->magic != MAGIC_LIST_HEADER )
    {
	(void) fprintf (stderr, "List header has bad magic number\n");
	a_prog_bug (function_name);
    }
    ds_dealloc_list_entries (list_desc, list_head);
    list_head->magic = 0;
    m_free ( (char *) list_head );
}   /*  End Function ds_dealloc_list  */

/*PUBLIC_FUNCTION*/
void ds_dealloc_list_entries (CONST packet_desc *list_desc,
			      list_header *list_head)
/*  [SUMMARY] Recursively deallocate list entries.
    [PURPOSE] This routine will deallocate all memory associated with the
    storage of data in a linked list.
    Any sub-arrays or linked lists are recursively deallocated.
    The routine is quite robust, deallocating in the correct order and cleanly
    bypassing missing sections in the hierarchy.
    The list header is NOT deallocated.
    <list_desc> The descriptor for the list.
    <list_head> The linked list header.
    [RETURNS] Nothing.
*/
{
    unsigned int count;
    unsigned int pack_size;
    char *data;
    list_entry *curr_entry;
    static char function_name[] = "ds_dealloc_list_entries";

    if (list_head == NULL)
    {
	/*  No linked list to deallocate  */
        return;
    }
    if ( list_head->magic != MAGIC_LIST_HEADER )
    {
	(void) fprintf (stderr, "List header has bad magic number\n");
	a_prog_bug (function_name);
    }
    if (list_desc == NULL)
    {
	/*  No descriptor for linked list data packets: deallocate blindly  */
        ds_dealloc2_list (list_head);
	return;
    }
    if (ds_packet_all_data (list_desc) != TRUE )
    {
	/*  Has sub-structure  */
	/*  Deallocate the sub-structure for contiguous data  */
	pack_size = ds_get_packet_size (list_desc);
	for (count = 0, data = list_head->contiguous_data;
	     count < list_head->contiguous_length;
	     ++count, data += pack_size)
	{
	    ds_dealloc_packet_subdata (list_desc, data);
	}
	/*  Deallocate the sub-structure for fragmented data  */
	for (curr_entry = list_head->first_frag_entry; curr_entry != NULL;
	     curr_entry = curr_entry->next)
	{
	    ds_dealloc_packet_subdata (list_desc, curr_entry->data);
	}
    }
    ds_dealloc2_list (list_head);
    list_head->sort_type = SORT_UNDEF;
    list_head->sort_elem_num = 0;
}   /*  End Function ds_dealloc_list_entries  */

/*PUBLIC_FUNCTION*/
void ds_dealloc2_list (list_header *list_head)
/*  [SUMMARY] Deallocate linked list entries.
    [PURPOSE] This routine will deallocate the data and the list entries in a
    linked list. This routine does not recursively deallocate sub-arrays or
    linked lists: it will only remove the list. The list header is NOT
    deallocated.
    <list_head> The list header.
    [RETURNS] Nothing.
*/
{
    list_entry *curr_entry;
    list_entry *next_entry;
    static char function_name[] = "ds_dealloc2_list";

    if (list_head == NULL)
    {
	/*  No list to deallocate  */
        return;
    }
    if (list_head->magic != MAGIC_LIST_HEADER)
    {
	(void) fprintf (stderr, "List header has bad magic number\n");
	a_prog_bug (function_name);
    }
    /*  Deallocate data  */
    if (list_head->contiguous_data != NULL)
    {
	/*  Free contiguous packet data  */
	m_free (list_head->contiguous_data);
    }
    /*  Deallocate the non-contiguous part of the list data  */
    for (curr_entry = list_head->first_frag_entry; curr_entry != NULL;
	 curr_entry = next_entry)
    {
	if (curr_entry->data != NULL)
        {
	    m_free (curr_entry->data);
        }
	next_entry = curr_entry->next;
	m_free ( (char *) curr_entry );
    }
    list_head->length = 0;
    list_head->contiguous_length = 0;
    list_head->contiguous_data = NULL;
    list_head->first_frag_entry = NULL;
    list_head->last_frag_entry = NULL;
}   /*  End Function ds_dealloc2_list  */

/*PUBLIC_FUNCTION*/
void ds_dealloc_array (array_desc *arr_desc, char *arr_element)
/*  [SUMMARY] Recursively deallocate array.
    [PURPOSE] This routine will deallocate all memory associated with the
    storage of data for an array.
    Any sub-arrays or linked lists are recursively deallocated.
    The routine is quite robust, deallocating in the correct order and cleanly
    bypassing missing sections in the hierarchy.
    <arr_desc> The array descriptor.
    <arr_element> The element containing the array pointer.
    [RETURNS] Nothing.
*/
{
    unsigned int packet_size;
    unsigned int array_size;
    unsigned int packet_count;
    char *array, *ptr;
    packet_desc *pack_desc;

    if (arr_element == NULL)
    {
	/*  No array to deallocate  */
        return;
    }
    array = *(char **) arr_element;
    if (array == NULL)
    {
	/*  No array to deallocate  */
        return;
    }
    if (*( (unsigned int *) ( arr_element + sizeof (char *) ) )
	!= K_ARRAY_M_ALLOC)
    {
	/*  Array was not allocated normally, forget it  */
	return;
    }
    if ( (arr_desc == NULL) ||
        ( ( pack_desc = arr_desc->packet ) == NULL ) )
    {
	/*  No descriptors for array and packets: deallocate blindly  */
        m_free (array);
        return;
    }
    if (ds_packet_all_data (pack_desc) != TRUE)
    {
	/*  Sub-structure to deallocate  */
	packet_size = ds_get_packet_size (pack_desc);
	array_size = ds_get_array_size (arr_desc);
	for (packet_count = 0, ptr = array; packet_count < array_size;
	     ++packet_count, ptr += packet_size)
	{
	    ds_dealloc_packet_subdata (pack_desc, ptr);
	}
    }
    m_free (array);
}   /*  End Function ds_dealloc_array  */
