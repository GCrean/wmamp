#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


#define SIZE 0x200000

int main (int argc, char *argv[])
{

  char buf[SIZE];
  int fd = open ("wma11b.rom", O_RDONLY);

  read (fd, buf, SIZE);
  close (fd);

  int out = open ("boot.rom", O_CREAT|O_WRONLY|O_TRUNC, 0666);
  write (out, buf, 0xE000);
  close (out);

  out = open ("settings.rom", O_CREAT|O_WRONLY|O_TRUNC, 0666);
  write (out, buf + 0xE000, 0x1000);
  close (out);


  out = open ("kernel.rom", O_CREAT|O_WRONLY|O_TRUNC, 0666);
  write (out, buf + 0x10000, SIZE - 0x10000);
  close (out);

  out = open ("kernelloader.rom", O_CREAT|O_WRONLY|O_TRUNC, 0666);
  write (out, buf + 0x10000, 0x2990);
  close (out);

  out = open ("kernel.gz", O_CREAT|O_WRONLY|O_TRUNC, 0666);
  write (out, buf + 0x10000 + 0x2990, 0x9c000 - 0x10000 - 0x2990);
  close (out);

  out = open ("ramdisc.gz", O_CREAT|O_WRONLY|O_TRUNC, 0666);
  write (out, buf + 0x9C000 + 0x14 , SIZE - 0x9C014 );
  close (out);


}
