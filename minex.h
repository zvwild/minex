// Minex - Minimal exceptions for C
// Refer to the example.c and minex_impl.c for more usage info

#ifndef MINEX_H_
#define MINEX_H_

#include <setjmp.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

struct minex_exception
{
    enum minex_exception_type
    {
        // Add types here
        MINEX_EXCEPTION_TYPE_EXAMPLE_FILE_NOT_FOUND,
        MINEX_EXCEPTION_TYPE_EXAMPLE_RUNTIME_ERROR,
        MINEX_EXCEPTION_TYPE_EXAMPLE_NETWORK_ERROR,
        ABCC
    } type;

    // Pointer to heap allocated memory which contains the payload (can be NULL)
    void* payload;
};

struct minex_jmp_buf_list
{
    struct minex_jmp_buf_list* prev;
    jmp_buf value;
};

struct minex_exception* minex_exception_new(enum exception_type type, void* payload);
void minex_free_exception(struct minex_exception* i, bool free_payload);
struct minex_exception* minex_get_most_recent();

jmp_buf* minex_peek_jmp_buf();
void minex_push_jmp_buf(struct minex_jmp_buf_list* elem);
void minex_pop_jmp_buf();

#define MINEX_TRY(C) struct minex_jmp_buf_list* minex_result = (struct minex_jmp_buf_list*)calloc(1, sizeof(*minex_result)); \
minex_push_jmp_buf(minex_result); \
minex_set_most_recent((struct minex_exception*)setjmp(minex_result->value)); \
if (minex_get_most_recent() == NULL) { C }

#define MINEX_CATCH(T, C) else if (minex_get_most_recent()->type == T) { minex_pop_jmp_buf(); C }
#define MINEX_CATCHALL(C) else { minex_pop_jmp_buf(); C }
#define MINEX_THROW(T, P) longjmp(*minex_peek_jmp_buf(), (uintptr_t)minex_exception_new(T, (void*)P))

#endif

#ifdef MINEX_DEFINE_IMPLEMENTATION

static struct minex_jmp_buf_list* minex_l = NULL;
static struct minex_exception* minex_most_recent;

struct minex_exception* minex_exception_new(enum exception_type type, void* payload)
{
    struct minex_exception* result = (struct minex_exception*)calloc(1, sizeof(*result));

    if (result == NULL)
    {
        return NULL;
    }

    result->type = type;
    result->payload = payload;
    return result;
}

void minex_free_exception(struct minex_exception* i, bool free_payload)
{
    if (free_payload && i->payload != NULL)
    {
        free(i->payload);
    }

    free(i);
}

void minex_push_jmp_buf(struct minex_jmp_buf_list* elem)
{
    elem->prev = minex_l;
    minex_l = elem;
}

jmp_buf* minex_peek_jmp_buf()
{
    return minex_l->value;
}

void minex_pop_jmp_buf()
{
    struct minex_jmp_buf_list* prev = minex_l->prev;
    free(minex_l);
    minex_l = prev;
}

struct minex_exception* minex_get_most_recent()
{
    return minex_most_recent;
}

struct minex_exception* minex_set_most_recent(struct minex_exception* e)
{
    minex_most_recent = e;
}

#endif
