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

node *build_h_tree(int fdin, int fdout);

node *list_to_tree(node *root);

node *add_to_tree(node *root, char myc, int mycount);

node *bfs(node *root, char searchfor);

void enqueue(mininode *toadd, mininode **queue);

mininode *dequeue(mininode **queue);

void free_queue(mininode *queue);

#endif
