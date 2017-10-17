#ifndef CODE_PROGRAM_STORE_H
#define CODE_PROGRAM_STORE_H

#include "program.h"

namespace Code {

class ProgramStore {
public:
  ProgramStore();
  Program editableProgram(int i);
  /* editableProgram moves the free space of m_history at the end of the
   * ith script. It returns a Program object that points to the beginning of the
   * wanted script and has a length taking into account both the script and the
   * free space. */
  Program program(int i);
  /* programAtIndex returns a Program object that points to the beginning of the
   * ith script and has the length of the script. */
  int numberOfPrograms() const;
  Program editableNewProgram();
  bool addDefaultProgram();
  void deleteProgram(int i);
  void deleteAll();
private:
  static constexpr char FreeSpaceMarker = 0x01;
  /* We made sure that 0x01 is not used in ion/include/ion/charset.h */
  static constexpr int k_historySize = 1024;
  int indexOfProgram(int i) const;
  int lastIndexOfProgram(int i) const;
  int indexOfFirstFreeSpaceMarker() const;
  int sizeOfFreeSpace() const;
  void cleanFreeSpace();
  void moveFreeSpaceAfterProgram(int i);
  void cleanAndMoveFreeSpaceAfterProgram(int i);
  char m_history[k_historySize];
  /* The m_history variable sequentially stores scripts as text buffers. The
   * free bytes of m_history contain the FreeSpaceMarker. By construction,
   * there is always at least one free byte, and the free space is always
   * continuous. */
  int m_numberOfPrograms;
  int m_lastProgramEdited;
};

}

#endif
