/*LINTLIBRARY*/
/*  md5.c

    This code provides support for the MD5 message digest algorithm.

    Copyright (C) 1994,1995  Richard Gooch

    Based on code obtained from Colin Plumb  (colin@nyx10.cs.du.edu)
    The MD5 algorithm is due to Ron Rivest.

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

/*  This file contains all routines needed for the generation of message
  digests using the MD5 algorithm.


    Written by      Colin Plumb     1993

    Updated by      Richard Gooch   22-AUG-1994: Modified for Karma.

    Updated by      Richard Gooch   24-AUG-1994

    Updated by      Richard Gooch   3-NOV-1994: Switched to OS_ and MACHINE_
  macros for machine determination.

    Updated by      Richard Gooch   26-NOV-1994: Moved to  packages/md/md5.c

    Updated by      Richard Gooch   27-FEB-1995: Changed to
  MACHINE_LITTLE_ENDIAN macro.

    Updated by      Richard Gooch   7-MAY-1995: Placate gcc -Wall

    Last updated by Richard Gooch   8-JUN-1995: Added temporary hack for Cray.


*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <karma.h>
#include <karma_md.h>
#include <karma_m.h>
#include <karma_a.h>
#include <os.h>


/*  Temporary hack for Cray so I can at least compile!  */
#if !defined(Kword32u) && defined(MACHINE_crayPVP)
#  define Kword32u unsigned long
#endif

#ifndef Kword32u
***error no 32 bit integer available
#endif

/*  Internal definition of MD5 context object structure type  */
struct md5_context_type
{
    Kword32u buf[4];
    Kword32u bytes[2];
    Kword32u in[16];
};


/*  Private functions follow  */

#ifdef MACHINE_LITTLE_ENDIAN
#  define byteSwap(buf, len)  /*  Nothing  */
#else

/*
 * Note: this code is harmless on little-endian machines.
 */
static void byteSwap (Kword32u *buf, unsigned words)
{
    unsigned char *p = (unsigned char *)buf;

    do
    {
	*buf++ = (Kword32u)((unsigned)p[3]<<8 | p[2]) << 16 |
	((unsigned)p[1]<<8 | p[0]);
	p += 4;
    }
    while (--words);
}
#endif

/* The four core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,in,s)  (w += f(x,y,z) + in,  w = (w<<s | w>>(32-s)) + x)

/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts bytes into longwords for this routine.
 */
static void transform (Kword32u buf[4], CONST Kword32u in[16])
{
    register Kword32u a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[ 0]+0xd76aa478,  7);
    MD5STEP(F1, d, a, b, c, in[ 1]+0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[ 2]+0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[ 3]+0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[ 4]+0xf57c0faf,  7);
    MD5STEP(F1, d, a, b, c, in[ 5]+0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[ 6]+0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[ 7]+0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[ 8]+0x698098d8,  7);
    MD5STEP(F1, d, a, b, c, in[ 9]+0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10]+0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11]+0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12]+0x6b901122,  7);
    MD5STEP(F1, d, a, b, c, in[13]+0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14]+0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15]+0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[ 1]+0xf61e2562,  5);
    MD5STEP(F2, d, a, b, c, in[ 6]+0xc040b340,  9);
    MD5STEP(F2, c, d, a, b, in[11]+0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[ 0]+0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[ 5]+0xd62f105d,  5);
    MD5STEP(F2, d, a, b, c, in[10]+0x02441453,  9);
    MD5STEP(F2, c, d, a, b, in[15]+0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[ 4]+0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[ 9]+0x21e1cde6,  5);
    MD5STEP(F2, d, a, b, c, in[14]+0xc33707d6,  9);
    MD5STEP(F2, c, d, a, b, in[ 3]+0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[ 8]+0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13]+0xa9e3e905,  5);
    MD5STEP(F2, d, a, b, c, in[ 2]+0xfcefa3f8,  9);
    MD5STEP(F2, c, d, a, b, in[ 7]+0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12]+0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[ 5]+0xfffa3942,  4);
    MD5STEP(F3, d, a, b, c, in[ 8]+0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11]+0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14]+0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[ 1]+0xa4beea44,  4);
    MD5STEP(F3, d, a, b, c, in[ 4]+0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[ 7]+0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10]+0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13]+0x289b7ec6,  4);
    MD5STEP(F3, d, a, b, c, in[ 0]+0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[ 3]+0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[ 6]+0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[ 9]+0xd9d4d039,  4);
    MD5STEP(F3, d, a, b, c, in[12]+0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15]+0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[ 2]+0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[ 0]+0xf4292244,  6);
    MD5STEP(F4, d, a, b, c, in[ 7]+0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14]+0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[ 5]+0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12]+0x655b59c3,  6);
    MD5STEP(F4, d, a, b, c, in[ 3]+0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10]+0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[ 1]+0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[ 8]+0x6fa87e4f,  6);
    MD5STEP(F4, d, a, b, c, in[15]+0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[ 6]+0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13]+0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[ 4]+0xf7537e82,  6);
    MD5STEP(F4, d, a, b, c, in[11]+0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[ 2]+0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[ 9]+0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}   /*  End Function transform  */


