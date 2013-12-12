#define _GNU_SOURCE
#include <string.h>
#include <assert.h>
#include "listview.h"
#include "screen.h"
#include "debug_config.h"

#if DEBUG_LISTVIEW
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

/* JDH - it looks like uClibc should support strverscmp(), but it's not working just now */
#if !defined(__UCLIBC__)
/* oh the hacks we have to be prepared to do... */
static int ourcomp(const char *a, const char *b)
{
    /* Let's resort to alloca(). It's faster. */
    char *aa = alloca(strlen(a) + 1);
    char *bb = alloca(strlen(b) + 1);
    char *p, *s;

    /* While copying also uppercase: */
    p = aa;
    s = (char *) a;
    while (*s) {
        *p++ = _toupper(*s++);
    }
    *p = '\0';

    /* While copying also uppercase: */
    p = bb;
    s = (char *) b;
    while (*s) {
        *p++ = _toupper(*s++);
    }
    *p = '\0';

    return strverscmp(aa, bb);
}
#define COMP(a,b) ourcomp(a,b)
#else
#define COMP(a,b) strcmp(a,b)
#endif

/* Number of pixels before text starts. */
#define LMARGIN		5

/* Number of pixels between text lines. (linespacing) */
#define GUTTER		5

/*
 *  listview_init
 */
void listview_init(lv_t *l, int x, int y, int width, int height)
{ 
    l->draw     = NULL;
    l->selected = NULL;
    l->root     = NULL;
    l->x        = x;
    l->y        = y;
    l->width    = width;
    l->height   = height;
    l->distance = 0;
    l->moved    = 0;

    listview_draw(l);
} 


/*
       (0,0) -> x
         |
         V   Artists
         y   +--------------------------+
             |Abba                      |
             |Bjorn                     |

             |                          |
             +--------------------------+
*/
void listview_draw(lv_t *l)
{
    lv_node_t *node;
    int y_advance;

    /* Nice dark background: */
    video_set_colour(&vid, 20, 20, 40);

    video_filled_rectangle(&vid, 
                           l->x, 
                           l->y, 
                           l->x + l->width, 
                           l->y + l->height);

    /* Light listview border: */
    video_set_colour(&vid, 230, 230, 230);

    video_rectangle(&vid, 
                    l->x-1, 
                    l->y-1, 
                    l->x + l->width  + 1, 
                    l->y + l->height + 1);

    y_advance = font_y_advance(&font);

    /* Coordinates for first text line: */
    video_setpen(&vid, l->x + LMARGIN, l->y + GUTTER + y_advance);

    /* Clip to listview window: */
    video_set_clipping(&vid, l->x, l->y, l->x + l->width, l->y + l->height);

    /*lv_node_t *node = listview_move_n (l->selected, -5, NULL); */

    for (node = l->draw; node ; node = listview_next_i(node)) {
        if (l->selected == node) {	/* `highlight' this line */
            /*video_set_colour (&vid, 137, 143, 154); */
            video_set_colour (&vid, 55, 65, 75);
            video_filled_rectangle(&vid, 
			                        l->x, 
			                        vid.peny - y_advance,
			                        l->x + l->width,
			                        vid.peny + GUTTER);
        }
        font_write ( &font, &vid, node->text );
        /* pen position has changed! */

        /* Don't list beyond window height: */
        if (vid.peny > l->y + l->height) {
            break;
        }

        /* Advance pen down 1 line: */
        vid.penx  = l->x + LMARGIN;
        vid.peny += GUTTER + y_advance;
    }
}


/*
 *  listview_up
 */
void listview_up(lv_t *l)
{
    lv_node_t *p;

    if (!l || !l->root) {
        return;
    }

    p = listview_prev_i(l->selected);

    if (p && (p != l->selected)) {
        if (l->distance == 0) {
            l->draw = listview_prev_i(l->draw);
        }
        else {
            l->distance--;
        }
        l->selected = p;
        listview_draw(l);
    }
    l->moved = 1;
}


/*
 *  listview_next
 */
lv_node_t *listview_next(lv_t *l)
{
    if (!l || !l->root) {
        return NULL;
    }

    return listview_next_i(l->selected);
}


/*
 *   listview_prev_i
 */
lv_node_t *listview_prev_i(lv_node_t *p)
{
    lv_node_t *orig = p;

    if (!p) {
        return NULL;
    }

    if (p->left) {
        return listview_next_smaller(p->left, p);
    }

    /* nothing on our left.  must be above us.. */
    while (p && COMP(p->key, orig->key) >= 0) {
        p = p->parent;
    }

    return p;
}


/*
 *  listview_prev
 */
lv_node_t *listview_prev(lv_t *l)
{
    if (!l || !l->root) {
        return NULL;
    }

    return listview_prev_i(l->selected);
}


/*
 *  listview_next_i
 *
 *  Although this is a listview internal function it is being used externally
 *  because we're cheeky like that.  So it's not a static..
 *
 */
