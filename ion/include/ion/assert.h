#ifndef ION_ASSERT_H
#define ION_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

void assertionAbort(const char * expression, const char * file, int line);

#ifdef __cplusplus
}
#endif

#endif
