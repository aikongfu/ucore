#include <stdio.h>
#include <stdlib.h>

typedef unsigned int uint32_t;

struct list_entry {
	struct list_entry *prev, *next;
};


struct page {
	int index;
	int flag;
	struct list_entry link;
};

void printPage(struct page *p) {
	printf("page: {index: %d, flag: %d, link: %d}\n", p->index, p->flag, &p->link);
}

void list_init(struct list_entry *elm) {
	elm->prev = elm;
	elm->next = elm;
}

int main() {

	struct list_entry *l0 = (struct list_entry *)malloc(sizeof(struct list_entry));
	struct list_entry *l1, *l2;

	struct page *p1 = (struct page *)malloc(sizeof(struct page));
	p1->index = 1;
	p1->flag = 1;
	p1->link = *(struct list_entry *)malloc(sizeof(struct list_entry));

	struct page *p2 = (struct page*)malloc(sizeof(struct page));
	p2->index = 2;
	p2->flag = 3;
	p2->link = *(struct list_entry *)malloc(sizeof(struct list_entry));

	p1->link.next = (struct list_entry *)&p2->link;


	printPage(p1);


	printPage(p2);
	
	uint32_t offset = (&((struct page *)0)->link);
	
	uint32_t l22_address = p1->link.next;

	uint32_t p2_address = l22_address - offset;

	struct page *p22 = (struct page *)p2_address;

	printPage(p22);

	return 0;
}