lv_node_t *listview_next_i(lv_node_t *p)
{
    lv_node_t *orig = p;

    if (!p) {
        return NULL;
    }

    if (p->right) {
        return listview_next_larger(p->right, p);
    }

    /* nothing on our right.  must be above us.. */
    while (p && COMP(p->key, orig->key) <= 0) {
        p = p->parent;
    }

    return p;
}


/*  
 *   listview_down
 */
int listview_down(lv_t *l)
{
    int rc;
    lv_node_t *p;

    if (!l || !l->root) {
        return 0;
    }

    p = listview_next_i(l->selected);

    rc = 0;
    if (p && (p != l->selected)) {
        l->selected = p;

        if (l->distance >= 10) {	/* 10 = screen height in lines - 1 */
            l->draw = listview_next_i(l->draw);
        }
        else {
            l->distance++;
        }

        listview_draw(l);
        rc = 1;
    }

    l->moved = 1;

    return rc;
}


/*
 *  listview_next_larger
 */
lv_node_t *listview_next_larger(lv_node_t *l, lv_node_t *find)
{
    while (l) {
        if (!l->left) {
            return l;
        }

        l = l->left;
    }

    return find;
}


lv_node_t *listview_next_smaller(lv_node_t *l, lv_node_t *find)
{
    while (l) {
        if (!l->right) {
            return l;
        }

        l = l->right;
    }

    return find;
}


/*
 *   listview_select
 */
void *listview_select(lv_t *l)
{
    if (!l || !l->selected) {
        return NULL;
    }

    return l->selected->data;
}


/*
 *  listview_remove_i
 *
 *  I'm going to wing this a little and bluff it...
 *
 */
static
int listview_remove_i(lv_t *lv, lv_node_t **l, const char *key)
{
    int cmp;
    lv_node_t *left, *right;

restart:
    if (!*l) {
        return 0; /* not in tree... */
    }

    /* search node to nuke. */

    cmp = COMP((*l)->key, key);

    left  = (*l)->left;
    right = (*l)->right;

    if (cmp == 0) { /* found: *l is one to remove. */
        /* This code doesn't work and needs to be fixed... */
        /* it's supposed to select something resonable if  */
        /* the currently selected node is remove from the list. */
        if (*l == lv->selected) {
            if (left) {
	            lv->selected = left;
            }
            else if ((*l)->parent) {
	            lv->selected = (*l)->parent;
            }
            else if (right) {
	            lv->selected = right;
            }
            else {
                lv->selected = NULL;
            }
        }

        /* check for easy add left */
        if (!left) {
            /* free node */
            free(*l);
            *l = right;
            return 0;
        }

        /* check for easy add right */
        if (!right) {
            /* free node */
            free(*l);
            *l = left;
            return 0;
        }

        listview_addtree(left, right);

        /* release it */
        free(*l);
        *l = left;

        /* deleted... (hopefully) */
        return 0;
    }

    if (cmp < 0) {
        /* Avoid tail-recursion:
           return listview_remove_i ( lv, &(*l)->right, key );
         */
        l = &(*l)->right;
        goto restart;
    }

    /* cmp > 0 */
    /* Avoid tail-recursion:
       return listview_remove_i ( lv, &(*l)->left,  key );
     */
    l = &(*l)->left;
    goto restart;
}


/*
 *  listview_remove
 */
int listview_remove( lv_t *l, const char *key)
{
    return listview_remove_i(l, &l->root , key);
}


/*
 *  listview_addtree
 */
int listview_addtree(lv_node_t *root, lv_node_t *src)
{
    int cmp;

restart:
    cmp = COMP(src->key, root->key);

    if (cmp < 0) {
        /* oh oh oh, we can add here. */
        if (!root->left) {
            root->left  = src;
            src->parent = root;
            return 0;
        }
        /* Avoid tail-recursion:
           return listview_addtree ( root->left, src );
         */
        root = root->left;
        goto restart;
    } 

    if (cmp > 0) {
        /* oh oh oh, we can add here. */
        if (!root->right) {
            root->right = src;
            src->parent = root;
            return 0;
        }
        /* Avoid tail-recursion:
           return listview_addtree ( root->right, src );
         */
        root = root->right;
        goto restart;
    } 

    /*eeek what now? */
    return -1;
}


/*
 *  listview_add_i
 *  Makes copies of text and key arguments, but will own data arg.
 */
