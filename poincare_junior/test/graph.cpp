#include "print.h"
#include "utils.h"
#include <poincare_junior/cached_tree.h>

using namespace Poincare;

// Dummy Plot class

class Graph {
public:
  Graph(const char * text);
  float approximateAtAbscissa(float x) const;
private:
  constexpr static int k_bufferSize = 128;
  char m_functionText[k_bufferSize];
  Poincare::CachedTree m_function;
};

Graph::Graph(const char * text) {
  strlcpy(m_functionText, text, k_bufferSize);
  m_function = Parse(m_functionText);
}

float Graph::approximateAtAbscissa(float x) const {
  return m_function.approximate(x);
}

void testGraph() {
  Graph graph("cos(x)");
  float valueAt0 = graph.approximateAtAbscissa(0);
  std::cout << "Approximation = " << valueAt0 << std::endl;
}
