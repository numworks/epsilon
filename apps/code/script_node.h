#ifndef CODE_SCRIPT_NODE_H
#define CODE_SCRIPT_NODE_H

#include <stddef.h>
#include <stdint.h>

namespace Code {

class ScriptNode {
 public:
  constexpr static char k_parentheses[] = "()";
  constexpr static char k_parenthesesWithEmpty[] = "(\x11)";
  enum class Type : bool { WithoutParentheses, WithParentheses };
  ScriptNode(Type type = Type::WithoutParentheses, const char* name = nullptr,
             int nameLength = -1, const char* nodeSourceName = nullptr,
             const char* description = nullptr)
      : m_type(type),
        m_nameLength(nameLength),
        m_name(name),
        m_nodeSourceName(nodeSourceName),
        m_description(description) {}
  Type type() const { return m_type; }
  const char* name() const { return m_name; }
  int nameLength() const { return static_cast<int>(m_nameLength); }
  const char* nodeSourceName() const { return m_nodeSourceName; }
  const char* description() const { return m_description; }

 private:
  Type m_type;
  int16_t m_nameLength;
  const char* m_name;
  const char* m_nodeSourceName;
  const char* m_description;
};

}  // namespace Code

#endif
