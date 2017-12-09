#ifndef CODE_SCRIPT_TEMPLATE_H
#define CODE_SCRIPT_TEMPLATE_H

namespace Code {

class ScriptTemplate final {
public:
  constexpr ScriptTemplate(const char * name, const char * content) : m_name(name), m_content(content) {}
  static const ScriptTemplate * Empty() { return &emptyScriptTemplate; }
  static const ScriptTemplate * Factorial() { return &factorialScriptTemplate; }
  static const ScriptTemplate * Fibonacci() { return &fibonacciScriptTemplate; }
  static const ScriptTemplate * Mandelbrot() { return &mandelbrotScriptTemplate; }
  static const ScriptTemplate * Polynomial() { return &polynomialScriptTemplate; }
  const char * name() const { return m_name; }
  const char * content() const { return m_content; }
private:
  const char * m_name;
  const char * m_content;

  static const ScriptTemplate emptyScriptTemplate;
  static const ScriptTemplate factorialScriptTemplate;
  static const ScriptTemplate fibonacciScriptTemplate;
  static const ScriptTemplate mandelbrotScriptTemplate;
  static const ScriptTemplate polynomialScriptTemplate;

};

}

#endif
