#include "cap_drop.h"

#include <stdlib.h>
#include <sys/capability.h>


int cap_drop(const cap_value_t *cap_keep, int count) {
    // cap_init initialises capability set with all capabilities unset/disabled.
    cap_t capabilities = cap_init();

    if(capabilities == NULL) {
        return -1;
    }

    if(cap_keep != NULL && count > 0) {
        if(
            cap_set_flag(capabilities, CAP_PERMITTED, count, cap_keep, CAP_SET) != 0 ||
            cap_set_flag(capabilities, CAP_EFFECTIVE, count, cap_keep, CAP_SET) != 0
            // we don't need CAP_INHERITABLE set.
        ) {
            cap_free(capabilities);
            return -1;
        }
    }

    int res = cap_set_proc(capabilities);

    cap_free(capabilities);

    return res;
}
