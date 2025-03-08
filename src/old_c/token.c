#include "token.h"

mcl_priority_t mcl_token_get_priority(mcl_token_kind_t kind) {
    mcl_priority_t priority = {.left=0, .right=0};
    switch (kind) {
    case MCL_TOKEN_PLUS:
        priority = (mcl_priority_t){.left=1, .right=2};
    }
    return priority;
}
