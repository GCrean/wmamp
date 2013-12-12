#ifndef VERSION_H
#define VERSION_H

#define VERSION_STR "WMAmp Version 0.6 GJ06"
#ifdef NIGHTLY
#define NIGHTLY_STR "Nightly Build"
#else
#define NIGHTLY_STR
#endif

#define VERSION VERSION_STR##NIGHTLY_STR##__DATE__##__TIME__

#endif
