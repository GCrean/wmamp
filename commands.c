/*
  Requests (in protocol order):
  /content-codes		- content codes, names, data types
  /server-info			- information about server, name, capabilities
  /login			- begin a session; mandatory
  /databases			- list of databases; usually just 1
  /databases/1/items		- all songs
  /databases/1/containers	- all playlists
  /databases/1/items/68916.mp3?session-id=69  - an mp3 file
  /logout			- end a session

  Not used:
  /update
  /resolve
  /databases/1/containers/n/items	- all items within playlist
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "http.h"
#include "daap.h"
#include "play.h"
#include "tree.h"
#include "commands.h"
#include "song.h"
#include "playlist.h"
#include "hexdump.h"
#include "debug_config.h"

#if DEBUG_COMMANDS
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


typedef struct {
  song_info_t info;
  tree_t **root;
} callback_arg;


#if 0
/*
 *  streamSpecificFields
 *
 *  technically the same as getSpecificFields but
 *  exploits knowledge of the stream and daap
 *  to use significantly less memory.
 *
 */
static void
streamSpecificFields(http_t *p, 
		      unsigned long sessionid, 
		      unsigned long dbid,
		      unsigned long plid,
		      const char *fields,
		      void (*fp) (daap_t *, void*),
		      void *arg)
{
    char url[256];
    int rc;
    unsigned int id;
    unsigned int expected_id;
    unsigned int len;
    unsigned int got;
    int gzipped;

    if (plid) {
        sprintf(url,
	        "/databases/%ld/containers/%ld/items?type=music&meta=%s&session-id=%ld",
	        dbid,
	        plid,
	        fields,
	        sessionid);
	    expected_id = apso;
    }
    else {
        sprintf(url, 
	        "/databases/%ld/items?type=music&meta=%s&session-id=%ld", 
	        dbid, 
	        fields, 
	        sessionid);
	    expected_id = adbs;
    }
    DBG_PRINTF("request: %s\n", url);

    http_send_auth(p, url);

    rc = http_get_headers(p, &gzipped);
    /* Not handling gzipped == 1 yet! */

    if (rc != 200) {
        DBG_PRINTF("songs request failed rc=%d\n", rc);
        return;
    }

    /* if gzipped must first uncompress; better built-in to http_read. */

    /* Process top-level container: */
    http_read(p, (char*) &id, 4);
    id = ntohl(id);
    http_read(p, (char*) &len, 4);
    len = ntohl(len);
 
    /* Expect reponse id to be adbs: */
    if (id != expected_id) {
        DBG_PRINTF("commands: Expected adbs/apso container. Instead got %08x\n", id);
        return;
    }

    got = 0;
    while (got < len) {
        unsigned int newid,  newid_orig;
        unsigned int newlen, newlen_orig;
        char *buf;

        http_read(p, (char*) &newid_orig, 4);
        newid = ntohl(newid_orig);
        http_read(p, (char*) &newlen_orig, 4);
        newlen = ntohl(newlen_orig);
        got += 8;

        if (newid == mlcl) {    /* dmap.listing */
            DBG_PRINTF("commands: mlcl len = %u\n", newlen);
            continue;
        }

        /* newlen is usually small, in the 10s to a 100. */
        /*buf = malloc (8 + newlen);*/
        buf = alloca(8 + newlen);

        memcpy(buf, (char *) &newid_orig, 4);
        memcpy(buf + 4, (char *) &newlen_orig, 4);
        http_read(p, buf + 8, newlen);
        got += newlen;

        DBG_PRINTF("commands: id = %08x len = %u\n", newid, newlen);
        if (newid == mlit) { /* dmap.listingitem */
            daap_t *daap = decode_daap(buf, newlen);
            /* daap_dump(daap); */
            daap_walk(daap, fp, arg);
            daap_free(daap);
        }
#if 0
        /* JDH this code appears to exist to malloc an array that's no longer used */
        else {
            if (newid == mrco) { /* dmap.returnedcount */
                daap_t *daap = decode_daap(buf, newlen);
                if (arg == &song_db.songids) {
	                /*DBG_PRINTF("alloc songsid: %u\n", daap->data.integer);*/
	                if (song_db.songids) {
	                    DBG_PRINTF("songids already alloc'd\n");
	                }
	                song_db.songids = malloc(sizeof(song_db.songids[0]) * daap->data.integer /*#songs*/);
	                if (!song_db.songids) {
	                    DBG_PRINTF("ERROR: songids out of memory\n");
                    }
                    else {
	                    DBG_PRINTF("commands: malloc'd %ld for songids\n", sizeof(song_db.songids[0]) * daap->data.integer);
                    }
                }
                daap_free(daap);
            }
        }
#endif
        /*free (buf);*/
    }
}
#endif


