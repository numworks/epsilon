#include "helper.h"
#include <poincare_junior/include/expression.h>

using namespace PoincareJ;

// Dummy Plot class

class Graph {
public:
  Graph(const char * text);
  float approximateAtAbscissa() const;
private:
  constexpr static int k_bufferSize = 128;
  char m_functionText[k_bufferSize];
  PoincareJ::Expression m_function;
};

Graph::Graph(const char * text) {
  strlcpy(m_functionText, text, k_bufferSize);
  m_function = PoincareJ::Expression::Parse(m_functionText);
}

float Graph::approximateAtAbscissa() const {
  return m_function.approximate();
}

QUIZ_CASE(pcj_graph) {
#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n---------------- Push Graph (1-2)/3/4 ----------------" << std::endl;
#endif
  Graph graph("(1-2)/3/4");
  float valueAt0 = graph.approximateAtAbscissa();
#if POINCARE_MEMORY_TREE_LOG
  std::cout << "Approximation = " << valueAt0 << std::endl;
#endif
}
