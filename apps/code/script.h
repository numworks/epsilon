#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <escher.h>

namespace Code {

/* File  : | Total Size | Type | Name |             Body                |
 * Script:                            | AutoImportationStatus | Content |*/

class Script : public File {
friend class ScriptStore;
public:
  Script(File f);

  bool importationStatus() const;
  void toggleImportationStatus();

  const char * readContent() const;
  ErrorStatus writeContent(const char * data, size_t size);

private:
  constexpr static size_t k_importationStatusSize = 1;
};

}

#endif