/*
 *  getGeneric - much like streamSpecificFields, except we are passed a
 *               pre-composed url (the caller knows what it wants better
 *               than this function ever could) and DAAP ids that tell
 *               us what kind of data to expect.
 */
static void
getGeneric(http_t *p,
            const char *url,                /* a fully-composed DAAP request */
            unsigned int id1,               /* id for the response to the above request */
            unsigned int id2,               /* id for the container of mlits*/
		    void (*fp) (daap_t *, void*),   /* once we get an mlit, use this function to process it*/
		    void *arg)

{
    int rc;
    unsigned int id;
    unsigned int len;
    unsigned int got;
    int gzipped;

    DBG_PRINTF("commands: getGeneric\n");

    http_send_auth(p, url);

    rc = http_get_headers(p, &gzipped);
    /* Not handling gzipped == 1 yet! */

    if (rc != 200) {
        return;
    }

    /* if gzipped must first uncompress; better built-in to http_read. */

    /* Process top-level container: */
    http_read(p, (char*) &id, 4);
    id = ntohl(id);
    http_read(p, (char*) &len, 4);
    len = ntohl(len);
 
    /* JDH -make sure the top-level container is correct */
    if (id != id1) {
        DBG_PRINTF("commands: Expected %08x container, got %08x\n", id1, id);
        return;
    }

    got = 0;
    while (got < len) {
        unsigned int newid, newid_orig;
        unsigned int newlen, newlen_orig;
        char *buf;

        http_read(p, (char*) &newid_orig, 4);
        newid = ntohl(newid_orig);
        http_read(p, (char*) &newlen_orig, 4);
        newlen = ntohl(newlen_orig);
        got += 8;

        if (newid == id2) { /* typically mlcl (dmap.listing), except when browsing */
            continue;
        }

        /* newlen is usually small, in the 10s to a 100. */
        /*buf = malloc (8 + newlen);*/
        buf = alloca(8 + newlen);

        memcpy(buf, (char *) &newid_orig, 4);
        memcpy(buf + 4, (char *) &newlen_orig, 4);
        http_read(p, buf + 8, newlen);
        got += newlen;

        if (newid == mlit) { /* dmap.listingitem */
            daap_t *daap = decode_daap(buf, newlen);
            /*daap_dump(daap);*/
            daap_walk(daap, fp, arg);
            daap_free(daap);
        }
        /*free (buf);*/
    }
}


static int
process_response(http_t *p, int rc_expected)
{
    int length;
    char *content;
    int rc;

    DBG_PRINTF("request response\n");
    rc = http_get(p, &content, &length);    /* JDH - when logged in to iTunes, this never returns! */

    DBG_PRINTF("[process_response]: rc: %d, expected: %d\n", rc, rc_expected);

    if (content) {
        free (content);
    }

    return rc;
}


/*
 *  sendLogout
 */
int sendLogout(http_t *p, unsigned long sessionid)
{
    char url[256];
    int rc;

    sprintf(url, "/logout?session-id=%ld", sessionid);

    DBG_PRINTF("send logout\n");
    if (http_send_auth(p, url) != 0) {
        DBG_PRINTF("Logout failed\n");
        return -1;
    }

    DBG_PRINTF("get logout response\n");
    rc = process_response(p, 204);

    return rc;
}


