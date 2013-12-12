#ifndef HTTP_H
#define HTTP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Response headers: */
#define HTTP_200_OK		"HTTP/1.1 200 OK"
#define HTTP_204_LOGOUT		"HTTP/1.1 204 Logout Successful"
#define HTTP_403_FORBIDDEN	"HTTP/1.1 403 Forbidden"
#define HTTP_503_UNAVAIL	"HTTP/1.1 503 Service Unavailable"
#define HTTP_400_BADREQ		"HTTP/1.1 400 Bad Request"
/* Length of response tag: */
#define CONTENTLENGTH		"Content-Length: "
#define CONTENTTYPE "Content-Type: "

#define CONTENT_GZIP		"Content-Encoding: gzip"


/* Block read from http server: */
typedef struct block_tag block_t;


typedef enum content_type {
	CT_MP3,
	CT_OGG,
	CT_UNKNOWN
} content_type_e;


typedef struct {
	content_type_e type;
	char *mime_string;
} content_type_t;


/* http connection record: */
typedef struct {
    int sd;
    char addr[INET_ADDRSTRLEN]; /* JDH  - this is the largest any IPv4 address will be  */
    char port[6];               /*      - ports can only be 0-65553                     */
    struct sockaddr_in server;  /*      - this is an IPv4-specific structure            */
    unsigned long contentlength;
    block_t *root;		/* blocks buffer (queue) */
    block_t *tail;
    content_type_e type;
} http_t;


#define http_reconnect		http_connect


int http_open(http_t *http, struct in_addr addr, unsigned short int port);
int http_clone(http_t *dst, http_t *src);
int http_connect(http_t *p);
int http_close(http_t *http);
int http_send(http_t *http, const char *path);
int http_send_auth(http_t *http, const char *path);
int http_send_additional_headers(http_t *p, const char *path, const char *addHeaders);
char *http_get_header(http_t *p);
int http_get_headers(http_t *p, int *gzipped);
int http_read(http_t *p, char *buf, int contentlength);

int http_get(http_t *http, char **c, int *length);

#endif

