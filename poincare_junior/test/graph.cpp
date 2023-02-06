#include "print.h"
#include <poincare_junior/include/expression.h>

using namespace PoincareJ;

// Dummy Plot class

class Graph {
public:
  Graph(const char * text);
  float approximateAtAbscissa(float x) const;
private:
  constexpr static int k_bufferSize = 128;
  char m_functionText[k_bufferSize];
  PoincareJ::Expression m_function;
};

Graph::Graph(const char * text) {
  strlcpy(m_functionText, text, k_bufferSize);
  m_function = PoincareJ::Expression::ParseFromText(m_functionText);
}

float Graph::approximateAtAbscissa(float x) const {
  return m_function.approximate(x);
}

void testGraph() {
#if POINCARE_MEMORY_TREE_LOG
  std::cout << "\n---------------- Push Graph (1-2)/3/4 ----------------" << std::endl;
#endif
  Graph graph("cos(x)");
  float valueAt0 = graph.approximateAtAbscissa(0);
#if POINCARE_MEMORY_TREE_LOG
  std::cout << "Approximation = " << valueAt0 << std::endl;
#endif
}
QUIZ_CASE(pcj_graph) { testGraph(); }
