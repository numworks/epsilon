#ifndef ION_STORAGE_H
#define ION_STORAGE_H

#include <stddef.h>
#include <stdint.h>

namespace Ion {

/* Storage : | Magic |             Record1                 |            Record2                  | ... | Magic |
 *           | Magic | Size1(uint16_t) | FullName1 | Body1 | Size2(uint16_t) | FullName2 | Body2 | ... | Magic |
 *
 * A record's fullName is baseName.extension. */

class StorageDelegate;

class Storage {
public:
  typedef uint16_t record_size_t;

  constexpr static size_t k_storageSize = 32768;
  static_assert(UINT16_MAX >= k_storageSize, "record_size_t not big enough");

  static Storage * sharedStorage();
  constexpr static char k_dotChar = '.';

  static constexpr char eqExtension[] = "eq";
  static constexpr char expExtension[] = "exp";
  static constexpr char funcExtension[] = "func";
  static constexpr char seqExtension[] = "seq";

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
#if ION_STORAGE_LOG
    void log();
#endif
    uint32_t checksum();
    bool isNull() const {
      return m_fullNameCRC32 == 0;
    }
    const char * fullName() const {
      return Storage::sharedStorage()->fullNameOfRecord(*this);
    }
    ErrorStatus setBaseNameWithExtension(const char * baseName, const char * extension) {
      return Storage::sharedStorage()->setBaseNameWithExtensionOfRecord(*this, baseName, extension);
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

#if ION_STORAGE_LOG
  void log();
#endif

  size_t availableSize();
  size_t putAvailableSpaceAtEndOfRecord(Record r);
  void getAvailableSpaceFromEndOfRecord(Record r, size_t recordAvailableSpace);
  uint32_t checksum();

  // Delegate
  void setDelegate(StorageDelegate * delegate) { m_delegate = delegate; }
  void notifyChangeToDelegate(const Record r = Record()) const;
  Record::ErrorStatus notifyFullnessToDelegate() const;

  int numberOfRecordsWithExtension(const char * extension);
  static bool FullNameHasExtension(const char * fullName, const char * extension, size_t extensionLength);

  // Record creation
  Record::ErrorStatus createRecordWithFullName(const char * fullName, const void * data, size_t size);
  Record::ErrorStatus createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size);

  // Record getters
  bool hasRecord(Record r) { return pointerOfRecord(r) != nullptr; }
  Record recordWithExtensionAtIndex(const char * extension, int index);
  Record recordNamed(const char * fullName);
  Record recordBaseNamedWithExtension(const char * baseName, const char * extension);
  Record recordBaseNamedWithExtensions(const char * baseName, const char * const extension[], size_t numberOfExtensions);
  const char * extensionOfRecordBaseNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extension[], size_t numberOfExtensions);

  // Record destruction
  void destroyAllRecords();
  void destroyRecordWithBaseNameAndExtension(const char * baseName, const char * extension);
  void destroyRecordsWithExtension(const char * extension);

private:
  constexpr static uint32_t Magic = 0xEE0BDDBA;
  constexpr static size_t k_maxRecordSize = (1 << sizeof(record_size_t)*8);

  Storage();

  /* Getters/Setters on recordID */
  const char * fullNameOfRecord(const Record record);
  Record::ErrorStatus setFullNameOfRecord(const Record record, const char * fullName);
  Record::ErrorStatus setBaseNameWithExtensionOfRecord(const Record record, const char * baseName, const char * extension);
  Record::Data valueOfRecord(const Record record);
  Record::ErrorStatus setValueOfRecord(const Record record, Record::Data data);
  void destroyRecord(const Record record);

  /* Getters on address in buffer */
  char * pointerOfRecord(const Record record) const;
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
  RecordIterator end() const { return RecordIterator(nullptr); }

  Record privateRecordAndExtensionOfRecordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions, const char * * extensionResult = nullptr, int baseNameLength = -1);

  uint32_t m_magicHeader;
  char m_buffer[k_storageSize];
  uint32_t m_magicFooter;
  StorageDelegate * m_delegate;
  mutable Record m_lastRecordRetrieved;
  mutable char * m_lastRecordRetrievedPointer;
};

/* Some apps memoize records and need to be notified when a record might have
 * become invalid. For instance in the Graph app, if f(x) = A+x, and A changed,
 * f(x) memoization which stores the reduced expression of f is outdated.
 * We could have computed and compared the checksum of the storage to detect
 * storage invalidity, but profiling showed that this slows down the execution
 * (for example when scrolling the functions list).
 * We thus decided to notify a delegate when the storage changes. */

class StorageDelegate {
public:
  virtual void storageDidChangeForRecord(const Storage::Record record) = 0;
  virtual void storageIsFull() = 0;
};

// emscripten read and writes must be aligned.
class StorageHelper {
public:
  static uint16_t unalignedShort(char * address) {
#if __EMSCRIPTEN__
    uint8_t f1 = *(address);
    uint8_t f2 = *(address+1);
    uint16_t f = (uint16_t)f1 + (((uint16_t)f2)<<8);
    return f;
#else
    return *(uint16_t *)address;
#endif
  }
  static void writeUnalignedShort(uint16_t value, char * address) {
#if __EMSCRIPTEN__
    *((uint8_t *)address) = (uint8_t)(value & ((1 << 8) - 1));
    *((uint8_t *)address+1) = (uint8_t)(value >> 8);
#else
    *((uint16_t *)address) = value;
#endif
  }
};

}

#endif
