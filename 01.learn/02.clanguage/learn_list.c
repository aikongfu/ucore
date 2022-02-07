#include <stdio.h>
#include <stdlib.h>

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
	// printf("page: {index: %d, flag: %d, link: \n", p->index, p->flag);
}

void list_init(struct list_entry *elm) {
	elm->prev = elm;
	elm->next = elm;
}

int main() {
	struct list_entry *l0 = (struct list_entry *)malloc(sizeof(struct list_entry));
	struct list_entry *l1 = (struct list_entry *)malloc(sizeof(struct list_entry));
	struct list_entry *l2 = (struct list_entry *)malloc(sizeof(struct list_entry));

	l0->next = l1;
	l1->prev = l0;
	l1->next = l2;
	l2->prev = l1;

	struct page *p1 = (struct page *)malloc(sizeof(struct page));
	p1->index = 1;
	p1->flag = 1;
	// p1->link = (struct link_entry *)&l1;
	p1->link = *l1;

	struct page *p2 = (struct page *)malloc(sizeof(struct page));
	p2->index = 1;
	p2->flag = 1;
	// p2->link = (struct link_entry *)&l2;
	p2->link = *l2;


	printPage(p1);

	return 0;
}
