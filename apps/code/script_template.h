#ifndef CODE_SCRIPT_TEMPLATE_H
#define CODE_SCRIPT_TEMPLATE_H

#include "script.h"

namespace Code {

class ScriptTemplate {
 public:
  constexpr ScriptTemplate(const char* name, const char* content)
      : m_name(name), m_content(content) {}
  static const ScriptTemplate* Empty();
  static const ScriptTemplate* Squares();
  static const ScriptTemplate* Mandelbrot();
  static const ScriptTemplate* Polynomial();
  static const ScriptTemplate* Parabola();
  const char* name() const { return m_name; }
  const char* content() const { return m_content; }

 private:
  const char* m_name;
  const char* m_content;
};

}  // namespace Code

#endif
