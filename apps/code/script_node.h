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
  ScriptNode() :
    m_type(Type::Function), m_name(nullptr), m_scriptIndex(0), m_nameLength(0) {}
  static ScriptNode FunctionNode(const char * name, int nameLength, uint16_t scriptIndex) {
    return ScriptNode(Type::Function, name, scriptIndex, nameLength);
  }
  static ScriptNode VariableNode(const char * name, int nameLength, uint16_t scriptIndex) {
    return ScriptNode(Type::Variable, name, scriptIndex, nameLength);
  }
  Type type() const { return m_type; }
  const char * name() const { return m_name; }
  int nameLength() const { return m_nameLength; }
  uint16_t scriptIndex() const { return m_scriptIndex; }
private:
  ScriptNode(Type type, const char * name, uint16_t scriptIndex, int nameLength) :
    m_type(type), m_name(name), m_scriptIndex(scriptIndex), m_nameLength(nameLength) {}
  Type m_type;
  const char * m_name;
  uint16_t m_scriptIndex;
  int m_nameLength;
};

}

#endif
