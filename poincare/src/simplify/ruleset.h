#ifndef POINCARE_SIMPLIFY_RULESET_H
#define POINCARE_SIMPLIFY_RULESET_H

#include "rule.h"
#include "type_selector.h"
#include "integer_addition.h"
#include "merge_addition.h"

namespace Poincare {

class RuleSet {
public:
  constexpr RuleSet(const Rule rules[], int numberOfRules) :
    m_rules(rules),
    m_numberOfRules(numberOfRules) {};
  const Rule * ruleAtIndex(int i) const { return m_rules+i; };
  int numberOfRules() const { return m_numberOfRules; };
private:
  const Rule * m_rules;
  int m_numberOfRules;
};

//R0: Int + Int -> AdditionDeInt
constexpr TypeSelector r0s0(Expression::Type::Integer, 0);
constexpr TypeSelector r0s1(Expression::Type::Integer, 1);
constexpr const Selector * r0s2c[] = {&r0s0, &r0s1};
constexpr TypeSelector r0s2(Expression::Type::Addition, 2, r0s2c, 2);
constexpr IntegerAddition r0t;
constexpr Rule r0(&r0s2, &r0t);

//R1: a+(b+c) -> +(a,b,c)
constexpr TypeSelector r1s0(Expression::Type::Addition, 0);
constexpr const Selector * r1s1c[] = {&r1s0};
constexpr TypeSelector r1s1(Expression::Type::Addition, 1, r1s1c, 1);
constexpr MergeAddition r1t;
constexpr Rule r1(&r1s1, &r1t);

// RuleSet
constexpr Rule rules[2] = {r1, r0};
constexpr RuleSet DemoRuleSet(rules, 2);

}

#endif
