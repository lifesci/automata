#pragma once
typedef struct REGEX {
	enum {
		alphaK,
		emptyK,
		altK,
		concatK,
		starK,
		endK
	} kind;
	union {
		char alphaR;
		struct {
			struct REGEX *left;
			struct REGEX *right;
		} altR;
		struct {
			struct REGEX *left;
			struct REGEX *right;
		} concatR;
		struct {
			struct REGEX *child;
		} starR;
	} val;
	int position;
	int nullable;
	struct POSTREE *firstPos;
	struct POSTREE *lastPos;
	struct POSTREE *followPos;
} REGEX;

REGEX *makeREGEXalpha(char alpha);

REGEX *makeREGEXempty();

REGEX *makeREGEXalt(REGEX *left, REGEX *right);

REGEX *makeREGEXconcat(REGEX *left, REGEX *right);

REGEX *makeREGEXstar(REGEX *child);

REGEX *makeREGEXend();

REGEX *augment(REGEX *root);

void labelPositions(REGEX *root);

int nullable(REGEX *node);