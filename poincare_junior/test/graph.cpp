#include <poincare_junior/include/expression.h>

#include "helper.h"

using namespace PoincareJ;

// Dummy Plot class

class Graph {
 public:
  Graph(const char* text);
  float approximateAtAbscissa() const;

 private:
  constexpr static int k_bufferSize = 128;
  char m_functionText[k_bufferSize];
  PoincareJ::Expression m_function;
};

Graph::Graph(const char* text) {
  strlcpy(m_functionText, text, k_bufferSize);
  m_function = PoincareJ::Expression::Parse(m_functionText);
}

float Graph::approximateAtAbscissa() const {
  return m_function.approximate<float>();
}

QUIZ_CASE(pcj_graph) {
  Graph graph("(1-2)/8/4");
  float valueAt0 = graph.approximateAtAbscissa();
  quiz_assert(valueAt0 == -0.03125);
}
