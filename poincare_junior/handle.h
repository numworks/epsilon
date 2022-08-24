#ifndef POINCARE_HANDLE_H
#define POINCARE_HANDLE_H

#include "tree_block.h"

namespace Poincare {

class TreeSandbox;

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
};

class Subtraction final : public Handle {
public:
  static TypeTreeBlock * PushNode();
  template<unsigned L1, unsigned L2> static constexpr TreeNode<L1+L2+1> Of(const TreeNode<L1> child1, const TreeNode<L2> child2) { return makeNary<false>(BlockType::Subtraction, child1, child2); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Subtraction"; }
#endif
  void basicReduction(TypeTreeBlock * treeBlock) const override;
};

class Division final : public Handle {
public:
  static TypeTreeBlock * PushNode();
  template<unsigned L1, unsigned L2> static constexpr TreeNode<L1+L2+1> Of(const TreeNode<L1> child1, const TreeNode<L2> child2) { return makeNary<false>(BlockType::Division, child1, child2); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Division"; }
#endif
  void basicReduction(TypeTreeBlock * treeBlock) const override;
};

class InternalHandle : public Handle {
public:
  using Handle::Handle;
  virtual void beautify(TypeTreeBlock * treeBlock) const {}
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
  static int Value(const TypeTreeBlock * treeBlock);

private:
  constexpr static size_t k_minimalNumberOfNodes = 4;
  constexpr static size_t k_maxValue = 1 << 8;
  size_t nodeSize(const TypeTreeBlock * typeTreeBlock, NextStep step) const;
};

static constexpr TreeNode<5> operator "" _n(unsigned long long value) {
  assert(value < 256); // TODO: larger values
  return {IntegerBlock, ValueTreeBlock(1), ValueTreeBlock(value), ValueTreeBlock(1), IntegerBlock};
}

class Constant final : public InternalHandle {
public:
  enum class Type : uint8_t {
    Pi,
    E,
    Undefined
  };
  static TypeTreeBlock * PushNode(Type type);
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Constant"; }
  void logAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream) const override;
#endif
  static float Value(const TypeTreeBlock * treeBlock);
};

static constexpr TreeNode<5> operator "" _n(char16_t name) {
  Constant::Type type = name == 'e' ? Constant::Type::E : name == u'π' ? Constant::Type::Pi : Constant::Type::Undefined;
  assert(type != Constant::Type::Undefined);
  return {ConstantBlock, ValueTreeBlock(static_cast<uint8_t>(type)), ConstantBlock};
}

class NAry : public InternalHandle {
public:
#if POINCARE_TREE_LOG
  void logAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream) const override;
#endif

  static TypeTreeBlock * Flatten(TypeTreeBlock * treeBlock);
protected:
  static TypeTreeBlock * PushNode(int numberOfChildren, TypeTreeBlock blockType);
private:
  static int CollectChildren(TypeTreeBlock * treeBlock);
};

class Addition final : public NAry {
public:
  static TypeTreeBlock * PushNode(int numberOfChildren);
  template<unsigned ...Len> static constexpr auto Of(const TreeNode<Len> (&...children)) { return makeNary<true>(BlockType::Addition, children...); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Addition"; }
#endif
};

class Multiplication final : public NAry {
public:
  static TypeTreeBlock * PushNode(int numberOfChildren);
  template<unsigned ...Len> static constexpr auto Of(const TreeNode<Len> (&...children)) { return makeNary<true>(BlockType::Multiplication, children...); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Multiplication"; }
#endif

  static TypeTreeBlock * DistributeOverAddition(TypeTreeBlock * treeBlock);
};

class Power final : public InternalHandle {
public:
  static TypeTreeBlock * PushNode();
  template<unsigned L1, unsigned L2> static constexpr TreeNode<L1+L2+1> Of(const TreeNode<L1> child1, const TreeNode<L2> child2) { return makeNary<false>(BlockType::Power, child1, child2); }
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Power"; }
#endif
};

}

#endif
