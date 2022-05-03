/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "ilist.h"
#include <stdio.h>

#ifdef DEBUG_ILIST
static bool ilist_integrity_check(struct list_head *head,
            struct list_head *prev, struct list_head *next)
{
    if (unlikely(prev->next != next)) {
        fprintf(stderr, "ilist corruption head (%p) (%p) prev->next should be next (%p), but was (%p)\n",
            head, prev, next, prev->next);
        return false;
    }

    if (unlikely(next->prev != prev)) {
        fprintf(stderr, "ilist corruption head (%p) (%p) next->prev should be prev (%p), but was (%p)\n",
            head, next, prev, next->prev);
        return false;
    }

    return true;
}

static bool ilist_list_check(struct list_head *head)
{
    struct list_head *prev = head, *next = head->next;
    bool success;

    success = ilist_integrity_check(head, prev, next);
    while (success && next != head) {
        prev = next;
        next = prev->next;
        success = ilist_integrity_check(head, prev, next);
    }

    return success;
}

static bool ilist_head_check(struct ilist_head *ihead)
{
    bool success;

    success = ilist_list_check(&ihead->node_list);
    if (success && !ilist_head_empty(ihead))
        ilist_list_check(&ilist_first(ihead)->index_list);

    return success;
}
#endif

/**
 * ilist_add - insert a new index list node.
 * @ihead: the index head to be insert.
 * @inode: the index node to insert.
 */
void ilist_add(struct ilist_head *ihead, struct ilist_node *inode)
{
    struct ilist_node *walk, *first, *prev = NULL;
    struct list_head *next = &ihead->node_list;

#ifdef DEBUG_ILIST
    if (unlikely(!ilist_head_check(ihead)))
        return;
#endif

    /* Check if node is initialized */
    if (unlikely(!list_check_empty(&inode->node_list))) {
        fprintf(stderr, "ilist corruption node_list should be empty\n");
        return;
    }

    if (unlikely(!list_check_empty(&inode->index_list))) {
        fprintf(stderr, "ilist corruption index_list should be empty\n");
        return;
    }

    /* Direct insertion of new nodes */
    if (ilist_head_empty(ihead))
        goto finish;

    /* Traverse to find a suitable insertion point */
    first = walk = ilist_first(ihead);
    do {
        if (inode->index < walk->index) {
            next = &walk->node_list;
            break;
        }
        prev = walk;
        walk = list_next_entry(walk, index_list);
    } while (walk != first);

    /* Whether to insert the index linked list */
    if (!prev || prev->index != inode->index)
        list_add_prev(&walk->index_list, &inode->index_list);

finish:
    list_add_prev(next, &inode->node_list);

#ifdef DEBUG_ILIST
    ilist_head_check(ihead);
#endif
}

/**
 * ilist_del - delete a node form index head.
 * @ihead: the index head to be delete.
 * @inode: the index node to delete.
 */
void ilist_del(struct ilist_head *ihead, struct ilist_node *inode)
{
    struct ilist_node *next;

#ifdef DEBUG_ILIST
    if (unlikely(!ilist_head_check(ihead)))
        return;
#endif

    /* Check if index list is inserted */
    if (!ilist_node_empty(inode)) {
        if (inode->node_list.next != &ihead->node_list) {
            next = list_next_entry(inode, index_list);
            if (ilist_node_empty(next))
                list_add(&inode->index_list, &inode->index_list);
        }
        list_del_init(&inode->index_list);
    }

    list_del_init(&inode->node_list);

#ifdef DEBUG_ILIST
    ilist_head_check(ihead);
#endif
}
