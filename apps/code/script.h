#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <ion.h>

namespace Code {

/* Record: | Size |  Name |             Body                                |
 * Script: |      |       | AutoImportationStatus | FetchedStatus | Content |
 *
 * AutoImportationStatus is 1 if the script should be auto imported when the
 * console opens.
 *
 * FetchedStatus has two purposes:
 * - It is used to detect which scripts are imported in the console, so we can
 *   retrieve the correct variables afterwards in the variable box. When a
 *   script has been imported, its fetchedStatus value is
 *   FetchedStatus::FromConsole.
 * - It is used to prevent circular importation problems, such as scriptA
 *   importing scriptB, which imports scriptA. Once we get the variables from a
 *   script to put them in the variable box, we switch the status to
 *   FetchedStatus::ForVariableBox and won't reload it afterwards. */

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
  bool contentFetchedForVariableBox() const;
  void setContentFetchedFromConsole();
  void setContentFetchedForVariableBox();
  void resetContentFetchedStatus();
private:
  /* Fetched status */
  enum class FetchedStatus : uint8_t {
    None = 0,
    FromConsole = 1,
    ForVariableBox = 2
  };
  FetchedStatus fetchedStatus() const;
  void setFetchedStatus(FetchedStatus status);
};

}

#endif
