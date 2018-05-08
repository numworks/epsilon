#include "helpers.h"
#include <string.h>

namespace Code {
namespace Helpers {

class EventTextPair {
public:
  constexpr EventTextPair(Ion::Events::Event event, const char * text) : m_event(event), m_text(text) {}
  Ion::Events::Event event() const { return m_event; }
  const char * text() const { return m_text; }
private:
  const Ion::Events::Event m_event;
  const char * m_text;
};

static constexpr EventTextPair sEventTextMap[] = {
  EventTextPair(Ion::Events::XNT, "x"),
  EventTextPair(Ion::Events::Exp, "exp()"),
  EventTextPair(Ion::Events::Ln, "log()"),
  EventTextPair(Ion::Events::Log, "log10()"),
  EventTextPair(Ion::Events::Sine, "sin()"),
  EventTextPair(Ion::Events::Cosine, "cos()"),
  EventTextPair(Ion::Events::Tangent, "tan()"),
  EventTextPair(Ion::Events::Imaginary, "1j"),
  EventTextPair(Ion::Events::Power, "**"),
  EventTextPair(Ion::Events::Pi, "pi"),
  EventTextPair(Ion::Events::Sqrt, "sqrt()"),
  EventTextPair(Ion::Events::Square, "**2"),
  EventTextPair(Ion::Events::Multiplication, "*"),
  EventTextPair(Ion::Events::EE, "e"),
  EventTextPair(Ion::Events::Arcsine, "asin()"),
  EventTextPair(Ion::Events::Arccosine, "acos()"),
  EventTextPair(Ion::Events::Arctangent, "atan()")
};

const char * PythonTextForEvent(Ion::Events::Event event) {
  for (size_t i=0; i<sizeof(sEventTextMap)/sizeof(sEventTextMap[0]); i++) {
    if (event == sEventTextMap[i].event()) {
      return sEventTextMap[i].text();
    }
  }
  return nullptr;
}

}
}
