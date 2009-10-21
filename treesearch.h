#ifndef TREESEARCH_H_INCLUDED
#define TREESEARCH_H_INCLUDED

struct h_node *build_h_tree(int fdin, int fdout);

struct h_node *list_to_tree(struct h_node *root);

struct h_node *add_to_tree(struct h_node *root, char myc, int mycount);

struct h_node *bfs(struct h_node *root, char searchfor);

void enqueue(struct q_node *toadd, struct q_node **queue);

struct q_node *dequeue(struct q_node **queue);

void free_queue(struct q_node *queue);

#endif
