#ifndef CODE_SCRIPT_STORE_H
#define CODE_SCRIPT_STORE_H

#include "script.h"
#include <python/port/port.h>
extern "C" {
#include "py/parse.h"
}

namespace Code {

class ScriptStore : public MicroPython::ScriptProvider {
public:
  static constexpr char k_scriptExtension[] = ".py";
  static constexpr char k_defaultScriptName[] = "script.py";
  static constexpr int k_maxNumberOfScripts = 8;

  ScriptStore();
  Script scriptAtIndex(int index) {
    return Script(Ion::storage.recordWithExtensionAtIndex(k_scriptExtension, index));
  }
  Script scriptNamed(const char * name) {
    return Script(Ion::storage.recordNamed(name));
  }
  int numberOfScripts() {
    return Ion::storage.numberOfRecordsWithExtension(k_scriptExtension);
  }
  Ion::Storage::Record::ErrorStatus addNewScript() {
    return Ion::storage.createScript(".py", "from math import *\n");
  }
  void deleteAllScripts();
  bool isFull();

  /* Provide scripts content information */
  typedef void (* ScanCallback)(void * context, const char * p, int n);
  void scanScriptsForFunctionsAndVariables(void * context, ScanCallback storeFunction,ScanCallback storeVariable);

  /* MicroPython::ScriptProvider */
  const char * contentOfScript(const char * name) override;

  Ion::Storage::Record::ErrorStatus addScript(const char * name, const char * content) {
    return Ion::storage.createScript(name, content);
  }
private:
  /* If the storage available space has a smaller size than
   * k_fullFreeSpaceSizeLimit, we consider the script store as full.
   * To be able to add a new empty record, the available space should at least
   * stores a Script with default name "script99.py" (12 char), the importation
   * status (1 char), the default content "from math import *\n" (20 char) and
   * 10 char of free space. */
  static constexpr int k_fullFreeSpaceSizeLimit = sizeof(Ion::Storage::record_size_t)+12+1+20+10;
  static constexpr size_t k_fileInput2ParseNodeStructKind = 1;
  static constexpr size_t k_functionDefinitionParseNodeStructKind = 3;
  static constexpr size_t k_expressionStatementParseNodeStructKind = 5;
  const char * structID(mp_parse_node_struct_t *structNode);
};

}

#endif
