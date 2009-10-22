#ifndef TREESEARCH_H_INCLUDED
#define TREESEARCH_H_INCLUDED

typedef struct h_node node;
struct h_node {
	int c;
	int count;
	struct h_node *left;
	struct h_node *right;
	struct h_node *parent;
	struct h_node *listnext;
};

typedef struct q_node mininode;
struct q_node {
	struct h_node *n;
	struct q_node *next;
};

/* Generate a node and add it to the linked list */
node *add_to_list(node *root, char myc, int mycount);

/* Convert a linked list to a search tree */
node *list_to_tree(node *root);

/* Build tree with char counts */
node *build_h_tree(int counts[], int totalchars);

/* Destroy a search tree */
void free_tree(node *root);

/* Enqueue a mininode to a queue */
void enqueue(mininode *toadd, mininode **queue);

/* Dequeue the next node from a queue */
mininode *dequeue(mininode **queue);

/* Destroy a queue */
void free_queue(mininode *queue);

/* Run a breadth-first search on a tree */
node *bfs(node *root, char searchfor);

#endif
