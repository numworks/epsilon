#ifndef POINCARE_SIMPLIFY_RULES_GENERATION_BUILDER_H
#define POINCARE_SIMPLIFY_RULES_GENERATION_BUILDER_H

#include "node.h"
#include <string>

class Builder : public Node {
public:
  enum class Type {
    ExpressionGenerator,
    ExpressionType,
    Variable,
    Wildcard
  };
  Builder(Type type, std::string * name, std::vector<Builder *> * children = nullptr);
protected:
  void generateFields(Rule * context, std::string &indentation) override;
private:
  Type m_type;
};

#endif
