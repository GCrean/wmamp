#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <zlib.h>

#define DEFLATE_BUFSIZ	16384

void
http_gzip(char *contents, int contentlength)
{
  int rc;
  char buf[DEFLATE_BUFSIZ];
  z_stream gz_mgr_struct;
  z_streamp gz_mgr = &gz_mgr_struct;
  int fd;

  /*memset(gz_mgr, 0, sizeof(*gz_mgr));*/
  gz_mgr->zalloc   = Z_NULL;
  gz_mgr->zfree    = Z_NULL;
  gz_mgr->opaque   = Z_NULL;
  /* We give it all the input data we have: */
  gz_mgr->next_in   = contents;
  gz_mgr->avail_in  = contentlength;
  gz_mgr->next_out  = buf;
  gz_mgr->avail_out = DEFLATE_BUFSIZ;

  fd = open("/tmp/xyz", O_CREAT|O_WRONLY, 0666);

#if 0
  if ((rc = deflateInit(gz_mgr, Z_DEFAULT_COMPRESSION)) != Z_OK) {
#else
  if ((rc = deflateInit2(gz_mgr, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			 24, 8, Z_DEFAULT_STRATEGY)) != Z_OK) {
#endif
    fprintf(stderr, "[deflateInit2]: error %d\n", rc);
  }

  rc = deflate(gz_mgr, Z_FINISH);
  if (rc != Z_STREAM_END) {
    fprintf(stderr, "[deflate]: error %d\n", rc);
  }
  /* consume the output: */
  write(fd, buf, DEFLATE_BUFSIZ-gz_mgr->avail_out);

  close(fd);

  if ((rc = deflateEnd(gz_mgr)) != Z_OK) {
    fprintf(stderr, "[deflateEnd]: error %d\n", rc);
  }
}

int
main(int argc, char *argv[])
{
  char *buf;
  int len;
  struct stat filestat;
  int fd;

  fd = open("/tmp/in", O_RDONLY);
  fstat(fd, &filestat);
  len = filestat.st_size;
  fprintf(stderr, "File has size: %d\n", len);
  buf = mmap(0, len, PROT_READ, MAP_SHARED, fd, 0);

  http_gzip(buf, len);

  munmap(buf, len);
  close(fd);

  return 0;
}
