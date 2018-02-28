#ifndef ION_KALLAX_H
#define ION_KALLAX_H

#include <ion/record.h>

/* Kallax : | Magic |                    Record1                    |                Record2                    | ... | Magic |
 *              | Magic | Size1 | Type1 | Name1 | BodySize1 | Body1 | Size2 | Type2 | Name2 | BodySize2 | Body2 | ... | Magic */

class Kallax {
public:
  Kallax();
  static Kallax * sharedKallax();
  int numberOfRecordOfType(Record::Type type);
  Record recordOfTypeAtIndex(Record::Type type, int index);
  Record getRecord(Record::Type type, const char * name);

  Record addRecord(const char * name, Record::Type type, const char * content);

  // availableSize takes into account the the size of the last Record must be 0.
  size_t availableSize();

  bool isNameTaken(const char * name, Record::Type type);
  bool moveNextRecord(char * start, int delta);
  size_t sizeOfRecordWithBody(const char * body) const;
private:
  // lastUsedData takes into account the the size of the last Record must be 0.
  char * lastUsedData();
  size_t * sizeAddressOfRecordStarting(char * start) const;
  size_t sizeOfRecordStarting(char * start) const;
  Record::Type typeOfRecordStarting(char * start) const;
  char * nameOfRecordStarting(char * start);
  char * bodyOfRecordStarting(char * start);
  constexpr static size_t k_totalSize = 4096;
  constexpr static uint32_t Magic = 0xDECA0DF0;
  uint32_t m_dataHeader;
  char m_data[k_totalSize];
  uint32_t m_dataFooter;
};

#endif

