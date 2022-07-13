#ifndef POINCARE_HANDLE_H
#define POINCARE_HANDLE_H

#include "tree_block.h"

namespace Poincare {

class TreeSandbox;

class Handle {
public:
  static Handle * CreateFromBlock(const TypeTreeBlock * treeBlock);

  Handle(const TypeTreeBlock * treeBlock = nullptr) : m_typeTreeBlock(const_cast<TypeTreeBlock *>(treeBlock)) {}
  virtual ~Handle() = default;

#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const {}
  virtual void logAttributes(std::ostream & stream) const {}
#endif
  virtual size_t nodeSize() const { return 1; } // Should it be virtual?
  virtual int numberOfChildren() const { return 0; } // Should it be virtual

protected:
  TypeTreeBlock * m_typeTreeBlock;
};

class Ghost final : public Handle {
public:
  using Handle::Handle;
#if POINCARE_TREE_LOG
  void logNodeName(std::ostream & stream) const override { stream << "Ghost"; }
#endif
};

class Integer final : public Handle {
public:
  using Handle::Handle;
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

class NAry : public Handle {
public:
  using Handle::Handle;
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
};

union HandleBuffer {
friend class Handle;
public:
  HandleBuffer();
  ~HandleBuffer();
private:
  Handle m_handle;
  Ghost m_ghost;
  Integer m_integer;
  Addition m_addition;
  Multiplication m_multiplication;
};

static_assert(sizeof(Handle) == sizeof(Ghost));
static_assert(sizeof(Handle) == sizeof(Integer));
static_assert(sizeof(Handle) == sizeof(Addition));
static_assert(sizeof(Handle) == sizeof(Multiplication));

}

#endif
