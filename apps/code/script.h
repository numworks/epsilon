#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <ion.h>

namespace Code {

/* Record  : | Total Size |  Name |             Body                |
 * Script:                        | AutoImportationStatus | Content |*/

class Script : public Ion::Storage::Record {
public:
  Script(Ion::Storage::Record r);

  bool importationStatus() const;
  void toggleImportationStatus();

  const char * readContent() const;
  ErrorStatus writeContent(const char * data, size_t size);

  constexpr static size_t k_importationStatusSize = 1;
};

}

#endif