#if 0
/*
dmap.listingitem c 
  dmap.itemid 5 0x149f9 84473
*/
static void
songid_callback(daap_t *d, void *arg)
{
    if (strcmp(d->name, "dmap.itemid")) {
        return;
    }

    if (song_db.songids) {
        DBG_PRINTF("songid[%u]: %lu\n", song_db.nr_songs, d->data.integer);
        song_db.songids[song_db.nr_songs++] = d->data.integer;
    }
}


/* Get all song ids.

daap.databasesongs c 
  dmap.status 5 0xc8 200
  dmap.updatetype 1 0x0 0
  dmap.specifiedtotalcount 5 0x2163 8547
  dmap.returnedcount 5 0x2163 8547
  dmap.listing c 
    dmap.listingitem c 
      dmap.itemid 5 0x149f9 84473
    dmap.listingitem c 
      dmap.itemid 5 0x14e4c 85580
      ...
*/
void getSongs(http_t *p, unsigned long sessionid, unsigned long databaseid)
{
    song_db.nr_songs = 0;
    song_db.songids  = NULL;

    streamSpecificFields (p, sessionid, databaseid, 0,
	        "dmap.itemid", songid_callback, &song_db.songids);
}


/*   getItems

daap.databasesongs c 
  dmap.status 5 0xc8 200
  dmap.updatetype 1 0x0 0
  dmap.specifiedtotalcount 5 0x2163 8547
  dmap.returnedcount 5 0x2163 8547
  dmap.listing c 
    dmap.listingitem c 
      daap.songalbum 9 Tycoon [Berger,Plamondon,Rice]
      daap.songartist 9 Kevin Robinson
      dmap.itemid 5 0x149f9 84473
      dmap.itemname 9 Pollution's child
      ...
*/
daap_t *getItems(http_t *p, unsigned long sessionid, unsigned long databaseid)
{
  char url[256];

  sprintf (url,
	   "/databases/%ld/items?type=music&meta=dmap.itemid,"
	   "dmap.itemname,daap.songalbum,daap.songartist&session-id=%ld",
	   databaseid, sessionid);

  http_send_auth (p, url);

  int length;
  char *content;
  int rc = http_get(p, &content, &length);

  if ((rc == 200) && content && length) {
    daap_t *daap = decode_daap (content, length);
    /*daap_dump (daap);*/
    free(content);
    return daap;
  }
  return NULL;
}


static void
getBrowseResponse(http_t *p, 
		        unsigned long sessionid, 
		        unsigned long dbid,
		        unsigned int browsereq,
		        void (*fp) (daap_t *, void*),
		        void *arg)
{
    char url[256];
    int rc;
    unsigned int id;
    unsigned int len;
    unsigned int got;
    char browsename[32];
    int gzipped;

    DBG_PRINTF("getBrowseResponse\n");

    switch (browsereq) {
    case abal:
        strncpy(browsename, "albums", 32);
        break;
    case abar:
        strncpy(browsename, "artists", 32);
        break;
    case abcp:
        strncpy(browsename, "composers", 32);
        break;
    case abgn:
        strncpy(browsename, "genres", 32);
        break;
    }

    /* sprintf(url, "/databases/%ld/browse/%s?type=music&session-id=%ld", dbid, browsename, sessionid); */
    sprintf(url, "/databases/%ld/browse/%s?type=music&session-id=%ld", dbid, browsename, sessionid);
    DBG_PRINTF("%s\n", url);

    http_send_auth(p, url);

    rc = http_get_headers(p, &gzipped);
    /* Not handling gzipped == 1 yet! */

    if (rc != 200) {
        DBG_PRINTF("browse request failed rc=%d\n", rc);
        return;
    }

    /* if gzipped must first uncompress; better built-in to http_read. */

    /* Process top-level container: */
    http_read(p, (char*) &id, 4);
    id = ntohl(id);
    http_read(p, (char*) &len, 4);
    len = ntohl(len);
 
    /* Expect reponse id to be adbs: */
    if (id != abro) {
        DBG_PRINTF("commands: Expected abro container. Instead got %08x\n", id);
        return;
    }

    got = 0;
    while (got < len) {
        unsigned int newid,  newid_orig;
        unsigned int newlen, newlen_orig;
        char *buf;

        http_read(p, (char*) &newid_orig, 4);
        newid = ntohl(newid_orig);
        http_read(p, (char*) &newlen_orig, 4);
        newlen = ntohl(newlen_orig);
        got += 8;

        if (newid == browsereq) {    /* dmap.listing */
            continue;
        }

        /* newlen is usually small, in the 10s to a 100. */
        /*buf = malloc (8 + newlen);*/
        if (newlen) {   /* JDH - iTunes throws out a zero-length record at the end of the playlist that must be ignored */
            buf = alloca(8 + newlen);
            if (buf) {
                DBG_PRINTF("alloca'd %u\n", 8 + newlen);
            }
            else {
                DBG_PRINTF("alloca failed. That's bad.\n");
            }

            memcpy(buf, (char *) &newid_orig, 4);
            memcpy(buf + 4, (char *) &newlen_orig, 4);
            http_read(p, buf + 8, newlen);
            got += newlen;

            if (newid == mlit) { /* dmap.listingitem */
                daap_t *daap = decode_daap(buf, newlen);
                /*daap_dump(daap);*/
                daap_walk(daap, fp, arg);
                daap_free(daap);
            }
            /*free (buf);*/
        }
    }
}
#endif


