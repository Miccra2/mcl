#include "token.h"

const char *TOKEN_KIND[] = {
    "TOKEN_UNDEFINED",
    "TOKEN_INTEGER",
    "TOKEN_PLUS",
    "TOKEN_MINUS",
    "TOKEN_SEMICOLON",
    "TOKEN_COUNT",
};

priority_t get_priority(token_kind_t kind) {
    switch (kind) {
    case TOKEN_PLUS:
        return new_priority (2, 1);
    default:
        return new_priority (0, 0);
    }
}
