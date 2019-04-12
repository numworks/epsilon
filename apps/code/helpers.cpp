#include "helpers.h"
#include <string.h>
#include <ion/unicode/code_point.h>
#include <ion.h>

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

static_assert('\x11' == UCodePointEmpty, "Unicode error");
static constexpr EventTextPair sEventTextMap[] = {
  EventTextPair(Ion::Events::XNT, "x"),
  EventTextPair(Ion::Events::Exp, "exp(\x11)"),
  EventTextPair(Ion::Events::Ln, "log(\x11)"),
  EventTextPair(Ion::Events::Log, "log10(\x11)"),
  EventTextPair(Ion::Events::Imaginary, "1j"),
  EventTextPair(Ion::Events::Power, "**"),
  EventTextPair(Ion::Events::Pi, "pi"),
  EventTextPair(Ion::Events::Sqrt, "sqrt(\x11)"),
  EventTextPair(Ion::Events::Square, "**2"),
  EventTextPair(Ion::Events::Multiplication, "*"),
  EventTextPair(Ion::Events::EE, "e"),
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
