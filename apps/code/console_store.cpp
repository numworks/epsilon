#include "console_store.h"
#include <assert.h>
#include <string.h>

namespace Code {

ConsoleStore::ConsoleStore() :
  m_history{0}
{
}

void ConsoleStore::clear() {
  assert(k_historySize > 0);
  m_history[0] = 0;
}

void ConsoleStore::startNewSession() {
  if (k_historySize < 1) {
    return;
  }

  m_history[0] = makePrevious(m_history[0]);

  for (int i = 0; i < k_historySize - 1; i++) {
    if (m_history[i] == 0) {
      if (m_history[i+1] == 0) {
        return ;
      }
      m_history[i+1] = makePrevious(m_history[i+1]);
    }
  }
}

ConsoleLine ConsoleStore::lineAtIndex(int i) const {
  assert(i >= 0 && i < numberOfLines());
  int currentLineIndex = 0;
  for (int j=0; j<k_historySize; j++) {
    if (m_history[j] == 0) {
      currentLineIndex++;
      j++;
    }
    if (currentLineIndex == i) {
      return ConsoleLine(lineTypeForMarker(m_history[j]), m_history+j+1);
    }
  }
  assert(false);
  return ConsoleLine();
}

int ConsoleStore::numberOfLines() const {
  if (m_history[0] == 0) {
    return 0;
  }
  int result = 0;
  for (int i = 0; i < k_historySize - 1; i++) {
    if (m_history[i] == 0) {
      result++;
      if (m_history[i+1] == 0) {
        return result;
      }
    }
  }
  assert(false);
  return 0;
}

void ConsoleStore::pushCommand(const char * text, size_t length) {
  push(CurrentSessionCommandMarker, text, length);
}

void ConsoleStore::pushResult(const char * text, size_t length) {
  push(CurrentSessionResultMarker, text, length);
}

void ConsoleStore::deleteLastLineIfEmpty() {
  ConsoleLine lastLine = lineAtIndex(numberOfLines()-1);
  char lastLineFirstChar = lastLine.text()[0];
  if (lastLineFirstChar == 0 || lastLineFirstChar == '\n') {
    deleteLastLine();
  }
}

void ConsoleStore::push(const char marker, const char * text, size_t length) {
  // TODO: Verify that the text field does not accept texts that are bigger than
  // k_historySize, or put an alert message if the command is too big.
  assert(ConsoleLine::sizeOfConsoleLine(length) < k_historySize);

  int i = indexOfNullMarker();
  // If needed, make room for the text we want to push.
  while (i + ConsoleLine::sizeOfConsoleLine(length) > k_historySize - 1) {
    deleteFirstLine();
    i = indexOfNullMarker();
  }
  m_history[i] = marker;
  strlcpy(&m_history[i+1], text, length+1);
  m_history[i+1+length+1] = 0;
}

ConsoleLine::Type ConsoleStore::lineTypeForMarker(char marker) const {
  assert(marker == CurrentSessionCommandMarker || marker == CurrentSessionResultMarker || marker == PreviousSessionCommandMarker || marker == PreviousSessionResultMarker);
  return static_cast<ConsoleLine::Type>(marker-1);
}

int ConsoleStore::indexOfNullMarker() const {
  if (m_history[0] == 0) {
    return 0;
  }
  for (int i=0; i<k_historySize; i++) {
    if (m_history[i] == 0 && m_history[i+1] == 0) {
      return (i+1);
    }
  }
  assert(false);
  return 0;
}

void ConsoleStore::deleteFirstLine() {
  if (m_history[0] == 0) {
    return;
  }
  int secondLineMarkerIndex = 1;
  while (m_history[secondLineMarkerIndex] != 0) {
    secondLineMarkerIndex++;
  }
  secondLineMarkerIndex++;
  for (int i=0; i<k_historySize - secondLineMarkerIndex; i++) {
    m_history[i] = m_history[secondLineMarkerIndex+i];
  }
}

void ConsoleStore::deleteLastLine() {
  int lineCount = numberOfLines();
  if (lineCount < 0) {
    return;
  }
  if (lineCount == 1) {
    deleteFirstLine();
    return;
  }
  int currentLineIndex = 1;
  int lastLineMarkerIndex = 0;
  for (int i=0; i<k_historySize; i++) {
    if (m_history[i] == 0) {
      currentLineIndex++;
      if (currentLineIndex == lineCount) {
        lastLineMarkerIndex = i+1;
        break;
      }
    }
  }
  m_history[lastLineMarkerIndex] = 0;
}

}
