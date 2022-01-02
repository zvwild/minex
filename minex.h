// Minex - Minimal exceptions for C
// Refer to the example for more usage info

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
    } type;

    // Pointer to heap allocated memory which contains the payload (can be NULL)
    void* payload;
};

struct minex_jmp_buf_list
{
    struct minex_jmp_buf_list* prev;
    jmp_buf value;
};

static struct minex_jmp_buf_list* minex_l = NULL;
static struct minex_exception* minex_most_recent;

struct minex_exception* minex_exception_new(enum exception_type type, void* payload);
void minex_free_exception(struct minex_exception* i, bool free_payload);

#define MINEX_POP_L { struct minex_jmp_buf_list *p = minex_l->prev; free(minex_l); minex_l = p; }

#define MINEX_TRY(C) struct minex_jmp_buf_list* minex_result = (struct minex_jmp_buf_list*)calloc(1, sizeof(*minex_result)); \
minex_result->prev = minex_l; \
minex_l = minex_result; \
if ((minex_most_recent = (struct minex_exception*)setjmp(minex_l->value)) == 0) { C }

#define MINEX_CATCH(T, C) else if (minex_most_recent->type == T) { MINEX_POP_L C }
#define MINEX_CATCHALL(C) else { MINEX_POP_L C }
#define MINEX_THROW(T, P) longjmp(minex_l->value, (uintptr_t)minex_exception_new(T, (void*)P))

#endif

#ifdef MINEX_DEFINE_IMPLEMENTATION

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

#endif
