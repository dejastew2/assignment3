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
void encode(int fdin, int fdout, node *treeroot);
node *bfs(node *root, char searchfor);
void enqueue(mininode *toadd, mininode **queue);
mininode *dequeue(mininode **queue);
void free_queue(mininode *queue);

int main(int argc, char *argv[]) {
	int infile, outfile;
	node *treeroot;

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
	encode(infile, outfile, treeroot);

	/*
	while (read(infile, &c, sizeof(char)) > 0) {
		c = encode(c, treeroot);
		write(outfile, &c, sizeof(char));
	}
	*/

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

void encode(int fdin, int fdout, node *treeroot) {
	char code = 0;
	int location = 0;
	char c;
	int parentinfo[8];
	int i;

	while (read(fdin, &c, sizeof(char)) > 0) {

		node *thenode = bfs(treeroot, c);

		for (i = 0; i < 8; i ++) {
			parentinfo[i] = -1;
		}
		i = 0;
		while (thenode->parent != NULL) {
			if (thenode->parent->left == thenode) {
				parentinfo[i] = 0;
			} else {
				parentinfo[i] = 1;
			}
			thenode = thenode->parent;
			i ++;
		}

		for (i = 7; i >= 0; i --) {
			if (parentinfo[i] > -1) {
				if (parentinfo[i] == 0)
					code = (code << 1) & 0xfe;
				else
					code = (code << 1) | 0x01;

				/* printf("   %d => 0x%x\n", parentinfo[i], code); */
				location ++;
				if (location > 7) {
					/* printf(" 0x%x\n", code); */
					write(fdout, &code, sizeof(char));
					location = 0;
					code = 0;
				}
			}
		}

	}

	if (location != 0) {
		for (i = location; i < 8; i ++)
			code = (code << 1) & 0xfe;

		write(fdout, &code, sizeof(char));
	}
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
			free_queue(queue);
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
