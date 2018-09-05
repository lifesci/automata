#include "syntax.h"
#include <stdio.h>

void printTabs(FILE *fp, int tabs);

void prettyPrint(REGEX *r, int tabs, FILE *fp) {
	printTabs(fp, tabs);
	switch (r->kind) {
	case alphaK:
		fprintf(fp, "%c\n", r->val.alphaR);
		break;
	
	case emptyK:
		fprintf(fp, "Epsilon\n");
		break;

	case altK:
		fprintf(fp, "OR\n");
		prettyPrint(r->val.altR.left, tabs + 1, fp);
		prettyPrint(r->val.altR.right, tabs + 1, fp);
		break;

	case concatK:
		fprintf(fp, "AND\n");
		prettyPrint(r->val.concatR.left, tabs + 1, fp);
		prettyPrint(r->val.concatR.right, tabs + 1, fp);
		break;

	case starK:
		fprintf(fp, "STAR\n");
		prettyPrint(r->val.starR.child, tabs + 1, fp);
		break;

	case endK:
		fprintf(fp, "#\n");
		break;

	default:
		fprintf(fp, "ERROR\n");
		break;
	}
}

void printTabs(FILE *fp, int tabs) {
	int i;
	for (i = 0; i < tabs; i++) {
		fprintf(fp, "\t");
	}
}