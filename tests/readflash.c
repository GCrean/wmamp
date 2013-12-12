// read wma11b flash and dump to file.
// j pitts - www.turtlehead.co.uk - 2004

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#define FLASHSIZE 0x200000
#define GZIPSTART 0x2990

int main (void)
{

  char buf[1024 * 64];

  printf ("Reading flash...\n");

  int fd = open ("/dev/flash", O_RDONLY);

  if (fd <= 0)
  {
    perror ("Open error ");
    return -1;
  }

// sainity check we're hitting the flash
  int rc = ioctl (fd, 0x40045201, 0);

  if (rc != 0x8C2)
  {
    printf ("problem with flash\n");
    return -1;
  }


  lseek (fd, 0, SEEK_SET);

  rc = read (fd, buf, 0xE000); // 56k first block

  if (rc != 0xE000)
  {
    perror ("read error ");
    close (fd);
    return -1;
  }
 

  int out = open ("/tmp/boot.rom", O_CREAT|O_WRONLY|O_TRUNC, 0666);
  if (out <= 0)
  {
    perror ("write");
    return -1;
  }

  write (out, buf, 0xE000);
  close (out);

  lseek (fd, 0x10000 + GZIPSTART, SEEK_SET);


#if 0
  rc = read (fd, buf + 0x10000, FLASHSIZE - 0x10000); // 64k -> 128k

  if (rc != FLASHSIZE - 0x10000)
  {
    perror ("read error ");
    close (fd);
    return -1;
  }
 
  out = open ("/tmp/filesys.rom", O_CREAT|O_WRONLY|O_TRUNC, 0666);
  if (out <= 0)
  {
    perror ("write");
    return -1;
  }

  write (out, buf + 0x10000, FLASHSIZE - 0x10000);

  close (out);
#endif

// read the whole damn thing..
  out = open ("/tmp/wma11b.rom", O_CREAT|O_WRONLY|O_TRUNC, 0666);
  if (out <= 0)
  {
    perror ("write");
    return -1;
  }

  int got = 0;

  
  while (got < (FLASHSIZE))
  {
    lseek (fd, got, SEEK_SET);
    rc = read (fd, buf, 0x1000);
    if (rc != 0x1000)
    {
      perror ("read");
      return -1;
    }
    got += rc;
    printf ("got %d of %d\n", got, FLASHSIZE);
    rc = write (out, buf, rc );
  }

  perror ("write ");

  close (out);

  close (fd);
  
}
