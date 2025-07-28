/* FIXME Some SHIFT_ALPHA_KEY define an unused Shift+Alpha event (e.g. there is
 * no upper-case Colon). The only consequence should be defining an unused
 * constexpr, which is not a big deal. */

SHIFT_KEY(Left, 0, 0, ShiftLeft)
SHIFT_KEY(Up, 0, 1, ShiftUp)
SHIFT_KEY(Down, 0, 2, ShiftDown)
SHIFT_KEY(Right, 0, 3, ShiftRight)
PLAIN_KEY(OK, 0, 4)
PLAIN_KEY(Back, 0, 5)

PLAIN_KEY(Home, 1, 0)
PLAIN_KEY(OnOff, 1, 2)

PLAIN_KEY(Shift, 2, 0)
SHIFT_KEY(Alpha, 2, 1, AlphaCaps)
SHIFT_ALPHA_KEY(XNT, 2, 2, Cut, Colon, UpperColonDummy)
SHIFT_ALPHA_KEY(Var, 2, 3, Copy, SemiColon, UpperSemiColonDummy)
SHIFT_ALPHA_KEY(Toolbox, 2, 4, Paste, DoubleQuotes, UpperDoubleQuotesDummy)
SHIFT_ALPHA_KEY(Backspace, 2, 5, Clear, Percent, UpperUnderscoreDummy)

SHIFT_ALPHA_KEY(Exp, 3, 0, LeftBracket, LowerA, UpperA)
SHIFT_ALPHA_KEY(Ln, 3, 1, RightBracket, LowerB, UpperB)
SHIFT_ALPHA_KEY(Log, 3, 2, LeftBrace, LowerC, UpperC)
SHIFT_ALPHA_KEY(Imaginary, 3, 3, RightBrace, LowerD, UpperD)
SHIFT_ALPHA_KEY(Comma, 3, 4, Underscore, LowerE, UpperE)
SHIFT_ALPHA_KEY(Power, 3, 5, Sto, LowerF, UpperF)

SHIFT_ALPHA_KEY(Sine, 4, 0, Arcsine, LowerG, UpperG)
SHIFT_ALPHA_KEY(Cosine, 4, 1, Arccosine, LowerH, UpperH)
SHIFT_ALPHA_KEY(Tangent, 4, 2, Arctangent, LowerI, UpperI)
SHIFT_ALPHA_KEY(Pi, 4, 3, Equal, LowerJ, UpperJ)
SHIFT_ALPHA_KEY(Sqrt, 4, 4, Lower, LowerK, UpperK)
SHIFT_ALPHA_KEY(Square, 4, 5, Greater, LowerL, UpperL)

ALPHA_KEY(Seven, 5, 0, LowerM, UpperM)
ALPHA_KEY(Eight, 5, 1, LowerN, UpperN)
ALPHA_KEY(Nine, 5, 2, LowerO, UpperO)
ALPHA_KEY(LeftParenthesis, 5, 3, LowerP, UpperP)
ALPHA_KEY(RightParenthesis, 5, 4, LowerQ, UpperQ)

ALPHA_KEY(Four, 6, 0, LowerR, UpperR)
ALPHA_KEY(Five, 6, 1, LowerS, UpperS)
ALPHA_KEY(Six, 6, 2, LowerT, UpperT)
ALPHA_KEY(Multiplication, 6, 3, LowerU, UpperU)
ALPHA_KEY(Division, 6, 4, LowerV, UpperV)

ALPHA_KEY(One, 7, 0, LowerW, UpperW)
ALPHA_KEY(Two, 7, 1, LowerX, UpperX)
ALPHA_KEY(Three, 7, 2, LowerY, UpperY)
ALPHA_KEY(Plus, 7, 3, LowerZ, UpperZ)
ALPHA_KEY(Minus, 7, 4, Space, UpperSpaceDummy)

ALPHA_KEY(Zero, 8, 0, Question, UpperQuestionDummy)
ALPHA_KEY(Dot, 8, 1, Exclamation, UpperExclamationDummy)
PLAIN_KEY(EE, 8, 2)
PLAIN_KEY(Ans, 8, 3)
PLAIN_KEY(EXE, 8, 4)