/*  
 *   browse_callback
 */
static
void browse_callback(daap_t *d, void *arg)
{
    char *browsename;
    callback_arg *p;
    tree_t **root;

    p = (callback_arg*) arg;
    root = p->root;

    DBG_PRINTF("browse_callback\n");

    if (strcmp(d->name, "dmap.listingitem")) {
        DBG_PRINTF("\tnot an item listing\n");
        return;
    }

    browsename = strdup(d->data.text);

    DBG_PRINTF("commands: add '%s'\n", browsename);
    if (!tree_add(root, browsename, browsename)) {
        DBG_PRINTF("\tcommands: duplicate '%s'\n", browsename);
        free(browsename);
    }

}


/*
 *  browseArtists
 */
tree_t *browseArtists(http_t *p, unsigned long sessionid, unsigned long databaseid)
{
    tree_t *root = NULL;
    callback_arg arg;
    char url[256];

    memset(&arg, 0, sizeof(arg));
    arg.root = &root;

    DBG_PRINTF("browseArtists\n");
    sprintf(url, "/databases/%ld/browse/artists?type=music&session-id=%ld", dbid, sessionid);

#if 0
    getBrowseResponse(p, sessionid, databaseid, abar, browse_callback, (void *) &arg);
#else
    getGeneric(p, url, abro, abar, browse_callback, (void *) &arg);
#endif

    return root;
}


/*
 *  browseAlbums
 */
tree_t *browseAlbums(http_t *p, unsigned long sessionid, unsigned long databaseid)
{
    tree_t *root = NULL;
    callback_arg arg;
    char url[256];

    memset(&arg, 0, sizeof(arg));
    arg.root = &root;

    DBG_PRINTF("browseAlbums\n");
    sprintf(url, "/databases/%ld/browse/albums?type=music&session-id=%ld", dbid, sessionid);

#if 0
    getBrowseResponse(p, sessionid, databaseid, abal, browse_callback, (void *) &arg);
#else
    getGeneric(p, url, abro, abal, browse_callback, (void *) &arg);
#endif

    return root;
}


/*
 *  browseGenres
 */
tree_t *browseGenres(http_t *p, unsigned long sessionid, unsigned long databaseid)
{
    tree_t *root = NULL;
    callback_arg arg;
    char url[256];

    memset(&arg, 0, sizeof(arg));
    arg.root = &root;

    DBG_PRINTF("browseGenres\n");
    sprintf(url, "/databases/%ld/browse/genres?type=music&session-id=%ld", dbid, sessionid);

#if 0
    getBrowseResponse(p, sessionid, databaseid, abgn, browse_callback, (void *) &arg);
#else
    getGeneric(p, url, abro, abgn, browse_callback, (void *) &arg);
#endif

    return root;
}


/*
 *  getSong
 */
int getSong(http_t *p, unsigned long sessionid, unsigned long databaseid, const char *song)
{
    char url[256];
    int gzipped;

    sprintf(url,
        "/databases/%ld/items/%s?session-id=%ld",
        databaseid, song, sessionid);

    http_send_additional_headers(p, url, "Connection: close\r\n");

    return http_get_headers(p, &gzipped);
}


