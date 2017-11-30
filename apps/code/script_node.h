#ifndef CODE_SCRIPT_NODE_H
#define CODE_SCRIPT_NODE_H

#include <stdint.h>

namespace Code {

class ScriptNode {
public:
  enum class Type {
    Function = 0,
    Variable = 1
  };
  ScriptNode() :
    m_type(Type::Function), m_name(nullptr), m_scriptIndex(0) {}
  static ScriptNode FunctionNode(const char * name, uint16_t scriptIndex) {
    return ScriptNode(Type::Function, name, scriptIndex);
  }
  static ScriptNode VariableNode(const char * name, uint16_t scriptIndex) {
    return ScriptNode(Type::Variable, name, scriptIndex);
  }
  Type type() const { return m_type; }
  const char * name() const { return m_name; }
  uint16_t scriptIndex() const { return m_scriptIndex; }
private:
  ScriptNode(Type type, const char * name, uint16_t scriptIndex) :
    m_type(type), m_name(name), m_scriptIndex(scriptIndex) {}
  Type m_type;
  const char * m_name;
  uint16_t m_scriptIndex;
};

}

#endif
