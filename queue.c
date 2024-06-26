#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_head = malloc(sizeof(struct list_head));
    if (!new_head)
        return NULL;

    INIT_LIST_HEAD(new_head);

    return new_head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *current, *next;
    list_for_each_entry_safe (current, next, head, list) {
        q_release_element(current);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = strdup(s);
    if (!new->value) {
        free(new);
        return false;
    }
    list_add_tail(&new->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *rm_node = list_first_entry(head, element_t, list);
    list_del(&rm_node->list);
    if (sp != NULL) {
        strncpy(sp, rm_node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return rm_node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *rm_node = list_last_entry(head, element_t, list);
    list_del(&rm_node->list);
    if (sp != NULL) {
        strncpy(sp, rm_node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return rm_node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || !head->next)
        return false;
    struct list_head *slow = head->next;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        slow = slow->next;
    }
    list_del(slow);
    q_release_element(list_entry(slow, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head) || list_is_singular(head))
        return false;
    struct list_head *cur = head->next, *next = cur->next;
    bool flag = false;

    while (cur != head && next != head) {
        element_t *cur_entry = list_entry(cur, element_t, list);
        element_t *nxt_entry = list_entry(next, element_t, list);
        while (next != head && !strcmp(cur_entry->value, nxt_entry->value)) {
            list_del(next);
            q_release_element(nxt_entry);
            next = cur->next;
            nxt_entry = list_entry(next, element_t, list);
            flag = true;
        }
        if (flag) {
            list_del(cur);
            q_release_element(cur_entry);
            flag = false;
        }
        cur = next;
        next = next->next;
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head *first = head->next;

    while (first != head && first->next != head) {
        struct list_head *second = first->next;
        list_move(first, second);
        first = first->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *cur, *safe;
    list_for_each_safe (cur, safe, head) {
        list_move(cur, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/ komark06
    if (!head || list_empty(head))
        return;
    struct list_head *node, *safe, *cut = head;
    LIST_HEAD(tmp);
    int count = 0;
    list_for_each_safe (node, safe, head) {
        count++;
        if (count == k) {
            list_cut_position(&tmp, cut, node);
            q_reverse(&tmp);
            count = 0;
            list_splice(&tmp, cut);
            cut = safe->prev;
        }
    }
}
static int q_merge_two(struct list_head *L1, struct list_head *L2, bool descend)
{
    if (!L1 || !L2)
        return 0;

    int count = 0;
    LIST_HEAD(tmp);
    while (!list_empty(L1) && !list_empty(L2)) {
        element_t *L1_entry = list_first_entry(L1, element_t, list);
        element_t *L2_entry = list_first_entry(L2, element_t, list);
        if (descend ^ (strcmp(L1_entry->value, L2_entry->value) <= 0))
            list_move_tail(&L1_entry->list, &tmp);
        else
            list_move_tail(&L2_entry->list, &tmp);
        count++;
    }
    count += q_size(L1) + q_size(L2);
    list_splice(&tmp, L1);
    list_splice_tail_init(L2, L1);

    return count;
}
/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /* Find middle point */
    struct list_head *mid;
    struct list_head *slow = head->next;
    for (struct list_head *fast = head->next->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        slow = slow->next;
    }

    mid = slow;

    LIST_HEAD(second);
    list_cut_position(&second, mid, head->prev);

    /* Conquer */
    q_sort(head, descend);
    q_sort(&second, descend);

    /* Merge */
    q_merge_two(head, &second, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;

    struct list_head *cur = head->prev;
    struct list_head *next;

    while (cur->prev != head) {
        next = cur->prev;
        element_t *cur_entry = list_entry(cur, element_t, list);
        element_t *next_entry = list_entry(next, element_t, list);

        if (strcmp(cur_entry->value, next_entry->value) < 0) {
            list_del(next);
            q_release_element(next_entry);
        } else {
            cur = next;
        }
    }
    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head)
        return 0;

    struct list_head *cur = head->prev;
    struct list_head *next;

    while (cur->prev != head) {
        next = cur->prev;

        element_t *cur_entry = list_entry(cur, element_t, list);
        element_t *next_entry = list_entry(next, element_t, list);

        if (strcmp(cur_entry->value, next_entry->value) > 0) {
            list_del(next);
            q_release_element(next_entry);
        } else {
            cur = next;
        }
    }
    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head)) {
        return 0;
    }

    if (list_is_singular(head)) {
        return list_first_entry(head, queue_contex_t, chain)->size;
    }

    LIST_HEAD(tmp);
    queue_contex_t *cur, *safe;
    queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);

    list_for_each_entry_safe (cur, safe, head, chain) {
        list_splice_init(cur->q, &tmp);
    }

    int size = q_size(&tmp);
    list_splice_init(&tmp, first->q);
    q_sort(first->q, descend);

    return size;
}