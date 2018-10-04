#ifndef ION_STORAGE_H
#define ION_STORAGE_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {

/* Storage : | Magic |             Record1                 |            Record2                  | ... | Magic |
 *           | Magic | Size1(uint16_t) | FullName1 | Body1 | Size2(uint16_t) | FullName2 | Body2 | ... | Magic |
 *
 * A record's fullName is baseName.extension. */

class Storage {
public:
  typedef uint16_t record_size_t;
  constexpr static size_t k_storageSize = 16384;
  static Storage * sharedStorage();
  constexpr static char k_dotChar = '.';

  class Record {
    /* A Record is identified by the CRC32 on its fullName because:
     * - A record is identified by its fullName, which is unique
     * - We cannot keep the address pointing to the fullName because if another
     *   record is modified, it might alter our record's fullName address.
     *   Keeping a buffer with the fullNames will waste memory as we cannot
     *   forsee the size of the fullNames. */
  friend class Storage;
  public:
    enum class ErrorStatus {
      None = 0,
      NameTaken = 1,
      NonCompliantName = 2,
      NotEnoughSpaceAvailable = 3,
      RecordDoesNotExist = 4
    };
    struct Data {
      const void * buffer;
      size_t size;
    };
    Record(const char * fullName = nullptr);
    Record(const char * basename, const char * extension);
    bool operator==(const Record & other) const {
      return m_fullNameCRC32 == other.m_fullNameCRC32;
    }
    bool operator!=(const Record & other) const {
      return !(*this == other);
    }
    bool isNull() const {
      return m_fullNameCRC32 == 0;
    }
    const char * fullName() const {
      return Storage::sharedStorage()->fullNameOfRecord(*this);
    }
    ErrorStatus setName(const char * fullName) {
      return Storage::sharedStorage()->setFullNameOfRecord(*this, fullName);
    }
    Data value() const {
      return Storage::sharedStorage()->valueOfRecord(*this);
    }
    ErrorStatus setValue(Data data) {
      return Storage::sharedStorage()->setValueOfRecord(*this, data);
    }
    void destroy() {
      return Storage::sharedStorage()->destroyRecord(*this);
    }
  private:
    Record(const char * basename, int basenameLength, const char * extension, int extensionLength);
    uint32_t m_fullNameCRC32;
  };

  Storage();
  size_t availableSize();
  uint32_t checksum();
  int numberOfRecordsWithExtension(const char * extension);

  // Record creation
  Record::ErrorStatus createRecordWithFullName(const char * fullName, const void * data, size_t size);
  Record::ErrorStatus createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size);

  // Record getters
  Record recordWithExtensionAtIndex(const char * extension, int index);
  Record recordNamed(const char * fullName);
  Record recordBaseNamedWithExtension(const char * baseName, const char * extension);
  Record recordBaseNamedWithExtensions(const char * baseName, const char * extension[], size_t numberOfExtensions);

  // Record destruction
  void destroyRecordsWithExtension(const char * extension);
private:
  constexpr static uint32_t Magic = 0xEE0BDDBA;
  constexpr static size_t k_maxRecordSize = (1 << sizeof(record_size_t)*8);

  /* Getters/Setters on recordID */
  const char * fullNameOfRecord(const Record record);
  Record::ErrorStatus setFullNameOfRecord(const Record record, const char * fullName);
  Record::ErrorStatus setBaseNameWithExtensionOfRecord(const Record record, const char * baseName, const char * extension);
  Record::Data valueOfRecord(const Record record);
  Record::ErrorStatus setValueOfRecord(const Record record, Record::Data data);
  void destroyRecord(const Record record);

  /* Getters on address in buffer */
  record_size_t sizeOfRecordStarting(char * start) const;
  const char * fullNameOfRecordStarting(char * start) const;
  const void * valueOfRecordStarting(char * start) const;

  /* Overriders */
  size_t overrideSizeAtPosition(char * position, record_size_t size);
  size_t overrideFullNameAtPosition(char * position, const char * fullName);
  size_t overrideBaseNameWithExtensionAtPosition(char * position, const char * baseName, const char * extension);
  size_t overrideValueAtPosition(char * position, const void * data, record_size_t size);

  bool isFullNameTaken(const char * fullName, const Record * recordToExclude = nullptr);
  bool isBaseNameWithExtensionTaken(const char * baseName, const char * extension, Record * recordToExclude = nullptr);
  bool isNameOfRecordTaken(Record r, const Record * recordToExclude);
  static bool FullNameCompliant(const char * name);
  static bool FullNameHasExtension(const char * fullName, const char * extension, size_t extensionLength);
  char * endBuffer();
  size_t sizeOfBaseNameAndExtension(const char * baseName, const char * extension) const;
  size_t sizeOfRecordWithBaseNameAndExtension(const char * baseName, const char * extension, size_t size) const;
  size_t sizeOfRecordWithFullName(const char * fullName, size_t size) const;
  bool slideBuffer(char * position, int delta);
  class RecordIterator {
  public:
    RecordIterator(char * start) : m_recordStart(start) {}
    char * operator*() { return m_recordStart; }
    RecordIterator& operator++();
    bool operator!=(const RecordIterator& it) const { return m_recordStart != it.m_recordStart; }
  private:
    char * m_recordStart;
  };
  RecordIterator begin() const {
    if (sizeOfRecordStarting((char *)m_buffer) == 0) {
      return nullptr;
    }
    return RecordIterator((char *)m_buffer);
  };
  RecordIterator end() const { return RecordIterator(nullptr); };

  uint32_t m_magicHeader;
  char m_buffer[k_storageSize];
  uint32_t m_magicFooter;
};

}

#endif
