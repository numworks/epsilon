#include "expression_and_layout.h"

namespace ExpressionEditor {

ExpressionAndLayout::ExpressionAndLayout() {
  //const char * expression = "2/3";
  //const char * expression = "1+2/(3+4)";
  //const char * expression = "1+2/3+5+8";
  //const char * expression = "[[1+5,2,3][4,5,6]]";
  //const char * expression = "1+2/(7+5/3/467777777)/3+8";
  //const char * expression = "1+2/7467777777";
  //const char * expression = "2385658/(7+5/46)";
  //const char * expression = "1+conj(100)+2";
  //const char * expression = "1+abs(100)+2";
  //const char * expression = "1+root(100,41)+2";
  //const char * expression = "ln(36)";
  //const char * expression = "1+floor(36)+2";
  //const char * expression = "ln(36)+root(542,52)+sum(12,3,4)+int(22,3,4)+conj(988)";
  //const char * expression = "2385658/(7/78+int(5/46*7/8,3,45))+sum(12,3,4)+(1111111)/(2/3+4/5)";
  //const char * expression = "2385658/(int(5/46*7/8,3,45))";
  //const char * expression = "1+binomial(6,88)+1";
  //const char * expression = "[[1+5,2,3][4,5,int(5/46+1,3,4)]]";
  //const char * expression = "2385658/(7/78+int(5/46*7/8,3,45))+sum(12,3,4)+[[1+5,2,3][4/2,5,6]]";
  //const char * expression = "1/2+[[1,2,3][4,5,6]]+1";
  //const char * expression = "1+(3+4)+1";
  //const char * expression = "1+product(23,46,123)+[[2,3][5,6]]+1/4";
  //const char * expression = "2385658/(7/78+int(5/46*7/8,3,45))+sum(12,3,4)+[[1+5,2,3][4/2,5,6]]";
  //const char * expression = "7/78+int(5/46*7/8,3,45)+sum(12,3,4)+[[1+5,2,3][4/2,5,6]]";
  //const char * expression = "1+conj(39)+abs(3)+root(6000,23)";
  //const char * expression = "1.5+3.4";
  //const char * expression = "abs(5)+int(5/46*7/8,3,4544444)+conj(4)+int(333,4,5)";
  //const char * expression = "conj(int(5/46*7/8,3,45))+conj(4)";
  //const char * expression = "abs(1+conj(conj(4))+(23)+conj(42))+abs(1+2)";
  const char * expression = "13+(23)";
  //const char * expression = "1+sum(12,3,4)+product(12,3,4)+2";

  m_expression = Poincare::Expression::parse(expression);
  m_expressionLayout = m_expression->createLayout();
}
ExpressionAndLayout::~ExpressionAndLayout() {
  if (m_expressionLayout) {
    delete m_expressionLayout;
    m_expressionLayout = nullptr;
  }
  if (m_expression) {
    delete m_expression;
    m_expression = nullptr;
  }
}

}
