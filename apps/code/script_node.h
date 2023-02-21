#ifndef CODE_SCRIPT_NODE_H
#define CODE_SCRIPT_NODE_H

#include <stddef.h>
#include <stdint.h>

namespace Code {

class ScriptNode {
 public:
  enum class Type : bool { WithoutParentheses, WithParentheses };
  ScriptNode(Type type = Type::WithoutParentheses, const char* name = nullptr,
             int nameLength = -1, const char* nodeSourceName = nullptr,
             const char* description = nullptr)
      : m_type(type),
        m_name(name),
        m_nodeSourceName(nodeSourceName),
        m_description(description),
        m_nameLength(nameLength) {}
  Type type() const { return m_type; }
  const char* name() const { return m_name; }
  int nameLength() const { return static_cast<int>(m_nameLength); }
  const char* nodeSourceName() const { return m_nodeSourceName; }
  const char* description() const { return m_description; }

 private:
  Type m_type;
  const char* m_name;
  const char* m_nodeSourceName;
  const char* m_description;
  size_t m_nameLength;
};

}  // namespace Code

#endif
