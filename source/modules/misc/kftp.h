/*  kftp.h

    Header file for  kftp  and  kftpd  (file transfer modules for Karma).

    Copyright (C) 1994  Richard Gooch

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Richard Gooch may be reached by email at  karma-request@atnf.csiro.au
    The postal address is:
      Richard Gooch, c/o ATNF, P. O. Box 76, Epping, N.S.W., 2121, Australia.
*/

/*
    This header file prodive definitions for the Karma file transfer modules.


    Written by      Richard Gooch   3-MAY-1994

    Last updated by Richard Gooch   3-MAY-1994


*/
#include <karma.h>
#include <karma_conn.h>

#define KFTP_REQUEST_SEND (unsigned int) 0
#define KFTP_REQUEST_GET (unsigned int) 1

#define KFTP_RESPONSE_OK (unsigned int) 0
#define KFTP_RESPONSE_FAILED (unsigned int) 1
