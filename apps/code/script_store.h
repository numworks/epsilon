#ifndef CODE_SCRIPT_STORE_H
#define CODE_SCRIPT_STORE_H

#include "script.h"
#include <python/port/port.h>

namespace Code {

class ScriptStore : public MicroPython::ScriptProvider {
public:
  ScriptStore();
  Script editableScript(int i);
  /* editableScript moves the free space of m_history at the end of the
   * ith script. It returns a Script object that points to the beginning of the
   * wanted script and has a length taking into account both the script and the
   * free space. */
  Script script(int i);
  /* script returns a Script object that points to the beginning of the
   * ith script and has the length of the script. */
  Script script(const char * name);
  /* script(const char * name) looks for a script that has the right name and
   * returns it. If there is no such script, it returns an empty Script. */
  char * editableNameOfScript(int i);
  int sizeOfEditableNameOfScript(int i);
  int numberOfScripts() const;
  bool addNewScript();
  bool addMandelbrotScript();
  void deleteScript(int i);
  void deleteAll();

  /* MicroPython::ScriptProvider */
  const char * contentOfScript(const char * name) override;

private:
  static constexpr char FreeSpaceMarker = 0x01;
  static constexpr char AutoImportationMarker = 0x02;
  static constexpr char NoAutoImportationMarker = 0x03;
  /* We made sure that these chars are not used in ion/include/ion/charset.h */
  static constexpr int k_historySize = 1024;
  char * nameOfScript(int i);
  bool copyName(int position, const char * name = nullptr);
  int indexOfScriptContent(int i) const;
  int indexOfScript(int i) const;
  int lastIndexOfScript(int i) const;
  int indexOfFirstFreeSpaceMarker() const;
  int sizeOfFreeSpace() const;
  void cleanFreeSpace();
  void moveFreeSpaceAfterScriptContent(int i);
  void moveFreeSpaceAfterScriptName(int i);
  void moveFreeSpaceAtPosition(int i);
  void cleanAndMoveFreeSpaceAfterScriptContent(int i);
  void cleanAndMoveFreeSpaceAfterScriptName(int i);
  bool copyMandelbrotScriptOnFreeSpace();
  bool copyDefaultScriptOnFreeSpace();
  int indexBeginningFilename(const char * path);
  char m_history[k_historySize];
  /* The m_history variable sequentially stores scripts as text buffers.
   * Each script is stored as follow:
   *  - First, a char that says whether the script should be automatically
   *    imported in the console.
   *  - Then, the name of the script.
   *  - Finally, the content of the script.
   * The free bytes of m_history contain the FreeSpaceMarker. By construction,
   * there is always at least one free byte, and the free space is always
   * continuous. */
  int m_numberOfScripts;
  int m_lastEditedStringPosition;
};

}

#endif
