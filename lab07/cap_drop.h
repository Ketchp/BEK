#ifndef BEK_CAP_DROP_H
#define BEK_CAP_DROP_H

#include <sys/capability.h>

/**
 * Drops all permitted, effective, inheritable capabilities from process.
 * Sets permitted and effective flag to `count` flags from `cap_keep` array.
 *
 * @param cap_keep Array of capabilities.
 * @param count Number of capabilities in array.
 * @return 0 on success, -1 on error.
 */
int cap_drop(const cap_value_t *cap_keep, int count);

#endif //BEK_CAP_DROP_H
