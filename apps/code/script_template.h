#ifndef CODE_SCRIPT_TEMPLATE_H
#define CODE_SCRIPT_TEMPLATE_H

namespace Code {

class ScriptTemplate {
public:
  constexpr ScriptTemplate(const char * name, const char * content) : m_name(name), m_content(content) {}
  static const ScriptTemplate * Empty();
  static const ScriptTemplate * Factorial();
  static const ScriptTemplate * Fibonacci();
  static const ScriptTemplate * Mandelbrot();
  static const ScriptTemplate * Polynomial();
  const char * name() const { return m_name; }
  const char * content() const { return m_content; }
private:
  const char * m_name;
  const char * m_content;
};

}

#endif
