#ifndef CALCULATION_EXPRESSION_FIELD_H
#define CALCULATION_EXPRESSION_FIELD_H

#include <escher.h>

namespace Calculation {

class ExpressionField : public ::ExpressionField {
public:
  using ::ExpressionField::ExpressionField;
protected:
  bool handleEvent(Ion::Events::Event event) override;
};

}

#endif
