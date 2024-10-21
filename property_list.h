#include <stdio.h>

typedef struct Property {
	char name[100];
	int type; // 0-字符串，1-数字，2-id，3-数组
	struct Property *next;

} Property;

typedef struct PropertyList {
	Property *head;
	Property *tail;
} PropertyList;

void propertyListAppend(PropertyList *list, char *name, int len, int type);
void propertyListDestroy(PropertyList *list);
