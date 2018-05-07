#include <escher/python_highlighter.h>

#include <stddef.h>

#include "highlight_lexer.hpp"

extern int highlight_lexer_yylex(char *attr, int position);

void PythonHighlighter::highlight(char * buffer, char * attr_buffer, size_t bufferSize) {
  if(bufferSize > 0) {
    YY_BUFFER_STATE buf = highlight_lexer_yy_scan_string(buffer);
    highlight_lexer_yylex(attr_buffer, 0);
    highlight_lexer_yy_delete_buffer(buf);
  }
}

PythonHighlighter pythonHighlighter = PythonHighlighter();
