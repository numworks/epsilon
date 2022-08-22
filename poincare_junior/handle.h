#ifndef POINCARE_HANDLE_H
#define POINCARE_HANDLE_H

#define POINCARE_TREE_LOG 1
#if POINCARE_TREE_LOG
#include <ostream>
#endif

#include <stdint.h>
#include <stddef.h>

namespace Poincare {

class TreeBlock;
class TreeSandbox;
class TypeTreeBlock;

typedef TreeBlock * (TreeBlock::*NextStep)();
typedef TreeBlock * (TreeBlock::*NextNthStep)(int i);

typedef void (*TreeFunction)(TypeTreeBlock *);

class Handle {
public:
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const = 0;
  virtual void logAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream) const {}
#endif
  virtual void basicReduction(TypeTreeBlock * treeBlock) const {}
  virtual size_t nodeSize(const TypeTreeBlock * treeBlock, bool head = true) const { return 1; }
  virtual int numberOfChildren(const TypeTreeBlock * treeBlock) const { return 0; }
};

class Subtraction final : public Handle {
public:
  static TypeTreeBlock * PushNode();
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Subtraction"; }
#endif
  int numberOfChildren(const TypeTreeBlock * treeBlock) const override { return 2; }
  void basicReduction(TypeTreeBlock * treeBlock) const override;
};

class Division final : public Handle {
public:
  static TypeTreeBlock * PushNode();
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Division"; }
#endif
  int numberOfChildren(const TypeTreeBlock * treeBlock) const override { return 2; }
  void basicReduction(TypeTreeBlock * treeBlock) const override;
};

class InternalHandle : public Handle {
public:
  using Handle::Handle;
  virtual void Beautify(TypeTreeBlock * treeBlock) const {}
};

#if GHOST_REQUIRED
class Ghost final : public InternalHandle {
public:
  using InternalHandle::InternalHandle;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Ghost"; }
#endif
};
#endif

class Integer final : public InternalHandle {
public:
  static TypeTreeBlock * PushNode(int value);
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Integer"; }
  void logAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream) const override;
#endif
  size_t nodeSize(const TypeTreeBlock * typeTreeBlock, bool head = true) const override;
  static int Value(const TypeTreeBlock * treeBlock);

private:
  constexpr static size_t k_minimalNumberOfNodes = 4;
  constexpr static size_t k_maxValue = 1 << 8;
  size_t nodeSize(const TypeTreeBlock * typeTreeBlock, NextStep step) const;
};

class NAry : public InternalHandle {
public:
#if POINCARE_TREE_LOG
  void logAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream) const override;
#endif
  int numberOfChildren(const TypeTreeBlock * treeBlock) const override;
  size_t nodeSize(const TypeTreeBlock * typeTreeBlock, bool head = true) const override { return 3; }

  static TypeTreeBlock * Flatten(TypeTreeBlock * treeBlock);
protected:
  static TypeTreeBlock * PushNode(int numberOfChildren, TypeTreeBlock blockType);
private:
  static int CollectChildren(TypeTreeBlock * treeBlock);
};

class Addition final : public NAry {
public:
  static TypeTreeBlock * PushNode(int numberOfChildren);
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Addition"; }
#endif
};

class Multiplication final : public NAry {
public:
  static TypeTreeBlock * PushNode(int numberOfChildren);
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Multiplication"; }
#endif

  static TypeTreeBlock * DistributeOverAddition(TypeTreeBlock * treeBlock);
};

class Power final : public InternalHandle {
public:
  static TypeTreeBlock * PushNode();
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Power"; }
#endif
  int numberOfChildren(const TypeTreeBlock * treeBlock) const override { return 2; }
};

}

#endif
