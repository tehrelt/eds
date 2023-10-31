#pragma once

#include "stddef.h"

typedef struct list_head {
    struct list_head* prev;
    struct list_head* next;
} list_head;

#define container_of(ptr, type, member) ({ \
                const typeof( ((type *)0)->member ) *__mptr = (ptr); \
                (type *)( (char *)__mptr - offsetof(type,member) );}

#define list_entry(ptr, type, member) \
        ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define list_for_each(pos, head) \
     for ((pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)

#define list_for_each_prev(pos, head) \
     for ((pos) = (head)->prev; (pos) != (head); (pos) = (pos)->prev)

#define list_for_each_safe(pos, store, head)                           \
     for ((pos) = (head)->next, (store) = (pos)->next; (pos) != (head); \
          (pos) = (store), (store) = (pos)->next)

#define list_for_each_decl(pos, head) \
     for (list_head * (pos) = (head)->next; (pos) != (head); (pos) = (pos)->next)



static inline void list_head_init(list_head* head)
{
    head->next = head->prev = head;
}

static inline int list_head_empty(const list_head* head)
{
    return head->next == head;
}

static inline unsigned list_head_size(const list_head* head)
{
    unsigned size = 0;
    if (!list_head_empty(head))
        list_for_each_decl(it, head) size += 1;
    return size;
}

static inline void list_head_insert_after(list_head* new_entry, list_head* location)
{
    // We store the old `next` element.
    list_head* old_next = location->next;
    // We insert our element.
    location->next = new_entry;
    // We update the `previous` link of our new entry.
    new_entry->prev = location;
    // We update the `next` link of our new entry.
    new_entry->next = old_next;
    // We link the previously `next` element to our new entry.
    old_next->prev = new_entry;
}

static inline void list_head_insert_before(list_head* new_entry, list_head* location)
{
    // We store the old `previous` element.
    list_head* old_prev = location->prev;
    // We link the old `previous` element to our new entry.
    old_prev->next = new_entry;
    // We update the `previous` link of our new entry.
    new_entry->prev = old_prev;
    // We update the `next` link of our new entry.
    new_entry->next = location;
    // Finally, we close the link with the old insertion location element.
    location->prev = new_entry;
}

static inline void list_head_remove(list_head* entry)
{
    // Check if the element is actually in a list.
    if (!list_head_empty(entry)) {
        // We link the `previous` element to the `next` one.
        entry->prev->next = entry->next;
        // We link the `next` element to the `previous` one.
        entry->next->prev = entry->prev;
        // We initialize the entry again.
        list_head_init(entry);
    }
}

static inline list_head* list_head_pop(list_head* head)
{
    // Check if the list is not empty.
    if (!list_head_empty(head)) {
        // Store the pointer.
        list_head* value = head->next;
        // Remove the element from the list.
        list_head_remove(head->next);
        // Return the pointer to the element.
        return value;
    }
    return NULL;
}

static inline void list_head_append(list_head* main, list_head* secondary)
{
    // Check that both lists are actually filled with entries.
    if (!list_head_empty(main) && !list_head_empty(secondary)) {
        // Connect the last element of the main list to the first one of the secondary list.
        main->prev->next = secondary->next;
        // Connect the first element of the secondary list to the last one of the main list.
        secondary->next->prev = main->prev;

        // Connect the last element of the secondary list to our main.
        secondary->prev->next = main;
        // Connect our main to the last element of the secondary list.
        main->prev = secondary->prev;

        // Re-initialize the secondary list.
        list_head_init(secondary);
    }
}