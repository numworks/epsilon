#include <escher/file.h>
#include <escher/file_system.h>
#include <string.h>
#include <assert.h>

FileSystem::FileSystem() :
  m_dataHeader(Magic),
  m_data(),
  m_dataFooter(Magic)
{
  size_t * p = (size_t *)m_data;
  p[0] = 0;
}

FileSystem * FileSystem::sharedFileSystem() {
  static FileSystem f;
  return &f;
}

int FileSystem::numberOfFileOfType(File::Type type) {
  assert(m_dataHeader == Magic);
  assert(m_dataFooter == Magic);
  int count = 0;
  char * currentPointer = m_data;
  size_t size = sizeOfFileStarting(currentPointer);
  while (size != 0 && currentPointer < m_data + k_totalSize) {
    if (typeOfFileStarting(currentPointer) == type) {
      count++;
    }
    currentPointer += size;
    size = sizeOfFileStarting(currentPointer);
  }
  return count;
}

File FileSystem::fileOfTypeAtIndex(File::Type type, int index) {
  int currentIndex = -1;
  char * currentPointer = m_data;
  size_t size = sizeOfFileStarting(currentPointer);
  while (size != 0 && currentPointer < m_data + k_totalSize) {
    if (typeOfFileStarting(currentPointer) == type) {
      currentIndex++;
      if (currentIndex == index) {
        break;
      }
    }
    currentPointer += size;
    size = sizeOfFileStarting(currentPointer);
  }
  return File(sizeAddressOfFileStarting(currentPointer), nameOfFileStarting(currentPointer), type, bodyOfFileStarting(currentPointer));
}

File FileSystem::getFile(File::Type type, const char * name) {
  for (int i = 0; i < numberOfFileOfType(type); i++) {
    File currentFile = fileOfTypeAtIndex(type, i);
    if (strcmp(currentFile.name(), name) == 0) {
      return currentFile;
    }
  }
  return File();
}

File FileSystem::addFile(const char * name, File::Type type, const char * body) {
  // assert name is short enough and there is enough space to add the file
  assert(strlen(name) < File::k_nameSize);
  assert(availableSize() >= sizeOfFileWithBody(body));
  // Find the end of data
  char * currentPointer = m_data;
  size_t size = sizeOfFileStarting(currentPointer);
  while (size != 0 && currentPointer < m_data + k_totalSize) {
    currentPointer += size;
    size = sizeOfFileStarting(currentPointer);
  }
  size_t fileSize = sizeOfFileWithBody(body);
  // Fill totalSize
  *((size_t *)currentPointer) = fileSize;
  // Fill type
  *(currentPointer+File::k_sizeSize) = (uint8_t)type;
  // Fill name
  strlcpy(currentPointer+File::k_sizeSize+File::k_typeSize, name, File::k_nameSize);
  // Fill body
  strlcpy(currentPointer+File::k_sizeSize+File::k_typeSize+File::k_nameSize, body, strlen(body)+1);
  char * nextPointer = currentPointer + fileSize;
  *((size_t *)nextPointer) = 0;
  return File(sizeAddressOfFileStarting(currentPointer), nameOfFileStarting(currentPointer), type, bodyOfFileStarting(currentPointer));
}

char * FileSystem::lastUsedData() {
  size_t usedSize = 0;
  char * currentPointer = m_data;
  size_t size = sizeOfFileStarting(currentPointer);
  while (size != 0  && currentPointer < m_data + k_totalSize) {
    usedSize += size;
    currentPointer += size;
    size = sizeOfFileStarting(currentPointer);
  }
  return currentPointer + File::k_sizeSize;
}


size_t FileSystem::availableSize() {
  return k_totalSize-(lastUsedData()-m_data);
}

bool FileSystem::isNameTaken(const char * name, File::Type type) {
  char * currentPointer = m_data;
  size_t size = sizeOfFileStarting(currentPointer);
  while (size != 0 && currentPointer < m_data + k_totalSize) {
    if (typeOfFileStarting(currentPointer) == type && strcmp(nameOfFileStarting(currentPointer), name) == 0) {
      return true;
    }
    currentPointer += size;
    size = sizeOfFileStarting(currentPointer);
  }
  return false;
}

bool FileSystem::moveNextFile(char * start, int delta) {
  if (delta > (int)availableSize()) {
    return false;
  }
  char * nextFile = start + sizeOfFileStarting(start);
  memmove(nextFile+delta, nextFile, lastUsedData()-nextFile);
  return true;
}

size_t * FileSystem::sizeAddressOfFileStarting(char * start) const {
  return (size_t *)start;
}

size_t FileSystem::sizeOfFileStarting(char * start) const {
  if (start >= m_data + k_totalSize) {
    return 0;
  }
  return *(sizeAddressOfFileStarting(start));
}

File::Type FileSystem::typeOfFileStarting(char * start) const {
   return (File::Type)*((uint8_t *)start+File::k_sizeSize);
}

char * FileSystem::nameOfFileStarting(char * start) {
  return start+File::k_sizeSize+File::k_typeSize;
}

char * FileSystem::bodyOfFileStarting(char * start) {
  return start+File::k_sizeSize+File::k_typeSize+File::k_nameSize;
}

size_t FileSystem::sizeOfFileWithBody(const char * body) const {
  return File::k_sizeSize+File::k_typeSize+File::k_nameSize+strlen(body)+1;
}
