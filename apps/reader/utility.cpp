#include "utility.h"

#include <string.h>

#ifndef DEVICE
#include <stdio.h>
#include <dirent.h> 
#include <sys/stat.h>
#endif 

namespace Reader
{

bool stringEndsWith(const char* str, const char* pattern) {
  int strLength = strlen(str);
  int patternLength = strlen(pattern);
  if (patternLength > strLength) {
    return false;
  }

  const char* strIter = str + strlen(str);
  const char* patternIter = pattern + strlen(pattern);

  while(*strIter == *patternIter)   {
    if(patternIter == pattern) {
      return true;
    }
    strIter--;
    patternIter--;
  }
  return false;
}

void stringNCopy(char* dest, int max, const char* src, int len) {
  while(len>0 && max >1 && *src)
  {
    *dest = *src;
    dest++;
    src++;
    len--;
    max--;
  }
  *dest=0;
}


#ifdef DEVICE

int filesWithExtension(const char* extension, External::Archive::File* files, int filesSize) {
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

static void fillFileData(External::Archive::File& file) {  
  file.data = nullptr;
  file.dataLength = 0;   

  struct stat info;
  if (stat(file.name, &info) != 0) {
    return;
  }   
  
  unsigned char* content = new unsigned char[info.st_size];
  if (content == NULL) {
    return;
  }   
  FILE *fp = fopen(file.name, "rb");
  if (fp == NULL) {
    return ;
  }
  
  fread(content, info.st_size, 1, fp);    
  fclose(fp);
  file.data = content;
  file.dataLength = info.st_size;      
}

int filesWithExtension(const char* extension, External::Archive::File* files, int filesSize) {
  dirent *file;
  DIR *d = opendir(".");
  int nb = 0;
  if (d) {
    while ((file = readdir(d)) != NULL) {
      if(stringEndsWith(file->d_name, extension)) {
        files[nb].name = strdup(file->d_name);//will probably leak
        fillFileData(files[nb]);
        nb++;
        if(nb == filesSize) {
          break;
        }
      }
    }
    closedir(d);
  }
  return nb;
}
#endif

const char * EndOfPrintableWord(const char * word, const char * end) {
  if (word == end) {
    return word;
  }
  UTF8Decoder decoder(word);
  CodePoint codePoint = decoder.nextCodePoint();
  const char * result = word;
  while (codePoint != '\n' && codePoint != ' ' && codePoint != '%' && codePoint != '$' && codePoint != '\\') {
    result = decoder.stringPosition();
    if (result >= end) {
      break;
    }
    codePoint = decoder.nextCodePoint();
  }
  return result;
}

const char * StartOfPrintableWord(const char * word, const char * start) {
  if (word == start) {
    return word;
  }
  UTF8Decoder decoder(start, word);
  CodePoint codePoint = decoder.previousCodePoint();
  const char * result = word;
  while (codePoint != '\n' && codePoint != ' ' && codePoint != '%' && codePoint != '$') {
    result = decoder.stringPosition();
    if (result <= start) {
      break;
    }
    codePoint = decoder.previousCodePoint();
  }
  return result;
}

}