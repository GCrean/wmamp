#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "daap.h"
#include "hexdump.h"
#include "debug_config.h"

#if DEBUG_DAAP
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

/* Number of predefined entries in dictionary: */
#define DICT_FIXED	 84
/* Total number of entries ever in dictionary (enough for now): */
#define DICT_SIZE	 96

typedef struct {
  unsigned long id;		/* content code */
  unsigned char type;		/* content type */
  char *name;			/* content name - textual description */
} dict_t;

/* Idea: put all known content codes in dictionary.
   Sort by content code (id). Then implement `find' by binary search.
   Could ignore any additional content codes that are not anticipated.
   Nah, let's just put them at the end, unsorted; there won't be (m)any,
   and if we see some new ones we could always put them on the fixed list.
   Indeed, this approach turns out to be much faster!
*/
static dict_t dictionary[DICT_SIZE] = {
  {abal, 0xc, "daap.browsealbumlisting"},		/*  1 */
  {abar, 0xc, "daap.browseartistlisting"},
  {abcp, 0xc, "daap.browsecomposerlisting"},
  {abgn, 0xc, "daap.browsegenrelisting"},
  {abpl, 0x1, "daap.baseplaylist"},
  {abro, 0xc, "daap.databasebrowse"},
  {adbs, 0xc, "daap.databasesongs"},
  {aeNV, 0x5, "com.apple.itunes.norm-volume"},
  {aeSP, 0x1, "com.apple.itunes.smart-playlist"},
  {aply, 0xc, "daap.databaseplaylists"},		/* 10 */
  {apro, 0xb, "daap.protocolversion"},
  {apso, 0xc, "daap.playlistsongs"},
  {arif, 0xc, "daap.resolveinfo"},
  {arsv, 0xc, "daap.resolve"},
  {asal, 0x9, "daap.songalbum"},
  {asar, 0x9, "daap.songartist"},
  {asbr, 0x3, "daap.songbitrate"},
  {asbt, 0x3, "daap.songbeatsperminute"},
  {ascm, 0x9, "daap.songcomment"},
  {asco, 0x1, "daap.songcompilation"},			/* 20 */
  {ascp, 0x9, "daap.songcomposer"},
  {asda, 0xa, "daap.songdateadded"},
  {asdb, 0x1, "daap.songdisabled"},
  {asdc, 0x3, "daap.songdisccount"},
  {asdk, 0x1, "daap.songdatakind"},
  {asdm, 0xa, "daap.songdatemodified"},
  {asdn, 0x3, "daap.songdiscnumber"},
  {asdt, 0x9, "daap.songdescription"},
  {aseq, 0x9, "daap.songeqpreset"},
  {asfm, 0x9, "daap.songformat"},			/* 30 */
  {asgn, 0x9, "daap.songgenre"},
  {asrv, 0x2, "daap.songrelativevolume"},
  {assp, 0x5, "daap.songstoptime"},
  {assr, 0x5, "daap.songsamplerate"},
  {asst, 0x5, "daap.songstarttime"},
  {assz, 0x5, "daap.songsize"},
  {astc, 0x3, "daap.songtrackcount"},
  {astm, 0x5, "daap.songtime"},
  {astn, 0x3, "daap.songtracknumber"},
  {asul, 0x9, "daap.songdataurl"},			/* 40 */
  {asur, 0x1, "daap.songuserrating"},
  {asyr, 0x3, "daap.songyear"},
  {avdb, 0xc, "daap.serverdatabases"},
  {mbcl, 0xc, "dmap.bag"},
  {mccr, 0xc, "dmap.contentcodesresponse"},
  {mcna, 0x9, "dmap.contentcodesname"},
  {mcnm, 0x5, "dmap.contentcodesnumber"},
  {mcon, 0xc, "dmap.container"},
  {mctc, 0x5, "dmap.containercount"},
  {mcti, 0x5, "dmap.containeritemid"},			/* 50 */
  {mcty, 0x3, "dmap.contentcodestype"},
  {mdcl, 0xc, "dmap.dictionary"},
  {miid, 0x5, "dmap.itemid"},
  {mikd, 0x1, "dmap.itemkind"},
  {mimc, 0x5, "dmap.itemcount"},
  {minm, 0x9, "dmap.itemname"},
  {mlcl, 0xc, "dmap.listing"},
  {mlid, 0x5, "dmap.sessionid"},
  {mlit, 0xc, "dmap.listingitem"},
  {mlog, 0xc, "dmap.loginresponse"},			/* 60 */
  {mpco, 0x5, "dmap.parentcontainerid"},
  {mper, 0x7, "dmap.persistentid"},
  {mpro, 0xb, "dmap.protocolversion"},
  {mrco, 0x5, "dmap.returnedcount"},
  {msal, 0x1, "dmap.supportsautologout"},
  {msau, 0x1, "dmap.authenticationmethod"},
  {msbr, 0x1, "dmap.supportsbrowse"},
  {msdc, 0x5, "dmap.databasescount"},
  {msex, 0x1, "dmap.supportsextensions"},
  {msix, 0x1, "dmap.supportsindex"},			/* 70 */
  {mslr, 0x1, "dmap.loginrequired"},
  {mspi, 0x1, "dmap.supportspersistentids"},
  {msqy, 0x1, "dmap.supportsquery"},
  {msrs, 0x1, "dmap.supportsresolve"},
  {msrv, 0xc, "dmap.serverinforesponse"},
  {mstm, 0x5, "dmap.timeoutinterval"},
  {msts, 0x9, "dmap.statusstring"},
  {mstt, 0x5, "dmap.status"},
  {msup, 0x1, "dmap.supportsupdate"},
  {mtco, 0x5, "dmap.specifiedtotalcount"},		/* 80 */
  {mudl, 0xc, "dmap.deletedidlisting"},
  {mupd, 0xc, "dmap.updateresponse"},
  {musr, 0x5, "dmap.serverrevision"},
  {muty, 0x1, "dmap.updatetype"},			/* 84 */
  {   0, 0x0, NULL}		/* rest must have id field 0 */
};