/*
 *  getContentCodes
 */
int getContentCodes(http_t *p)
{
    int rc;
    int length;
    char *content;

    http_send(p, "/content-codes");

    rc = http_get(p, &content, &length);

    if ((rc == 200) && content && length) {
        daap_t *daap = decode_daap(content, length);
#if DEBUG_COMMANDS
        daap_dump(daap);
#endif
        daap_createDictionary(daap);
#if DEBUG_COMMANDS
        dumpdict();
#endif
        daap_free(daap);
        free(content);
    }

    return rc;
}


/*
 *  getServerInfo
 */
int getServerInfo(http_t *p)
{
    int rc;
    int length;
    char *content;

    http_send(p, "/server-info");

    rc = http_get(p, &content, &length);

    if ((rc == 200) && content && length) {
        daap_t *daap = decode_daap(content, length);
        daap_dump(daap);
        daap_free(daap);
        free(content);
    }

    return rc;
}


/*
 *  login
 */
long login(http_t *p)
{
    int rc;
    int length;
    char *content;
    unsigned long sessionid = 0;

    http_send(p, "/login");

    rc = http_get(p, &content, &length);

    if (rc != 200) {
        DBG_PRINTF("Login failed\n");
        return -1;
    }

    if (content && length) {
        daap_t *daap = decode_daap(content, length);
        /*daap_dump (daap); */

        daap_t *session = daap_domain(daap, "dmap.loginresponse/dmap.sessionid");

        sessionid = session->data.integer;

        daap_free(daap);
        free(content);
    }

    return sessionid;
}


/*
 *  database
 */
unsigned long database(http_t *p, unsigned long sessionid)
{
    char url[256];

    sprintf(url, "/databases?session-id=%ld", sessionid);

    http_send_auth(p, url);

    int length;
    char *content;
    int rc = http_get(p, &content, &length);

    if (rc == 403) {
        DBG_PRINTF("Request for database returns forbidden\n");
        return 0;
    }

    unsigned long dbid = 0;

    if ((rc == 200) && content && length) {
        daap_t *daap = decode_daap(content, length);
        /*daap_dump (daap); */
        daap_t *dbidp = daap_domain(daap, "daap.serverdatabases/dmap.listing/dmap.listingitem/dmap.itemid");

        if (dbidp) {
            dbid = dbidp->data.integer;
        }

        daap_free(daap);
        free(content);
    }

    return dbid;
}


#if 0
/*
 *  getArtists 
 *    this will get all the artists iTunes knows about
 *    because we don't have access to a browse function
 *    we're going to have to do it the hard way and get
 *    everything and filter it.
 */
void artists_callback(daap_t *d, void *arg)
{
    char *artistname;
    tree_t **root = arg;

    if (strcmp(d->name, "dmap.listingitem")) {
        return;
    }

    daap_t *compilation = daap_domain(d, "dmap.listingitem/daap.songcompilation");

    if (compilation && (compilation->data.integer == 1)) {
        artistname = strdup("Compilations");
        if (!tree_add(root, artistname, artistname)) {
            DBG_PRINTF("commands: duplicate artist '%s'\n", artistname);
            free(artistname);
        }
        return;
    }
 
    daap_t *artist = daap_domain(d, "dmap.listingitem/daap.songartist");

    if (artist && strlen(artist->data.text)) {
        artistname = strdup(artist->data.text);
    }
    else {
        artistname = strdup("Unknown Artist");
    }

    if (!tree_add(root, artistname, artistname)) {
        DBG_PRINTF("commands: duplicate artist '%s'\n", artistname);
        free(artistname);
    }
}


/*
 *  getArtists
 */
tree_t *getArtists(http_t *p, unsigned long sessionid, unsigned long dbid)
{
    tree_t *root = NULL;

    /*  getSpecificFields (p, sessionid, dbid, "daap.songartist",  */
    /*                     artists_callback, (void*)&root); */

    streamSpecificFields (p, sessionid, dbid, 0,
            "daap.songartist,daap.songcompilation", 
            artists_callback, (void *) &root);

    return root;
}
#endif


