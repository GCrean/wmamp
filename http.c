#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

/* Doesn't seem to be a good thing! Processing power of WMA11B doesn't
   way up to network delay.
*/
#define _USE_GZIP

#ifdef USE_GZIP
#include <zlib.h>
#endif

#include "http.h"
#include "authentication/hasher.h"
#include "net.h"
#include "debug_config.h"

#if DEBUG_HTTP
/*
 * This macro definition is a GNU extension to the C Standard. It lets us pass
 * a variable number of arguments and does some special magic for those cases
 * when only a string is passed. For a more detailed explanation, see the gcc
 * manual here: http://gcc.gnu.org/onlinedocs/gcc-3.4.6/gcc/Variadic-Macros.html#Variadic-Macros
 */
#define DBG_PRINTF(format, ...) fprintf(stderr, format, ## __VA_ARGS__)
#else
#define DBG_PRINTF(format, ...)
#endif


content_type_t audio_formats[] = {
    { CT_MP3, "audio/mpeg" },
    { CT_OGG, "audio/ogg" }
};

/* Size in bytes of a block received via http (MTU related?) */
#define BLOCK_SIZE      1480
#define BLOCK_LEN(b)    ((b)->end - (b)->start)

/* Block read from http server.
   Blocks may be queued up in an http_t object. Then root refers to the first
   (least recent) block received and tail to the most recent block.
*/
struct block_tag {
    char *start;			/* init: buf */
    char *end;			/* init: buf+strlen(buf) */
    char buf[BLOCK_SIZE];		/* init: data received */
    struct block_tag *next;	/* init: NULL */
};


/*
 *  http_send_internal
 */
static
int http_send_internal(http_t *p, const char *buf, int len)
{
    /* Show actual http request: */
#if DEBUG_HTTP
    fputs(buf, stderr);
#endif

    if (send(p->sd, buf, len, 0) != len) {
        perror ("send");
        return -1;
    }

    return 0;
}


/* Open another connection to the same address and port as @a src. */
int http_clone(http_t *dst, http_t *src)
{
    memset(dst, 0, sizeof(http_t));

    return http_open(dst, src->server.sin_addr, src->server.sin_port);
}


/*
 * http_open
 */
int http_open(http_t *p, struct in_addr addr, unsigned short int port)
{
    DBG_PRINTF("http_open %s:%d\n", inet_ntoa(addr), ntohs(port));

    /* has been previously closed */
    if (p->sd != -1) {
        memset(&(p->server), 0, sizeof(p->server));
        p->server.sin_family = AF_INET;
        p->server.sin_port = port;
        p->server.sin_addr.s_addr = addr.s_addr;

        p->sd = -1;
    }

    strcpy(p->addr, inet_ntoa(addr));
    sprintf(p->port, "%d", ntohs(port));

    return http_connect(p);
}


/*
 *  http_connect
 */
