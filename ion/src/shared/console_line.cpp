#include <ion.h>

namespace Ion {
namespace Console {

char readChar();
void writeChar(char c);
bool transmissionDone();

void writeLine(const char * line) {
  while (*line != 0) {
    writeChar(*line++);
  }
  writeChar('\r');
  writeChar('\n');
  while (!transmissionDone()) {
  }
}

void readLine(char * line, int maxLineLength) {
  if (maxLineLength <= 0) {
    return;
  }
  char * cursor = line;
  char * last = line+maxLineLength-1;
  while (true) {
    *cursor = readChar();
    if (*cursor == '\r' || cursor == last) {
      *cursor = 0;
      return;
    }
    cursor++;
  }
}

}
}
