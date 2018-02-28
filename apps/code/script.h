#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <ion.h>

namespace Code {

/* Record  : | Total Size | Type | Name |             Body                |
 * Script:                              | AutoImportationStatus | Content |*/

class Script : public Ion::Record {
friend class ScriptStore;
public:
  Script(Ion::Record f);

  bool importationStatus() const;
  void toggleImportationStatus();

  const char * readContent() const;
  ErrorStatus writeContent(const char * data, size_t size);

private:
  constexpr static size_t k_importationStatusSize = 1;
};

}

#endif
