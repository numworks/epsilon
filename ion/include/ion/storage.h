#ifndef ION_STORAGE_H
#define ION_STORAGE_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "unicode/utf8_helper.h"

namespace Ion {

/* Storage : | Magic |             Record1                 |            Record2                  | ... | Magic |
 *           | Magic | Size1(uint16_t) | FullName1 | Body1 | Size2(uint16_t) | FullName2 | Body2 | ... | Magic |
 *
 * A record's fullName is baseName.extension. */

class StorageDelegate;
class RecordNameHelper;

class Storage {
public:
  typedef uint16_t record_size_t;

  constexpr static size_t k_storageSize = 32768;
  static_assert(UINT16_MAX >= k_storageSize, "record_size_t not big enough");

  static Storage * sharedStorage();
  constexpr static char k_dotChar = '.';

  constexpr static char eqExtension[] = "eq";
  constexpr static char expExtension[] = "exp";
  constexpr static char funcExtension[] = "func";
  constexpr static char lisExtension[] = "lis";
  constexpr static char seqExtension[] = "seq";
  constexpr static char matExtension[] = "mat";

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
    /* Extension is null terminated,
     * but baseName is not always, so we store its length.
     */
    struct Name {
      const char * baseName;
      size_t baseNameLength;
      const char * extension;
    };
    Record(const char * fullName = nullptr);
    Record(const char * basename, const char * extension);
    Record(Name name);
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
    Name name() const {
      return Storage::sharedStorage()->nameOfRecord(*this);
    }
    const char * fullName() const {
      // The baseName points towards the start of the fullName
      return Storage::sharedStorage()->nameOfRecord(*this).baseName;
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
    bool hasExtension(const char * extension) {
      const char * thisExtension = name().extension;
      return thisExtension == nullptr ? false : strcmp(thisExtension, extension) == 0;
    }
    // Record::Name
    static Name CreateRecordNameFromFullName(const char * fullName);
    static Name CreateRecordNameFromBaseNameAndExtension(const char * baseName, const char * extension);
    static size_t SizeOfName(Record::Name name);
    static bool NameIsEmpty(Name name);
    static Name EmptyName() {
      return Name({nullptr, 0, nullptr});
    }
    /* This methods are static to prevent any calling these methods on "this",
     * since these methods might modify in-place the record */
    static Record::ErrorStatus SetBaseNameWithExtension(Record * record, const char * baseName, const char * extension) {
      Name name = CreateRecordNameFromBaseNameAndExtension(baseName, extension);
      return Storage::sharedStorage()->setNameOfRecord(record, name);
    }
    static Record::ErrorStatus SetFullName(Record * record, const char * fullName) {
      Name name = CreateRecordNameFromFullName(fullName);
      return Storage::sharedStorage()->setNameOfRecord(record, name);
    }
  private:
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

  // Record name helper
  void setRecordNameHelper(RecordNameHelper * helper) { m_recordNameHelper = helper; }

  // Record counters
  int numberOfRecordsWithExtension(const char * extension) {
    return numberOfRecordsWithFilter(extension, ExtensionOnlyFilter);
  }
  // TODO Hugo : Maybe handle a "hidden" status at Record level instead ?
  int numberOfRecordsStartingWithout(const char nonStartingChar, const char * extension) {
    return numberOfRecordsWithFilter(extension, FirstCharFilter, &nonStartingChar);
  }

  // Record names helper
  int firstAvailableNameFromPrefix(char * buffer, size_t prefixLength, size_t bufferSize, const char * const extensions[], size_t numberOfExtensions, int maxId);

  // Record creation
  Record::ErrorStatus createRecordWithExtension(const char * baseName, const char * extension, const void * data, size_t size);
  Record::ErrorStatus createRecordWithFullNameAndDataChunks(const char * fullName, const void * dataChunks[], size_t sizeChunks[], size_t numberOfChunks);
  Record::ErrorStatus createRecordWithDataChunks(Record::Name recordName, const void * dataChunks[], size_t sizeChunks[], size_t numberOfChunks);

  // Record getters
  bool hasRecord(Record r) { return pointerOfRecord(r) != nullptr; }
  Record recordWithExtensionAtIndex(const char * extension, int index) {
    return recordWithFilterAtIndex(extension, index, ExtensionOnlyFilter);
  }
  Record recordWithExtensionAtIndexStartingWithout(const char nonStartingChar, const char * extension, int index) {
    return recordWithFilterAtIndex(extension, index, FirstCharFilter, &nonStartingChar);
  }
  Record recordNamed(Record::Name name);
  Record recordNamed(const char * fullName) {
    return recordNamed(Record::CreateRecordNameFromFullName(fullName));
  }
  Record recordBaseNamedWithExtension(const char * baseName, const char * extension) {
    return recordNamed(Record::CreateRecordNameFromBaseNameAndExtension(baseName, extension));
  }
  Record recordBaseNamedWithExtensions(const char * baseName, const char * const extensions[], size_t numberOfExtensions);
  const char * extensionOfRecordBaseNamedWithExtensions(const char * baseName, int baseNameLength, const char * const extensions[], size_t numberOfExtensions);

  // Record destruction
  void destroyAllRecords();
  void destroyRecordsWithExtension(const char * extension);

  /* Destroy a record with same baseName and a competing extension
   * Return false if there is a competing record but it can't be destroyed
   * since it has precedence on its base name. (See RecordNameHelper)
   * WARNING : If m_recordNameHelper == nullptr, record won't override
   * themself when replaced with a record with same name and same extension.
   * This in maily relevant for tests, where you have to set the helper by hand.*/
  bool destroyCompetingRecord(Record::Name recordName, Record * excludedRecord = nullptr);

private:
  constexpr static uint32_t Magic = 0xEE0BDDBA;
  constexpr static size_t k_maxRecordSize = (1 << sizeof(record_size_t)*8);

  // Record filter on names
  typedef bool (*RecordFilter)(Record::Name name, const void * auxiliary);
  static bool ExtensionOnlyFilter(Record::Name name, const void * auxiliary) { return true; };
  static bool FirstCharFilter(Record::Name name, const void * auxiliary) {
    assert(auxiliary != nullptr);
    return name.baseName[0] != *static_cast<const char *>(auxiliary);
  };
  // Private record counters and getters
  int numberOfRecordsWithFilter(const char * extension, RecordFilter filter, const void * auxiliary = nullptr);
  Record recordWithFilterAtIndex(const char * extension, int index, RecordFilter filter, const void * auxiliary = nullptr);

  Storage();

  /* Getters/Setters on recordID */
  Record::Name nameOfRecord(const Record record) const;
  Record::ErrorStatus setNameOfRecord(Record * record, Record::Name name);
  Record::Data valueOfRecord(const Record record);
  Record::ErrorStatus setValueOfRecord(const Record record, Record::Data data);
  void destroyRecord(const Record record);

  /* Getters on address in buffer */
  char * pointerOfRecord(const Record record) const;
  record_size_t sizeOfRecordStarting(char * start) const;
  const void * valueOfRecordStarting(char * start) const;
  Record::Name nameOfRecordStarting(char * start) const;

  /* Overriders */
  size_t overrideSizeAtPosition(char * position, record_size_t size);
  size_t overrideNameAtPosition(char * position, Record::Name name);
  size_t overrideValueAtPosition(char * position, const void * data, record_size_t size);

  bool isNameTaken(Record::Name name, const Record * recordToExclude = nullptr);
  bool isNameOfRecordTaken(Record r, const Record * recordToExclude);
  char * endBuffer();
  size_t sizeOfRecordWithName(Record::Name name, size_t dataSize);
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
  RecordNameHelper * m_recordNameHelper;
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

/* This helper is used by the storage to know if it can override a record with
 * a new one. It is pure virtual so that it is defined in Apps and not in Ion.
 * It contains only "static" functions.
 * See Apps::Shared::RecordNameHelper */
class RecordNameHelper {
public:
  enum class OverrideStatus {
    Forbidden = 0,
    Allowed,
    CanCoexist
  };
  virtual const char * const * competingExtensions() = 0;
  virtual int numberOfCompetingExtensions() = 0;
  virtual OverrideStatus shouldRecordBeOverridenWithNewExtension(Storage::Record previousRecord, const char * newExtension) = 0;
  virtual bool competingExtensionsOverrideThemselves() = 0;
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
