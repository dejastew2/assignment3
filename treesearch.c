#include "treesearch.h"
#include "safefunctions.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

struct h_node *add_to_tree(struct h_node *root, char myc, int mycount) {
	struct h_node *newnode;
	struct h_node *temp;

	newnode = malloc(sizeof(struct h_node));
	newnode->c = myc;
	newnode->count = mycount;
	newnode->left = NULL;
	newnode->right = NULL;
	newnode->parent = NULL;
	newnode->listnext = NULL;

	if (root) {
		for (temp = root; temp->listnext != NULL; temp = temp->listnext)
			;
		temp->listnext = newnode;
		return root;
	} else {
		return newnode;
	}
}

struct h_node *list_to_tree(struct h_node *root) {
	struct h_node *frontoflist;

	while (root->listnext != NULL) {

		struct h_node *lastnode = NULL;
		struct h_node *newnode = malloc(sizeof(struct h_node));

		/* printf("Current root: %x with value: %d\n", root->c, root->count); */

		newnode->c = -1;
		newnode->count = root->count + root->listnext->count;
		newnode->left = root;
		newnode->right = root->listnext;
		newnode->parent = NULL;

		newnode->listnext = root->listnext->listnext;
		while (newnode->listnext != NULL)
			if (newnode->listnext->count < newnode->count) {
			lastnode = newnode->listnext;
			newnode->listnext = newnode->listnext->listnext;
			} else
				break;

				frontoflist = root->listnext->listnext;
				root->listnext->listnext = NULL;
				root->listnext->parent = newnode;

				root->listnext = NULL;
				root->parent = newnode;

				if (lastnode != NULL) {
					lastnode->listnext = newnode;
					root = frontoflist;
				} else {
					root = newnode;
				}

	}

	/* printf("Current root: %x with value: %d\n", root->c, root->count); */

	return root;
}

struct h_node *build_h_tree(int fdin, int fdout) {
	int counts[256];
	int totalchars;
	char c;
	int i, cursmallest;
	struct h_node *root = NULL;

	/* Loads the counter array with zeroes */
	for (i = 0; i < 256; i ++) {
		counts[i] = 0;
	}
	totalchars = 0;

	/* Builds counters with char counts */
	while (read(fdin, &c, sizeof(char)) > 0) {
		counts[(int)c] ++;
		totalchars ++;
	}

	/* Write header to file */
	write(fdout, &totalchars, sizeof(int));
	for (i = 0; i < 256; i ++) {
		if (counts[i] > 0) {
			write(fdout, &i, sizeof(char));
			write(fdout, &counts[i], sizeof(int));
		}
	}

	/* Build linked list */
	while (totalchars > 0) {
		cursmallest = 0;
		for (i = 0; i < 256; i ++) {
			if (counts[i] > 0) {
				if (counts[cursmallest] > 0) {
					if (counts[i] < counts[cursmallest]) {
						cursmallest = i;
					}
				} else {
					cursmallest = i;
				}
			}
		}
		root = add_to_tree(root, cursmallest, counts[cursmallest]);
		totalchars -= counts[cursmallest];
		counts[cursmallest] = 0;
	}

	root = list_to_tree(root);

	return root;
}

void enqueue(struct q_node *toadd, struct q_node **queue) {
	if (*queue != NULL) {
		struct q_node *lastmn = *queue;
		while ((lastmn->next) != NULL)
			lastmn = lastmn->next;

		lastmn->next = toadd;
	} else {
		*queue = toadd;
	}
}

struct q_node *dequeue(struct q_node **queue) {
	if (queue != NULL) {
		struct q_node *ret = *queue;
		*queue = (*queue)->next;
		return ret;
	} else {
		return NULL;
	}
}

void free_queue(struct q_node *queue) {
	struct q_node *temp;
	while (queue != NULL) {
		temp = queue;
		queue = queue->next;
		free(temp);
	}
}

struct h_node *bfs(struct h_node *root, char searchfor) {
	struct q_node *temp;
	struct q_node *curmn;
	struct q_node *queue;
	struct h_node *foundnode = NULL;

	curmn = malloc(sizeof(struct q_node));
	curmn->n = root;
	curmn->next = NULL;
	queue = curmn;

	while (queue != NULL) {
		curmn = dequeue(&queue);

		if (curmn->n->c == searchfor) {
			foundnode = curmn->n;
			free_queue(queue);
			break;
		} else {
			if (curmn->n->left != NULL) {
				temp = malloc(sizeof(struct q_node));
				temp->n = curmn->n->left;
				temp->next = NULL;
				enqueue(temp, &queue);
			}

			if (curmn->n->right != NULL) {
				temp = malloc(sizeof(struct q_node));
				temp->n = curmn->n->right;
				temp->next = NULL;
				enqueue(temp, &queue);
			}
		}
		free(curmn);
	}

	return foundnode;

}

