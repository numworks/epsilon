#ifndef POINCARE_TRIGONOMETRY_CHEAT_TABLE_H
#define POINCARE_TRIGONOMETRY_CHEAT_TABLE_H

#include <poincare/expression.h>
#include <assert.h>

namespace Poincare {

  /* We use the cheat table to look for known simplifications (e.g. cos(0)=1,
   * cos(Pi/2)=1...). For each entry of the table, we store its expression and
   * its float approximation in order to quickly scan the table looking for our
   * input approximation. If one entry matches the float approximation, we then
   * check that the actual expression of our input is equivalent to the table
   * expression.
   *
   * Example: ArcSine(1/2)
   *  -> Approximate 1/2: 0.5
   *  -> Look up the row for 0.5 in the "Sine" column
   *  -> Return the "Angle" value of this row */

class TrigonometryCheatTable {
public:
  constexpr static int k_numberOfEntries = 37;
  enum class Type {
    AngleInDegrees = 0,
    AngleInRadians = 1,
    Cosine = 2,
    Sine = 3,
    Tangent = 4
  };
  static const TrigonometryCheatTable * Table();
  Expression simplify(const Expression e, ExpressionNode::Type type, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) const;

private:

  // ROW CLASS
  class Row {
  public:

    // PAIR
    class Pair {
    public:
      constexpr Pair(const char * expression, float value = NAN) :
        m_expression(expression), m_value(value) {}
      Expression reducedExpression(bool assertNotUninitialized, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) const;
      float value() const { return m_value; }
    private:
      const char * m_expression;
      float m_value;
    };
    // END OF PAIR CLASS

    constexpr Row(Pair angleInRadians, Pair angleInDegrees, Pair sine, Pair cosine, Pair tangent) :
      m_pairs{angleInRadians, angleInDegrees, sine, cosine, tangent} {}
    float floatForType(Type t) const {
      assert(((int) t) >= 0 && ((int) t) < k_numberOfPairs);
      return m_pairs[(int)t].value();
    }
    Expression expressionForType(Type t, bool assertNotUninitialized, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) const {
      assert(((int) t) >= 0 && ((int) t) < k_numberOfPairs);
      return m_pairs[(int)t].reducedExpression(assertNotUninitialized, context, complexFormat, angleUnit, target);
    }
  private:
    constexpr static int k_numberOfPairs = 5;
    const Pair m_pairs[k_numberOfPairs];
  };
  // END OF ROW CLASS

  constexpr TrigonometryCheatTable(const Row * rows) : m_rows(rows) {}
  float floatForTypeAtIndex(Type t, int i) const {
    assert(i >= 0 && i < k_numberOfEntries);
    return m_rows[i].floatForType(t);
  }
  Expression expressionForTypeAtIndex(Type t, int i, bool assertNotUninitialized, Context & context, Preferences::ComplexFormat complexFormat, Preferences::AngleUnit angleUnit, ExpressionNode::ReductionTarget target) const {
    assert(i >= 0 && i < k_numberOfEntries);
    return m_rows[i].expressionForType(t, assertNotUninitialized, context, complexFormat, angleUnit, target);
  }
  const Row * m_rows;
};

}

#endif