int http_connect(http_t *p)
{
    if (p->sd != -1) {  /* already connected */
        return p->sd;
    }

    if ((p->sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return -1;
    }

    if (connect(p->sd, (struct sockaddr*)&p->server, sizeof(p->server)) == -1) {
        http_close(p);
        perror("connect");
        return -1;
    }
    p->root = p->tail = NULL;

    return p->sd;
}


/* 
 *  http_close
 */
int http_close(http_t *p)
{
    /* free up any remaining blocks.. */
    block_t *ptr = p->root;

    while (ptr) {
        block_t *next = ptr->next;
        free(ptr);
        ptr = next;
    }
    /* Safety measure: */
    p->root = p->tail = NULL;  
  
    close(p->sd);
    p->sd = -1;

    return 0;
}


/*
 *  http_send
 */
int http_send(http_t *p, const char *path)
{
    char buf[1024];
    int len;

    http_reconnect(p);

    len = sprintf(buf,
        "GET daap://%s:%s%s HTTP/1.1\r\n"
        "Accept: */*\r\n"			/* make Emacs happy */
        "User-Agent: iTunes/4.6 (Windows; N)\r\n"
#ifdef USE_GZIP
        "Accept-Encoding: gzip\r\n"
#endif
        "Client-DAAP-Version: 3.0\r\n"
        "Client-DAAP-Access-Index: 2\r\n\r\n",
        p->addr, p->port, path);

    return http_send_internal(p, buf, len);
}


/*
 *  http_auth
 */
static void http_auth(char *hash, const char *path, int reqid)
{
    /* get these args from itune in future.. */
    GenerateHash(3, path, 2, hash, reqid);
}


/*
 *  http_send_auth
 */
int http_send_auth(http_t *p, const char *path)
{
    char hash[33];
    char buf[1024];
    int len;

    http_reconnect(p);

    memset(hash, 0, sizeof (hash));
    http_auth(hash, path, 0);
    /* Client-DAAP-Validation: 77245569C1D85E9A5FF3885B47B65010 */

    len = sprintf (buf,
        "GET daap://%s:%s%s HTTP/1.1\r\n"
        "Accept: */*\r\n"			/* make Emacs happy */
        "User-Agent: iTunes/4.6 (Windows; N)\r\n"
#ifdef USE_GZIP
        "Accept-Encoding: gzip\r\n"
#endif
        "Client-DAAP-Version: 3.0\r\n"
        "Client-DAAP-Access-Index: 2\r\n"
        "Client-DAAP-Validation: %s\r\n\r\n",
        p->addr, p->port, path, hash);

    return http_send_internal(p, buf, len);
}


/*
 *  http_send_additional_headers
 */
int http_send_additional_headers (http_t *p, const char *path,
				  const char *addHeaders)
{
    char hash[33];
    char buf[1024];
    int len;

    http_reconnect(p);

    memset(hash, 0, sizeof (hash));
    http_auth(hash, path, 1);

    len = sprintf (buf,
        "GET daap://%s:%s%s HTTP/1.1\r\n"
        "Accept: */*\r\n"			/* make Emacs happy */
        "Cache-Control: no-cache\r\n"
        "User-Agent: iTunes/4.6 (Windows; N)\r\n"
#ifdef USE_GZIP
        "Accept-Encoding: gzip\r\n"
#endif
        "Client-DAAP-Version: 3.0\r\n"
        "Client-DAAP-Access-Index: 2\r\n"
        "Client-DAAP-Validation: %s\r\n"
        "Client-DAAP-Request-ID: 1\r\n"
        "x-audiocast-udpport: 3799\r\n"
        "icy-metadata: 1\r\n"
        "%s\r\n",
        p->addr, p->port, path, hash, addHeaders);

    return http_send_internal(p, buf, len);
}


/*
 *  http_readblock
 */
int http_readblock(http_t *p)
{
    int rc;
    block_t *b = malloc(sizeof (block_t));

    if (!b) {
        DBG_PRINTF("Error allocating http block\n");
        return -1;
    }

    /* Get data from http server: */
    rc = net_recv(p->sd, b->buf, sizeof (b->buf), 0);
    /* rc = num bytes received or -1 for error */
    if (rc == 0) {		/* no more data */
        free(b);
        http_close(p);
        return -1;
    }

    if (rc < 0) {			/* error */
        DBG_PRINTF("recv (%d) errno (%d)\n", rc, errno);
        /* socket probably closed on other end.... */
        free(b);
        http_close(p);
        return -1;
    }

    b->start = b->buf;
    b->end = b->buf + rc;
    b->next = NULL;

    /* Append new element: */
    if (p->tail) {
        p->tail->next = b;
    }
    else {
        p->root = b;
    }
    p->tail = b;

    return 0;
}


#if 0
/* Read some blocks till either no more data or requested size achieved. */
static int
http_read_ahead(http_t *p, int requested)
{
  int got = 0;

  while (!http_readblock(p)) {
    block_t *last = p->tail;

    got += BLOCK_LEN(b);
    if (got >= requested)
      return 0;
  }
  return -1;
}
#endif


/* Pops block off p's list. Frees that block. (Assumes there is one!)
   If more left return 0; else return whatever http_readblock() returns.
*/
static int http_nextblock(http_t *p)
{
    block_t *next = p->root->next;

    free(p->root);
    p->root = next;

    if (next) {
        return 0;
    }
    p->tail = NULL;

    return http_readblock(p);
}


/* 
 *  http_read
 */
int http_read(http_t *p, char *buf, int contentlength)
{
    int copied = 0; 

    /* Now reading blocks in sync with consuming them.
       Alternate implementation: read several (enough) blocks ahead, then
       consume them one after the other.
     */

    if (!p->root) {			  /* buffer is empty */
        if (http_readblock(p) == -1) { /* oops, no luck getting one */
            return 0;
        }
    }

    /* Here: have at least 1 block. */

    while (copied < contentlength) {
        int have = BLOCK_LEN(p->root);		/* buffered amount */
        int need = contentlength - copied;		/* wanted amount > 0 */
        int len;

        /* Don't give more than we have: */
        if (need > have) {
            len = have;
        }
        else {
            len = need;
        }

        memcpy(buf, p->root->start, len);
        /* Advance: */
        buf += len;
        copied += len;
        p->root->start += len;

        if (copied == contentlength) { /* done */
            break;
        }

        /* Here: copied < contentlength, but we gave it all, hence
           p->root->start == p->root->end: first block is empty.
         */
        if (http_nextblock (p) == -1) {
            /* that's it; end of stream */
            DBG_PRINTF("[http_read]: end of stream\n");
            return copied;		/* < contentlength */
        }
    }
    /* copied == contentlength */

    if (p->root->start >= p->root->end) {	/* this block is emptied */
        /* May we assume there's only a single block? Guess so. */
        free(p->root);		/* bye bye block */
        p->tail = p->root = NULL;
    }

    return copied;
}


#define HTTP_NEXTLINE(p) \
    do { \
      /* Assume at \r now. */ \
      p->root->start++; \
      \
      if (p->root->start >= p->root->end) \
        http_nextblock(p); \
      \
      if (*p->root->start == '\n') \
        p->root->start++; \
      \
      if (p->root->start >= p->root->end) \
        http_nextblock(p); \
    } while (0)


/* Reads 1 line from http socket till empty line is detected, signalling the
   end of the header record. Returns the header line (empty line exclusive).
   Returns NULL when at end of header.
   Caller is responsible for freeing the returned memory.
*/
char *http_get_header(http_t *p)
{
    if (!p->root) {
        if (http_readblock (p) == -1) {
            return NULL;
        }
    }

    /* The end of the header is indicated by a blank line (\r\n). */
    if (*p->root->start == '\r') {
        HTTP_NEXTLINE(p);
        return NULL;
    }

    /* Reasonable safe size for now, fix later by reallocating if required. */
    char *buf = malloc(1024);
    char *c = buf;
    char *s = p->root->start;

    while (*s != '\r') {
        /*  DBG_PRINTF("(%c %d)", *s, *s); */

        if (c > (buf + 1024)) {
            DBG_PRINTF("[http_get_header]: Buffer size exceeded\n");
            free(buf); 
            p->root->start = s;
            return NULL;
        }

        *c++ = *s++;		/* Note: delaying advance of root->start */

        if (s >= p->root->end) {
            /* Block is emptied; get another */
            http_nextblock(p);
            s = p->root->start;
        }
    }
    *c = '\0';
    p->root->start = s;

    /* Found what we're looking for.. */

    /* Here: *p->root->starts == '\r'; read the end-of-line characters: */
    HTTP_NEXTLINE(p);

    return buf;
}


/*
 *   http_get_headers
 *
 *   Gets all the headers and processes the contentlength
 */
int http_get_headers(http_t *p, int *gzipped)
{
    int rc = 0;
    char *header = NULL;
    p->type = CT_MP3;

    /* Typical response might look like:

        HTTP/1.1 200 OK^M
        Content-Length: 32^M
        Content-Type: application/x-dmap-tagged^M
        Content-Encoding: gzip^M
        DAAP-Server: mt-daapd/0.2.3^M
        Accept-Ranges: bytes^M
        Date: Tue, 31 Jan 2006 16:46:24 GMT^M
        ^M
    */

    *gzipped = 0;
    p->contentlength = 0;

    while ((header = http_get_header(p))) {
        /* Show http response line: */
        DBG_PRINTF("%s\n", header);

        if (!strcmp(header, HTTP_200_OK)) {
            rc = 200;
        }
        else {
	        if (!strncmp(header, CONTENTTYPE, strlen (CONTENTTYPE))) {
		        int i = 0;
		        char *mime_type = header + strlen (CONTENTTYPE);

		        for (i = 0; i < CT_UNKNOWN; i++) {
			        if (!strncmp(mime_type, audio_formats[i].mime_string, strlen(audio_formats[i].mime_string))) {
				        DBG_PRINTF("Declared mime-type %d: %s\n", i, audio_formats[i].mime_string);
				        p->type = audio_formats[i].type;
				        break;
			        }
		        }
	        }
        }      
        if (!strncmp (header, CONTENTLENGTH, sizeof (CONTENTLENGTH) - 1)) {
            p->contentlength = atoi (header + sizeof (CONTENTLENGTH) - 1);
        }
        else if (!strncmp (header, CONTENT_GZIP, sizeof (CONTENT_GZIP) - 1)) {
            *gzipped = 1;
        }
        else if (!strcmp (header, HTTP_204_LOGOUT)) {
            rc = 204;
        }
        else if (!strcmp (header, HTTP_400_BADREQ)) {
            rc = 400;
        }
        else if (!strcmp (header, HTTP_403_FORBIDDEN)) {
            rc = 403;
        }
        else if (!strcmp (header, HTTP_503_UNAVAIL)) {
            rc = 503;
        }

        free (header);
    }

    return rc;
}


#ifdef USE_GZIP
#define INFLATE_BUFSIZ	2048

static int
http_gunzip(char *inbuf, int contentlen, char **gunzipped, int *len)
{
    int rc;
    char *outbuf, *p;
    int outsize, outlen;
    char buf[INFLATE_BUFSIZ];
    z_stream gz_mgr_struct;
    z_streamp gz_mgr = &gz_mgr_struct;

    gz_mgr->zalloc = Z_NULL;
    gz_mgr->zfree = Z_NULL;
    gz_mgr->opaque = Z_NULL;

    /* We give it all the input data we have: */
    gz_mgr->next_in  = inbuf;
    gz_mgr->avail_in = contentlen;

#if 1
    {
        int fd = open("/tmp/listing.gz", O_CREAT|O_WRONLY, 0666);
        write(fd, inbuf, contentlen);
        close(fd);
    }
#endif

    *gunzipped = NULL;
    *len = 0;

    if ((rc = inflateInit2(gz_mgr, 24)) != Z_OK) {
        DBG_PRINTF("[inflateInit]: error %d\n", rc);
        return rc;
    }

    outsize = 2048;
    p = outbuf = malloc(outsize);
    outlen = 0;

    do {
        int size;

        gz_mgr->next_out  = buf;
        gz_mgr->avail_out = INFLATE_BUFSIZ;
        rc = inflate(gz_mgr, Z_NO_FLUSH);
        if (rc < 0) {
            DBG_PRINTF("[inflate]: error %d\n", rc);
            break;
        }

        /* consume the output: */
        size = INFLATE_BUFSIZ - gz_mgr->avail_out;
        if (size > outsize - outlen) {
            outsize <<= 1;
            outbuf = realloc(outbuf, outsize);
            if (!outbuf) {
	            DBG_PRINTF("Cannot reallocate inflate output buffer\n");
	            rc = -1;
	            goto exit;
            }
            p = outbuf + outlen;
        }
        memcpy(p, buf, size);
        p += size;
        outlen += size;
    } while (rc != Z_STREAM_END);

exit:
    if ((rc = inflateEnd(gz_mgr)) != Z_OK)
    DBG_PRINTF("[inflateEnd]: error %d\n", rc);

    *gunzipped = outbuf;
    *len = outlen;

    return rc;
}
#endif


/* Reads up to *contentlength of http response data and returns this.
   Return value (contents) must be freed by caller!
*/
static
char *http_get_content(http_t *p, int *contentlength, int gzipped)
{
    int len = *contentlength;
    char *buf = malloc(len);

    if (!buf) {
        DBG_PRINTF("Error allocating http content (%d)\n", len);
        return NULL;
    }

    http_read(p, buf, len);
#ifdef USE_GZIP
    if (gzipped) {
        char *gunzipped;

        http_gunzip(buf, len, &gunzipped, contentlength);
        free(buf);
        buf = gunzipped;
    }
#endif

    return buf;
}


/* Gets http reply. Analyzes the header and provides the actual contents
   and its length. Caller is responsible for freeing the contents memory.
   Returns the response code.
*/
int http_get(http_t *p, char **content, int *contentlength)
{
    int gzipped;
    int rc = http_get_headers (p, &gzipped);

    *contentlength = p->contentlength;
    if (*contentlength > 0) {
        *content = http_get_content (p, contentlength, gzipped);
    }
    else {
        *content = NULL;
    }

    return rc;
}

