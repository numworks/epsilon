#include <poincare/trigonometry_cheat_table.h>

namespace Poincare {

static constexpr TrigonometryCheatTable::Type s_targetType[] = {
  TrigonometryCheatTable::Type::AngleInDegrees,
  TrigonometryCheatTable::Type::AngleInRadians,
  TrigonometryCheatTable::Type::AngleInGradians
};

Expression TrigonometryCheatTable::Row::Pair::reducedExpression(bool assertNotUninitialized, ExpressionNode::ReductionContext reductionContext) const {
  Expression e = Expression::Parse(m_expression);
  if (assertNotUninitialized) {
    assert(!e.isUninitialized());
  } else {
    if (e.isUninitialized()) {
      return Expression();
    }
  }
  return e.deepReduce(reductionContext);
}

Expression TrigonometryCheatTable::simplify(const Expression e, ExpressionNode::Type type, ExpressionNode::ReductionContext reductionContext) const {
  assert(type == ExpressionNode::Type::Sine
      || type == ExpressionNode::Type::Cosine
      || type == ExpressionNode::Type::Tangent
      || type == ExpressionNode::Type::ArcCosine
      || type == ExpressionNode::Type::ArcSine
      || type == ExpressionNode::Type::ArcTangent);

  // Compute the input and output types
  Type angleUnitType = s_targetType[(int)reductionContext.angleUnit()];
  Type trigonometricFunctionType;
  if (type == ExpressionNode::Type::Cosine || type == ExpressionNode::Type::ArcCosine) {
    trigonometricFunctionType = Type::Cosine;
  } else if (type == ExpressionNode::Type::Sine || type == ExpressionNode::Type::ArcSine) {
    trigonometricFunctionType = Type::Sine;
  } else {
    trigonometricFunctionType = Type::Tangent;
  }
  bool isIndirectType = type == ExpressionNode::Type::ArcCosine || type == ExpressionNode::Type::ArcSine || type == ExpressionNode::Type::ArcTangent;
  Type inputType = isIndirectType ? trigonometricFunctionType : angleUnitType;
  Type outputType = isIndirectType ? angleUnitType : trigonometricFunctionType;

  // Avoid looping if we can exclude quickly that e is in the table
  if (((inputType == Type::AngleInDegrees || inputType == Type::AngleInGradians)
        && e.type() != ExpressionNode::Type::Rational)
      || (inputType == Type::AngleInRadians
        && e.type() != ExpressionNode::Type::Rational
        && e.type() != ExpressionNode::Type::Multiplication
        && e.type() != ExpressionNode::Type::Constant)
      || (inputType > Type::AngleInRadians
        && e.type() != ExpressionNode::Type::Rational
        && e.type() != ExpressionNode::Type::Multiplication
        && e.type() != ExpressionNode::Type::Power
        && e.type() != ExpressionNode::Type::Addition))
  {
    return Expression();
  }

  // Approximate e to quickly compare it to cheat table entries
  float eValue = e.node()->approximate(float(), reductionContext.context(), reductionContext.complexFormat(), reductionContext.angleUnit()).toScalar();
  if (std::isnan(eValue) || std::isinf(eValue)) {
    return Expression();
  }
  for (int i = 0; i < k_numberOfEntries; i++) {
    float inputValue = floatForTypeAtIndex(inputType, i);
    if (std::isnan(inputValue) || std::fabs(inputValue - eValue) > Expression::Epsilon<float>()) {
      continue;
    }
    /* e's approximation matches a table entry, check that both expressions are
     * identical */
    Expression input = expressionForTypeAtIndex(inputType, i, true, reductionContext);
    if (input.isIdenticalTo(e)) {
      return expressionForTypeAtIndex(outputType, i, false, reductionContext);
    }
  }
  return Expression();
}

/* Some cheat tables values were not entered because they would never be needed
 * For instance, when simplfy a Cosine, we always compute the value for an angle
 * in the top right trigonometric quadrant. */
const TrigonometryCheatTable * TrigonometryCheatTable::Table() {
  const Row sTableRows[] = {
    Row(Row::Pair("-90", -90.0f),
        Row::Pair("π*(-2)^(-1)", -1.5707963267948966f),
        Row::Pair("-100", -100.0f),
        Row::Pair(""),
        Row::Pair("-1",-1.0f),
        Row::Pair("undef")),
    Row(Row::Pair("-75",-75.0f),
        Row::Pair("π*(-5)*12^(-1)",-1.3089969389957472f),
        Row::Pair("-250/3",-250.0f/3.0f),
        Row::Pair(""),
        Row::Pair("(-1)*6^(1/2)*4^(-1)-2^(1/2)*4^(-1)",-0.9659258262890683f),
        Row::Pair("-(3^(1/2)+2)",-3.7320508075688776f)),
    Row(Row::Pair("-72",-72.0f),
        Row::Pair("π*2*(-5)^(-1)",-1.2566370614359172f),
        Row::Pair("-80",-80.0f),
        Row::Pair(""),
        Row::Pair("-(5/8+5^(1/2)/8)^(1/2)",-0.9510565162951535f),
        Row::Pair("-(5+2*5^(1/2))^(1/2)",-3.077683537175253f)),
    Row(Row::Pair("-135/2",67.5f),
        Row::Pair("π*(-3)*8^(-1)",-1.1780972450961724f),
        Row::Pair("-75",-75.0f),
        Row::Pair(""),
        Row::Pair("-(2+2^(1/2))^(1/2)*2^(-1)",-0.9238795325112867f),
        Row::Pair("-1-2^(1/2)",-2.4142135623730945f)),
    Row(Row::Pair("-60",-60.0f),
        Row::Pair("π*(-3)^(-1)",-1.0471975511965976f),
        Row::Pair("-200/3",-200.0f/3.0f),
        Row::Pair(""),
        Row::Pair("-3^(1/2)*2^(-1)",-0.8660254037844386f),
        Row::Pair("-3^(1/2)",-1.7320508075688767f)),
    Row(Row::Pair("-54",-54.0f),
        Row::Pair("π*(-3)*10^(-1)",-0.9424777960769379),
        Row::Pair("-60",-60.0f),
        Row::Pair(""),
        Row::Pair("4^(-1)*(-1-5^(1/2))",-0.8090169943749473f),
        Row::Pair("-(1+2*5^(-1/2))^(1/2)",-1.3763819204711731f)),
    Row(Row::Pair("-45",-45.0f),
        Row::Pair("π*(-4)^(-1)",-0.7853981633974483f),
        Row::Pair("-50",-50.0f),
        Row::Pair(""),
        Row::Pair("(-1)*(2^(-1/2))",-0.7071067811865475f),
        Row::Pair("-1",-1.0f)),
    Row(Row::Pair("-36",-36.0f),
        Row::Pair("π*(-5)^(-1)",-0.6283185307179586f),
        Row::Pair("-40",-40.0),
        Row::Pair(""),
        Row::Pair("-(5/8-5^(1/2)/8)^(1/2)",-0.5877852522924731f),
        Row::Pair("-(5-2*5^(1/2))^(1/2)",-0.7265425280053609f)),
    Row(Row::Pair("-30",-30.0f),
        Row::Pair("π*(-6)^(-1)",-0.5235987755982988f),
        Row::Pair("-100/3",-100.0f/3.0f),
        Row::Pair(""),
        Row::Pair("-0.5",-0.5f),
        Row::Pair("-3^(-1/2)",-0.5773502691896256f)),
    Row(Row::Pair("-45/2",-22.5f),
        Row::Pair("π*(-8)^(-1)",-0.39269908169872414f),
        Row::Pair("-25",-25.0f),
        Row::Pair(""),
        Row::Pair("(2-2^(1/2))^(1/2)*(-2)^(-1)",-0.3826834323650898f),
        Row::Pair("1-2^(1/2)",-0.4142135623730951f)),
    Row(Row::Pair("-18",-18.0f),
        Row::Pair("π*(-10)^(-1)",-0.3141592653589793f),
        Row::Pair("-20",-20.0f),
        Row::Pair(""),
        Row::Pair("4^(-1)*(1-5^(1/2))",-0.3090169943749474f),
        Row::Pair("-(1-2*5^(-1/2))^(1/2)",-0.3249196962329063f)),
    Row(Row::Pair("-15",-15.0f),
        Row::Pair("π*(-12)^(-1)",-0.2617993877991494f),
        Row::Pair("-50/3",-50.0f/3.0f),
        Row::Pair(""),
        Row::Pair("-6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",-0.25881904510252074f),
        Row::Pair("3^(1/2)-2",-0.2679491924311227f)),
    Row(Row::Pair("0",0.0f),
        Row::Pair("0",0.0f),
        Row::Pair("0",0.0f),
        Row::Pair("1",1.0f),
        Row::Pair("0",0.0f),
        Row::Pair("0",0.0f)),
    Row(Row::Pair("15",15.0f),
        Row::Pair("π*12^(-1)",0.2617993877991494f),
        Row::Pair("50/3",50.0f/3.0f),
        Row::Pair("6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",0.9659258262890683f),
        Row::Pair("6^(1/2)*4^(-1)+2^(1/2)*(-4)^(-1)",0.25881904510252074f),
        Row::Pair("-(3^(1/2)-2)",0.2679491924311227f)),
    Row(Row::Pair("18",18.0f),
        Row::Pair("π*10^(-1)",0.3141592653589793f),
        Row::Pair("20",20.0f),
        Row::Pair("(5/8+5^(1/2)/8)^(1/2)",0.9510565162951535f),
        Row::Pair("4^(-1)*(5^(1/2)-1)",0.3090169943749474f),
        Row::Pair("(1-2*5^(-1/2))^(1/2)",0.3249196962329063f)),
    Row(Row::Pair("45/2",22.5f),
        Row::Pair("π*8^(-1)",0.39269908169872414f),
        Row::Pair("25",25.0f),
        Row::Pair("(2+2^(1/2))^(1/2)*2^(-1)",0.9238795325112867f),
        Row::Pair("(2-2^(1/2))^(1/2)*2^(-1)",0.3826834323650898f),
        Row::Pair("2^(1/2)-1",0.4142135623730951f)),
    Row(Row::Pair("30",30.0f),
        Row::Pair("π*6^(-1)",0.5235987755982988f),
        Row::Pair("100/3",100.0f/3.0f),
        Row::Pair("3^(1/2)*2^(-1)",0.8660254037844387f),
        Row::Pair("0.5",0.5f),
        Row::Pair("3^(-1/2)",0.5773502691896256f)),
    Row(Row::Pair("36",36.0f),
        Row::Pair("π*5^(-1)",0.6283185307179586f),
        Row::Pair("40",40.0f),
        Row::Pair("(5^(1/2)+1)*4^(-1)",0.8090169943749475f),
        Row::Pair("(5/8-5^(1/2)/8)^(1/2)",0.5877852522924731f),
        Row::Pair("(5-2*5^(1/2))^(1/2)",0.7265425280053609f)),
    Row(Row::Pair("45",45.0f),
        Row::Pair("π*4^(-1)",0.7853981633974483f),
        Row::Pair("50",50.0f),
        Row::Pair("2^(-1/2)",0.7071067811865476f),
        Row::Pair("2^(-1/2)",0.7071067811865475f),
        Row::Pair("1",1.0f)),
    Row(Row::Pair("54",54.0f),
        Row::Pair("π*3*10^(-1)",0.9424777960769379f),
        Row::Pair("60",60.0f),
        Row::Pair("(5/8-5^(1/2)/8)^(1/2)",0.5877852522924732f),
        Row::Pair("4^(-1)*(5^(1/2)+1)",0.8090169943749473f),
        Row::Pair("(1+2*5^(-1/2))^(1/2)",1.3763819204711731f)),
    Row(Row::Pair("60",60.0f),
        Row::Pair("π*3^(-1)",1.0471975511965976f),
        Row::Pair("200/3",200.0f/3.0f),
        Row::Pair("0.5",0.5f),
        Row::Pair("3^(1/2)*2^(-1)",0.8660254037844386f),
        Row::Pair("3^(1/2)",1.7320508075688767f)),
    Row(Row::Pair("135/2",67.5f),
        Row::Pair("π*3*8^(-1)",1.1780972450961724f),
        Row::Pair("75",75.0f),
        Row::Pair("(2-2^(1/2))^(1/2)*2^(-1)",0.38268343236508984f),
        Row::Pair("(2+2^(1/2))^(1/2)*2^(-1)",0.9238795325112867f),
        Row::Pair("1+2^(1/2)",2.4142135623730945f)),
    Row(Row::Pair("72",72.0f),
        Row::Pair("π*2*5^(-1)",1.2566370614359172f),
        Row::Pair("80",80.0f),
        Row::Pair("(5^(1/2)-1)*4^(-1)",0.30901699437494745f),
        Row::Pair("(5/8+5^(1/2)/8)^(1/2)",0.9510565162951535f),
        Row::Pair("(5+2*5^(1/2))^(1/2)",3.077683537175253f)),
    Row(Row::Pair("75",75.0f),
        Row::Pair("π*5*12^(-1)",1.3089969389957472f),
        Row::Pair("250/3",250.0f/3.0f),
        Row::Pair("6^(1/2)*4^(-1)+2^(1/2)*(-4)^(-1)",0.25881904510252074f),
        Row::Pair("6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",0.9659258262890683f),
        Row::Pair("3^(1/2)+2",3.7320508075688776f)),
    Row(Row::Pair("90",90.0f),
        Row::Pair("π*2^(-1)",1.5707963267948966f),
        Row::Pair("100",100.0f),
        Row::Pair("0",0.0f),
        Row::Pair("1",1.0f),
        Row::Pair("undef")),
    Row(Row::Pair("105",105.0f),
        Row::Pair("π*7*12^(-1)",1.832595714594046f),
        Row::Pair("350/3",350.0f/3.0f),
        Row::Pair("-6^(1/2)*4^(-1)+2^(1/2)*4^(-1)",-0.25881904510252063f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("108",108.0f),
        Row::Pair("π*3*5^(-1)",1.8849555921538759f),
        Row::Pair("120",120.0f),
        Row::Pair("(1-5^(1/2))*4^(-1)",-0.30901699437494734f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("225/2",112.5f),
        Row::Pair("π*5*8^(-1)",1.9634954084936207f),
        Row::Pair("125",125.0f),
        Row::Pair("(2-2^(1/2))^(1/2)*(-2)^(-1)",-0.3826834323650897f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("120",120.0f),
        Row::Pair("π*2*3^(-1)",2.0943951023931953f),
        Row::Pair("400/3",400.0f/3.0f),
        Row::Pair("-0.5",-0.5f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("126",126.0f),
        Row::Pair("π*7*10^(-1)",2.199114857512855f),
        Row::Pair("140",140.0f),
        Row::Pair("-(5*8^(-1)-5^(1/2)*8^(-1))^(1/2)",-0.587785252292473f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("135",135.0f),
        Row::Pair("π*3*4^(-1)",2.356194490192345f),
        Row::Pair("150",150.0f),
        Row::Pair("(-1)*(2^(-1/2))",-0.7071067811865475f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("144",144.0f),
        Row::Pair("π*4*5^(-1)",2.5132741228718345f),
        Row::Pair("160",160.0f),
        Row::Pair("(-5^(1/2)-1)*4^(-1)",-0.8090169943749473f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("150",150.0f),
        Row::Pair("π*5*6^(-1)",2.6179938779914944f),
        Row::Pair("500/3",500.0f/3.0f),
        Row::Pair("-3^(1/2)*2^(-1)",-0.8660254037844387f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("315/2",157.5f),
        Row::Pair("π*7*8^(-1)",2.748893571891069f),
        Row::Pair("350",350.0f),
        Row::Pair("-(2+2^(1/2))^(1/2)*2^(-1)",-0.9238795325112867f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("162",162.0f),
        Row::Pair("π*9*10^(-1)",2.827433388230814f),
        Row::Pair("180",180.0f),
        Row::Pair("-(5*8^(-1)+5^(1/2)*8^(-1))^(1/2)",-0.9510565162951535f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("165",165.0f),
        Row::Pair("π*11*12^(-1)",2.8797932657906435f),
        Row::Pair("550/3",550.0f/3.0f),
        Row::Pair("(-1)*6^(1/2)*4^(-1)-2^(1/2)*4^(-1)",-0.9659258262890682f),
        Row::Pair(""),
        Row::Pair("")),
    Row(Row::Pair("180",180.0f),
        Row::Pair("π",3.141592653589793f),
        Row::Pair("200",200.0f),
        Row::Pair("-1",-1.0f),
        Row::Pair("0",0.0f),
        Row::Pair("0",0.0f))
  };
  static TrigonometryCheatTable sTable(sTableRows);
  return &sTable;
}

}
