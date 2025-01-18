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
} while (0) // This is a macro, so it needs to end with a semicolon, the do-while(0) idiom is used to make it a single statement

#ifdef __cplusplus
}
#endif

#endif // MICROVISION_ASSERT_H
