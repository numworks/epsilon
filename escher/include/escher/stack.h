#ifndef ESCHER_STACK_H
#define ESCHER_STACK_H

namespace Escher {

class StackElement {
public:
  virtual bool isNull() const = 0;
  virtual void reset() = 0;
};

// All implementation are in header to avoid having to specify templates

template<typename T, int N>
class Stack {
public:
  void push(T element) {
    int stackDepth = depth();
    assert(stackDepth < N && stackElementAtIndex(stackDepth)->isNull());
    m_stack[stackDepth] = element;
  }

  T * elementAtIndex(int index) { return &m_stack[index]; }

  T pop() {
    int stackDepth = depth();
    if (stackDepth == 0) {
      return m_stack[0]; // It should be the empty element
    }
    T element = m_stack[stackDepth-1];
    stackElementAtIndex(stackDepth - 1)->reset();
    return element;
  }

  int depth() {
    int depth = 0;
    for (int i = 0; i < N; i++) {
      depth += (!stackElementAtIndex(i)->isNull());
    }
    return depth;
  }

  void resetStack() {
    for (int i = 0; i < N; i++) {
      stackElementAtIndex(i)->reset();
    }
  }

private:
  StackElement * stackElementAtIndex(int index) { return static_cast<StackElement *>(elementAtIndex(index)); }
  T m_stack[N];
};

}

#endif
