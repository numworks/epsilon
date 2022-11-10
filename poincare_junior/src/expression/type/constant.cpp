#include "constant.h"
#include "../node.h"

namespace Poincare {

float Constant::Value(const TypeBlock * typeBlock) {
  const Block * block = typeBlock->next();
  const Constant::Type type = static_cast<Constant::Type>(static_cast<uint8_t>(*block));
  switch (type) {
  case Constant::Type::Pi:
    return 3.14;
  case Constant::Type::E:
    return 2.72;
  default:
    assert(false);
  }
}


}
