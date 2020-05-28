#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <ion.h>

namespace Code {

/* Record: | Size |  Name |             Body                                                  |
 * Script: |      |       | AutoImportationStatus | ContentFetchedFromConsoleStatus | Content |
 *
 * AutoImportationStatus is 1 if the script should be auto imported when the
 * console opens.
 *
 * ContentFetchedFromConsoleStatus is 1 if hte console has currently imported
 * this script. This is used to import the right variables in the variable box. */

class Script : public Ion::Storage::Record {
private:
  // Default script names are chosen between script1 and script99
  static constexpr int k_maxNumberOfDefaultScriptNames = 99;
  static constexpr int k_defaultScriptNameNumberMaxSize = 2; // Numbers from 1 to 99 have 2 digits max

  static constexpr size_t k_autoImportationStatusSize = 1; // TODO use only 1 byte for both status flags
  static constexpr size_t k_currentImportationStatusSize = 1;
public:
  static constexpr int k_defaultScriptNameMaxSize = 6 + k_defaultScriptNameNumberMaxSize + 1;
  /* 6 = strlen("script")
   * k_defaultScriptNameNumberMaxSize = maxLength of integers between 1 and 99
   * 1 = null-terminating char */

  static bool DefaultName(char buffer[], size_t bufferSize);
  static bool nameCompliant(const char * name);
  static constexpr size_t InformationSize() { return k_autoImportationStatusSize + k_currentImportationStatusSize; }

  Script(Ion::Storage::Record r = Ion::Storage::Record()) : Record(r) {}
  bool autoImportationStatus() const;
  void toggleAutoimportationStatus();
  const char * content() const;
  bool contentFetchedFromConsole() const;
  void setContentFetchedFromConsole(bool fetch);
};

}

#endif
