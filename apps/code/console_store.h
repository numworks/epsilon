#ifndef CODE_CONSOLE_STORE_H
#define CODE_CONSOLE_STORE_H

#include "console_line.h"
#include <stddef.h>

namespace Code {

class ConsoleStore {
public:
  ConsoleStore();
  void clear();
  void startNewSession();
  ConsoleLine lineAtIndex(int i) const;
  int numberOfLines() const;
  void pushCommand(const char * text, size_t length);
  void pushResult(const char * text, size_t length);
  void deleteLastLineIfEmpty();
  int deleteCommandAndResultsAtIndex(int index);
private:
  static constexpr char CurrentSessionCommandMarker = 0x01;
  static constexpr char CurrentSessionResultMarker = 0x02;
  static constexpr char PreviousSessionCommandMarker = 0x03;
  static constexpr char PreviousSessionResultMarker = 0x04;
  static constexpr int k_historySize = 1024;
  static char makePrevious(char marker) {
    if (marker == CurrentSessionCommandMarker || marker == CurrentSessionResultMarker) {
      return marker + 0x02;
    }
    return marker;
  }
  void push(const char marker, const char * text, size_t length);
  ConsoleLine::Type lineTypeForMarker(char marker) const;
  int indexOfNullMarker() const;
  void deleteLineAtIndex(int index);
  void deleteFirstLine();
  /* When there is no room left to store a new ConsoleLine, we have to delete
   * old ConsoleLines. deleteFirstLine() deletes the first ConsoleLine of
   * m_history and shifts the rest of the ConsoleLines towards the beginning of
   * m_history. */
  void deleteLastLine();
  char m_history[k_historySize];
  /* The m_history variable sequentially stores an array of ConsoleLine objects.
   * Each ConsoleLine is stored as follow:
   *  - First, a char that says whether the ConsoleLine is a Command or a Result
   *  - Then, the text content of the ConsoleLine
   *  - Last but not least, a null byte.
   * The buffer ends whenever the marker char is null. */
};

}

#endif
