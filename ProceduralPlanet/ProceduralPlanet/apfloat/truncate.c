#include <fcntl.h>
#include <io.h>

int truncate (const char *filename, long size)
{
   int handle, retval;

   handle = open (filename, O_WRONLY);

   retval = chsize (handle, size);

   close (handle);

   return retval;
}
