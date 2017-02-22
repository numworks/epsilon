#include <poincare/symbol.h>
#include <poincare/context.h>
#include <math.h>
#include "layout/string_layout.h"
#include "layout/baseline_relative_layout.h"
extern "C" {
#include <assert.h>
}

namespace Poincare {

Symbol::Symbol(char name) :
  m_name(name)
{
}

float Symbol::privateApproximate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (context.expressionForSymbol(this)) {
    return context.expressionForSymbol(this)->approximate(context, angleUnit);
  }
  // TODO: decide with Leo what we should return
  return NAN;
}

Expression * Symbol::privateEvaluate(Context& context, AngleUnit angleUnit) const {
  assert(angleUnit != AngleUnit::Default);
  if (context.expressionForSymbol(this)) {
    return context.expressionForSymbol(this)->evaluate(context, angleUnit);
  }
  return nullptr;
}

Expression::Type Symbol::type() const {
  return Expression::Type::Symbol;
}

const char Symbol::name() const {
  return m_name;
}

ExpressionLayout * Symbol::privateCreateLayout(FloatDisplayMode floatDisplayMode) const {
  assert(floatDisplayMode != FloatDisplayMode::Default);
  if (m_name == SpecialSymbols::Ans) {
    return new StringLayout("ans", 4);
  }
  if (m_name == SpecialSymbols::un) {
    return new BaselineRelativeLayout(new StringLayout("u", 1), new StringLayout("n",1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::un1) {
    return new BaselineRelativeLayout(new StringLayout("u", 1), new StringLayout("n+1",3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::vn) {
    return new BaselineRelativeLayout(new StringLayout("v", 1), new StringLayout("n",1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::vn1) {
    return new BaselineRelativeLayout(new StringLayout("v", 1), new StringLayout("n+1",3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::wn) {
    return new BaselineRelativeLayout(new StringLayout("w", 1), new StringLayout("n",1, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  if (m_name == SpecialSymbols::wn1) {
    return new BaselineRelativeLayout(new StringLayout("w", 1), new StringLayout("n+1",3, KDText::FontSize::Small), BaselineRelativeLayout::Type::Subscript);
  }
  return new StringLayout(&m_name, 1);
}

Expression * Symbol::clone() const {
  return new Symbol(m_name);
}

bool Symbol::valueEquals(const Expression * e) const {
  assert(e->type() == Expression::Type::Symbol);
  return (m_name == ((Symbol *)e)->m_name);
}

}
