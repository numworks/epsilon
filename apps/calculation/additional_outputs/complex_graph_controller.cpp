#include "complex_graph_controller.h"

using namespace Poincare;

namespace Calculation {

ComplexGraphController::ComplexGraphController() :
  ViewController(nullptr),
  m_complex(),
  m_graphView(&m_complex)
{
}

}

