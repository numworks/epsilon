#include "helpers.h"
#include <string.h>
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

static constexpr const char k_exponential[7] = {'e', 'x', 'p', '(', Ion::Charset::Empty, ')', 0};
static constexpr const char k_logarithm[7] = {'l', 'o', 'g', '(', Ion::Charset::Empty, ')', 0};
static constexpr const char k_logarithm10[9] = {'l', 'o', 'g', '1', '0', '(', Ion::Charset::Empty, ')', 0};
static constexpr const char k_sqrt[8] = {'s', 'q', 'r', 't', '(', Ion::Charset::Empty, ')', 0};

static constexpr EventTextPair sEventTextMap[] = {
  EventTextPair(Ion::Events::XNT, "x"),
  EventTextPair(Ion::Events::Exp, k_exponential),
  EventTextPair(Ion::Events::Ln, k_logarithm),
  EventTextPair(Ion::Events::Log, k_logarithm10),
  EventTextPair(Ion::Events::Imaginary, "1j"),
  EventTextPair(Ion::Events::Power, "**"),
  EventTextPair(Ion::Events::Pi, "pi"),
  EventTextPair(Ion::Events::Sqrt, k_sqrt),
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
