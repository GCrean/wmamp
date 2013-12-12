#include <stdio.h>
#include <directfb.h>



static IDirectFBSurface      *surface;
static IDirectFB             *dfb;

int main (int argc, char *argv[])
{

  printf ("startup\n");
  int ret = 0;

   ret = DirectFBInit( &argc, &argv );
   if (ret) {
     DirectFBError( "DirectFBInit() failed", ret );
     return 1;
   }

  ret = DirectFBCreate (& dfb );
  if ( ret )
  {
    DirectFBError( "DirectFBCreate() failed", ret );
    return 1;
  }

  DFBSurfaceDescription surface_desc;

  dfb->CreateSurface (dfb, &surface_desc, &surface);

  
  surface->Clear (surface, 0, 0, 0xff, 0);
  surface->Flip (surface, NULL, DSFLIP_WAITFORSYNC);
  surface->Clear (surface, 0, 0, 0xff, 0);
  surface->Flip (surface, NULL, DSFLIP_WAITFORSYNC);

  surface->Release (surface);
  dfb->Release (dfb);


 return 0;
}
