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
typedef struct LogTreeBlockVTable {
  LogNameFunction m_logName;
  LogAttributeFunction m_logAttribute;
} LogTreeBlockVTable;
#endif
typedef void (*TreeFunction)(TypeTreeBlock *);
typedef size_t (*NodeSizeFunction)(const TypeTreeBlock *, bool);
typedef int (*NumberOfChildrenFunction)(const TypeTreeBlock *);

typedef struct TreeBlockVTable {
  constexpr TreeBlockVTable(TreeFunction basicReduction, NodeSizeFunction nodeSize, NumberOfChildrenFunction numberOfChildren) :
    m_basicReduction(basicReduction),
    m_nodeSize(nodeSize),
    m_numberOfChildren(numberOfChildren) {}
  TreeFunction m_basicReduction;
  NodeSizeFunction m_nodeSize;
  NumberOfChildrenFunction m_numberOfChildren;
} TreeBlockVTable;

typedef struct InternalTreeBlockVTable : TreeBlockVTable {
  constexpr InternalTreeBlockVTable(TreeFunction basicReduction, NodeSizeFunction nodeSize, NumberOfChildrenFunction numberOfChildren, TreeFunction beautify) :
    TreeBlockVTable(basicReduction, nodeSize, numberOfChildren),
    m_beautify(beautify) {}
  TreeFunction m_beautify;
} InternalTreeBlockVTable;

class Handle {
public:
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) {}
  static void LogAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream) {}
#endif
  static void BasicReduction(TypeTreeBlock * treeBlock) {}
  static size_t NodeSize(const TypeTreeBlock * treeBlock, bool head = true) { return 1; }
  static int NumberOfChildren(const TypeTreeBlock * treeBlock) { return 0; }
};

class Subtraction final : public Handle {
public:
  static TypeTreeBlock * PushNode();
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Subtraction"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &Handle::LogAttributes};
#endif
  static int NumberOfChildren(const TypeTreeBlock * treeBlock) { return 2; }
  static void BasicReduction(TypeTreeBlock * treeBlock);
  constexpr static TreeBlockVTable s_vTable = {&BasicReduction, &Handle::NodeSize, &NumberOfChildren};
};

class Division final : public Handle {
public:
  static TypeTreeBlock * PushNode();
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Division"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &Handle::LogAttributes};
#endif
  static int NumberOfChildren(const TypeTreeBlock * treeBlock) { return 2; }
  static void BasicReduction(TypeTreeBlock * treeBlock);
  constexpr static TreeBlockVTable s_vTable = {&BasicReduction, &Handle::NodeSize, &NumberOfChildren};
};

class InternalHandle : public Handle {
public:
  using Handle::Handle;
  static void Beautify(TypeTreeBlock * treeBlock) {}
};

#if GHOST_REQUIRED
class Ghost final : public InternalHandle {
public:
  using InternalHandle::InternalHandle;
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Ghost"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &Handle::LogAttributes};
#endif
  constexpr static InternalTreeBlockVTable s_vTable = {&Handle::BasicReduction, &Handle::NodeSize, &Handle::NumberOfChildren, &InternalHandle::Beautify};
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
  static size_t NodeSize(const TypeTreeBlock * typeTreeBlock, bool head = true);
  static int Value(const TypeTreeBlock * treeBlock);
  constexpr static InternalTreeBlockVTable s_vTable = {&Handle::BasicReduction, &NodeSize, &Handle::NumberOfChildren, &InternalHandle::Beautify};

private:
  constexpr static size_t k_minimalNumberOfNodes = 4;
  constexpr static size_t k_maxValue = 1 << 8;
  static size_t NodeSize(const TypeTreeBlock * typeTreeBlock, NextStep step);
};

class NAry : public InternalHandle {
public:
#if POINCARE_TREE_LOG
  static void LogAttributes(const TypeTreeBlock * treeBlock, std::ostream & stream);
#endif
  static int NumberOfChildren(const TypeTreeBlock * treeBlock);
  static size_t NodeSize(const TypeTreeBlock * typeTreeBlock, bool head = true) { return 3; }

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
  constexpr static InternalTreeBlockVTable s_vTable = {&Handle::BasicReduction, &NodeSize, &NumberOfChildren, &InternalHandle::Beautify};
};

class Multiplication final : public NAry {
public:
  static TypeTreeBlock * PushNode(int numberOfChildren);
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Multiplication"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &LogAttributes};
#endif

  constexpr static InternalTreeBlockVTable s_vTable = {&Handle::BasicReduction, &NodeSize, &NumberOfChildren, &InternalHandle::Beautify};

  static TypeTreeBlock * DistributeOverAddition(TypeTreeBlock * treeBlock);
};

class Power final : public InternalHandle {
public:
  static TypeTreeBlock * PushNode();
#if POINCARE_TREE_LOG
  static void LogNodeName(std::ostream & stream) { stream << "Power"; }
  constexpr static LogTreeBlockVTable s_logVTable = {&LogNodeName, &Handle::LogAttributes};
#endif
  static int NumberOfChildren(const TypeTreeBlock * treeBlock) { return 2; }
  constexpr static InternalTreeBlockVTable s_vTable = {&Handle::BasicReduction, &Handle::NodeSize, &NumberOfChildren, &InternalHandle::Beautify};
};

}

#endif
