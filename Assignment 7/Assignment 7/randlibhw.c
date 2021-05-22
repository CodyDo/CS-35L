#include "randlib.h"
#include <immintrin.h> // Needed for "_rdrand64_step" to work

/* Contains ONLY what randlib.h has. However, we will be doing this through
 hardware NOT software. The software version will have the same name but
 a different implementation */

/* Return a random value, using hardware operations.  */
static unsigned long long
hardware_rand64 (void)
{
  unsigned long long int x;
  while (! _rdrand64_step (&x))
    continue;
  return x;
}

/* Wrapper function to return. This will keep _rdrand64_step usage/immintrin.h
usage within this file */
extern unsigned long long
rand64 (void) {
    return hardware_rand64();
}

