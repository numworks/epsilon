#include <ion.h>

namespace Ion {
namespace Console {

static bool s_logEvents = true;

char readChar();
void writeChar(char c);
void startReception();
bool transmissionDone();

void writeLine(const char* line, bool appendCRLF) {
  while (*line != 0) {
    writeChar(*line++);
  }
  if (appendCRLF) {
    writeChar('\r');
    writeChar('\n');
  }
  while (!transmissionDone()) {
  }
}

void readLine(char* line, int maxLineLength) {
  if (maxLineLength <= 0) {
    return;
  }
  startReception();
  char* cursor = line;
  char* last = line + maxLineLength - 1;
  while (true) {
    *cursor = readChar();
    if (*cursor == '\r' || cursor == last) {
      *cursor = 0;
      return;
    }
    cursor++;
  }
}

bool logEvents() { return s_logEvents; }

bool setLogEvents(bool doNotLogEvents) { return s_logEvents = doNotLogEvents; }

}  // namespace Console
}  // namespace Ion
