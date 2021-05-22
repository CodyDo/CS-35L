#include "randcpuid.h"
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h> // Needed for dynamic linking: dlopen and dlclose

static bool
writebytes (unsigned long long x, int nbytes)
{
  int ndigits = nbytes * 2;
  do
    {
      if (putchar ("0123456789abcdef"[x & 0xf]) < 0)
          return false;
      x >>= 4;
      ndigits--;
    }
  while (0 < ndigits);

  return 0 <= putchar ('\n');
}

/* Main program, which outputs N bytes of random data.  */
int
main (int argc, char **argv)
{
  /* Check arguments.  */
  bool valid = false;
  long long nbytes;
  if (argc == 2)
    {
      char *endptr;
      errno = 0;
      nbytes = strtoll (argv[1], &endptr, 10);
      if (errno)
          perror (argv[1]);
      else
          valid = !*endptr && 0 <= nbytes;
    }
  if (!valid)
    {
      fprintf (stderr, "%s: usage: %s NBYTES\n", argv[0], argv[0]);
      return 1;
    }

  /* If there's no work to do, don't worry about which library to use.  */
    if (nbytes == 0)
    return 0;

  /* Everything above does not need to be changed from the original.
   Logic below has been changed from the original. It will perform
   dynamic linking depending on what is available on the system.
   */

    
  /* Now that we know we have work to do, arrange to use the
     appropriate library.  */

  // dl_helper is used to help with the dynamic linking process.
  unsigned long long (*rand64) (void);
  void *dl_helper;

  // Hardware linking
  if (rdrand_supported ())
    {
        // RTLD_LAZY means that symbols are resolved only as the code that
        // references them is executed. If the symbol is never referenced,
        // it is never resolved. The first is the pathname to the .so file
        // so use ./ to specify current directory
      dl_helper = dlopen("./randlibhw.so", RTLD_LAZY);
      
      //Check for errors. If dl_helper isn't properly initalized, exit.
      if (!dl_helper) {
        fprintf(stderr, "Error accessing randlibhw.so with dlopen(): %s\n", dlerror());
        exit(1);
      }
        
      // Link the proper command
      rand64 = dlsym(dl_helper, "rand64");
        
      // Check if there was an error in linking
      char *error = dlerror();
      if(error != NULL) {
        fprintf(stderr, "Error linking rand64 from randlibhw.so using dlsym(): %s\n", dlerror());
        exit(1);
      }
      
    }

  // Software linking
  else
    {
        dl_helper = dlopen("./randlibsw.so", RTLD_LAZY);
        
        //Check for errors. If dl_helper isn't properly initalized, exit.
        if (!dl_helper) {
          fprintf(stderr, "Error accessing randlibsw.so with dlopen(): %s\n", dlerror());
          exit(1);
        }
          
        // Link the proper command
        rand64 = dlsym(dl_helper, "rand64");
          
        // Check if there was an error in linking
        char *error = dlerror();
        if(error != NULL) {
          fprintf(stderr, "Error linking rand64 from randlibsw.so using dlsym(): %s\n", dlerror());
          exit(1);
        }
    }

  int wordsize = sizeof rand64 ();
  int output_errno = 0;

  do
    {
      unsigned long long x = rand64 ();
      int outbytes = nbytes < wordsize ? nbytes : wordsize;
      if (!writebytes (x, outbytes))
        {
          output_errno = errno;
          break;
        }
      nbytes -= outbytes;
    }
  while (0 < nbytes);

  if (fclose (stdout) != 0)
    output_errno = errno;

  if (output_errno)
    {
      errno = output_errno;
      perror ("output");
      return 1;
    }

  // Everything is complete. Use dlclose() to close the dynamic linkage!
  if (dlclose(dl_helper)) {
    fprintf(stderr, "Error with dlclose(): %s\n", dlerror());
    exit(1);
  }

  return 0;
}
