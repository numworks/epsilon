#ifndef CODE_SCRIPT_H
#define CODE_SCRIPT_H

#include <ion.h>

namespace Code {

/* Record: | Size |  Name |            Body                                         |
 * Script: |      |       | Status | CursorPosition |           Content             |
 *
 *
 *                           |FetchedForVariableBoxBit
 * Status is one byte long: xxxxxxxx
 *                          ^      ^
 *      FetchedFromConsoleBit      AutoImportationBit
 *
 * AutoImportationBit is 1 if the script should be auto imported when the
 * console opens.
 *
 * FetchedFromConsoleBit is 1 if its content has been fetched from the console,
 * so we can retrieve the correct variables afterwards in the variable box.
 *
 * FetchedForVariableBoxBit is used to prevent circular importation problems,
 * such as scriptA importing scriptB, which imports scriptA. Once we get the
 * variables from a script to put them in the variable box, we switch the bit to
 * 1 and won't reload it afterwards. 
 * 
 * Cursor Position is one byte long and has the cursor position value*/

class Script : public Ion::Storage::Record {
private:
  // Default script names are chosen between script1 and script99
  static constexpr int k_maxNumberOfDefaultScriptNames = 99;
  static constexpr int k_defaultScriptNameNumberMaxSize = 2; // Numbers from 1 to 99 have 2 digits max

  // See the comment at the beginning of the file
  static constexpr size_t k_statusSize = 1;
  static constexpr size_t k_cursorPositionSize = 1;

public:
  static constexpr int k_defaultScriptNameMaxSize = 6 + k_defaultScriptNameNumberMaxSize + 1;
  /* 6 = strlen("script")
   * k_defaultScriptNameNumberMaxSize = maxLength of integers between 1 and 99
   * 1 = null-terminating char */

  static bool DefaultName(char buffer[], size_t bufferSize);
  static bool nameCompliant(const char * name);
  static constexpr size_t StatusSize() { return k_statusSize; }
  static constexpr size_t CursorPositionSize() { return k_cursorPositionSize; }


  Script(Ion::Storage::Record r = Ion::Storage::Record()) : Record(r) {}
  bool autoImportationStatus() const;
  void toggleAutoimportationStatus();
  const char * content() const;
  size_t contentSize() { return value().size - k_statusSize - k_cursorPositionSize; }
  void setCursorPosition(uint8_t position);
  uint8_t * CursorPosition();

  /* Fetched status */
  bool fetchedFromConsole() const;
  void setFetchedFromConsole(bool fetched);
  bool fetchedForVariableBox() const;
  void setFetchedForVariableBox(bool fetched);

private:
  static constexpr uint8_t k_autoImportationStatusMask = 0b1;
  static constexpr uint8_t k_fetchedForVariableBoxOffset = 7;
  static constexpr uint8_t k_fetchedFromConsoleOffset = 6;
  static constexpr uint8_t k_fetchedForVariableBoxMask = 0b1 << k_fetchedForVariableBoxOffset;
  static constexpr uint8_t k_fetchedFromConsoleMask = 0b1 << k_fetchedFromConsoleOffset;

  bool getStatutBit(uint8_t offset) const;
  void setStatutBit(uint8_t mask, uint8_t offset, bool value);
};

}

#endif
