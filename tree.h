/* a nasty nasty string tree. */
/* this is mostly to obtain uniqueness than sorting. */

#ifndef TREE_H
#define TREE_H


typedef struct tree_node_tag {
  struct tree_node_tag *left, *right;
  char *key;			/* UPPERCASE */
  void *data;
} tree_node_t;

typedef tree_node_t tree_t;

/* JDH returns 1 if data was added (key not found) or 0 if data
 * not added (key already present).
 */
int tree_add(tree_node_t **root, 
             const char *key, 
             void *data);

void tree_walk(tree_node_t *root, 
               void (*fp) (const char *key, void *data,  void *), 
               void *arg);

void tree_delete(tree_node_t *root, void (*fp) (void*));

void tree_count(tree_node_t *root, unsigned int *counter);

#endif

