#ifndef MICROVISION_ASSERT_H
#define MICROVISION_ASSERT_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT_OR_RETURN(condition, retval) do {           \
    if (!(condition)) {                                    \
        fprintf(stderr, "Assertion failed: %s\n", #condition); \
        return retval;                                     \
    }                                                      \
} while (0)

#ifdef __cplusplus
}
#endif

#endif // MICROVISION_ASSERT_H
