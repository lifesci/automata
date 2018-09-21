#include <stdlib.h>
#include "syntax.h"
#include "posTree.h"

REGEX *makeREGEXalpha(char alpha) {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = alphaK;
	r->val.alphaR = alpha;
	assignDefaults(r);
	return r;
}

REGEX *makeREGEXempty() {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = emptyK;
	assignDefaults(r);
	return r;
}

REGEX *makeREGEXalt(REGEX *left, REGEX *right) {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = altK;
	r->val.altR.left = left;
	r->val.altR.right = right;
	assignDefaults(r);
	return r;
}

REGEX *makeREGEXconcat(REGEX *left, REGEX *right) {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = concatK;
	r->val.concatR.left = left;
	r->val.concatR.right = right;
	assignDefaults(r);
	return r;
}

REGEX *makeREGEXstar(REGEX *child) {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = starK;
	r->val.starR.child = child;
	assignDefaults(r);
	return r;
}

REGEX *makeREGEXend() {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = endK;
	assignDefaults(r);
	return r;
}

void assignDefaults(REGEX *r) {
	r->firstPos = NULL;
	r->lastPos = NULL;
	r->followPos = NULL;
	r->position = -1;
	r->nullable = -1;
}

REGEX *augment(REGEX *root) {
	REGEX *end = makeREGEXend();
	REGEX *newRoot = makeREGEXconcat(root, end);
	int i = 0;
	labelPositions(newRoot, 0);
	nullable(newRoot);
	firstPos(newRoot);
	lastPos(newRoot);
	followPos(newRoot);
	return newRoot;
}

int labelPositions(REGEX *node, int i) {
	switch (node->kind) {
	case alphaK:
		node->position = i;
		return i + 1;
		
	case emptyK:
		return i;
	
	case altK:
		i = labelPositions(node->val.altR.left, i);
		i = labelPositions(node->val.altR.right, i);
		return i;

	case concatK:
		i = labelPositions(node->val.concatR.left, i);
		i = labelPositions(node->val.concatR.right, i);
		return i;

	case starK:
		i = labelPositions(node->val.starR.child, i);
		return i;

	case endK:
		node->position = i;
		return i + 1;
	}
}

int nullable(REGEX *node) {
	switch (node->kind) {
	case alphaK:
		node->nullable = 0;
		break;

	case emptyK:
		node->nullable = 1;
		break;

	case altK:
		node->nullable = (nullable(node->val.altR.left) || nullable(node->val.altR.right));
		break;

	case concatK:
		node->nullable = (nullable(node->val.concatR.left) && nullable(node->val.concatR.right));
		break;

	case starK:
		node->nullable = 1;
		break;

	case endK:
		node->nullable = 0;
		break;
	}
	return node->nullable;
}

POSTREE *firstPos(REGEX *node) {
	switch (node->kind) {
	case alphaK:
		node->firstPos = makeNode(node->position, node);
		break;

	case emptyK:
		node->firstPos = NULL;
		break;

	case altK:
		node->firstPos = merge(firstPos(node->val.altR.left), firstPos(node->val.altR.right));
		break;

	case concatK: {
		REGEX *left = node->val.concatR.left;
		REGEX *right = node->val.concatR.right;
		if (left->nullable) {
			node->firstPos = merge(firstPos(left), firstPos(right));
		} else {
			node->firstPos = firstPos(left);
			firstPos(right);
		}
		break;
	}
	case starK:
		node->firstPos = firstPos(node->val.starR.child);
		break;

	case endK:
		node->firstPos = makeNode(node->position, node);
		break;
	}
	return node->firstPos;
}

POSTREE *lastPos(REGEX *node) {
	switch (node->kind) {
	case alphaK:
		node->lastPos = makeNode(node->position, node);
		break;

	case emptyK:
		node->lastPos = NULL;
		break;

	case altK:
		node->lastPos = merge(lastPos(node->val.altR.left), lastPos(node->val.altR.right));
		break;

	case concatK: {
		REGEX *left = node->val.concatR.left;
		REGEX *right = node->val.concatR.right;
		if (right->nullable) {
			node->lastPos = merge(lastPos(left), lastPos(right));
		}
		else {
			node->lastPos = lastPos(right);
			lastPos(left);
		}
		break;
	}
	case starK:
		node->lastPos = lastPos(node->val.starR.child);
		break;

	case endK:
		node->lastPos = makeNode(node->position, node);
		break;
	}
	return node->lastPos;
}

void followPos(REGEX *node) {
	switch (node->kind) {
	case alphaK:
		break;

	case emptyK:
		break;

	case altK:
		followPos(node->val.altR.left);
		followPos(node->val.altR.right);
		break;

	case concatK: {
		REGEX *left = node->val.concatR.left->lastPos;
		REGEX *right = node->val.concatR.right->firstPos;
		buildFollowPosConcat(left, right);
		followPos(node->val.concatR.left);
		followPos(node->val.concatR.right);
		break;
	}
	case starK:
		buildFollowPosStar(node->lastPos, node->firstPos);
		followPos(node->val.starR.child);
		break;

	case endK:
		break;
	}
}

void buildFollowPosConcat(POSTREE *left, POSTREE *right) {
	if (left) {
		POSTREE *leftFollow = (left->ref)->followPos;
		(left->ref)->followPos = merge(leftFollow, right);
		buildFollowPosConcat(left->left, right);
		buildFollowPosConcat(left->right, right);
	}
}

void buildFollowPosStar(POSTREE *first, POSTREE *last) {
	if (last) {
		POSTREE *follow = (last->ref)->followPos;
		(last->ref)->followPos = merge(follow, first);
		buildFollowPosStar(first, last->left);
		buildFollowPosStar(first, last->right);
	}
}