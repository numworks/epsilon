#ifndef ION_CONSOLE_H
#define ION_CONSOLE_H

namespace Ion {
namespace Console {

void writeChar(char c);
char readChar();

// The lines are NULL-terminated
void writeLine(const char * line, bool appendCRLF = true);
void readLine(char * line, int maxLineLength);
bool transmissionDone();

}
}

#endif
