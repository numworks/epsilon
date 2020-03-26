#ifndef CODE_SCRIPT_NODE_H
#define CODE_SCRIPT_NODE_H

#include <stddef.h>
#include <stdint.h>

namespace Code {

class ScriptNode {
public:
  enum class Type : bool {
    Function,
    Variable
  };
  ScriptNode(Type type = Type::Variable, const char * name = nullptr, int nameLength = 0, uint16_t scriptIndex = 0) :
    m_type(type),
    m_name(name),
    m_scriptIndex(scriptIndex),
    m_nameLength(nameLength)
  {}
  Type type() const { return m_type; }
  const char * name() const { return m_name; }
  int nameLength() const { return m_nameLength; }
  uint16_t scriptIndex() const { return m_scriptIndex; }
private:
  Type m_type;
  const char * m_name;
  uint16_t m_scriptIndex; // TODO LEA smaller type ?
  int m_nameLength; // TODO LEA smaller type ?
};

}

#endif
