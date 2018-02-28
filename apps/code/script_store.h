#ifndef CODE_SCRIPT_STORE_H
#define CODE_SCRIPT_STORE_H

#include "script.h"
#include "script_template.h"
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
  Script scriptAtIndex(int index);
  Script scriptNamed(const char * name);
  int numberOfScripts();
  bool addNewScript();
  void deleteAllScripts();
  bool isFull();

  /* Provide scripts content information */
  typedef void (* ScanCallback)(void * context, const char * p, int n);
  void scanScriptsForFunctionsAndVariables(void * context, ScanCallback storeFunction,ScanCallback storeVariable);

  /* MicroPython::ScriptProvider */
  const char * contentOfScript(const char * name) override;

  bool addScriptFromTemplate(const ScriptTemplate * scriptTemplate);
private:
  // If the kallax free space has a size smaller than
  // k_fullFreeSpaceSizeLimit, we consider the script store as full.
  static constexpr int k_fullFreeSpaceSizeLimit = Ion::Record::k_sizeSize+Ion::Record::k_nameSize+Ion::Record::k_typeSize+10;
  static constexpr size_t k_fileInput2ParseNodeStructKind = 1;
  static constexpr size_t k_functionDefinitionParseNodeStructKind = 3;
  static constexpr size_t k_expressionStatementParseNodeStructKind = 5;
  const char * structID(mp_parse_node_struct_t *structNode);
};

}

#endif
