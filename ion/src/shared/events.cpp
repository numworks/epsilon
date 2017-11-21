#include <ion/events.h>
#include <ion/charset.h>

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
  bool isDefined() const { return (m_data != nullptr); }
  const char * text() const;
private:
  static constexpr const char * k_textless = "";
  constexpr EventData(const char * data) : m_data(data) {}
  const char * m_data;
};

#define TL() EventData::Textless()
#define U() EventData::Undefined()
#define T(x) EventData::Text(x)

static constexpr const char k_pi[2] = {Ion::Charset::SmallPi, 0};
static constexpr const char k_root[4] = {Ion::Charset::Root, '(', ')', 0};
static constexpr const char k_complexI[2] = {Ion::Charset::IComplex, 0};
static constexpr const char k_exponential[5] = {Ion::Charset::Exponential, '^', '(', ')', 0};
static constexpr const char k_sto[2] = {Ion::Charset::Sto, 0};
static constexpr const char k_exponent[2] = {Ion::Charset::Exponent, 0};
static constexpr const char k_multiplicationSign[2] = {Ion::Charset::MultiplicationSign, 0};

static constexpr EventData s_dataForEvent[4*Event::PageSize] = {
// Plain
  TL(), TL(), TL(), TL(), TL(), TL(),
  TL(), TL(), U(),   U(),  U(),  U(),
  TL(), TL(), TL(), TL(), TL(), TL(),
  T(k_exponential), T("ln()"),  T("log()"), T(k_complexI), T(","), T("^"),
  T("sin()"), T("cos()"), T("tan()"), T(k_pi), T(k_root), T("^2"),
  T("7"), T("8"), T("9"), T("("), T(")"), U(),
  T("4"), T("5"), T("6"), T(k_multiplicationSign), T("/"), U(),
  T("1"), T("2"), T("3"), T("+"), T("-"), U(),
  T("0"), T("."), T(k_exponent), TL(), TL(), U(),
// Shift
  TL(), U(), U(), TL(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), TL(), TL(), TL(), TL(),
  T("["), T("]"), T("{"), T("}"), T("_"), T(k_sto),
  T("asin()"), T("acos()"), T("atan()"), T("="), T("<"), T(">"),
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
// Alpha
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), T(":"), T(";"), T("\""), U(),
  T("a"), T("b"), T("c"), T("d"), T("e"), T("f"),
  T("g"), T("h"), T("i"), T("j"), T("k"), T("l"),
  T("m"), T("n"), T("o"), T("p"), T("q"), U(),
  T("r"), T("s"), T("t"), T("u"), T("v"), U(),
  T("w"), T("x"), T("y"), T("z"), T(" "), U(),
  T("?"), T("!"), U(), U(), U(), U(),
// Shift+Alpha
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  T("A"), T("B"), T("C"), T("D"), T("E"), T("F"),
  T("G"), T("H"), T("I"), T("J"), T("K"), T("L"),
  T("M"), T("N"), T("O"), T("P"), T("Q"), U(),
  T("R"), T("S"), T("T"), T("U"), T("V"), U(),
  T("W"), T("X"), T("Y"), T("Z"), U(), U(),
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
  assert((int)key >= 0 && (int)key < Keyboard::NumberOfKeys);
  m_id = Events::None.m_id;

  int noFallbackOffsets[] = {0};
  int shiftFallbackOffsets[] = {PageSize, 0};
  int alphaFallbackOffsets[] = {2*PageSize, 0};
  int shiftAlphaFallbackOffsets[] = {3*PageSize, 2*PageSize, 0};

  int * fallbackOffsets[] = {noFallbackOffsets, shiftFallbackOffsets, alphaFallbackOffsets, shiftAlphaFallbackOffsets};

  int * fallbackOffset = fallbackOffsets[shift+2*alpha];
  int i=0;
  int offset = 0;
  do {
    offset = fallbackOffset[i++];
    m_id = offset + (int)key;
  } while (offset > 0 && !s_dataForEvent[m_id].isDefined() && m_id < 4*PageSize);

  assert(m_id != Events::None.m_id);
}

const char * Event::text() const {
  if (m_id >= 4*PageSize) {
    return nullptr;
  }
  return s_dataForEvent[m_id].text();
}

bool Event::hasText() const {
  return text() != nullptr;
}

bool Event::isDefined() const {
  if (isKeyboardEvent()) {
    return s_dataForEvent[m_id].isDefined();
  } else {
    return (*this == None || *this == Termination);
  }
}

#if DEBUG

static constexpr const char * s_nameForEvent[255] = {
 // Plain
  "Left", "Up", "Down", "Right", "OK", "Back",
  "Home", "OnOff", nullptr, nullptr, nullptr, nullptr,
  "Shift", "Alpha", "XNT", "Var", "Toolbox", "Backspace",
  "Exp", "Ln", "Log", "Imaginary", "Comma", "Power",
  "Sine", "Cosine", "Tangent", "Pi", "Sqrt", "Square",
  "Seven", "Eight", "Nine", "LeftParenthesis", "RightParenthesis", nullptr,
  "Four", "Five", "Six", "Multiplication", "Division", nullptr,
  "One", "Two", "Three", "Plus", "Minus", nullptr,
  "Zero", "Dot", "EE", "Ans", "EXE", nullptr,
  //Shift,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, "AlphaLock", "Cut", "Copy", "Paste", "Clear",
  "LeftBracket", "RightBracket", "LeftBrace", "RightBrace", "Underscore", "Sto",
  "Arcsine", "Arccosine", "Arctangent", "Equal", "Lower", "Greater",
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  //Alpha,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, "Colon", "SemiColon", "DoubleQuotes", nullptr,
  "LowerA", "LowerB", "LowerC", "LowerD", "LowerE", "LowerF",
  "LowerG", "LowerH", "LowerI", "LowerJ", "LowerK", "LowerL",
  "LowerM", "LowerN", "LowerO", "LowerP", "LowerQ", nullptr,
  "LowerR", "LowerS", "LowerT", "LowerU", "LowerV", nullptr,
  "LowerW", "LowerX", "LowerY", "LowerZ", "Space", nullptr,
  "Question", "Exclamation", nullptr, nullptr, nullptr, nullptr,
  //Shift+Alpha,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  "UpperA", "UpperB", "UpperC", "UpperD", "UpperE", "UpperF",
  "UpperG", "UpperH", "UpperI", "UpperJ", "UpperK", "UpperL",
  "UpperM", "UpperN", "UpperO", "UpperP", "UpperQ", nullptr,
  "UpperR", "UpperS", "UpperT", "UpperU", "UpperV", nullptr,
  "UpperW", "UpperX", "UpperY", "UpperZ", nullptr, nullptr,
  // Special
  "None", "Termination", nullptr, nullptr, nullptr, nullptr,
};

const char * Event::name() const {
  return s_nameForEvent[m_id];
}

#endif

}
}
