#ifndef _LIST_H
#define _LIST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct listnode
{
    struct listnode *next;
    struct listnode *prev;
};

#define node_to_item(node, container, member) \
    (container *) (((char*) (node)) - offsetof(container, member))

#define list_declare(name) \
    struct listnode name = { \
        .next = &name, \
        .prev = &name, \
    }

#define list_for_each(node, list) \
    for (node = (list)->next; node != (list); node = node->next)

#define list_for_each_reverse(node, list) \
    for (node = (list)->prev; node != (list); node = node->prev)

static inline void list_add_tail(struct listnode *head, struct listnode *item)
{
    item->next = head;
    item->prev = head->prev;
    head->prev->next = item;
    head->prev = item;
}

static inline void list_remove(struct listnode *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
}

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
