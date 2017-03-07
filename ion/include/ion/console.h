#ifndef ION_CONSOLE_H
#define ION_CONSOLE_H

namespace Ion {
namespace Console {

// The lines are NULL-terminated

void writeLine(const char * line);
void readLine(char * line, int maxLineLength);

}
}

#endif
