#ifndef CODE_CONSOLE_STORE_H
#define CODE_CONSOLE_STORE_H

#include <assert.h>
#include <stddef.h>

#include "console_line.h"

namespace Code {

class ConsoleStore {
 public:
  constexpr static size_t k_historySize = 1024;
  ConsoleStore() : m_history{0} {}
  void clear() {
    assert(k_historySize > 0);
    m_history[0] = 0;
  }
  void startNewSession();
  ConsoleLine lineAtIndex(int i) const;
  int numberOfLines() const;
  const char* pushCommand(const char* text);
  void pushResult(const char* text);
  void deleteLastLineIfEmpty();
  int deleteCommandAndResultsAtIndex(int index);

 private:
  constexpr static char CurrentSessionCommandMarker = 0x01;
  constexpr static char CurrentSessionResultMarker = 0x02;
  constexpr static char PreviousSessionCommandMarker = 0x03;
  constexpr static char PreviousSessionResultMarker = 0x04;
  static char makePrevious(char marker) {
    if (marker == CurrentSessionCommandMarker ||
        marker == CurrentSessionResultMarker) {
      return marker + 0x02;
    }
    return marker;
  }
  const char* push(const char marker, const char* text);
  ConsoleLine::Type lineTypeForMarker(char marker) const;
  size_t indexOfNullMarker() const;
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

}  // namespace Code

#endif
