#ifndef POINCARE_HANDLE_H
#define POINCARE_HANDLE_H

#include "tree_block.h"

namespace Poincare {

class TreeSandbox;
class InternalHandle;
union HandleBuffer;

class Handle {
public:
  template <typename T>
  static T Create(const TypeTreeBlock * treeBlock);
  // TODO: Use expiring pointers
  static Handle * CreateHandle(const TypeTreeBlock * treeBlock);

  Handle(const TypeTreeBlock * treeBlock = nullptr) : m_typeTreeBlock(const_cast<TypeTreeBlock *>(treeBlock)) {}
  virtual ~Handle() = default;

  TypeTreeBlock * typeTreeBlock() { return m_typeTreeBlock; }

  virtual void basicReduction(TreeSandbox * sandbox) {}

#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const {}
  virtual void logAttributes(std::ostream & stream) const {}
#endif
  virtual size_t nodeSize() const { return 1; } // Should it be virtual?
  virtual int numberOfChildren() const { return 0; } // Should it be virtual

protected:
  TypeTreeBlock * m_typeTreeBlock;
};

class Subtraction final : public Handle {
public:
  using Handle::Handle;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Subtraction"; }
#endif
  int numberOfChildren() const override { return 2; }
  static Subtraction PushNode(TreeSandbox * sandbox);
  void basicReduction(TreeSandbox * sandbox) override;
};

class Division final : public Handle {
public:
  using Handle::Handle;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Division"; }
#endif
  int numberOfChildren() const override { return 2; }
  static Division PushNode(TreeSandbox * sandbox);
  void basicReduction(TreeSandbox * sandbox) override;
};

class InternalHandle : public Handle {
public:
  using Handle::Handle;
  virtual Handle * shallowBeautify() { return this; }
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
  using InternalHandle::InternalHandle;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Integer"; }
  void logAttributes(std::ostream & stream) const override;
#endif
  size_t nodeSize() const override;
  int value() const;
  static Integer PushNode(TreeSandbox * sandbox, int value);
private:
  constexpr static size_t k_minimalNumberOfNodes = 4;
  constexpr static size_t k_maxValue = 1 << 8;
  size_t nodeSize(NextStep step) const;
};

class NAry : public InternalHandle {
public:
  using InternalHandle::InternalHandle;
#if POINCARE_TREE_LOG
  void logAttributes(std::ostream & stream) const override;
#endif
  size_t nodeSize() const override { return 3; }

protected:
  int privateNumberOfChildren(BlockType headType) const;

  static TypeTreeBlock * PushNode(TreeSandbox * sandbox, int numberOfChildren, TypeTreeBlock headBlock, TypeTreeBlock tailBlock);
};

class Addition final : public NAry {
public:
  using NAry::NAry;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Addition"; }
#endif
  int numberOfChildren() const override;

  static Addition PushNode(TreeSandbox * sandbox, int numberOfChildren);
};

class Multiplication final : public NAry {
public:
  using NAry::NAry;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Multiplication"; }
#endif
  int numberOfChildren() const override;

  static Multiplication PushNode(TreeSandbox * sandbox, int numberOfChildren);

  Handle distributeOverAddition(TreeSandbox * sandbox);
};

class Power final : public InternalHandle {
public:
  using InternalHandle::InternalHandle;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Power"; }
#endif
  int numberOfChildren() const override { return 2; }
  static Power PushNode(TreeSandbox * sandbox);
};

union HandleBuffer {
friend class Handle;
public:
  HandleBuffer();
  ~HandleBuffer();
private:
  Handle m_handle;
#if GHOST_REQUIRED
  Ghost m_ghost;
#endif
  Integer m_integer;
  Addition m_addition;
  Multiplication m_multiplication;
  Subtraction m_subtraction;
  Division m_division;
  Power m_power;
};

#if GHOST_REQUIRED
static_assert(sizeof(Handle) == sizeof(Ghost));
#endif
static_assert(sizeof(Handle) == sizeof(Integer));
static_assert(sizeof(Handle) == sizeof(Addition));
static_assert(sizeof(Handle) == sizeof(Multiplication));
static_assert(sizeof(Handle) == sizeof(Subtraction));
static_assert(sizeof(Handle) == sizeof(Division));
static_assert(sizeof(Handle) == sizeof(Power));

}

#endif