/*
 *    album_callback 
 */
static
void album_callback(daap_t *d, void *arg)
{
    callback_arg *p = (callback_arg*) arg;
    tree_t **root = p->root;
    int unkArtist;

    if (strcmp(d->name, "dmap.listingitem")) {
        return;
    }

    unkArtist = !strcmp(p->info.artist, "Unknown Artist");

    daap_t *artist = daap_domain(d, "dmap.listingitem/daap.songartist");

    if (!artist) {
        return;
    }

    daap_t *compilation = daap_domain (d, "dmap.listingitem/daap.songcompilation");
    int comp = 0;

    if (compilation && compilation->data.integer == 1) {
        if (strcasecmp("Compilations", p->info.artist)) {
            return;
        }
        comp = 1;
    }

    if (comp == 0) {
        if (strlen(artist->data.text) && unkArtist) {
            return;
        }

        if (!unkArtist && strcasecmp(artist->data.text, p->info.artist)) {
            return;
        }
    }

    daap_t *album = daap_domain(d, "dmap.listingitem/daap.songalbum");

    if (album) {
        char *albumname;
   
        if (strlen (album->data.text) > 0) {
            albumname = strdup(album->data.text);
        }
        else {
            albumname = strdup("Unknown Album");
        }

        if (!tree_add(root, albumname, albumname)) {
            DBG_PRINTF("commands: duplicate album '%s'\n", albumname);
            free(albumname);
        }
    }
}


/*  
 *   playlist_callback
 */
static
void playlist_callback(daap_t *d, void *arg)
{
    callback_arg *p;
    tree_t **root;
    int id = 0;
    int pid = 0;

    p = (callback_arg*) arg;
    root = p->root;

    if (strcmp(d->name, "dmap.listingitem")) {
        DBG_PRINTF("\tnot an item listing\n");
        return;
    }

    daap_t *persistentid = daap_domain(d, "dmap.listingitem/dmap.persistentid");

    if (!persistentid) {
        DBG_PRINTF("\tmissing persistent id\n");
        return;
    }
    else {
        pid = persistentid->data.integer;
    }

    daap_t *playlistid = daap_domain(d, "dmap.listingitem/dmap.itemid");

    if (playlistid) {
        id = playlistid->data.integer;
    }
    else {
        DBG_PRINTF("\tmissing id\n");
        return;
    }

    daap_t *playlist = daap_domain(d, "dmap.listingitem/dmap.itemname");
    if (!playlist) {
        DBG_PRINTF("\tmissing playlist\n");
        return;
    }

    DBG_PRINTF("commands: creating playlist item\n");

    playlist_info_t *playlistinfo;

    playlistinfo = calloc(1, sizeof(playlist_info_t));
    if (!playlistinfo) {
        DBG_PRINTF("commands: malloc failed!\n");
    }
    playlistinfo->id = id;
    playlistinfo->pid = pid;
    playlist_set_name(playlistinfo, playlist->data.text);

    playlist_dump(playlistinfo);
    
    if (!tree_add(root, playlistinfo->name, playlistinfo)) {
        DBG_PRINTF("commands: duplicate playlist '%s'?!\n", playlistinfo->name);
        playlist_delete(playlistinfo);
    }
}


/*  
 *   song_callback
 */
