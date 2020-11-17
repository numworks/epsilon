#include "utility.h"

#include <string.h>

#ifndef DEVICE
#include <stdio.h>
#include <dirent.h> 
#include <sys/stat.h>
#endif 

namespace reader
{

bool stringEndsWith(const char* str, const char* pattern)
{
    int strLength = strlen(str);
    int patternLength = strlen(pattern);
    if (patternLength > strLength)
        return false;

    const char* strIter = str + strlen(str);
    const char* patternIter = pattern + strlen(pattern);

    while(*strIter == *patternIter)
    {
        if(patternIter == pattern)
            return true;
        strIter--;
        patternIter--;
    }
    return false;
}


#ifdef DEVICE

int filesWithExtension(const char* extension, External::Archive::File* files, int filesSize) 
{
    size_t nbTotalFiles = External::Archive::numberOfFiles();
    int nbFiles = 0;
    for(size_t i=0; i < nbTotalFiles; ++i)
    {
        External::Archive::File file;
        External::Archive::fileAtIndex(i, file);
        if(stringEndsWith(file.name, ".txt"))
        {
            files[nbFiles] = file;
            nbFiles++;
            if(nbFiles == filesSize)
                break;
        }
    }
    return nbFiles;
}
#else

int filesWithExtension(const char* extension, External::Archive::File* files, int filesSize) 
{
  dirent *file;
  DIR *d = opendir(".");
  int nb = 0;
  if (d) 
  {
    while ((file = readdir(d)) != NULL) 
    {
      if(stringEndsWith(dir->d_name, extension))
      {
        files[nb].name = strdup(file->d_name);//will probably leak
        nb++;
        if(nb == filesSize)
            break;
      }
    }
    closedir(d);
  }
  return nb;
}

#endif

}