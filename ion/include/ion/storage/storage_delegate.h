#ifndef ION_STORAGE_DELEGATE_H
#define ION_STORAGE_DELEGATE_H

#include "record.h"

namespace Ion {

namespace Storage {

/* Some apps memoize records and need to be notified when a record might have
 * become invalid. For instance in the Graph app, if f(x) = A+x, and A changed,
 * f(x) memoization which stores the reduced expression of f is outdated.
 * We could have computed and compared the checksum of the storage to detect
 * storage invalidity, but profiling showed that this slows down the execution
 * (for example when scrolling the functions list).
 * We thus decided to notify a delegate when the storage changes. */
class StorageDelegate {
  friend class FileSystem;
protected:
  /* Return false to cancel change */
  virtual bool storageCanChangeForRecordName(const Record::Name recordName) const { return true; };
  virtual void storageDidChangeForRecord(const Record record) = 0;
  virtual void storageIsFull() = 0;
};

}

}

#endif
