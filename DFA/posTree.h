#pragma once
typedef struct POSTREE {
	int pos;
	int height;
	struct POSTREE *left;
	struct POSTREE *right;
	struct REGEX *ref;
} POSTREE;

POSTREE *search(POSTREE *node, int key);
POSTREE *insert(POSTREE *root, int key);
POSTREE *rebalanceAVL(POSTREE *node, POSTREE *root);
void insertAt(POSTREE *node, POSTREE *child);
POSTREE *getPath(POSTREE *node, POSTREE *root);
POSTREE *restructure(POSTREE *parent, POSTREE *child, int cSide, int gcSide);
POSTREE *rightRotate(POSTREE *node);
POSTREE *leftRotate(POSTREE *node);
int getHeight(POSTREE *node);
int higherChild(POSTREE *node, POSTREE **highestChild);
int isLeaf(POSTREE *node);
POSTREE *makeNode(int key, struct REGEX *ref);
POSTREE *merge(POSTREE *root1, POSTREE *root2);
POSTREE *join(POSTREE *t1, POSTREE *t2);