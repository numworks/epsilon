#include <ion.h>

extern "C" {
#include <assert.h>
}

namespace Ion {
namespace Events {

class EventData {
public:
  static constexpr EventData Undefined() { return EventData(nullptr); }
  static constexpr EventData Textless() { return EventData(k_textless); }
  static constexpr EventData Text(const char * text) { return EventData(text); }
  bool isUndefined() const { return (m_data == nullptr); }
  const char * text() const;
private:
  static constexpr const char * k_textless = "";
  constexpr EventData(const char * data) : m_data(data) {}
  const char * m_data;
};

#define TL() EventData::Textless()
#define U() EventData::Undefined()
#define T(x) EventData::Text(x)

static constexpr EventData s_dataForEvent[] = {
// Plain
  TL(), TL(), TL(), TL(), TL(), TL(),
  TL(), TL(), U(),   U(),  U(),  U(),
  TL(), TL(), TL(), TL(), TL(), TL(),
  T("exp()"), T("ln()"),  T("log()"), T("i"), T(","),      T("^"),
  T("sin()"), T("cos()"), T("tan()"), T("Pi"), T("v()"), T("^2"),
  T("7"), T("8"), T("9"), T("("), T(")"), U(),
  T("4"), T("5"), T("6"), T("*"), T("/"), U(),
  T("1"), T("2"), T("3"), T("+"), T("-"), U(),
  T("0"), T("."), T("E"), TL(), TL(), U(),
// Shift
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), TL(), TL(), TL(), TL(),
  T("["), T("]"), T("{"), T("}"), T("_"), T("sto"),
  T("asin()"), T("acos()"), T("atan()"), T("="), T("<"), T(">"),
  U(), U(), TL(), TL(), TL(), TL(),
  U(), U(), TL(), TL(), TL(), TL(),
  U(), U(), TL(), TL(), TL(), TL(),
  U(), U(), TL(), TL(), TL(), TL(),
// Alpha
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), T(":"), T(";"), U(),
  T("a"), T("b"), T("c"), T("d"), T("e"), T("f"),
  T("g"), T("h"), T("i"), T("j"), T("k"), T("l"),
  T("m"), T("n"), T("o"), T("p"), T("q"), U(),
  T("r"), T("s"), T("t"), T("u"), T("v"), U(),
  T("w"), T("x"), T("y"), T("z"), T(" "), U(),
  T("?"), T("!"), U(), U(), U(),
// Shift+Alpha
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  T("A"), T("B"), T("C"), T("D"), T("E"), T("F"),
  T("G"), T("H"), T("I"), T("J"), T("K"), T("L"),
  T("M"), T("N"), T("O"), T("P"), T("Q"), U(),
  T("R"), T("S"), T("T"), T("U"), T("V"), U(),
  T("W"), T("X"), T("Y"), T("Z"), T(" "), U(),
  U(), U(), U(), U(), U(), U(),
};

const char * EventData::text() const {
  if (m_data == nullptr || m_data == k_textless) {
    return nullptr;
  }
  return m_data;
}

Event::Event(Keyboard::Key key, bool shift, bool alpha) {
  // We're mapping a key, shift and alpha to an event
  // This can be a bit more complicated than it seems since we want to fall back:
  // for example, alpha-up is just plain up.
  // Fallback order :
  // shift-X -> X
  // alpha-X -> X
  // shift-alpha-X -> alpha-X -> X

  m_id = Events::None.m_id;

  int noFallbackOffsets[] = {0};
  int shiftFallbackOffsets[] = {k_eventPageSize, 0};
  int alphaFallbackOffsets[] = {2*k_eventPageSize, 0};
  int shiftAlphaFallbackOffsets[] = {3*k_eventPageSize, 2*k_eventPageSize, 0};

  int * fallbackOffsets[] = {noFallbackOffsets, shiftFallbackOffsets, alphaFallbackOffsets, shiftAlphaFallbackOffsets};

  int * fallbackOffset = fallbackOffsets[shift+2*alpha];
  int i=0;
  int offset = 0;
  do {
    offset = fallbackOffset[i++];
    m_id = offset + (int)key;
  } while (offset > 0 && s_dataForEvent[m_id].isUndefined());

  assert(m_id != Events::None.m_id);
}

const char * Event::text() const {
  return s_dataForEvent[m_id].text();
}

bool Event::hasText() const {
  return text() != nullptr;
}

}
}
