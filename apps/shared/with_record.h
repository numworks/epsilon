#ifndef WITH_RECORD_H
#define WITH_RECORD_H

#include <ion/storage/record.h>

namespace Shared {

class WithRecord {
 public:
  virtual void setRecord(Ion::Storage::Record record) { m_record = record; }

 protected:
  Ion::Storage::Record m_record;
};

}  // namespace Shared

#endif
