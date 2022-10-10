#ifndef ION_RECORD_H
#define ION_RECORD_H

#include <stdint.h>
#include <string.h>

namespace Ion {

namespace Storage {

constexpr static char eqExtension[] = "eq";
constexpr static char expExtension[] = "exp";
constexpr static char funcExtension[] = "func";
constexpr static char lisExtension[] = "lis";
constexpr static char seqExtension[] = "seq";
constexpr static char matExtension[] = "mat";

/*  * A record's fullName is baseName.extension.
 * A Record is identified by the CRC32 on its fullName because:
 * - A record is identified by its fullName, which is unique
 * - We cannot keep the address pointing to the fullName because if another
 *   record is modified, it might alter our record's fullName address.
 *   Keeping a buffer with the fullNames will waste memory as we cannot
 *   forsee the size of the fullNames. */
class Record {
public:
  constexpr static char k_dotChar = '.';
  enum class ErrorStatus {
    None = 0,
    NameTaken = 1,
    NonCompliantName = 2,
    NotEnoughSpaceAvailable = 3,
    RecordDoesNotExist = 4,
    CanceledByDelegate = 5,
  };
  struct Data {
    const void * buffer;
    size_t size;
  };
  /* Extension is null terminated,
   * but baseName is not always, so its length is stored.
   */
  struct Name {
    const char * baseName;
    size_t baseNameLength;
    const char * extension;
  };
  Record(Name name);
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
  Name name() const;
  const char * fullName() const;
  Data value() const;
  ErrorStatus setValue(Data data);
  void destroy();
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
  static Record::ErrorStatus SetBaseNameWithExtension(Record * record, const char * baseName, const char * extension);
  static Record::ErrorStatus SetFullName(Record * record, const char * fullName);

private:
  uint32_t m_fullNameCRC32;
};

}

}

#endif
