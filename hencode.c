#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct h_node node;
struct h_node {
	int c;
	int count;
	node *left;
	node *right;
	node *parent;
	node *listnext;
};

typedef struct q_node mininode;
struct q_node {
	node *n;
	mininode *next;
};

node *build_h_tree(int fdin, int fdout);
node *list_to_tree(node *root);
node *add_to_tree(node *root, char myc, int mycount);
char encode(char c, node *treeroot);
node *bfs(node *root, char searchfor);
void enqueue(mininode *toadd, mininode **queue);
mininode *dequeue(mininode **queue);

int main(int argc, char *argv[]) {
	int infile, outfile;
	node *treeroot;
	char c;

	/* Checks to see if output file arg passed */
	if (argc > 2) {
		outfile = open(argv[2], O_WRONLY | O_TRUNC);
		if (outfile == -1) {
			printf("Invalid output file %s\n", argv[2]);
			exit(1);
		}
	/* If no output file specified, use stdout */
	} else {
		outfile = STDOUT_FILENO;
	}

	/* Checks to see if input file arg passed */
	if (argc > 1) {
		infile = open(argv[1], O_RDONLY);
		if (infile == -1) {
			printf("Invalid input file %s\n", argv[1]);
			exit(1);
		}
	} else {
		printf("No file passed!\n");
		exit(1);
	}

	treeroot = build_h_tree(infile, outfile);
	lseek(infile, 0, SEEK_SET);

	/* Compresses file */
	while (read(infile, &c, sizeof(char)) > 0) {
		c = encode(c, treeroot);
		write(outfile, &c, sizeof(char));
	}

	/* Closes any open files */
	close(infile);
	if (outfile != STDOUT_FILENO) {
		close(outfile);
	}

	return 0;
}

node *build_h_tree(int fdin, int fdout) {
	int counts[256];
	int totalchars;
	char c;
	int i, cursmallest;
	node *root = NULL;

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

node *list_to_tree(node *root) {
	node *frontoflist;

	while (root->listnext != NULL) {

		node *lastnode = NULL;
		node *newnode = malloc(sizeof(node));

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

node *add_to_tree(node *root, char myc, int mycount) {
	node *newnode;
	node *temp;

	newnode = malloc(sizeof(node));
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

char encode(char c, node *treeroot) {
	return bfs(treeroot, c)->c;
}

node *bfs(node *root, char searchfor) {
	mininode *temp;
	mininode *curmn;
	mininode *queue;
	node *foundnode = NULL;

	curmn = malloc(sizeof(mininode));
	curmn->n = root;
	curmn->next = NULL;
	queue = curmn;

	while (queue != NULL) {
		curmn = dequeue(&queue);

		if (curmn->n->c == searchfor) {
			foundnode = curmn->n;
			break;
		} else {
			if (curmn->n->left != NULL) {
				temp = malloc(sizeof(mininode));
				temp->n = curmn->n->left;
				temp->next = NULL;
				enqueue(temp, &queue);
			}

			if (curmn->n->right != NULL) {
				temp = malloc(sizeof(mininode));
				temp->n = curmn->n->right;
				temp->next = NULL;
				enqueue(temp, &queue);
			}
		}
		free(curmn);
	}

	return foundnode;

	/*
	mininode *queue = NULL;
	mininode *one = malloc(sizeof(mininode));
	one->next = NULL;
	enqueue(one, &queue);
	dequeue(&queue);
	dequeue(&queue);
	return NULL;
	*/
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
