#ifndef CODE_SCRIPT_STORE_H
#define CODE_SCRIPT_STORE_H

#include "accordion.h"
#include "script.h"
#include "script_template.h"
#include <python/port/port.h>
extern "C" {
#include "py/parse.h"
}

namespace Code {

class ScriptStore : public MicroPython::ScriptProvider {
public:
  enum class EditableZone {
    None,
    Name,
    Content
  };

  static constexpr char k_scriptExtension[] = ".py";
  static constexpr char k_defaultScriptName[] = "script.py";
  static constexpr int k_maxNumberOfScripts = 8;

  ScriptStore();
  const Script scriptAtIndex(int index, EditableZone zone = EditableZone::None);
  const Script scriptNamed(const char * name);
  int numberOfScripts();
  bool addNewScript();
  bool renameScriptAtIndex(int index, const char * newName);
  void switchAutoImportAtIndex(int index);
  void deleteScriptAtIndex(int index);
  void deleteAllScripts();
  bool isFull();

  /* Provide scripts content information */
  typedef void (* ScanCallback)(void * context, const char * p, int n);
  void scanScriptsForFunctionsAndVariables(void * context, ScanCallback storeFunction,ScanCallback storeVariable);

  /* MicroPython::ScriptProvider */
  const char * contentOfScript(const char * name) override;

  bool addScriptFromTemplate(const ScriptTemplate * scriptTemplate);
private:
  static constexpr int k_fullFreeSpaceSizeLimit = 50;
  // If m_accordion's free space has a size smaller than
  // k_fullFreeSpaceSizeLimit, we consider the script store as full.
  static constexpr size_t k_scriptDataSize = 4096;
  static constexpr size_t k_fileInput2ParseNodeStructKind = 1;
  static constexpr size_t k_functionDefinitionParseNodeStructKind = 3;
  static constexpr size_t k_expressionStatementParseNodeStructKind = 5;
  bool copyStaticScriptOnFreeSpace(const ScriptTemplate * scriptTemplate);
  int accordionIndexOfScriptAtIndex(int index) const;
  int accordionIndexOfMarkersOfScriptAtIndex(int index) const;
  int accordionIndexOfNameOfScriptAtIndex(int index) const;
  int accordionIndexOfContentOfScriptAtIndex(int index) const;
  const char * structID(mp_parse_node_struct_t *structNode);
  char m_scriptData[k_scriptDataSize];
  Accordion m_accordion;
};

}

#endif
