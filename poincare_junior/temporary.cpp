#include "type_block.h"
#include "value_block.h"
#include "node.h"
#include "edition_pool.h"
#include "cache_reference.h"
#include "constexpr_node.h"
#include "interfaces/interfaces.h"
#include <iostream>

int main() {
  Poincare::Block array[12] = {0, 1,2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
  Poincare::Block * b = array[0].next();
  std::cout << static_cast<uint8_t>(*b) << std::endl;
  return 0;
}

