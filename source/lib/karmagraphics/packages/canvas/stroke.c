/*LINTLIBRARY*/
/*  stroke.c

    This code provides a routine to create an edit stroke instruction.

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

/*  This file contains a routine which can create a 2-dimensional editing
    instruction for a stroke.


    Written by      Richard Gooch   1-MAY-1993

    Updated by      Richard Gooch   2-JUN-1993: Added trapping for zero brush
  with in  canvas_create_stroke_instruction  .

    Last updated by Richard Gooch   27-AUG-1993: Took account of changes to
  iedit_  package.

    Updated by      Richard Gooch   26-NOV-1994: Moved to
  packages/canvas/stroke.c

    Updated by      Richard Gooch   14-APR-1996: Changed to new documentation
  format.

    Updated by      Richard Gooch   26-MAY-1996: Cleaned code to keep
  gcc -Wall -pedantic-errors happy.

    Last updated by Richard Gooch   21-JUL-1996: Changed to
  <canvas_convert_from_canvas_coords>.


*/
#include <stdio.h>
#include <math.h>

#define KWIN_GENERIC_ONLY
#include <karma_canvas.h>

#include <karma_iedit.h>
#include <karma_a.h>
#include <karma_m.h>


/*  Public functions follow  */

/*PUBLIC_FUNCTION*/
flag canvas_create_stroke_instruction (KWorldCanvas canvas,
				       double x0, double y0,
				       double x1, double y1,
				       unsigned int brush_width,
				       double value[2], KImageEditList ilist)
/*  [SUMMARY] Create an image editing stroke instruction.
    [PURPOSE] This routine will create a 2-dimensional edit stroke instruction,
    according to the specification in the [<iedit>] package and will append it
    to a managed edit list.
    <canvas> The world canvas which contains the world co-ordinate system.
    <x0> The starting horizontal co-ordidinate of the stroke.
    <y0> The starting vertical co-ordidinate of the stroke.
    <x1> The ending horizontal co-ordidinate of the stroke.
    <y1> The ending vertical co-ordidinate of the stroke.
    <brush_width> The width of the stroke (in canvas pixels). This must be at
    least 1.
    <value> The data value in the stroke instruction. This must be of type
    K_DCOMPLEX.
    <ilist> The managed edit list.
    [RETURNS] TRUE on success, else FALSE.
*/
{
    edit_coord brushradius;
    double px, py;
    double vector_x;
    double vector_y;
    double factor;
    double o_x, o_y;
    edit_coord *coords;
    static char function_name[] = "canvas_create_stroke_instruction";

    if (brush_width < 1)
    {
	(void) fprintf (stderr, "brush_width  must be at least 1\n");
	return (FALSE);
    }
    /*  Get pixel co-ordinates  */
    canvas_convert_from_canvas_coords (canvas, FALSE, FALSE, 1,
				       &x1, &y1, &px, &py);
    /*  Add brush width and convert back to world co-ordinates  */
    px += brush_width;
    py -= brush_width;
    canvas_convert_to_canvas_coords (canvas, FALSE, 1, &px, &py, NULL, NULL,
				     &brushradius.abscissa,
				     &brushradius.ordinate);
    brushradius.abscissa = (brushradius.abscissa - x1) / 2.0;
    brushradius.ordinate = (brushradius.ordinate - y1) / 2.0;
    if ( ( coords = iedit_alloc_edit_coords (4) ) == NULL )
    {
	m_error_notify (function_name, "array of edit co-ordinates");
	return (FALSE);
    }
    vector_x = (x1 - x0) / brushradius.abscissa;
    vector_y = (y1 - y0) / brushradius.ordinate;
    factor = 1.0 / sqrt (vector_x * vector_x + vector_y * vector_y);
    vector_x *= factor;
    vector_y *= factor;
    o_x = vector_y * brushradius.abscissa;
    o_y = -vector_x * brushradius.ordinate;
    coords[0].abscissa = x1 + o_x;
    coords[0].ordinate = y1 + o_y;
    coords[1].abscissa = x0 + o_x;
    coords[1].ordinate = y0 + o_y;
    coords[2].abscissa = x0 - o_x;
    coords[2].ordinate = y0 - o_y;
    coords[3].abscissa = x1 - o_x;
    coords[3].ordinate = y1 - o_y;
    /*  Create instruction entry  */
    return ( iedit_add_instruction (ilist, EDIT_INSTRUCTION_STROKE, coords,
				    4, value) );
}   /*  End Function canvas_create_stroke_instruction  */
