#ifndef ION_STORAGE_H
#define ION_STORAGE_H

#include <stddef.h>

namespace Ion {

class Storage;

}

extern Ion::Storage storage;

namespace Ion {

/* Storage : | Magic |             Record1             |            Record2              | ... | Magic |
 *           | Magic | Size1(uint16_t) | Name1 | Body1 | Size2(uint16_t) | Name2 | Body2 | ... | Magic */

class Storage {
public:
  class Record {
    /* A Record is identified by the CRC32 on his name because:
     * - a record is identified by its name which is unique
     * - we cannot keep the address pointing to the name because if another
     *   record is modified, it might alter our record name address and keeping
     *   a buffer with the name will waste memory as we cannot forsee the size
     *   of the name. */
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
    Record(const char * name = nullptr);
    bool operator==(const Record & other) const {
      return m_nameCRC32 == other.m_nameCRC32;
    }
    bool isNull() const {
      return m_nameCRC32 == 0;
    }
    const char * name() const {
      return storage.nameOfRecord(*this);
    }
    ErrorStatus setName(const char * name) {
      return storage.setNameOfRecord(*this, name);
    }
    Data value() const {
      return storage.valueOfRecord(*this);
    }
    ErrorStatus setValue(Data data) {
      return storage.setValueOfRecord(*this, data);
    }
    void destroy() {
      return storage.destroyRecord(*this);
    }
  private:
    uint32_t m_nameCRC32;
  };
  Storage();
  size_t availableSize();
  Record::ErrorStatus createRecord(const char * name, const void * data, size_t size);
  int numberOfRecordsWithExtension(const char * extension);
  Record recordWithExtensionAtIndex(const char * extension, int index);
  Record recordNamed(const char * name);
  typedef uint16_t record_size_t;
private:
  constexpr static uint32_t Magic = 0xEE0BDDBA;
  constexpr static size_t k_storageSize = 4096;
  constexpr static size_t k_maxRecordSize = (1 << sizeof(record_size_t)*8);

  /* Getters/Setters on recordID */
  const char * nameOfRecord(const Record record);
  Record::ErrorStatus setNameOfRecord(const Record record, const char * name);
  Record::Data valueOfRecord(const Record record);
  Record::ErrorStatus setValueOfRecord(const Record record, Record::Data data);
  void destroyRecord(const Record record);

  /* Getters on address in buffer */
  record_size_t sizeOfRecordStarting(char * start) const;
  const char * nameOfRecordStarting(char * start) const;
  const void * valueOfRecordStarting(char * start) const;

  /* Overriders */
  size_t overrideSizeAtPosition(char * position, record_size_t size);
  size_t overrideNameAtPosition(char * position, const char * name);
  size_t overrideValueAtPosition(char * position, const void * data, record_size_t size);

  bool isNameTaken(const char * name, Record * recordToExclude = nullptr);
  bool nameCompliant(const char * name) const;
  char * endBuffer();
  size_t sizeOfRecord(const char * name, size_t size) const;
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