/* Finds dictionary entry corresponding to @a id.
   Returns 1 when found and then @a *d points to that entry;
   otherwise returns 0 and then @a *d is either an empty entry or
   points beyond the dictionary.
*/
static int
daap_findId (unsigned long id, dict_t **d)
{
  int i = 0, j = DICT_FIXED;

  /* Binary search (obviously only in sorted part of table): */
  while (i < j) {
    int k = (i + j) >> 1;	/* i <= k < j  */
    *d = dictionary + k;

    if (id == (*d)->id) /* Found! */
      return 1;

    if (id < (*d)->id)
      j = k;			/* i <= j */
    else
      i = k + 1;		/* i <= j */
  }
  /* Here: not found; i == j */

  /* Linear search in spill area instead: */
  *d = dictionary + DICT_FIXED;
  for (i = DICT_FIXED; i < DICT_SIZE; i++, (*d)++) {
    if (!(*d)->id)			/* empty spot */
      return 0;

    if (id == (*d)->id) /* Found! */
      return 1;
  }
  /* Here: not found, but dictionary full. */
  return 0;
}

/*
 *    addtodict
 */
static void addtodict (dict_t *d)
{
  dict_t *p;

  if (daap_findId (d->id, &p))
    /* Already in dictionary. */
    return;

  if (p == dictionary + DICT_SIZE) {
    fprintf(stderr, "Dictionary full!\n");
    return;
  }

  /* Fill out the free spot: */
  p->id   = d->id;
  p->type = d->type;
  p->name = strdup(d->name);
  /* Dictionary might now be full! */
}

void dumpdict (void)
{
  dict_t *p = dictionary;
  char id[4];

  fputs("Code(Dec)  Code(Hex)  Mnemonic Type Description\n", stderr);
  while (p->id) {
    memcpy (id, &p->id, 4);
    fprintf(stderr, "%10lu 0x%lX %c%c%c%c     0x%x  %s\n",
	    p->id, p->id, id[3], id[2], id[1], id[0], p->type, p->name);
    p++;
    if (p == dictionary + DICT_SIZE)
      break;
  }
}

/*
 *  daap_createDictionary
 */
void daap_createDictionary (daap_t *p)
{
  while (p) {
    if (p->id == mdcl) {
      daap_t *c = p->child;
      dict_t d;

      while (c) {
	switch (c->id) {
	case mcnm:
	  d.id   = c->data.integer;
	  break;
	case mcna:
	  d.name = c->data.text;
	  break;
	case mcty:
	  d.type = c->data.integer;
	  break;
	default:
	  break;
	}
	c = c->next;
      }
      addtodict (&d);
    }
    else
      daap_createDictionary (p->child);
    p = p->next;
  }
}

/* DAAP uses network byte order = big-endian = most-significant byte first!
   i386 is little-endian.
*/

/* Turns next 4 bytes into unsigned int value. */
static unsigned long daapGetULong (const char **d)
{
  unsigned char *p = (unsigned char *) *d; /* the unsigned is vital! */
  unsigned long a;

  /*hex_dump (stderr, *d, 4);*/

  a  = *p++ << 24;
  a |= *p++ << 16;
  a |= *p++ <<  8;
  a |= *p++;
  /* a = [(MSB:)p0 p1 p2 p3] */

  /* side-effect: advance buf pointer: */
  *d = p;
  return a;
}

/* Gets 4-letter content code as unsigned int, first letter is MSB. */
#define daapGetID	daapGetULong

static unsigned long daapGetShort (const char **d)
{
  const char *p = *d;
  unsigned long a = 0;

  a  = *p++ << 8;
  a |= *p++;
  /* a = [(MSB:)0 0 p0 p1] */

  /* side-effect: advance buf pointer: */
  *d = p;
  return a;
}

static void daapGetText (const char **d, int len, char *dst)
{
  const char *p = *d;

  memcpy (dst, p, len);
  dst[len] = '\0';

  /* side-effect: advance buf pointer: */
  *d = p + len;
}


