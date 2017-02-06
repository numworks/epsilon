#ifndef SEQUENCE_SEQUENCE_H
#define SEQUENCE_SEQUENCE_H

#include "../shared/function.h"

namespace Sequence {

class Sequence : public Shared::Function {
public:
  using Shared::Function::Function;
private:
  char symbol() const override;
};

}

#endif
