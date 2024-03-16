#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
#include "shuffle.h"

/* Fisher-Yates shuffle Algorithm */
void q_shuffle(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;
    int q_len = q_size(head);
    struct list_head *new = head->prev;
    char *tmp;
    while (q_len > 1) {
        int randnum = rand() % q_len;

        struct list_head *old = head->next;
        for (int i = 0; i < randnum; i++) {
            old = old->next;
        }

        element_t *oldnode =
            list_entry(old, element_t, list);  // cppcheck-suppress nullPointer
        element_t *newnode =
            list_entry(new, element_t, list);  // cppcheck-suppress nullPointer
        tmp = oldnode->value;
        oldnode->value = newnode->value;
        newnode->value = tmp;

        new = new->prev;
        q_len -= 1;
    }
}