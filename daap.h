#ifndef DAAP_H
#define DAAP_H

typedef union {
  unsigned long integer;
  unsigned long long longlong;
  char text[1];  /* real block will be longer */
} daap_data_t;

typedef struct daap_chunk_tag {
  long id;
  long length;
  long type;
  char *name;			/* reference to name kept in dictionary */
  struct daap_chunk_tag *child;
  struct daap_chunk_tag *next;
  daap_data_t data;  
} daap_t;

/* DAAP content codes.
   Hex code is 4-char rendition of symbolic/mnemonic name.
   Listed here in ascending order of decimal value.
*/
#define abal 1633837420UL /* 0x6162616C 0xc daap.browsealbumlisting */
#define abar 1633837426UL /* 0x61626172 0xc daap.browseartistlisting */
#define abcp 1633837936UL /* 0x61626370 0xc daap.browsecomposerlisting */
#define abgn 1633838958UL /* 0x6162676E 0xc daap.browsegenrelisting */
#define abpl 1633841260UL /* 0x6162706C 0x1 daap.baseplaylist */
#define abro 1633841775UL /* 0x6162726F 0xc daap.databasebrowse */
#define adbs 1633968755UL /* 0x61646273 0xc daap.databasesongs */
#define aeNV 1634029142UL /* 0x61654E56 0x5 com.apple.itunes.norm-volume */
#define aeSP 1634030416UL /* 0x61655350 0x1 com.apple.itunes.smart-playlist */
#define aply 1634757753UL /* 0x61706C79 0xc daap.databaseplaylists */
#define apro 1634759279UL /* 0x6170726F 0xb daap.protocolversion */
#define apso 1634759535UL /* 0x6170736F 0xc daap.playlistsongs */
#define arif 1634888038UL /* 0x61726966 0xc daap.resolveinfo */
#define arsv 1634890614UL /* 0x61727376 0xc daap.resolve */
#define asal 1634951532UL /* 0x6173616C 0x9 daap.songalbum */
#define asar 1634951538UL /* 0x61736172 0x9 daap.songartist */
#define asbr 1634951794UL /* 0x61736272 0x3 daap.songbitrate */
#define asbt 1634951796UL /* 0x61736274 0x3 daap.songbeatsperminute */
#define ascm 1634952045UL /* 0x6173636D 0x9 daap.songcomment */
#define asco 1634952047UL /* 0x6173636F 0x1 daap.songcompilation */
#define ascp 1634952048UL /* 0x61736370 0x9 daap.songcomposer */
#define asda 1634952289UL /* 0x61736461 0xa daap.songdateadded */
#define asdb 1634952290UL /* 0x61736462 0x1 daap.songdisabled */
#define asdc 1634952291UL /* 0x61736463 0x3 daap.songdisccount */
#define asdk 1634952299UL /* 0x6173646B 0x1 daap.songdatakind */
#define asdm 1634952301UL /* 0x6173646D 0xa daap.songdatemodified */
#define asdn 1634952302UL /* 0x6173646E 0x3 daap.songdiscnumber */
#define asdt 1634952308UL /* 0x61736474 0x9 daap.songdescription */
#define aseq 1634952561UL /* 0x61736571 0x9 daap.songeqpreset */
#define asfm 1634952813UL /* 0x6173666D 0x9 daap.songformat */
#define asgn 1634953070UL /* 0x6173676E 0x9 daap.songgenre */
#define asrv 1634955894UL /* 0x61737276 0x2 daap.songrelativevolume */
#define assp 1634956144UL /* 0x61737370 0x5 daap.songstoptime */
#define assr 1634956146UL /* 0x61737372 0x5 daap.songsamplerate */
#define asst 1634956148UL /* 0x61737374 0x5 daap.songstarttime */
#define assz 1634956154UL /* 0x6173737A 0x5 daap.songsize */
#define astc 1634956387UL /* 0x61737463 0x3 daap.songtrackcount */
#define astm 1634956397UL /* 0x6173746D 0x5 daap.songtime */
#define astn 1634956398UL /* 0x6173746E 0x3 daap.songtracknumber */
#define asul 1634956652UL /* 0x6173756C 0x9 daap.songdataurl */
#define asur 1634956658UL /* 0x61737572 0x1 daap.songuserrating */
#define asyr 1634957682UL /* 0x61737972 0x3 daap.songyear */
#define avdb 1635148898UL /* 0x61766462 0xc daap.serverdatabases */
#define mbcl 1835164524UL /* 0x6D62636C 0xc dmap.bag */
#define mccr 1835230066UL /* 0x6D636372 0xc dmap.contentcodesresponse */
#define mcna 1835232865UL /* 0x6D636E61 0x9 dmap.contentcodesname */
#define mcnm 1835232877UL /* 0x6D636E6D 0x5 dmap.contentcodesnumber */
#define mcon 1835233134UL /* 0x6D636F6E 0xc dmap.container */
#define mctc 1835234403UL /* 0x6D637463 0x5 dmap.containercount */
#define mcti 1835234409UL /* 0x6D637469 0x5 dmap.containeritemid */
#define mcty 1835234425UL /* 0x6D637479 0x3 dmap.contentcodestype */
#define mdcl 1835295596UL /* 0x6D64636C 0xc dmap.dictionary */
#define miid 1835624804UL /* 0x6D696964 0x5 dmap.itemid */
#define mikd 1835625316UL /* 0x6D696B64 0x1 dmap.itemkind */
#define mimc 1835625827UL /* 0x6D696D63 0x5 dmap.itemcount */
#define minm 1835626093UL /* 0x6D696E6D 0x9 dmap.itemname */
#define mlcl 1835819884UL /* 0x6D6C636C 0xc dmap.listing */
#define mlid 1835821412UL /* 0x6D6C6964 0x5 dmap.sessionid */
#define mlit 1835821428UL /* 0x6D6C6974 0xc dmap.listingitem */
#define mlog 1835822951UL /* 0x6D6C6F67 0xc dmap.loginresponse */
#define mpco 1836082031UL /* 0x6D70636F 0x5 dmap.parentcontainerid */
#define mper 1836082546UL /* 0x6D706572 0x7 dmap.persistentid */
#define mpro 1836085871UL /* 0x6D70726F 0xb dmap.protocolversion */
#define mrco 1836213103UL /* 0x6D72636F 0x5 dmap.returnedcount */
#define msal 1836278124UL /* 0x6D73616C 0x1 dmap.supportsautologout */
#define msau 1836278133UL /* 0x6D736175 0x1 dmap.authenticationmethod */
#define msbr 1836278386UL /* 0x6D736272 0x1 dmap.supportsbrowse */
#define msdc 1836278883UL /* 0x6D736463 0x5 dmap.databasescount */
#define msex 1836279160UL /* 0x6D736578 0x1 dmap.supportsextensions */
#define msix 1836280184UL /* 0x6D736978 0x1 dmap.supportsindex */
#define mslr 1836280946UL /* 0x6D736C72 0x1 dmap.loginrequired */
#define mspi 1836281961UL /* 0x6D737069 0x1 dmap.supportspersistentids */
#define msqy 1836282233UL /* 0x6D737179 0x1 dmap.supportsquery */
#define msrs 1836282483UL /* 0x6D737273 0x1 dmap.supportsresolve */
#define msrv 1836282486UL /* 0x6D737276 0xc dmap.serverinforesponse */
#define mstm 1836282989UL /* 0x6D73746D 0x5 dmap.timeoutinterval */
#define msts 1836282995UL /* 0x6D737473 0x9 dmap.statusstring */
#define mstt 1836282996UL /* 0x6D737474 0x5 dmap.status */
#define msup 1836283248UL /* 0x6D737570 0x1 dmap.supportsupdate */
#define mtco 1836344175UL /* 0x6D74636F 0x5 dmap.specifiedtotalcount */
#define mudl 1836409964UL /* 0x6D75646C 0xc dmap.deletedidlisting */
#define mupd 1836413028UL /* 0x6D757064 0xc dmap.updateresponse */
#define musr 1836413810UL /* 0x6D757372 0x5 dmap.serverrevision */
#define muty 1836414073UL /* 0x6D757479 0x1 dmap.updatetype */

/* Data content types: */
#define DAAP_CHAR      0x1
#define DAAP_UCHAR     0x2
#define DAAP_SHORT     0x3
#define DAAP_USHORT    0x4
#define DAAP_INT       0x5
#define DAAP_UINT      0x6
#define DAAP_PERSIST   0x7
#define DAAP_ULONG     0x8
#define DAAP_TEXT      0x9
#define DAAP_DATE      0xA	/* seconds since epoch (1/1/1970) */
#define DAAP_2BY2      0xB	/* version */
#define DAAP_CONTAINER 0xC	/* list */


daap_t *decode_daap (const char *d, int length);
void daap_dump (daap_t *daap);
void daap_createDictionary (daap_t *daap);
void dumpdict (void);
void daap_free (daap_t *daap);
void daap_walk (daap_t *p, void (*fp) (daap_t *daap, void *arg), void *arg);
daap_t *daap_domain (daap_t *daap, const char *domain);

#endif
