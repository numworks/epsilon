#ifndef POINCARE_SIMPLIFICATION_RULESET_H
#define POINCARE_SIMPLIFICATION_RULESET_H

#include "rule.h"
#include "selector/type_selector.h"
#include "transform/integer_addition_transform.h"
#include "transform/merge_addition_transform.h"

namespace Poincare {
namespace Simplification {

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
constexpr IntegerAdditionTransform r0t;
constexpr Rule r0(&r0s2, &r0t);

#if 0

namespace R3 {

constexpr TypeSelector s0(Expression::Type::Subtraction, 0);
constexpr SubtractionTransform transform;
constexpr Rule rule(&s0, &transform);

}

namespace R2 {

constexpr TypeSelector s0(Expression::Type::Integer, 0);
constexpr TypeSelector s1(Expression::Type::Integer, 1);
constexpr const Selector * s2c[] = {&s0, &s1};
constexpr TypeSelector s2(Expression::Type::Multiplication, 2, s2c, 2);
constexpr IntegerMultiplication transform;
constexpr Rule rule(&s2, &transform);

}
#endif

#if 0
//R0: Int * Int -> MultiplicationOfIntegers
constexpr TypeSelector r0s0(Expression::Type::Integer, 0);
constexpr TypeSelector r0s1(Expression::Type::Integer, 1);
constexpr const Selector * r0s2c[] = {&r0s0, &r0s1};
constexpr TypeSelector r0s2(Expression::Type::Addition, 2, r0s2c, 2);
constexpr IntegerAddition r0t;
constexpr Rule r0(&r0s2, &r0t);
#endif

//R1: a+(b+c) -> +(a,b,c)
constexpr TypeSelector r1s0(Expression::Type::Addition, 0);
constexpr const Selector * r1s1c[] = {&r1s0};
constexpr TypeSelector r1s1(Expression::Type::Addition, 1, r1s1c, 1);
constexpr MergeAdditionTransform r1t;
constexpr Rule r1(&r1s1, &r1t);

// RuleSet
//constexpr Rule rules[2] = {R3::rule, r1, r0, R2::rule};
constexpr Rule rules[2] = {r0, r1};
constexpr RuleSet DemoRuleSet(rules, 2);

}
}

#endif
