#include "property_list.h"
#include <stdlib.h>
#include <string.h>

void propertyListAppend(PropertyList *list, char *name, int len, int type) {

	Property *p = malloc(sizeof(Property));
	strncpy(p->name, name, len);
	p->type = type;

	if (list->head == NULL) {
		list->head = p;
		list->tail = p;
	}else {
		list->tail->next = p;
		list->tail = p;
	}
}

void propertyListDestroy(PropertyList *list) {
    Property *node = list->head;
    while (node != NULL) {
        Property *next = node->next;
        free(node);
        node = next;
    }
}