static
int listview_add_i(lv_t *lv, lv_node_t **l, lv_node_t *parent,  
                   const char *text, const char *key, void *data)
{
    int cmp;

    assert(l != NULL);
    assert(lv != NULL);

restart:

    if (*l == NULL) {
        lv_node_t *node = malloc(sizeof(lv_node_t)); 

        node->parent = parent;
        node->left   = NULL;
        node->right  = NULL;
        node->data   = data;
        strncpy(node->text, text, 127);
        node->text[127] = '\0';
        strncpy(node->key, key, 127);
        node->key[127] = '\0';

        if (lv->moved == 0) {
            if (lv->selected == NULL) {
                lv->draw = lv->selected = node;
            }
            else {
                if (COMP(lv->selected->key, node->key) > 0) {
	                lv->draw = lv->selected = node;
                }
            }
        }
        *l = node;
        DBG_PRINTF(" add %s\n", key);
        return 0;
    }

    /* DBG_PRINTF("%s %s\n", key, (*l)->key); */

    cmp = COMP(key, (*l)->key);

    if (cmp == 0) {
        /* we've found ourself.. eek */
        DBG_PRINTF(" dup\n");
        return -1;
    }

    if (cmp < 0) {
        /* Avoid tail-recursion:
           return listview_add_i (lv, &(*l)->left, *l, text, key, data);
         */
        parent = *l;
        l = &(*l)->left;
        DBG_PRINTF("L");
        goto restart;
    }

    /* ( cmp > 0) */
    /* Avoid tail-recursion:
       return listview_add_i (lv, &(*l)->right, *l, text, key, data);
     */
    parent = *l;
    l = &(*l)->right;
    DBG_PRINTF("R");
    goto restart;
}


/*
 *  listview_add
 */
int listview_add(lv_t *l, const char *text, void *data)
{
    return listview_add_i(l, &l->root, NULL, text, text, data);
}


/*
 *  listview_add_key
 */
int listview_add_key(lv_t *l, 
                        const char *text, 
                        const char *key, 
                        void *data)
{
    return listview_add_i(l, 
                          &l->root, 
                          NULL, 
                          text, 
                          key, 
                          data);
}


/*
 *  listview_dump_i
 */
static
void listview_dump_i(lv_node_t *l, int indent)
{
    while (l) {
        listview_dump_i(l->left, indent + 2);

        DBG_PRINTF("%*s%s %s\n", indent, "", l->key, l->text);

        /* Avoid tail-recursion:
           listview_dump_i ( l->right, indent + 2 );
         */
        l = l->right;
        indent += 2;
    }
}


/*
 *  listview_dump
 */
void listview_dump(lv_t *l)
{
    listview_dump_i(l->root, 0);
}


/*
 *  listview_free_i
 */
static
void listview_free_i(lv_node_t *n, void (*fp) (void *))
{
    while (n) {
        lv_node_t *right;

        if (n->data) {
            if (fp) {
                fp(n->data);
            }
            else {
                free(n->data);
            }
        }

        listview_free_i(n->left, fp);

        right = n->right;
        free(n);

        /* Avoid tail-recursion:
           listview_free_i ( n->right, fp );
         */
        n = right;
    }
}


/*
 *  listview_free
 */
void listview_free(lv_t *lv, void (*fp) (void *data))
{
    listview_free_i(lv->root, fp);

    memset(lv, 0, sizeof (lv_t));
}


/*
 *  listview_move_n
 */
lv_node_t *listview_move_n(lv_node_t *root, int n, int *moved)
{
    lv_node_t *p;
    int i, moves = 0;

    if (n == 0) {
        if (moved) {
            *moved = moves;
        }
        return root;
    }

    p = root;
    i = 0;

    if (n < 0) {
        for (i = n; i < 0; i++) {
            lv_node_t *tmp = listview_prev_i(p);

            if (tmp == NULL) {
	            break;
            }

            moves--;
            p = tmp;
        }
    }
    else {			/* n > 0 */
        for (i = 0; i < n; i++) {
            lv_node_t *tmp = listview_next_i(p);

            if (tmp == NULL) {
	            break;
            }

            moves++;
            p = tmp;
        }
    }
    if (moved) {
        *moved = moves;
    }
    return p;
}


/*
 *  listview_find_closest_i
 */
static
void listview_find_closest_i(lv_t *lv, lv_node_t *n, const char *key)
{
    int cmp;

restart:
    cmp = COMP(key, n->key);

    if (cmp == 0) {
        lv->selected = n;
        return;
    }

    if (cmp < 0) {
        if (!n->left) {
            lv->selected = n;
            return;
        }
        /* Avoid tail-recursion:
           return listview_find_closest_i ( lv, n->left, key );
         */
        n = n->left;
        goto restart;
    }

    /* Here: cmp > 0 */
    if (!n->right) {
        lv->selected = n;
        return;
    }
    /* Avoid tail-recursion:
       return listview_find_closest_i ( lv, n->right, key );
     */
    n = n->right;
    goto restart;
}


/*
 *  listview_find_closest
 */
void listview_find_closest(lv_t *lv, const char *key)
{
    int dist;

    listview_find_closest_i(lv, lv->root, key);
  
    lv->draw = listview_move_n(lv->selected, -5, &dist);
    lv->distance = -dist;
}

