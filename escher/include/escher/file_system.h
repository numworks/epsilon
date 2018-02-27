#ifndef ESCHER_FILE_SYSTEM_H
#define ESCHER_FILE_SYSTEM_H

/* FileSystem : | Magic |                   File1                   |                  File2                    | ... | Magic |
 *              | Magic | Size1 | Type1 | Name1 | BodySize1 | Body1 | Size2 | Type2 | Name2 | BodySize2 | Body2 | ... | Magic */

class FileSystem {
public:
  FileSystem();
  static FileSystem * sharedFileSystem();
  int numberOfFileOfType(File::Type type);
  File fileOfTypeAtIndex(File::Type type, int index);
  File getFile(File::Type type, const char * name);

  File addFile(const char * name, File::Type type, const char * content);

  // availableSize takes into account the the size of the last File must be 0.
  size_t availableSize();

  bool isNameTaken(const char * name, File::Type type);
  bool moveNextFile(char * start, int delta);
  size_t sizeOfFileWithBody(const char * body) const;
private:
  // lastUsedData takes into account the the size of the last File must be 0.
  char * lastUsedData();
  size_t * sizeAddressOfFileStarting(char * start) const;
  size_t sizeOfFileStarting(char * start) const;
  File::Type typeOfFileStarting(char * start) const;
  char * nameOfFileStarting(char * start);
  char * bodyOfFileStarting(char * start);
  constexpr static size_t k_totalSize = 4096;
  constexpr static uint32_t Magic = 0xDECA0DF0;
  uint32_t m_dataHeader;
  char m_data[k_totalSize];
  uint32_t m_dataFooter;
};

class FileSystemInfo {
public:
  constexpr FileSystemInfo(uint32_t address) :
    m_header(Magic),
    m_address(address),
    m_footer(Magic) { }
private:
  constexpr static uint32_t Magic = 0xDECB0DF0;
  uint32_t m_header;
  uint32_t m_address;
  uint32_t m_footer;
};

#endif

