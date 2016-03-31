#ifndef POINCARE_SIMPLIFY_RULES_GENERATION_SELECTOR_H
#define POINCARE_SIMPLIFY_RULES_GENERATION_SELECTOR_H

#include "node.h"
#include <string>

class Selector : public Node {
public:
  enum class Type {
    Variable,
    Wildcard,
    ExpressionType
  };
  Selector(Type type, std::string * name, std::vector<Selector *> * children = nullptr);
protected:
  void generateFields(Rule * context, std::string &indentation) override;
private:
  Type m_type;
};

#endif
