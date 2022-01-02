#include "minex.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void some_other_method(bool p)
{
    if (p)
    {
        MINEX_THROW(MINEX_EXCEPTION_TYPE_EXAMPLE_FILE_NOT_FOUND, NULL);
    }
    else
    {
        printf("No ex\n");
    }
}

void some_method(int p)
{
    if (p >= 10)
    {
        MINEX_TRY({
            some_other_method((p & 1) == 1);
            printf("YOOO!\n");
        })
        MINEX_CATCHALL({
            printf("Exception in some_other_method\n");
            MINEX_THROW(MINEX_EXCEPTION_TYPE_EXAMPLE_RUNTIME_ERROR, NULL);
        })
    }
    else
    {
        const char* msg = "Network crashed";
        size_t length = strlen(msg);
        char* payload = calloc(length, sizeof(*payload) * (length + 1));

        if (payload == NULL) {
            return;
        }

        memcpy(payload, msg, length);
        payload[length] = '\0';

        MINEX_THROW(MINEX_EXCEPTION_TYPE_EXAMPLE_NETWORK_ERROR, payload);
    }
}

int main(void)
{
    MINEX_TRY({
        some_method(1);
    })
    MINEX_CATCH(MINEX_EXCEPTION_TYPE_EXAMPLE_FILE_NOT_FOUND, {
        printf("Oh my God! We did not find the file\n");
        minex_free_exception(minex_get_most_recent(), true); 
    })
    MINEX_CATCH(MINEX_EXCEPTION_TYPE_EXAMPLE_NETWORK_ERROR, {
        printf("Random network error %s\n", (char*)minex_get_most_recent()->payload);
        minex_free_exception(minex_get_most_recent(), true);
    })
    MINEX_CATCHALL({
        printf("Exception occured!");
        minex_free_exception(minex_get_most_recent(), true);
    })
}
