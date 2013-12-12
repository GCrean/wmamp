#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <zlib.h>

#define INFLATE_BUFSIZ	2048

void
http_gunzip(char *contents, int contentlength)
{
  int rc;
  char buf[INFLATE_BUFSIZ];
  z_stream gz_mgr_struct;
  z_streamp gz_mgr = &gz_mgr_struct;
  int fd;
  int have = 0;

  gz_mgr->zalloc   = Z_NULL;
  gz_mgr->zfree    = Z_NULL;
  gz_mgr->opaque   = Z_NULL;
  /* We give it all the input data we have: */
  gz_mgr->next_in  = contents;
  gz_mgr->avail_in = contentlength;
  gz_mgr->next_out  = buf;
  gz_mgr->avail_out = INFLATE_BUFSIZ;

  fd = open("/tmp/out", O_CREAT|O_WRONLY, 0666);

  if ((rc = inflateInit2(gz_mgr, 24)) != Z_OK) {
    fprintf(stderr, "[inflateInit]: error %d\n", rc);
  }

  do {
    rc = inflate(gz_mgr, Z_NO_FLUSH);
    if (rc < 0) {
      fprintf(stderr, "[inflate]: error %d\n", rc);
      break;
    }
    /* consume the output: */
    have += INFLATE_BUFSIZ-gz_mgr->avail_out;
    fprintf(stderr, "[inflate]: rc: %d; have: %d\n", rc, have);
    write(fd, buf, INFLATE_BUFSIZ-gz_mgr->avail_out);

    gz_mgr->next_out  = buf;
    gz_mgr->avail_out = INFLATE_BUFSIZ;
  } while (rc != Z_STREAM_END);

  close(fd);

  if ((rc = inflateEnd(gz_mgr)) != Z_OK) {
    fprintf(stderr, "[inflateEnd]: error %d\n", rc);
  }

  /*free(contents);*/
}

int
main(int argc, char *argv[])
{
  char *buf;
  int len;
  struct stat filestat;
  int fd;

  fd = open("/tmp/xyz", O_RDONLY);
  fstat(fd, &filestat);
  len = filestat.st_size;
  fprintf(stderr, "File has size: %d\n", len);
  buf = mmap(0, len, PROT_READ, MAP_SHARED, fd, 0);

  /*
  http_gzip(buf, len);
  */
  http_gunzip(buf, len);

  munmap(buf, len);
  close(fd);

  return 0;
}