static
void song_callback(daap_t *d, void *arg)
{
    callback_arg *p = (callback_arg*) arg;
    tree_t **root = p->root;
    int id = 0;
    int unkArtist = 0;
    int unkAlbum = 0;
    int unkGenre = 0;

    if (strcmp(d->name, "dmap.listingitem")) {
        return;
    }

    /* daap_t *dataurl = daap_domain (d, "dmap.listingitem/daap.songdataurl");

    if (dataurl) {
        DBG_PRINTF("Data URL %s\n", dataurl->data.text);
    }
    */

    if (p->info.artist) {
        unkArtist = !strcmp(p->info.artist, "Unknown Artist");
    }
    else {
        /* JDH - we want songs, but didn't specify the artist */
        unkArtist = 1;
    }

    if (p->info.album) {
        unkAlbum = !strcmp(p->info.album, "Unknown Album");
    }
    else {
        /* JDH - we want songs, but didn't specify the album */
        unkAlbum = 1;
    }

    if (p->info.genre) {
        unkGenre = !strcmp(p->info.genre, "Unknown Album");
    }
    else {
        /* JDH - we want songs, but didn't specify the genre */
        unkGenre = 1;
    }

    daap_t *compilation = daap_domain(d, "dmap.listingitem/daap.songcompilation");
    int comp = 0;
    if (compilation && (compilation->data.integer == 1)) {
        if (strcasecmp("Compilations", p->info.artist)) {
            return;
        }
        comp = 1;
    }

    daap_t *artist = daap_domain(d, "dmap.listingitem/daap.songartist");

    /* JDH - some bad encoders don't populate all fields, but we still want to play the songs */
    if (!artist) {
        DBG_PRINTF("\tmissing artist\n");
        if (!unkArtist) {
            /* JDH - no artist returned, but we asked for one, so this isn't a match */
            return;
        }
    }

#if 0
    /* JDH - this isn't a failure if we ask for all songs by server */
    if (strlen(artist->data.text) && unkArtist) {
        DBG_PRINTF("\tartist data for unknown artist?\n");
        return;
    }
#endif

    if (!comp && !unkArtist && strcasecmp(artist->data.text, p->info.artist)) {
        DBG_PRINTF("\tartist doesn't match?\n");
        return;
    }

    daap_t *album = daap_domain(d, "dmap.listingitem/daap.songalbum");

    /* JDH - some bad encoders don't populate all fields, but we still want to play the songs */
    if (!album) {
        DBG_PRINTF("\tmissing album\n");
        if (!unkAlbum) {
            /* JDH - no album returned, but we asked for one, so this isn't a match */
            return;
        }
    }

#if 0
    /* JDH - this isn't a failure if we ask for all songs by artist */
    if (unkAlbum && strlen(album->data.text)) {
        DBG_PRINTF("\talbum data for unknown album?\n");
        return;
    }
#endif

    if (!unkAlbum && strcasecmp(album->data.text, p->info.album)) {
        DBG_PRINTF("\talbum doesn't match?\n");
        return;
    }

    daap_t *genre = daap_domain(d, "dmap.listingitem/daap.songgenre");

    /* JDH - some bad encoders don't populate all fields, but we still want to play the songs */
    if (!genre) {
        DBG_PRINTF("\tmissing genre\n");
        if (!unkGenre) {
            /* JDH - no genre returned, but we asked for one, so this isn't a match */
            return;
        }
    }

    if (!unkGenre && strcasecmp(genre->data.text, p->info.genre)) {
        DBG_PRINTF("\tgenre doesn't match?\n");
        return;
    }

    daap_t *songid = daap_domain(d, "dmap.listingitem/dmap.itemid");

    if (songid) {
        id = songid->data.integer;
    }

    daap_t *tracknum = daap_domain(d, "dmap.listingitem/daap.songtracknumber");

    daap_t *song = daap_domain(d, "dmap.listingitem/dmap.itemname");
    if (!song) {
        DBG_PRINTF("\tmissing song\n");
        return;
    }

    song_info_t *songinfo = song_clone(&p->info);
    song_set_album(songinfo, (album)?album->data.text:"Unknown Album");
    song_set_artist(songinfo, (artist)?artist->data.text:"Unknown Artist");
    song_set_genre(songinfo, (genre)?genre->data.text:"Unknown Genre");
    songinfo->id = id;

    if (tracknum) {
        songinfo->track = tracknum->data.integer;
    }
    else {
        songinfo->track = 0;
    }

    if (strlen (song->data.text) > 0) {
        song_set_song(songinfo, song->data.text);
    }
    else {
        char unknown[128];

        sprintf(unknown, "Unknown %d\n", id);
        song_set_song(songinfo, unknown);
    }

    song_dump(songinfo);

    if (!tree_add(root, songinfo->song, songinfo)) {
        DBG_PRINTF("commands: duplicate song '%s'?!\n", songinfo->song);
        song_delete(songinfo);
    }
}