static int
decode_daap_internal(const char *d, int length, daap_t **daapptr)
{
    const char *b = d;
    char *b_rewind;

    while (b < d + length) {
        unsigned long id   = daapGetID(&b);
        unsigned long len  = daapGetULong(&b);
        dict_t *dict;
        daap_t *daap;
        unsigned char d_type;

        if (!daap_findId(id, &dict)) {
            DBG_PRINTF("%lx not in dictionary\n", id);
            DBG_PRINTF("%s\n", d);
            return 0;   /* JDH - browse responses claim to be containers, but hold only a string */
        }
        else {
            DBG_PRINTF("NAME='%s' TYPE='%02x' ID='%08lx'\n", dict->name, dict->type, dict->id);
        }
        d_type = dict->type;    /* JDH - stuff this into a temp vbl so we can malloc based on it. */
retry:
        if (d_type == DAAP_TEXT) {
            (*daapptr) = malloc(sizeof(daap_t) + len + 1);
        }
        else {
            (*daapptr) = malloc(sizeof(daap_t));
        }

        daap = *daapptr;
        daap->id     = dict->id;
        daap->length = len;
        daap->type   = d_type;  /* JDH - hack in the fudged type once so we never have to do this again! */
        /* Just keep a ref; no need to copy! Saves space. */
        daap->name   = dict->name;
        daap->child  = NULL;
        daap->next   = NULL;

//        DBG_PRINTF("NAME='%s' TYPE='%02x' ID='%08lx'\n", daap->name, daap->type, daap->id);

        switch (daap->type) {
        case DAAP_CONTAINER:
            b_rewind = b;
            if (decode_daap_internal(b, len, &daap->child)) {
                b += len;
                break;
            }
            /* JDH - if container decode fails it's probably a browse response, try that */
            DBG_PRINTF("DAAP REWIND\n");
            b = b_rewind;
            free(*daapptr);
            d_type = DAAP_TEXT; /* JDH - hack the type (ick!) */
            goto retry; /* JDH - ugh, I don't like this, but the protocol breaks here IMHO */

        case DAAP_TEXT:
            daapGetText(&b, len, daap->data.text);
            break;

        case DAAP_CHAR:
            daap->data.integer = *b++; 
            break;

        case DAAP_SHORT:
            daap->data.integer = daapGetShort(&b);
            break;

        case DAAP_INT:
            daap->data.integer = daapGetULong(&b);
            break;

        case DAAP_2BY2:
            daap->data.integer = daapGetULong(&b);
            break;

        case DAAP_PERSIST:
            b += 8;
            break;

        default:
            DBG_PRINTF("Unknown DAAP type %ld %lx\n", daap->type, daap->type);
            hex_dump(stderr, b, len);
            free(daap);
            *daapptr = NULL;
            return 0;
        }
        daapptr = &daap->next;
    }
    return 1;
}


/*
 *  decode_daap
 */
daap_t *decode_daap(const char *d, int length)
{
    daap_t *daap;

    decode_daap_internal(d, length, &daap);

    return daap;
}


/*
 *  daap_dump
 */
static void daap_dump_internal(FILE *fp, daap_t *p, int indent)
{
    while (p) {
        fprintf(fp, "%*s%s %lx ", indent, "", p->name, p->type);
        switch (p->type) {
        case DAAP_CHAR:
        case DAAP_UCHAR:
        case DAAP_SHORT:
        case DAAP_INT:
        case DAAP_2BY2:
            fprintf(fp, "0x%lx %ld", p->data.integer, p->data.integer);
            break;

        case DAAP_TEXT:
            fprintf(fp, "%s", p->data.text);
            break;
        }
        putc('\n', fp);

        daap_dump_internal(fp, p->child, indent + 2);
        p = p->next;
    }
}


/*
 *  daap_dump
 */
void daap_dump(daap_t *p)
{
    daap_dump_internal(stderr, p, 0);
}


static daap_t *daap_domain_aux (daap_t *daap, char *domain)
{
  char *end;

  /* Get stretch of domain till first /: */
  end = strchr (domain, '/');
  if (end)
    *end++ = '\0';

  while (daap) {
    if (!strcmp (domain, daap->name)) { /* match */
      daap_t *rc;

      if (!end) 		/* domain exhausted */
        return daap;

      rc = daap_domain_aux (daap->child, end);
      if (rc)
	return rc;
    }
    daap = daap->next;
  }
  return NULL;
}

/*
 * daap_domain
 */
daap_t *daap_domain (daap_t *daap, const char *domain)
{
  char buf[256];		/* more than enough for now */

  strcpy(buf, domain);		/* need mutable string! */
  return daap_domain_aux (daap, buf);
}

void daap_free (daap_t *daap)
{
  while (daap) {
    daap_t *next = daap->next;

    daap_free (daap->child);
    free (daap);		/* also frees data.text! */
    daap = next;
  }
}

/*
 *  daap_walk
 */
void daap_walk (daap_t *p, void (*fp) (daap_t *daap, void *arg), void *arg)
{
  if (!fp) 
    return;

  while (p) {
    fp (p, arg);    
    daap_walk (p->child, fp, arg);
    p = p->next;
  }
}
