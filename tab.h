#ifndef TAB_H
#define TAB_H
/*
 * tab.h - structures and prototypes for tab drawing
 */


#define MAX_TABTEXT 12

typedef struct {
    int x1, y1, x2, y2;
    char title[MAX_TABTEXT];
} tab_t;

void draw_active_tab(tab_t tab);
void draw_inactive_tab(tab_t tab);
void clear_tab_region(void);

#endif

