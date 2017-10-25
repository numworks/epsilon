#ifndef CODE_SCRIPT_STORE_H
#define CODE_SCRIPT_STORE_H

#include "script.h"
#include "script_template.h"
#include <escher/accordion.h>
#include <python/port/port.h>

namespace Code {

class ScriptStore : public MicroPython::ScriptProvider {
public:
  enum class EditableZone {
    None,
    Name,
    Content
  };

  ScriptStore();
  const Script scriptAtIndex(int index, EditableZone zone = EditableZone::None);
  const Script scriptNamed(const char * name);
  int numberOfScripts();
  bool addNewScript();
  bool renameScriptAtIndex(int index, const char * newName);
  void switchAutoImportAtIndex(int index);
  void deleteScriptAtIndex(int index);
  void deleteAllScripts();

  /* MicroPython::ScriptProvider */
  const char * contentOfScript(const char * name) override;

private:
  static constexpr char k_defaultScriptName[] = ".py";
  static constexpr size_t k_scriptDataSize = 1024;
  bool addScriptFromTemplate(const ScriptTemplate * scriptTemplate);
  bool copyStaticScriptOnFreeSpace(const ScriptTemplate * scriptTemplate);
  int accordionIndexOfScriptAtIndex(int index) const;
  int accordionIndexOfMarkersOfScriptAtIndex(int index) const;
  int accordionIndexOfNameOfScriptAtIndex(int index) const;
  int accordionIndexOfContentOfScriptAtIndex(int index) const;
  char m_scriptData[k_scriptDataSize];
  Accordion m_accordion;
};

}

#endif
