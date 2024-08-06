#ifndef ION_KEYBOARD_LAYOUT_LAYOUT_H
#define ION_KEYBOARD_LAYOUT_LAYOUT_H

#include <array>

namespace Ion {
namespace Keyboard {

// clang-format off
enum class Key : uint8_t {
  Left=0,  Up=1,     Down=2,    Right=3,           OK=4,              Back=5,
  Home=6,  /*B2=7,*/ OnOff=8,   /*B4=9,            B5=10,             B6=11, */
  Shift=12,Alpha=13, XNT=14,    Var=15,            Toolbox=16,        Backspace=17,
  Exp=18,  Ln=19,    Log=20,    Imaginary=21,      Comma=22,          Power=23,
  Sine=24, Cosine=25,Tangent=26,Pi=27,             Sqrt=28,           Square=29,
  Seven=30,Eight=31, Nine=32,   LeftParenthesis=33,RightParenthesis=34,// F6=35,
  Four=36, Five=37,  Six=38,    Multiplication=39, Division=40,        // G6=41,
  One=42,  Two=43,   Three=44,  Plus=45,           Minus=46,           // H6=47,
  Zero=48, Dot=49,   EE=50,     Ans=51,            EXE=52,             // I6=53,
  None = 54
};
// clang-format on

constexpr int NumberOfKeys = static_cast<int>(Key::None);

constexpr Key ValidKeys[] = {
    Key::Left,
    Key::Up,
    Key::Down,
    Key::Right,
    Key::OK,
    Key::Back,
    Key::Home,
    Key::OnOff,
    Key::Shift,
    Key::Alpha,
    Key::XNT,
    Key::Var,
    Key::Toolbox,
    Key::Backspace,
    Key::Exp,
    Key::Ln,
    Key::Log,
    Key::Imaginary,
    Key::Comma,
    Key::Power,
    Key::Sine,
    Key::Cosine,
    Key::Tangent,
    Key::Pi,
    Key::Sqrt,
    Key::Square,
    Key::Seven,
    Key::Eight,
    Key::Nine,
    Key::LeftParenthesis,
    Key::RightParenthesis,
    Key::Four,
    Key::Five,
    Key::Six,
    Key::Multiplication,
    Key::Division,
    Key::One,
    Key::Two,
    Key::Three,
    Key::Plus,
    Key::Minus,
    Key::Zero,
    Key::Dot,
    Key::EE,
    Key::Ans,
    Key::EXE,
};

constexpr int NumberOfValidKeys = std::size(ValidKeys);

}  // namespace Keyboard
}  // namespace Ion

#endif
