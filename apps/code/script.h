#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <ion.h>

namespace Code {

/* Record  : | Total Size |  Name |             Body                |
 * Script:                        | AutoImportationStatus | Content |*/

class Script : public Ion::Storage::Record {
public:
  static constexpr size_t k_importationStatusSize = 1;
  static constexpr int k_defaultScriptNameMaxSize = 6 + 2 + 1;
  /* k_defaultScriptNameMaxSize is the length of a name between script1 and
   * script99
   * 6 = strlen("script")
   * 2 = maxLength of integers between 1 and 99
   * 1 = null-terminating char */

  static void DefaultName(char buffer[], size_t bufferSize);
  static bool nameCompliant(const char * name);

  Script(Ion::Storage::Record r) : Record(r) {}
  bool importationStatus() const;
  void toggleImportationStatus();
  const char * readContent() const;
};

}

#endif
