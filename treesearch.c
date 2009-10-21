#include "treesearch.h"
#include "safefunctions.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

node *add_to_tree(node *root, char myc, int mycount) {
	node *newnode;
	node *temp;

	newnode = safe_malloc(sizeof(node));
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

node *list_to_tree(node *root) {
	node *frontoflist;

	while (root->listnext != NULL) {

		node *lastnode = NULL;
		node *newnode = safe_malloc(sizeof(node));

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

node *build_h_tree(int counts[], int totalchars) {
	int i, cursmallest;
	node *root = NULL;

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

void enqueue(mininode *toadd, mininode **queue) {
	if (*queue != NULL) {
		mininode *lastmn = *queue;
		while ((lastmn->next) != NULL)
			lastmn = lastmn->next;

		lastmn->next = toadd;
	} else {
		*queue = toadd;
	}
}

mininode *dequeue(mininode **queue) {
	if (queue != NULL) {
		mininode *ret = *queue;
		*queue = (*queue)->next;
		return ret;
	} else {
		return NULL;
	}
}

void free_queue(mininode *queue) {
	mininode *temp;
	while (queue != NULL) {
		temp = queue;
		queue = queue->next;
		free(temp);
	}
}

node *bfs(node *root, char searchfor) {
	mininode *temp;
	mininode *curmn;
	mininode *queue;
	node *foundnode = NULL;

	curmn = safe_malloc(sizeof(mininode));
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
				temp = safe_malloc(sizeof(mininode));
				temp->n = curmn->n->left;
				temp->next = NULL;
				enqueue(temp, &queue);
			}

			if (curmn->n->right != NULL) {
				temp = safe_malloc(sizeof(mininode));
				temp->n = curmn->n->right;
				temp->next = NULL;
				enqueue(temp, &queue);
			}
		}
		free(curmn);
	}

	return foundnode;

}

