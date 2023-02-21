#ifndef CODE_SCRIPT_STORE_H
#define CODE_SCRIPT_STORE_H

#include <ion.h>
#include <python/port/port.h>

#include "script.h"
#include "script_template.h"
extern "C" {
#include "py/parse.h"
}

namespace Code {

class ScriptStore : public MicroPython::ScriptProvider {
 public:
  constexpr static char k_scriptExtension[] = "py";
  constexpr static size_t k_scriptExtensionLength = 2;

  // Storage information
  static bool ScriptNameIsFree(const char* baseName);

  ScriptStore();
  Script scriptAtIndex(int index) {
    return Script(
        Ion::Storage::FileSystem::sharedFileSystem->recordWithExtensionAtIndex(
            k_scriptExtension, index));
  }
  static Script ScriptNamed(const char* fullName) {
    return Script(
        Ion::Storage::FileSystem::sharedFileSystem->recordNamed(fullName));
  }
  static Script ScriptBaseNamed(const char* baseName) {
    return Script(
        Ion::Storage::FileSystem::sharedFileSystem
            ->recordBaseNamedWithExtension(baseName, k_scriptExtension));
  }
  int numberOfScripts() {
    return Ion::Storage::FileSystem::sharedFileSystem
        ->numberOfRecordsWithExtension(k_scriptExtension);
  }
  Ion::Storage::Record::ErrorStatus addNewScript() {
    return addScriptFromTemplate(ScriptTemplate::Empty());
  }
  void deleteAllScripts();
  bool isFull();

  /* MicroPython::ScriptProvider */
  const char* contentOfScript(const char* name, bool markAsFetched) override;
  void clearVariableBoxFetchInformation();
  void clearConsoleFetchInformation();

 private:
  /* If the storage available space has a smaller size than
   * k_fullFreeSpaceSizeLimit, we consider the script store as full.
   * To be able to add a new empty record, the available space should at least
   * be able to store a Script with default name and its extension, the
   * importation status (1 char), the default content "from math import *\n"
   * (20 char) and 10 char of free space. */
  constexpr static int k_fullFreeSpaceSizeLimit =
      sizeof(Ion::Storage::FileSystem::record_size_t) +
      Script::k_defaultScriptNameMaxSize + k_scriptExtensionLength + 1 + 20 +
      10;

  Ion::Storage::Record::ErrorStatus addScriptFromTemplate(
      const ScriptTemplate* scriptTemplate) {
    return Script::Create(scriptTemplate->name(), scriptTemplate->content());
  }
};

}  // namespace Code

#endif
