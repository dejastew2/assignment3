#include "treesearch.h"
#include "safefunctions.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

node *add_to_list(node *root, char myc, int mycount) {
	node *newnode;
	node *temp;

	/* Create the node with proper values */
	newnode = safe_malloc(sizeof(node));
	newnode->c = myc;
	newnode->count = mycount;
	newnode->left = NULL;
	newnode->right = NULL;
	newnode->parent = NULL;
	newnode->listnext = NULL;

	/* If we already have a linked list, add to end of list */
	if (root) {
		for (temp = root; temp->listnext != NULL; temp = temp->listnext)
			;
		temp->listnext = newnode;
		return root;
	/* If this is the first element, return it as the linked list */
	} else
		return newnode;

}

node *list_to_tree(node *root) {
	node *frontoflist;

	if (root != NULL) {
	/* Moves through tree, from smallest to largest */
	while (root->listnext != NULL) {

		node *lastnode = NULL;

		/* Creates a blank node in tree */
		node *newnode = safe_malloc(sizeof(node));
		newnode->c = -1;
		newnode->count = root->count + root->listnext->count;
		newnode->left = root;
		newnode->right = root->listnext;
		newnode->parent = NULL;

		/* Moves through list until it finds count > new total */
		newnode->listnext = root->listnext->listnext;
		while (newnode->listnext != NULL)
			if (newnode->listnext->count < newnode->count) {
				lastnode = newnode->listnext;
				newnode->listnext = newnode->listnext->listnext;
			} else
				break;

		/* Sets up right child node */
		frontoflist = root->listnext->listnext;
		root->listnext->listnext = NULL;
		root->listnext->parent = newnode;

		/* Sets up left child node */
		root->listnext = NULL;
		root->parent = newnode;

		/* If there is a node before the new node, insert it */
		if (lastnode != NULL) {
			lastnode->listnext = newnode;
			root = frontoflist;
		/* If this is the last or smallest node return it as new root */
		} else
			root = newnode;

	}
	}

	return root;
}

node *build_h_tree(int counts[], int totalchars) {
	int i, cursmallest;
	node *root = NULL;

	/* Build linked list */
	while (totalchars > 0) {
		cursmallest = 0;
		/* Move through each count to find char with smallest count */
		for (i = 0; i < 256; i ++) {
			if (counts[i] > 0) {
				/* If cursmallest has been set */
				if (counts[cursmallest] > 0) {
					/* If cursmallest no longer smallest */
					if (counts[i] < counts[cursmallest])
						cursmallest = i;
				/* If cursmallest hasn't been set, set it */
				} else
					cursmallest = i;
			}
		}
		/* Add smallest char to list, then remove it from array */
		root = add_to_list(root, cursmallest, counts[cursmallest]);
		totalchars -= counts[cursmallest];
		counts[cursmallest] = 0;
	}

	/* Convert linked list to tree */
	root = list_to_tree(root);

	return root;
}

void enqueue(mininode *toadd, mininode **queue) {
	/* If we already have a queue */
	if (*queue != NULL) {
		/* Find last item in queue and add new node to end of queue */
		mininode *lastmn = *queue;
		while ((lastmn->next) != NULL)
			lastmn = lastmn->next;

		lastmn->next = toadd;
	/* If queue is empty, point queue to added node */
	} else
		*queue = toadd;
}

mininode *dequeue(mininode **queue) {
	/* If we have a nonempty queue */
	if (queue != NULL) {
		/* Get first node, increment queue pointer, then return first */
		mininode *ret = *queue;
		*queue = (*queue)->next;
		return ret;
	} else
		return NULL;
}

void free_queue(mininode *queue) {
	mininode *temp;
	/* Move through queue, removing nodes as we go */
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

	/* Enqueue root node into mininode queue */
	curmn = safe_malloc(sizeof(mininode));
	curmn->n = root;
	curmn->next = NULL;
	queue = curmn;

	/* Perform breadth-first search */
	while (queue != NULL) {
		curmn = dequeue(&queue);

		/* If we've found the char, destroy queue and end loop */
		if (curmn->n->c == searchfor) {
			foundnode = curmn->n;
			free_queue(queue);
			break;
		/* If we're still looking */
		} else {
			/* If we have a left child, add to end of queue */
			if (curmn->n->left != NULL) {
				temp = safe_malloc(sizeof(mininode));
				temp->n = curmn->n->left;
				temp->next = NULL;
				enqueue(temp, &queue);
			}

			/* If we have a right child, add to end of queue */
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

void free_tree(node *root) {
	mininode *temp;
	mininode *curmn;
	mininode *queue;

	if (root != NULL) {
	
	/* Enqueue root node into mininode queue */
	curmn = safe_malloc(sizeof(mininode));
	curmn->n = root;
	curmn->next = NULL;
	queue = curmn;

	/* Perform breadth-first search */
	while (queue != NULL) {
		curmn = dequeue(&queue);

		/* If we have a left child, add to end of queue */
		if (curmn->n->left != NULL) {
			temp = safe_malloc(sizeof(mininode));
			temp->n = curmn->n->left;
			temp->next = NULL;
			enqueue(temp, &queue);
		}

		/* If we have a right child, add to end of queue */
		if (curmn->n->right != NULL) {
			temp = safe_malloc(sizeof(mininode));
			temp->n = curmn->n->right;
			temp->next = NULL;
			enqueue(temp, &queue);
		}

		/* Free tree node and queue node */
		free(curmn->n);
		free(curmn);
	}
	}
}
