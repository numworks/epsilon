#include "print.h"
#include <poincare_junior/expression_reference.h>

using namespace Poincare;

// Dummy Plot class

class Graph {
public:
  Graph(const char * text);
  float approximateAtAbscissa(float x) const;
private:
  constexpr static int k_bufferSize = 128;
  char m_functionText[k_bufferSize];
  Poincare::ExpressionReference m_function;
};

Graph::Graph(const char * text) {
  strlcpy(m_functionText, text, k_bufferSize);
  m_function = Poincare::ExpressionReference::Parse(m_functionText);
}

float Graph::approximateAtAbscissa(float x) const {
  return m_function.approximate(x);
}

void testGraph() {
  std::cout << "\n---------------- Push Graph (1-2)/3/4 ----------------" << std::endl;
  Graph graph("cos(x)");
  float valueAt0 = graph.approximateAtAbscissa(0);
  std::cout << "Approximation = " << valueAt0 << std::endl;
}
