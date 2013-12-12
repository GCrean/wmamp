/* 2004 James Pitts Turtle Head http://www.turtlehead.co.uk */

#ifndef LISTVIEW_H
#define LISTVIEW_H

typedef struct lv_node_tag {
    char text[128]; /* TODO: alloc this  // text is display text */
    char key[128]; /* TODO: alloc this // this is the sort key */
    struct lv_node_tag *left, *right, *parent;
    void *data;
} lv_node_t;

/* List view object */
typedef struct {
    lv_node_t *draw;		/* node/line to start drawing from */
    int distance;  /* distance between drawing state node and selected node. */
    lv_node_t *selected;		/* selected/highlighted node */
    lv_node_t *root;
    int x;			/* left */
    int y;			/* upper */
    int width;
    int height;
    int moved; /* indicated if location has been moved since list started... */
} lv_t;



void listview_init(lv_t *l, 
                    int x, 
                    int y, 
                    int width, 
                    int height);

void listview_draw(lv_t *l);

void listview_up(lv_t *l);

int listview_down(lv_t *l);

void *listview_select(lv_t *l);

int listview_remove(lv_t *l, 
                    const char *text);

int listview_add(lv_t *l, 
                    const char *text, 
                    void *data);

int listview_add_key(lv_t *l, 
                    const char *text, 
                    const char *key,
                    void *data );

int listview_addtree(lv_node_t *root, 
                    lv_node_t *src);

int listview_remove(lv_t *l, 
                    const char *text);


void listview_dump(lv_t *l);


lv_node_t *listview_next_larger(lv_node_t *l, 
                                lv_node_t *find);

lv_node_t *listview_next_smaller(lv_node_t *l, 
                                lv_node_t *find);

lv_node_t *listview_next_i(lv_node_t *p);
lv_node_t *listview_prev_i(lv_node_t *p);

lv_node_t *listview_next(lv_t *l);
lv_node_t *listview_prev(lv_t *l);

void listview_free(lv_t *lv,  void (*fp) (void *data));
void listview_find_closest(lv_t *lv, const char *key);

lv_node_t *listview_move_n(lv_node_t *root, int n, int *moved);


#endif

