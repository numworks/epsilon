#include <escher/python_highlighter.h>

#include <stddef.h>

#include "highlight_lexer.hpp"

extern int highlight_lexer_yylex(char *attr, int position);

const char DEFAULT_BG_MASK = (char) 0x0f;
const char PARENTHESIS_BG_MASK = (char) ~0x7f;

PythonHighlighter::PythonHighlighter():
  m_onParenthesis(false) {
}

bool PythonHighlighter::highlight(char * buffer, char * attr_buffer, size_t bufferSize) {
  if(bufferSize > 0) {
    YY_BUFFER_STATE buf = highlight_lexer_yy_scan_string(buffer);
    highlight_lexer_yylex(attr_buffer, 0);
    highlight_lexer_yy_delete_buffer(buf);
  }
  return true;
}

void clearAttributes(char * buffer, char * attr_buffer, size_t bufferSize) {
  for(size_t i=0; i<bufferSize && buffer[i] != '\0'; i++) {
    attr_buffer[i] &= DEFAULT_BG_MASK;
  }
}

bool PythonHighlighter::cursorMoved(char * buffer, char * attr_buffer, size_t bufferSize, int location) {
  if(bufferSize > 0 && location > 0 && (size_t)location < bufferSize) {
    location--;
    int count = 0;
    if(buffer[location] == ')') {
      m_onParenthesis = true;
      clearAttributes(buffer, attr_buffer, bufferSize);
      attr_buffer[location] |= PARENTHESIS_BG_MASK;
      while(location > 0) {
        location--;
        attr_buffer[location] |= PARENTHESIS_BG_MASK;
        if(buffer[location] == ')') {
          count++;
        } else if(buffer[location] == '(') {
          if(count == 0) {
            break;
          } else {
            count--;
          }
        }
      }
      return true;
    } else if (buffer[location] == '(') {
      m_onParenthesis = true;
      clearAttributes(buffer, attr_buffer, bufferSize);
      attr_buffer[location] |= PARENTHESIS_BG_MASK;
      while((size_t)location < bufferSize - 1) {
        location++;
        attr_buffer[location] |= PARENTHESIS_BG_MASK;
        if(buffer[location] == '(') {
          count++;
        } else if(buffer[location] == ')') {
          if(count == 0) {
            break;
          } else {
            count--;
          }
        }
      }
      return true;
    } else if(m_onParenthesis) {
      m_onParenthesis = false;
      clearAttributes(buffer, attr_buffer, bufferSize);
      return true;
    } else {
      m_onParenthesis = false;
      return false;
    }
  }
  return false;
}
