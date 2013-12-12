#include <stdio.h>
#include "listview.h"



int main (int argc, char *argv[])
{
  lv_t l;
  
  listview_init ( &l, 0, 0, 0, 0 );

  listview_add ( &l, "1", NULL );
  listview_add ( &l, "7", NULL );
  listview_add ( &l, "2", NULL );
  listview_add ( &l, "8", NULL );
  listview_add ( &l, "4", NULL );
  listview_add ( &l, "5", NULL );
  listview_add ( &l, "6", NULL );
  listview_add ( &l, "3", NULL );
  listview_add ( &l, "9", NULL );


  printf (" initial dump\n");
  listview_draw ( &l );

  listview_down ( &l );

  listview_draw ( &l );

  listview_down ( &l );

  listview_draw ( &l );

  listview_down ( &l );
  listview_down ( &l );
  listview_down ( &l );
  listview_down ( &l );
  listview_down ( &l );
  listview_down ( &l );
  listview_down ( &l );
  listview_down ( &l );
  listview_down ( &l );
  listview_down ( &l );

  listview_draw ( &l );
  listview_up ( &l );
  listview_draw ( &l );
  listview_up ( &l );
  listview_draw ( &l );
  listview_up ( &l );
  listview_draw ( &l );
  listview_up ( &l );
  listview_up ( &l );
  listview_up ( &l );
  listview_up ( &l );
  listview_up ( &l );
  listview_up ( &l );
  listview_up ( &l );
  listview_draw ( &l );

  return 0;
}
