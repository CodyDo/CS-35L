#include "randlib.h"
#include <stdio.h>  // Needed since some functions use I/O
#include <stdlib.h> // Needed for "abort()" on line 19

/* Contains  what randlib.h has. However, we will be doing this through
software NOT hardware. The hardware version will have the same name but
a different implementation */

/* Input stream containing random bytes.  */
static FILE *urandstream;

/* Initialize the software rand64 implementation. This will run with dlopen() */
__attribute__ ((constructor))
static void
software_rand64_init (void)
{
  urandstream = fopen ("/dev/urandom", "r");
  if (! urandstream)
    abort ();
}

/* Return a random value, using software operations.  */
static unsigned long long
software_rand64 (void)
{
  unsigned long long int x;
  if (fread (&x, sizeof x, 1, urandstream) != 1)
    abort ();
  return x;
}

/* Finalize the software rand64 implementation. This will run with dlclose() */
__attribute__ ((destructor))
static void
software_rand64_fini (void)
{
  fclose (urandstream);
}

/* Wrapper function to return. */
extern unsigned long long
rand64 (void) {
    return software_rand64();
}
