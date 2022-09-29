#ifndef POINCARE_INTERFACE_H
#define POINCARE_INTERFACE_H

#include "../type_block.h"
#include "../value_block.h"

namespace Poincare {

class Interface {
public:
  template <typename T, typename... Types>
  static TypeBlock * PushNode(Types... args);
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const = 0;
  virtual void logAttributes(const TypeBlock * block, std::ostream & stream) const {}
#endif
  virtual constexpr size_t nodeSize(const TypeBlock * block, bool head = true) const { return 1; }
  virtual constexpr int numberOfChildren(const TypeBlock * block) const { return 0; }
};

}

#endif
