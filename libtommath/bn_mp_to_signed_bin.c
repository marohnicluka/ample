#include <tommath_private.h>
#ifdef BN_MP_TO_SIGNED_BIN_C
/* LibTomMath, multiple-precision integer library -- Tom St Denis
 *
 * LibTomMath is a library that provides multiple-precision
 * integer arithmetic as well as number theoretic functionality.
 *
 * The library was designed directly after the MPI library by
 * Michael Fromberger but has been written from scratch with
 * additional optimizations in place.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tstdenis82@gmail.com, http://libtom.org
 */

/* store in signed [big endian] format */
int mp_to_signed_bin(const mp_int *a, unsigned char *b)
{
   int     res;

   if ((res = mp_to_unsigned_bin(a, b + 1)) != MP_OKAY) {
      return res;
   }
   b[0] = (a->sign == MP_ZPOS) ? (unsigned char)0 : (unsigned char)1;
   return MP_OKAY;
}
#endif

/* ref:         HEAD -> develop */
/* git commit:  99057f6759576514f8d72e31854d0814a091d050 */
/* commit time: 2017-12-06 21:46:11 +0100 */
