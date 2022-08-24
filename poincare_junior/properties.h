#ifndef POINCARE_PROPERTIES_H
#define POINCARE_PROPERTIES_H

namespace Poincare {

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

class TreeBlock {
friend class TreePool;
public:
  constexpr TreeBlock(uint8_t content = 0) : m_content(content) {}
  bool operator==(const TreeBlock& b) const { return b.m_content == m_content; }
  bool operator!=(const TreeBlock& b) { return b.m_content != m_content; }

  // Block Navigation
  TreeBlock * nextBlock() { return this + 1; }
  const TreeBlock * nextBlock() const { return this + 1; }
  TreeBlock * nextNthBlock(int i) { return this + i; }
  const TreeBlock * nextNthBlock(int i) const { return this + i; }
  TreeBlock * previousBlock() { return this - 1; }
  const TreeBlock * previousBlock() const { return this - 1; }
  TreeBlock * previousNthBlock(int i) { return this - i; }
  const TreeBlock * previousNthBlock(int i) const { return this - i; }

  uint8_t content() const { return m_content; }
protected:
  uint8_t m_content;
};

class ValueTreeBlock;
class TreeBlock;
typedef TreeBlock * (TreeBlock::*NextStep)();
typedef TreeBlock * (TreeBlock::*NextNthStep)(int i);

class Properties {
public:
  virtual constexpr size_t nodeSize(const TreeBlock * treeBlock, bool head = true) const { return 1; }
  virtual constexpr int numberOfChildren(const TreeBlock * treeBlock) const { return 0; }
};

class SubtractionProperties final : public Properties {
  constexpr int numberOfChildren(const TreeBlock * treeBlock) const override { return 2; }
};

class DivisionProperties final : public Properties {
  constexpr int numberOfChildren(const TreeBlock * treeBlock) const override { return 2; }
};

#if GHOST_REQUIRED
class GhostProperties final : public Properties {
public:
  using Properties::Properties;
};
#endif

class IntegerProperties final : public Properties {
  constexpr static size_t k_minimalNumberOfNodes = 4;
  constexpr static size_t k_maxValue = 1 << 8;
  constexpr size_t nodeSize(const TreeBlock * block, bool head = true) const override {
    // return k_minimalNumberOfNodes + static_cast<const ValueTreeBlock *>(head ? block->nextBlock() : block->previousBlock())->value();
    return k_minimalNumberOfNodes + (head ? block->nextBlock() : block->previousBlock())->content();
  }
};

class ConstantProperties final : public Properties {
public:
  constexpr size_t nodeSize(const TreeBlock * typeTreeBlock, bool head = true) const override { return 3; }
};

class NAryProperties : public Properties {
  constexpr int numberOfChildren(const TreeBlock * treeBlock) const override {
    return treeBlock->nextBlock()->content();
  }

  constexpr size_t nodeSize(const TreeBlock * typeTreeBlock, bool head = true) const override { return 3; }
};

class AdditionProperties final : public NAryProperties {
};

class MultiplicationProperties final : public NAryProperties {
};

class PowerProperties final : public Properties {
  constexpr int numberOfChildren(const TreeBlock * treeBlock) const override { return 2; }
};

}

#endif
