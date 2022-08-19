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

#if POINCARE_TREE_LOG
typedef void (*LogNameFunction)(std::ostream &);
typedef void (*LogAttributeFunction)(const TypeTreeBlock *, std::ostream &);
struct LogTreeBlockVTable {
  LogNameFunction m_logName;
  LogAttributeFunction m_logAttribute;
};
#endif
typedef void (*TreeFunction)(TypeTreeBlock *);
typedef size_t (*NodeSizeFunction)(const TypeTreeBlock *, bool);
typedef int (*NumberOfChildrenFunction)(const TypeTreeBlock *);

class Handle {
public:
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) {}
  static void LogAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream) {}
#endif
  virtual void BasicReduction(TypeTreeBlock * treeBlock) const {}
  virtual size_t NodeSize(const TypeTreeBlock * treeBlock, bool head = true) const { return 1; }
  virtual int NumberOfChildren(const TypeTreeBlock * treeBlock) const { return 0; }
};

class Subtraction final : public Handle {
public:
  static TypeTreeBlock * PushNode();
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Subtraction"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &Handle::LogAttributes};
#endif
  int NumberOfChildren(const TypeTreeBlock * treeBlock) const override { return 2; }
  void BasicReduction(TypeTreeBlock * treeBlock) const override;
};

class Division final : public Handle {
public:
  static TypeTreeBlock * PushNode();
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Division"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &Handle::LogAttributes};
#endif
  int NumberOfChildren(const TypeTreeBlock * treeBlock) const override { return 2; }
  void BasicReduction(TypeTreeBlock * treeBlock) const override;
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
  static void LogNodeName(std::ostream & stream) { stream << "Ghost"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &Handle::LogAttributes};
#endif
};
#endif

class Integer final : public InternalHandle {
public:
  static TypeTreeBlock * PushNode(int value);
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Integer"; }
  static void LogAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream);
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &LogAttributes};
#endif
  size_t NodeSize(const TypeTreeBlock * typeTreeBlock, bool head = true) const override;
  static int Value(const TypeTreeBlock * treeBlock);

private:
  constexpr static size_t k_minimalNumberOfNodes = 4;
  constexpr static size_t k_maxValue = 1 << 8;
  size_t NodeSize(const TypeTreeBlock * typeTreeBlock, NextStep step) const;
};

class NAry : public InternalHandle {
public:
#if POINCARE_TREE_LOG
  static void LogAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream);
#endif
  int NumberOfChildren(const TypeTreeBlock * treeBlock) const override;
  size_t NodeSize(const TypeTreeBlock * typeTreeBlock, bool head = true) const override { return 3; }

protected:
  static TypeTreeBlock * PushNode(int numberOfChildren, TypeTreeBlock blockType);
};

class Addition final : public NAry {
public:
  static TypeTreeBlock * PushNode(int numberOfChildren);
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Addition"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &LogAttributes};
#endif

  static int CollectChildren(TypeTreeBlock * treeBlock);
  static TypeTreeBlock * Merge(TypeTreeBlock * treeBlock);
};

class Multiplication final : public NAry {
public:
  static TypeTreeBlock * PushNode(int numberOfChildren);
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Multiplication"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &LogAttributes};
#endif

  static TypeTreeBlock * DistributeOverAddition(TypeTreeBlock * treeBlock);
};

class Power final : public InternalHandle {
public:
  static TypeTreeBlock * PushNode();
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Power"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &Handle::LogAttributes};
#endif
  int NumberOfChildren(const TypeTreeBlock * treeBlock) const override { return 2; }
};

}

#endif
