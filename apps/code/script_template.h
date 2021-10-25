#ifndef CODE_SCRIPT_TEMPLATE_H
#define CODE_SCRIPT_TEMPLATE_H

#include "script.h"

namespace Code {

class ScriptTemplate {
public:
  constexpr ScriptTemplate(const char * name, const char * value) : m_name(name), m_value(value) {}
  static const ScriptTemplate * Empty();
  const char * name() const { return m_name; }
  const char * content() const { return m_value + Script::StatusSize(); }
  const char * value() const { return m_value; }
private:
  const char * m_name;
  const char * m_value; // holds the 'importation status' and 'current importation status' flags concatenated with the script content
};

}

#endif
