#include <stdio.h>
#include <stdlib.h>
#include "syntax.h"
#include "postree.h"
#include "pretty.h"

REGEX* parseRegex(char *input, int *index);
REGEX* parseTerm(char *input, int *index);
REGEX *parseFactor(char *input, int *index);
REGEX *parseBase(char *input, int *index);
char peek(char *regex, int *index);
char next(char *regex, int *index);
int eat(char *regex, int *index, char item);
int more(char *regex, int *index);

int main(int argc, char* argv[]) {
	char* input;
	if (argc == 1) {
		printf("No REGEX detected");
	} else {
		input = argv[1];
		printf("REGEX: %s\n", input);
		int index = 0;
		REGEX *root = parseRegex(input, &index);
		root = augment(root);
		FILE *fp;
		fopen_s(&fp, "output.txt", "w");
		prettyPrint(root, 0, fp);
		fclose(fp);
	}
}

REGEX* parseRegex(char *input, int *index) {
	REGEX *term = parseTerm(input, index);
	if (more(input, index) && peek(input, index) == '|') {
		eat(input, index, '|');
		REGEX *regex = parseRegex(input, index);
		return makeREGEXalt(term, regex);
	} else {
		return term;
	}
}

REGEX* parseTerm(char *input, int *index) {
	REGEX *factor = makeREGEXempty();
	while (more(input, index) && peek(input, index) != ')' && peek(input, index) != '|') {
		REGEX *nextFactor = parseFactor(input, index);
		factor = makeREGEXconcat(factor, nextFactor);
	}
	return factor;
}

REGEX *parseFactor(char *input, int *index) {
	REGEX *base = parseBase(input, index);
	while (more(input, index) && peek(input, index) == '*') {
		eat(input, index, '*');
		base = makeREGEXstar(base);
	}
	return base;
}

REGEX *parseBase(char *input, int *index) {
	switch (peek(input, index)) {
	case '(':
		eat(input, index, '(');
		REGEX *r = parseRegex(input, index);
		eat(input, index, ')');
		return r;

	case '\\':
		eat(input, index, '\\');
		char esc = next(input, index);
		return makeREGEXalpha(esc);

	default:
		return makeREGEXalpha(next(input, index));
	}
}

char peek(char *input, int *index) {
	int i = (*index);
	return input[i];
}

char next(char *input, int *index) {
	char next = input[(*index)];
	(*index)++;
	return next;
}

int eat(char *input, int *index, char item) {
	char next = peek(input, index);
	if (next == item) {
		(*index)++;
		return 1;
	} else {
		exit(1);
		return 0;
	}
}

int more(char *input, int *index) {
	char next = input[(*index)];
	if (next == '\0') {
		return 0;
	} else {
		return 1;
	}
}