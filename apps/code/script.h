#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <ion.h>

namespace Code {

/* Record  : | Total Size |  Name |             Body                |
 * Script:                        | AutoImportationStatus | Content |*/

class Script : public Ion::Storage::Record {
private:
  // Default script names are chosen between script1 and script99
  static constexpr int k_maxNumberOfDefaultScriptNames = 99;
  static constexpr int k_defaultScriptNameNumberMaxSize = 2; // Numbers from 1 to 99 have 2 digits max
public:
  static constexpr size_t k_importationStatusSize = 1;
  static constexpr int k_defaultScriptNameMaxSize = 6 + k_defaultScriptNameNumberMaxSize + 1;
  /* 6 = strlen("script")
   * k_defaultScriptNameNumberMaxSize = maxLength of integers between 1 and 99
   * 1 = null-terminating char */

  static bool DefaultName(char buffer[], size_t bufferSize);
  static bool nameCompliant(const char * name);

  Script(Ion::Storage::Record r) : Record(r) {}
  bool importationStatus() const;
  void toggleImportationStatus();
  const char * scriptContent() const;
};

}

#endif