/*  Public functions follow  */

/*PUBLIC_FUNCTION*/
MD5Context md_md5_init ()
/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
{
    MD5Context ctx;
    static char function_name[] = "md_md5_init";

    if ( ( ctx = (MD5Context) m_alloc (sizeof *ctx) ) == NULL )
    {
	m_error_notify (function_name, "MD5 context");
	return (NULL);
    }
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bytes[0] = 0;
    ctx->bytes[1] = 0;
    return (ctx);
}   /*  End Function md_md5_init  */

/*PUBLIC_FUNCTION*/
void md_md5_update (MD5Context ctx, CONST unsigned char *buf, unsigned int len)
/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
{
    Kword32u t;

    /*  Update byte count  */

    t = ctx->bytes[0];
    if ((ctx->bytes[0] = t + len) < t)
    ctx->bytes[1]++;		/* Carry from low to high */

    t = 64 - (t & 0x3f);	/* Space available in ctx->in (at least 1) */
    if (t > len) {
	memcpy((unsigned char *)ctx->in + 64-t, buf, len);
	return;
    }

    /* First chunk is an odd size */
    memcpy((unsigned char *)ctx->in + 64-t, buf, t);
    byteSwap(ctx->in, 16);
    transform (ctx->buf, ctx->in);
    buf += t;
    len -= t;

    /* Process data in 64-byte chunks */
    while (len >= 64) {
	memcpy(ctx->in, buf, 64);
	byteSwap(ctx->in, 16);
	transform (ctx->buf, ctx->in);
	buf += 64;
	len -= 64;
    }

    /* Handle any remaining bytes of data. */
    memcpy(ctx->in, buf, len);
}   /*  End Function md_md5_update  */

/*PUBLIC_FUNCTION*/
void md_md5_final (MD5Context ctx, unsigned char digest[16])
/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
{
    int count = ctx->bytes[0] & 0x3f; /* Number of bytes in ctx->in */
    unsigned char *p = (unsigned char *)ctx->in + count;

    /* Set the first char of padding to 0x80.  There is always room. */
    *p++ = 0x80;

    /* Bytes of padding needed to make 56 bytes (-8..55) */
    count = 56 - 1 - count;

    if (count < 0) {		/* Padding forces an extra block */
	memset(p, 0, count+8);
	byteSwap(ctx->in, 16);
	transform (ctx->buf, ctx->in);
	p = (unsigned char *)ctx->in;
	count = 56;
    }
    memset(p, 0, count);
    byteSwap(ctx->in, 14);

    /* Append length in bits and transform */
    ctx->in[14] = ctx->bytes[0]<<3;
    ctx->in[15] = ctx->bytes[1]<<3 | ctx->bytes[0] >> 29;
    transform (ctx->buf, ctx->in);

    byteSwap(ctx->buf, 4);
    memcpy(digest, ctx->buf, 16);
    memset(ctx, 0, sizeof(ctx)); /* In case it's sensitive */
    m_free ( (char *) ctx );
}   /*  End Function md_md5_final  */

/*PUBLIC_FUNCTION*/
void md_md5_transform (unsigned char buf[16], CONST unsigned char in[64])
/*  This routine will run the core MD5 transform over a block of data.
    The digest accumulation buffer must be pointed to by  buf  .
    The block of data must be pointed to by  in  .
    The routine returns nothing.
*/
{
    Kword32u w_in[16];
    unsigned int count;

    byteSwap ( (Kword32u *) buf, 4 );
    for (count = 0; count < 16; ++count)
    {
	w_in[count] = *in++ | (Kword32u) *in++ << 8 |
	(Kword32u) *in++ << 16 | (Kword32u) *in++ << 24;
    }
    transform ( (Kword32u *) buf, w_in );
    byteSwap ( (Kword32u *) buf, 4 );
}   /*  End Function md_md5_transform  */
