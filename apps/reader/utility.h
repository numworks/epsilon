#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <apps/external/archive.h>

namespace reader
{

bool stringEndsWith(const char* str, const char* end);
int filesWithExtension(const char* extension, External::Archive::File* files, int filesSize) ;

}
#endif