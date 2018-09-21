#include <stdlib.h>
#include "posTree.h"
#include "syntax.h"

POSTREE *search(POSTREE *node, int key) {
	int val = node->pos;
	if (key == val) {
		return node;
	} else if (key < val) {
		if (node->left) {
			return search(node->left, key);
		} else {
			return node;
		}
	} else {
		if (node->right) {
			return search(node->right, key);
		} else {
			return node;
		}
	}
}

POSTREE *insert(POSTREE *root, int key, REGEX *ref) {
	if (root) {
		POSTREE *res = search(root, key);
		if (res->pos != key) {
			POSTREE *newNode = makeNode(key, ref);
			if (key < res->pos) {
				res->left = newNode;
			}
			else {
				res->right = newNode;
			}
			return rebalanceAVL(res, root);
		}
	} else {
		return makeNode(key, ref);
	}
}

POSTREE *rebalanceAVL(POSTREE *node, POSTREE *root) {
	POSTREE *child = getPath(node, root);
	if (child) {
		POSTREE *newChild = rebalanceAVL(node, child);
		insertAt(root, newChild);
	}
	int lHeight = getHeight(root->left);
	int rHeight = getHeight(root->right);
	if (abs(lHeight - rHeight) > 1) {
		POSTREE *highestChild;
		int cSide = higherChild(root, &highestChild);
		POSTREE *highestGrandChild;
		int gcSide = higherChild(highestChild, &highestGrandChild);
		root = restructure(root, highestChild, cSide, gcSide);
	}
	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;
	return root;
}

void insertAt(POSTREE *node, POSTREE *child) {
	if (child->pos < node->pos) {
		node->left = child;
	} else {
		node->right = child;
	}
}

POSTREE *getPath(POSTREE *node, POSTREE *root) {
	int key = node->pos;
	int curKey = root->pos;
	if (key == curKey) {
		return NULL;
	}
	if (key < curKey) {
		return root->left;
	} else {
		return root->right;
	}
}

POSTREE *restructure(POSTREE *parent, POSTREE *child, int cSide, int gcSide) {
	if (!cSide && !gcSide) {
		return rightRotate(parent);
	} else if (!cSide && gcSide) {
		POSTREE *newChild = leftRotate(child);
		insertAt(newChild, parent);
		return rightRotate(parent);
	} else if (cSide && !gcSide) {
		POSTREE *newChild = rightRotate(child);
		insertAt(newChild, parent);
		return leftRotate(parent);
	} else {
		return leftRotate(parent);
	}
}

POSTREE *rightRotate(POSTREE *node) {
	POSTREE *left = node->left;
	POSTREE *leftChild = left->right;
	left->right = node;
	node->left = leftChild;
	node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
	left->height = max(getHeight(left->left), getHeight(left->right)) + 1;
	return left;
}

POSTREE *leftRotate(POSTREE *node) {
	POSTREE *right = node->right;
	POSTREE *rightChild = right->left;
	right->left = node;
	node->right = rightChild;
	node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
	right->height = max(getHeight(right->left), getHeight(right->right)) + 1;
	return right;
}

int getHeight(POSTREE *node) {
	if (node) {
		return node->height;
	}
	return -1;
}

int higherChild(POSTREE *node, POSTREE **highestChild) {
	int lHeight = getHeight(node->left);
	int rHeight = getHeight(node->right);
	if (lHeight > rHeight) {
		(*highestChild) = node->left;
		return 0;
	} else {
		(*highestChild) = node->right;
		return 1;
	}
}

int isLeaf(POSTREE *node) {
	return (!node->left && !node->right);
}

POSTREE *makeNode(int key, REGEX *ref) {
	POSTREE *node = (struct POSTREE *) malloc(sizeof(struct POSTREE));
	node->pos = key;
	node->height = 0;
	node->left = NULL;
	node->right = NULL;
	node->ref = ref;
	return node;
}

POSTREE *merge(POSTREE *root1, POSTREE *root2) {
	POSTREE *newRoot = NULL;
	if (root1) {
		newRoot = makeNode(root1->pos, root1->ref);
		newRoot = join(newRoot, root1->left);
		newRoot = join(newRoot, root1->right);
		newRoot = join(newRoot, root2);
	} else if (root2) {
		newRoot = makeNode(root2->pos, root2->ref);
		newRoot = join(newRoot, root2->left);
		newRoot = join(newRoot, root2->right);
	}
	return newRoot;
}

POSTREE *join(POSTREE *t1, POSTREE *t2) {
	if (t2) {
		t1 = insert(t1, t2->pos, t2->ref);
		t1 = join(t1, t2->left);
		t1 = join(t1, t2->right);
	}
	return t1;
}