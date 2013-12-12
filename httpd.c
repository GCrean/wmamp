#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <httpd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "httpd.h"
#include "version.h"
#include "nowplaying.h"
#include "play.h"

static httpd *server;


#define HEADER(s) \
  do { \
    dumpfile((s), "www/header.tmpl", 1); \
    httpdPrintf((s), "<div id=\"content\"><div class=\"blog\">"); \
  } while(0)

#define FOOTER(s) \
  do { \
    httpdPrintf((s), "</div></div>"); \
    dumpfile((s), "www/footer.tmpl", 1); \
  } while(0)

/*
 *  dumpfile
 *
 *  raw = 1 to add <br/> when required 
 */
static void dumpfile( httpd *server, const char *filename, int raw )
{
  char buf[1024];
  int rc;
  int fd = open ( filename, O_RDONLY );

  if (fd <= 0)
    return;

  if (raw == 0)
    httpdPrintf ( server, "<pre>");

  while ( (rc = read (fd, buf, sizeof ( buf ) - 1 )) > 0) {
    buf[rc] = '\0';

    httpdPrintf ( server, "%s", buf );
  }

  if (raw == 0)
    httpdPrintf ( server, "</pre>");

  close ( fd );
}

static void index_html( httpd *server )
{
  HEADER(server);

  httpdPrintf ( server, "<p>Welcome to the web interface.<p>"
	"You can choose some options from the menu on the right if you like.");

  FOOTER(server);
}

static void notyet_html( httpd *server )
{
  HEADER(server);

  httpdPrintf ( server, "%s", "Sorry, not yet implemented." );

  FOOTER(server);
}

static void ps_html( httpd *server )
{
  HEADER(server);

  system ("ps -ef> /tmp/ps.txt");

  dumpfile ( server, "/tmp/ps.txt", 0 );
  unlink ("/tmp/ps.txt");

  FOOTER(server);
}

static void dmesg_html( httpd *server )
{
  HEADER(server);

  system ("dmesg > /tmp/dmesg.txt");

  dumpfile ( server, "/tmp/dmesg.txt", 0 );
  unlink ("/tmp/dmesg.txt");

  FOOTER(server);
}

static void credits_html( httpd *server )
{
  HEADER(server);

  dumpfile ( server, "CREDITS", 0 );
  dumpfile ( server, "PACKAGES", 0 );

  FOOTER(server);
}

static void general_html( httpd *server )
{
  HEADER(server);

  dumpfile ( server, "/proc/version", 0 );
  dumpfile ( server, "/proc/cpuinfo", 0 );
  dumpfile ( server, "/proc/meminfo", 0 );
  dumpfile ( server, "/proc/modules", 0 );
  dumpfile ( server, "/proc/mounts", 0 );
  dumpfile ( server, "/proc/devices", 0 );
  dumpfile ( server, "/proc/iomem", 0 );
  dumpfile ( server, "/proc/ioports", 0 );
  dumpfile ( server, "/proc/slabinfo", 0 );

  FOOTER(server);
}

static void version_html( httpd *server )
{
  HEADER(server);

  httpdPrintf ( server, "%s", VERSION_STR);
  dumpfile ( server, "BUILD", 0 );

  FOOTER(server);
}

static void nowplaying_xml( httpd *server )
{
  song_info_t *song = get_current_song ();

  httpdPrintf ( server, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
  httpdPrintf ( server, "<nowplaying>\n");
  httpdPrintf ( server, "<song>\n");

  if (song) {
    httpdPrintf ( server, "<title>%s</title>\n", song->song);
    httpdPrintf ( server, "<artist>%s</artist>\n", song->artist);
    httpdPrintf ( server, "<album>%s</album>\n", song->album);
  }
  else {
    httpdPrintf ( server, "<title></title>\n");
    httpdPrintf ( server, "<artist></artist>\n");
    httpdPrintf ( server, "<album></album>\n");
  }

  httpdPrintf ( server, "</song>\n");
  httpdPrintf ( server, "</nowplaying>\n");
}

static void nowplaying_html( httpd *server )
{
  HEADER(server);

  song_info_t *song = get_current_song ();

  httpdPrintf ( server, "<h3>Now Playing</h3>\n");

  if (song) {
    httpdPrintf ( server, "<ul><li>title - %s</li>\n", song->song);
    httpdPrintf ( server, "<li>artist - %s</li>\n", song->artist);
    httpdPrintf ( server, "<li>album - %s</li></ul>\n", song->album);
  }
  else {
    httpdPrintf ( server, "Nothing playing\n");
  }

  FOOTER(server);
}

/*
 *  httpd_init
 */
void httpd_init (void)
{
  /* Create an HTTP server instance: */
  /* arg NULL means all host IP addresses */
  server = httpdCreate(NULL, 8080);
  if (!server) {
    perror("[httpd]: Can't create server");
    return;
  }

  /* Set up logging: */
  httpdSetAccessLog(server, stdout);
  httpdSetErrorLog(server, stdout);

  /* Set up file base: */
  httpdSetFileBase ( server, "www");

  /* Register content: */
  httpdAddCContent(server,"/", "index.html", HTTP_TRUE, 0, index_html);
  httpdAddFileContent(server, "/", "style.css", HTTP_FALSE, 0, "style.css");
  httpdAddCContent(server,"/", "credits", HTTP_FALSE, 0, credits_html);
  httpdAddCContent(server,"/", "version", HTTP_FALSE, 0, version_html);
  httpdAddCContent(server,"/", "nowplaying.xml", HTTP_FALSE, 0, nowplaying_xml);
  httpdAddCContent(server,"/", "nowplaying", HTTP_FALSE, 0, nowplaying_html);
  httpdAddCContent(server,"/", "ps", HTTP_FALSE, 0, ps_html);
  httpdAddCContent(server,"/", "dmesg", HTTP_FALSE, 0, dmesg_html);
  httpdAddCContent(server,"/", "general", HTTP_FALSE, 0, general_html);

  httpdAddCContent(server,"/", "status", HTTP_FALSE, 0, notyet_html);
  httpdAddCContent(server,"/", "admin", HTTP_FALSE, 0, notyet_html);
  httpdAddCContent(server,"/", "browse", HTTP_FALSE, 0, notyet_html);
  httpdAddCContent(server,"/", "network", HTTP_FALSE, 0, notyet_html);
}

/*
 *  httpd_run - run as separate thread
 */
void httpd_run (void)
{
  for (;;) {
    /* Block until incoming connection from client: */
    int result = httpdGetConnection(server, NULL);

    if (result == 0)		/* timeout; should never happen! */
      continue;

    if (result < 0) {
      perror("[httpd]: Can't accept connection");
      continue;
    }

    /* Read request from client: */
    if (httpdReadRequest(server) >= 0)
      /* Process the valid request: */
      httpdProcessRequest(server);

    /* Free up stuff and reset: */
    httpdEndRequest(server);
  }
}
