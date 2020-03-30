#ifndef CODE_SCRIPT_TEMPLATE_H
#define CODE_SCRIPT_TEMPLATE_H

namespace Code {

class ScriptTemplate {
public:
  constexpr ScriptTemplate(const char * name, const char * value) : m_name(name), m_value(value) {}
  static const ScriptTemplate * Empty();
  static const ScriptTemplate * Squares();
  static const ScriptTemplate * Mandelbrot();
  static const ScriptTemplate * Polynomial();
  static const ScriptTemplate * Parabola();
  const char * name() const { return m_name; }
  const char * content() const { return m_value+1; }
  const char * value() const { return m_value; }
private:
  const char * m_name;
  const char * m_value; // hold the 'importation status' flag concatenate with the script content
};

}

#endif
