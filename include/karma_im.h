/*  karma_im.h

    Header for  im_  package.

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

    This include file contains all the definitions and function declarations
  needed to interface to the im_ routines in the Karma library.


    Written by      Richard Gooch   3-OCT-1992

    Last updated by Richard Gooch   15-APR-1995

*/

#if !defined(KARMA_H) || defined(MAKEDEPEND)
#  include <karma.h>
#endif

#if !defined(K_VERSION_H) || defined(MAKEDEPEND)
#  include <k_version.h>
#endif

#ifndef KARMA_IM_H
#define KARMA_IM_H


/*  For the file: initialise.c  */
EXTERN_FUNCTION (void im_register_module_name, (char *name_string) );
EXTERN_FUNCTION (void im_register_module_version_date, (char *date_string) );
EXTERN_FUNCTION (void im_register_lib_version, (char *version_string) );


#endif /*  KARMA_IM_H  */
