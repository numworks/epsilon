#ifndef ION_CONSOLE_H
#define ION_CONSOLE_H

namespace Ion {
namespace Console {

void writeChar(char c);
// Return true if the screen could be cleared
bool clear();
char readChar();
void startReception();
bool transmissionDone();

// The lines are NULL-terminated
void writeLine(const char* line, bool appendCRLF = true);
void readLine(char* line, int maxLineLength);

}  // namespace Console
}  // namespace Ion

#endif
