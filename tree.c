#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tree.h"
#include "debug_config.h"

#if DEBUG_TREE
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


/*
 *  tree_add
 */
int
tree_add(tree_node_t **root, const char *key, void *data)
{
    tree_node_t *n = *root;
    int cmp;

    if (!n) {
        DBG_PRINTF(" tree: adding '%s'\n", key);

        n = malloc(sizeof (tree_node_t));
        n->key = strdup(key);

        n->left  = NULL;
        n->right = NULL;
        n->data  = data;
        *root = n;
        return 1;   /* JDH - we saved the data into the tree */
    }

    cmp = strcasecmp(n->key, key);

    if (cmp < 0) {
        return tree_add(&n->left, key, data);
    }

    if (cmp > 0) {
        return tree_add(&n->right, key, data);
    }

    /* must have found it if == or has already been added. return. */
    /* JDH - whoa! if data was malloc'd we didn't save it anywhere */
    return 0;   /* JDH - data not saved */
}


/*
 *  tree_walk
 */
void tree_walk(tree_node_t *root, 
                 void (*fp) (const char *key, void *, void *), 
                 void *arg)
{
restart:
    if (!root) {
        return;
    }

    tree_walk(root->right, fp, arg);

    if (root->key) {
        fp(root->key, root->data, arg);
    }

    /* Avoid tail-recursion:
       tree_walk (root->left, fp, arg);
     */
    root = root->left;
    goto restart;
}


/*
 *  tree_delete
 */
void tree_delete(tree_node_t *root, void (*fp) (void *))
{
    tree_node_t *right;

restart:
    if (!root) {
        return;
    }

    if (root->data) {
        if (fp) {
            fp(root->data);
        }
        else {
            free(root->data);
        }
    }

    if (root->key) {
        free(root->key);
    }
   
    tree_delete(root->left, fp);

    right = root->right;
    free(root);

    /* Avoid tail-recursion:
       tree_delete (right, fp);
     */
    root = right;
    goto restart;
}


/*
 *  tree_count - count nodes in the tree
 *  WARNING: init variable passed in counter to 0 before calling!
 */
void tree_count(tree_node_t *root, unsigned int *counter)
{
restart:
    if (!root) {
        return;
    }

    /* check to the right */
    tree_count(root->right, counter);

    /* count the node we're in */
    (*counter)++;

    /* check to the left
       Avoid tail-recursion: tree_walk (root->left, fp, arg);
     */
    root = root->left;
    goto restart;
}