/*
 *  getAlbumsByArtists
 */
tree_t *getAlbumsByArtists(http_t *p, 
			    unsigned long sessionid, 
			    unsigned long dbid, 
			    const char *artist)
{
    tree_t *root = NULL;
    callback_arg arg;

    memset(&arg, 0, sizeof(arg));
    arg.info.artist = (char *) artist;
    arg.root = &root;

#if 0
    streamSpecificFields(p, 
            sessionid, dbid, 0,
            "daap.songartist,daap.songalbum,"
            "daap.songcompilation", 
            album_callback, 
            (void *) &arg);
#else
    char url[256];

    sprintf(url, 
            "/databases/%ld/items?type=music&meta=%s&session-id=%ld",
            dbid,
            "daap.songartist,daap.songalbum,"
            "daap.songcompilation", 
            sessionid);

    getGeneric(p, url, adbs, mlcl, album_callback, (void *) &arg);
#endif
    return root;
}


/*
 *  getSongsByArtistAlbumPlaylist
 *
 *  gets a tree of all the songs by a particular artist (if not NULL)
 *  and a particular album (if not NULL).
 *
 *  When artist and/or album are NULL it matches anything.
 */
#if 0
tree_t *getSongsByArtistAlbumPlaylist(http_t *p, 
                unsigned long sessionid, 
                unsigned long dbid,
                unsigned long plid,
                const char *artist, 
                const char *album,
                const char *genre)
#else
tree_t *getSongsByArtistAlbumPlaylist(http_t *p, 
                unsigned long sessionid, 
                unsigned long dbid,
                song_filter_t sfilt)
#endif
{
    tree_t *root = NULL;
    callback_arg arg;

    DBG_PRINTF("getSongs artist=%s, album=%s, genre=%s, playlist=%8lu\n",
                (sfilt.artist)?sfilt.artist:"", (sfilt.album)?sfilt.album:"", (sfilt.genre)?sfilt.genre:"", sfilt.playlist_id);

    memset(&arg, 0, sizeof(arg));
#if 0
    arg.info.album  = (char *) album;
    arg.info.artist = (char *) artist;
    arg.info.genre  = (char *) genre;
#else
    arg.info.album  = (char *) sfilt.album;
    arg.info.artist = (char *) sfilt.artist;
    arg.info.genre  = (char *) sfilt.genre;
#endif
    arg.root = &root;

#if 0
    streamSpecificFields(p, 
            sessionid, dbid, sfilt.playlist_id,
            "dmap.itemname,daap.songartist,daap.songalbum,daap.songgenre,"
            "daap.songtracknumber,dmap.itemid,"
            "daap.songcompilation,"
            /* "daap.songdataurl" */,
            song_callback, 
            (void *) &arg);
#else
    char url[256];
    char pl[128] = "";

    if (sfilt.playlist_id) {
        sprintf(pl, "containers/%ld/", sfilt.playlist_id);
    }
    sprintf(url, 
            "/databases/%ld/%sitems?type=music&meta=%s&session-id=%ld",
            dbid,
            pl,
            "dmap.itemname,daap.songartist,daap.songalbum,daap.songgenre,"
            "daap.songtracknumber,dmap.itemid,"
            "daap.songcompilation,",
            sessionid);

    getGeneric(p, url, (sfilt.playlist_id)?apso:adbs, mlcl, song_callback, (void *) &arg);
#endif

    return root;
}


/*
 *  getPlaylists
 *
 *  Gets a tree of all the playlists available on the server.
 *
 */
tree_t *getPlaylists(http_t *p, 
                unsigned long sessionid, 
                unsigned long dbid)
{
    tree_t *root = NULL;
    callback_arg arg;
    char url[256];

    memset(&arg, 0, sizeof(arg));
    arg.root = &root;

    sprintf(url,
	        "/databases/%ld/containers?meta=dmap.itemid,dmap.itemname,dmap.persistentid,com.apple.itunes.smart-playlist&session-id=%ld",
	        dbid,
	        sessionid);

    getGeneric(p, url, aply, mlcl, playlist_callback, (void *) &arg);

    return root;
}

