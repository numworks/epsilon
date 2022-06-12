#ifndef __TEX_PARSER_H__
#define __TEX_PARSER_H__

#include <poincare_layouts.h>
#include <poincare/layout_helper.h>
#include <string.h>

using namespace Poincare;

namespace Reader
{
/// @brief Class used in the WordWrapTextView class to parse a Tex expression
class TexParser {
public:
  TexParser(const char * text, const char * endOfText);
  Layout getLayout();
private:
  Layout popBlock();
  Layout popText(char stop);
  Layout popCommand();

  // Expressions
  Layout popBinomCommand();
  Layout popCeilCommand();
  Layout popFloorCommand();
  Layout popFracCommand();
  Layout popIntsetCommand();
  Layout popLeftCommand();
  Layout popRightCommand();
  Layout popSqrtCommand();
  Layout popSpaceCommand();
  Layout popOverrightarrowCommand();
  Layout popOverlineCommand();

  //Symbols
  Layout popSymbolCommand(int SymbolIndex);

  const char * m_text;
  const char * m_endOfText;
  bool m_hasError;

  inline bool isCommandEnded(char c) const;

  // Expressions that require specific handling
  static constexpr char const * k_binomCommand = "binom";
  static constexpr char const * k_ceilCommand = "ceil";
  static constexpr char const * k_floorCommand = "floor";
  static constexpr char const * k_fracCommand = "frac";
  static constexpr char const * k_intsetCommand = "intset";
  static constexpr char const * k_leftCommand = "left";
  static constexpr char const * k_rightCommand = "right";
  static constexpr char const * k_sqrtCommand = "sqrt";
  static constexpr char const * k_spaceCommand = "space";
  static constexpr char const * k_overrightArrowCommand = "overrightarrow";
  static constexpr char const * k_overlineCommand = "overline";
  
};

}

#endif
