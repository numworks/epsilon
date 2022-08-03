#include <ion/keyboard/layout_events.h>

namespace Ion {
namespace Events {

// TODO factorize in header without creating conflicts with template named T
#define TL() EventData::Textless()
#define U() EventData::Undefined()
#define T(x) EventData::Text(x)

static_assert('\x11' == UCodePointEmpty, "Unicode error");
const EventData s_dataForEvent[4 * Event::PageSize] = {
// Plain
  TL(), TL(), TL(), TL(), TL(), TL(),
  TL(), U(), TL(), U(),  U(),  U(),
  TL(), TL(), TL(), TL(), TL(), TL(),
  T("e^(\x11)"), T("ln(\x11)"),  T("log(\x11)"), T("i"), T(","), T("^"),
  T("sin(\x11)"), T("cos(\x11)"), T("tan(\x11)"), T("π"), T("√(\x11)"), T("^2"),
  T("7"), T("8"), T("9"), T("("), T(")"), U(),
  T("4"), T("5"), T("6"), T("×"), T("/"), U(),
  T("1"), T("2"), T("3"), T("+"), T("-"), U(),
  T("0"), T("."), T("ᴇ"), TL(), TL(), U(),
// Shift
  TL(), TL(), TL(), TL(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), TL(), TL(), TL(), TL(),
  T("["), T("]"), T("{"), T("}"), T("_"), T("→"),
  T("asin(\x11)"), T("acos(\x11)"), T("atan(\x11)"), T("="), T("<"), T(">"),
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
// Alpha
  U(), U(), U(), U(), U(), U(),
  U(), U(), U(), U(), U(), U(),
  U(), U(), T(":"), T(";"), T("\""), T("%"),
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

#ifndef NDEBUG

const char * const s_nameForEvent[255] = {
 // Plain
  "Left", "Up", "Down", "Right", "OK", "Back",
  "Home", "nullptr", "OnOff", nullptr, nullptr, nullptr,
  "Shift", "Alpha", "XNT", "Var", "Toolbox", "Backspace",
  "Exp", "Ln", "Log", "Imaginary", "Comma", "Power",
  "Sine", "Cosine", "Tangent", "Pi", "Sqrt", "Square",
  "Seven", "Eight", "Nine", "LeftParenthesis", "RightParenthesis", nullptr,
  "Four", "Five", "Six", "Multiplication", "Division", nullptr,
  "One", "Two", "Three", "Plus", "Minus", nullptr,
  "Zero", "Dot", "EE", "Ans", "EXE", nullptr,
  //Shift,
  "ShiftLeft", "ShiftUp", "ShiftDown", "ShiftRight", nullptr, nullptr,
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
  nullptr, nullptr, "Colon", "SemiColon", "DoubleQuotes", "Percent",
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
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
  // Special
  "None", "Termination", nullptr, "USBEnumeration", "USBPlug", "BatteryCharging",
};

#endif

}
}
