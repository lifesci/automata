#include "syntax.h"

REGEX *makeREGEXalpha(char alpha) {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = alphaK;
	r->val.alphaR = alpha;
	return r;
}

REGEX *makeREGEXempty() {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = emptyK;
	return r;
}

REGEX *makeREGEXalt(REGEX *left, REGEX *right) {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = altK;
	r->val.altR.left = left;
	r->val.altR.right = right;
	return r;
}

REGEX *makeREGEXconcat(REGEX *left, REGEX *right) {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = concatK;
	r->val.concatR.left = left;
	r->val.concatR.right = right;
	return r;
}

REGEX *makeREGEXstar(REGEX *child) {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = starK;
	r->val.starR.child = child;
	return r;
}

REGEX *makeREGEXend() {
	REGEX *r;
	r = (struct REGEX *) malloc(sizeof(struct REGEX));
	r->kind = endK;
	return r;
}

REGEX *augment(REGEX *root) {
	REGEX *end = makeREGEXend();
	REGEX *newRoot = makeREGEXconcat(root, end);
	int i = 0;
	labelPositions(newRoot, &i);
	nullable(newRoot);
	return newRoot;
}

void labelPositions(REGEX *node, int *i) {
	switch (node->kind) {
	case alphaK:
		node->position = (*i);
		(*i)++;
		break;
		
	case emptyK:
		break;
	
	case altK:
		labelPositions(node->val.altR.left, i);
		labelPositions(node->val.altR.right, i);
		break;

	case concatK:
		labelPositions(node->val.concatR.left, i);
		labelPositions(node->val.concatR.right, i);
		break;

	case starK:
		labelPositions(node->val.starR.child, i);
		break;

	case endK:
		node->position = (*i);
		(*i)++;
		break;
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
		node->nullable = (nullable(node->val.altR.left) && nullable(node->val.altR.right));
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