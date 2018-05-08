#include "python_toolbox.h"
#include "../shared/toolbox_helpers.h"
#include <assert.h>
extern "C" {
#include <string.h>
#include <ctype.h>
}

namespace Code {

static constexpr int catalogChildrenCount = 94;
static constexpr int MathModuleChildrenCount = 43;
static constexpr int KandinskyModuleChildrenCount = 7;
static constexpr int CMathModuleChildrenCount = 13;
static constexpr int RandomModuleChildrenCount = 10;
static constexpr int conditionsChildrenCount = 9;
static constexpr int forLoopChildrenCount = 4;
static constexpr int functionsChildrenCount = 2;
static constexpr int ifStatementChildrenCount = 5;
static constexpr int loopsAndTestsChildrenCount = 4;
static constexpr int menuChildrenCount = 4;
static constexpr int modulesChildrenCount = 4;
static constexpr int whileLoopChildrenCount = 1;


const ToolboxMessageTree forLoopChildren[forLoopChildrenCount] = {
  ToolboxMessageTree(I18n::Message::ForInRange1ArgLoopWithArg, I18n::Message::Default, I18n::Message::ForInRange1ArgLoop),
  ToolboxMessageTree(I18n::Message::ForInRange2ArgsLoopWithArg, I18n::Message::Default, I18n::Message::ForInRange2ArgsLoop),
  ToolboxMessageTree(I18n::Message::ForInRange3ArgsLoopWithArg, I18n::Message::Default, I18n::Message::ForInRange3ArgsLoop),
  ToolboxMessageTree(I18n::Message::ForInListLoopWithArg, I18n::Message::Default, I18n::Message::ForInListLoop)};

const ToolboxMessageTree ifStatementChildren[ifStatementChildrenCount] = {
  ToolboxMessageTree(I18n::Message::IfElseStatementWithArg, I18n::Message::Default, I18n::Message::IfElseStatement),
  ToolboxMessageTree(I18n::Message::IfThenStatementWithArg, I18n::Message::Default, I18n::Message::IfThenStatement),
  ToolboxMessageTree(I18n::Message::IfElifElseStatementWithArg, I18n::Message::Default, I18n::Message::IfElifElseStatement),
  ToolboxMessageTree(I18n::Message::IfAndIfElseStatementWithArg, I18n::Message::Default, I18n::Message::IfAndIfElseStatement),
  ToolboxMessageTree(I18n::Message::IfOrIfElseStatementWithArg, I18n::Message::Default, I18n::Message::IfOrIfElseStatement)};

const ToolboxMessageTree whileLoopChildren[whileLoopChildrenCount] = {
  ToolboxMessageTree(I18n::Message::WhileLoopWithArg, I18n::Message::Default, I18n::Message::WhileLoop)};

const ToolboxMessageTree conditionsChildren[conditionsChildrenCount] = {
  ToolboxMessageTree(I18n::Message::EqualityConditionWithArg, I18n::Message::Default, I18n::Message::EqualityCondition),
  ToolboxMessageTree(I18n::Message::NonEqualityConditionWithArg, I18n::Message::Default, I18n::Message::NonEqualityCondition),
  ToolboxMessageTree(I18n::Message::SuperiorStrictConditionWithArg, I18n::Message::Default, I18n::Message::SuperiorStrictCondition),
  ToolboxMessageTree(I18n::Message::InferiorStrictConditionWithArg, I18n::Message::Default, I18n::Message::InferiorStrictCondition),
  ToolboxMessageTree(I18n::Message::SuperiorConditionWithArg, I18n::Message::Default, I18n::Message::SuperiorCondition),
  ToolboxMessageTree(I18n::Message::InferiorConditionWithArg, I18n::Message::Default, I18n::Message::InferiorCondition),
  ToolboxMessageTree(I18n::Message::ConditionAnd, I18n::Message::Default, I18n::Message::ConditionAnd),
  ToolboxMessageTree(I18n::Message::ConditionOr, I18n::Message::Default, I18n::Message::ConditionOr),
  ToolboxMessageTree(I18n::Message::ConditionNot, I18n::Message::Default, I18n::Message::ConditionNot)};

const ToolboxMessageTree loopsAndTestsChildren[loopsAndTestsChildrenCount] = {
  ToolboxMessageTree(I18n::Message::ForLoopMenu, I18n::Message::Default, I18n::Message::Default, forLoopChildren, forLoopChildrenCount),
  ToolboxMessageTree(I18n::Message::IfStatementMenu, I18n::Message::Default, I18n::Message::Default, ifStatementChildren, ifStatementChildrenCount),
  ToolboxMessageTree(I18n::Message::WhileLoopMenu, I18n::Message::Default, I18n::Message::Default, whileLoopChildren, whileLoopChildrenCount),
  ToolboxMessageTree(I18n::Message::ConditionsMenu, I18n::Message::Default, I18n::Message::Default, conditionsChildren, conditionsChildrenCount)};

const ToolboxMessageTree MathModuleChildren[MathModuleChildrenCount] = {
  ToolboxMessageTree(I18n::Message::PythonCommandImportMath, I18n::Message::PythonImportMath, I18n::Message::PythonCommandImportMath),
  ToolboxMessageTree(I18n::Message::PythonCommandImportFromMath, I18n::Message::PythonImportFromMath, I18n::Message::PythonCommandImportFromMath),
  ToolboxMessageTree(I18n::Message::PythonCommandMathFunction, I18n::Message::PythonMathFunction, I18n::Message::PythonCommandMathFunctionWithoutArg),
  ToolboxMessageTree(I18n::Message::PythonCommandConstantE, I18n::Message::PythonConstantE, I18n::Message::PythonCommandConstantE),
  ToolboxMessageTree(I18n::Message::PythonCommandConstantPi, I18n::Message::PythonConstantPi, I18n::Message::PythonCommandConstantPi),
  ToolboxMessageTree(I18n::Message::PythonCommandSqrt, I18n::Message::PythonSqrt, I18n::Message::PythonCommandSqrt),
  ToolboxMessageTree(I18n::Message::PythonCommandPower, I18n::Message::PythonPower, I18n::Message::PythonCommandPower),
  ToolboxMessageTree(I18n::Message::PythonCommandExp, I18n::Message::PythonExp, I18n::Message::PythonCommandExp),
  ToolboxMessageTree(I18n::Message::PythonCommandExpm1, I18n::Message::PythonExpm1, I18n::Message::PythonCommandExpm1),
  ToolboxMessageTree(I18n::Message::PythonCommandLog, I18n::Message::PythonLog, I18n::Message::PythonCommandLog),
  ToolboxMessageTree(I18n::Message::PythonCommandLog2, I18n::Message::PythonLog2, I18n::Message::PythonCommandLog2),
  ToolboxMessageTree(I18n::Message::PythonCommandLog10, I18n::Message::PythonLog10, I18n::Message::PythonCommandLog10),
  ToolboxMessageTree(I18n::Message::PythonCommandCosh, I18n::Message::PythonCosh, I18n::Message::PythonCommandCosh),
  ToolboxMessageTree(I18n::Message::PythonCommandSinh, I18n::Message::PythonSinh, I18n::Message::PythonCommandSinh),
  ToolboxMessageTree(I18n::Message::PythonCommandTanh, I18n::Message::PythonTanh, I18n::Message::PythonCommandTanh),
  ToolboxMessageTree(I18n::Message::PythonCommandAcosh, I18n::Message::PythonAcosh, I18n::Message::PythonCommandAcosh),
  ToolboxMessageTree(I18n::Message::PythonCommandAsinh, I18n::Message::PythonAsinh, I18n::Message::PythonCommandAsinh),
  ToolboxMessageTree(I18n::Message::PythonCommandAtanh, I18n::Message::PythonAtanh, I18n::Message::PythonCommandAtanh),
  ToolboxMessageTree(I18n::Message::PythonCommandCos, I18n::Message::PythonCos, I18n::Message::PythonCommandCos),
  ToolboxMessageTree(I18n::Message::PythonCommandSin, I18n::Message::PythonSin, I18n::Message::PythonCommandSin),
  ToolboxMessageTree(I18n::Message::PythonCommandTan, I18n::Message::PythonTan, I18n::Message::PythonCommandTan),
  ToolboxMessageTree(I18n::Message::PythonCommandAcos, I18n::Message::PythonAcos, I18n::Message::PythonCommandAcos),
  ToolboxMessageTree(I18n::Message::PythonCommandAsin, I18n::Message::PythonAsin, I18n::Message::PythonCommandAsin),
  ToolboxMessageTree(I18n::Message::PythonCommandAtan, I18n::Message::PythonAtan, I18n::Message::PythonCommandAtan),
  ToolboxMessageTree(I18n::Message::PythonCommandAtan2, I18n::Message::PythonAtan2, I18n::Message::PythonCommandAtan2),
  ToolboxMessageTree(I18n::Message::PythonCommandCeil, I18n::Message::PythonCeil, I18n::Message::PythonCommandCeil),
  ToolboxMessageTree(I18n::Message::PythonCommandCopySign, I18n::Message::PythonCopySign, I18n::Message::PythonCommandCopySign),
  ToolboxMessageTree(I18n::Message::PythonCommandFabs, I18n::Message::PythonFabs, I18n::Message::PythonCommandFabs),
  ToolboxMessageTree(I18n::Message::PythonCommandFloor, I18n::Message::PythonFloor, I18n::Message::PythonCommandFloor),
  ToolboxMessageTree(I18n::Message::PythonCommandFmod, I18n::Message::PythonFmod, I18n::Message::PythonCommandFmod),
  ToolboxMessageTree(I18n::Message::PythonCommandFrExp, I18n::Message::PythonFrExp, I18n::Message::PythonCommandFrExp),
  ToolboxMessageTree(I18n::Message::PythonCommandLdexp, I18n::Message::PythonLdexp, I18n::Message::PythonCommandLdexp),
  ToolboxMessageTree(I18n::Message::PythonCommandModf, I18n::Message::PythonModf, I18n::Message::PythonCommandModf),
  ToolboxMessageTree(I18n::Message::PythonCommandIsFinite, I18n::Message::PythonIsFinite, I18n::Message::PythonCommandIsFinite),
  ToolboxMessageTree(I18n::Message::PythonCommandIsInfinite, I18n::Message::PythonIsInfinite, I18n::Message::PythonCommandIsInfinite),
  ToolboxMessageTree(I18n::Message::PythonCommandIsNaN, I18n::Message::PythonIsNaN, I18n::Message::PythonCommandIsNaN),
  ToolboxMessageTree(I18n::Message::PythonCommandTrunc, I18n::Message::PythonTrunc, I18n::Message::PythonCommandTrunc),
  ToolboxMessageTree(I18n::Message::PythonCommandRadians, I18n::Message::PythonRadians, I18n::Message::PythonCommandRadians),
  ToolboxMessageTree(I18n::Message::PythonCommandDegrees, I18n::Message::PythonDegrees, I18n::Message::PythonCommandDegrees),
  ToolboxMessageTree(I18n::Message::PythonCommandErf, I18n::Message::PythonErf, I18n::Message::PythonCommandErf),
  ToolboxMessageTree(I18n::Message::PythonCommandErfc, I18n::Message::PythonErfc, I18n::Message::PythonCommandErfc),
  ToolboxMessageTree(I18n::Message::PythonCommandGamma, I18n::Message::PythonGamma, I18n::Message::PythonCommandGamma),
  ToolboxMessageTree(I18n::Message::PythonCommandLgamma, I18n::Message::PythonLgamma, I18n::Message::PythonCommandLgamma)};

const ToolboxMessageTree KandinskyModuleChildren[KandinskyModuleChildrenCount] = {
  ToolboxMessageTree(I18n::Message::PythonCommandImportKandinsky, I18n::Message::PythonImportKandinsky, I18n::Message::PythonCommandImportKandinsky),
  ToolboxMessageTree(I18n::Message::PythonCommandImportFromKandinsky, I18n::Message::PythonImportFromKandinsky, I18n::Message::PythonCommandImportFromKandinsky),
  ToolboxMessageTree(I18n::Message::PythonCommandKandinskyFunction, I18n::Message::PythonKandinskyFunction, I18n::Message::PythonCommandKandinskyFunctionWithoutArg),
  ToolboxMessageTree(I18n::Message::PythonCommandGetPixel, I18n::Message::PythonGetPixel, I18n::Message::PythonCommandGetPixel),
  ToolboxMessageTree(I18n::Message::PythonCommandSetPixel, I18n::Message::PythonSetPixel, I18n::Message::PythonCommandSetPixel),
  ToolboxMessageTree(I18n::Message::PythonCommandColor, I18n::Message::PythonColor, I18n::Message::PythonCommandColor),
  ToolboxMessageTree(I18n::Message::PythonCommandDrawString, I18n::Message::PythonDrawString, I18n::Message::PythonCommandDrawString)};

const ToolboxMessageTree RandomModuleChildren[RandomModuleChildrenCount] = {
  ToolboxMessageTree(I18n::Message::PythonCommandImportRandom, I18n::Message::PythonImportRandom, I18n::Message::PythonCommandImportRandom),
  ToolboxMessageTree(I18n::Message::PythonCommandImportFromRandom, I18n::Message::PythonImportFromRandom, I18n::Message::PythonCommandImportFromRandom),
  ToolboxMessageTree(I18n::Message::PythonCommandRandomFunction, I18n::Message::PythonRandomFunction, I18n::Message::PythonCommandRandomFunctionWithoutArg),
  ToolboxMessageTree(I18n::Message::PythonCommandGetrandbits, I18n::Message::PythonGetrandbits, I18n::Message::PythonCommandGetrandbits),
  ToolboxMessageTree(I18n::Message::PythonCommandSeed, I18n::Message::PythonSeed, I18n::Message::PythonCommandSeed),
  ToolboxMessageTree(I18n::Message::PythonCommandRandrange, I18n::Message::PythonRandrange, I18n::Message::PythonCommandRandrange),
  ToolboxMessageTree(I18n::Message::PythonCommandRandint, I18n::Message::PythonRandint, I18n::Message::PythonCommandRandint),
  ToolboxMessageTree(I18n::Message::PythonCommandChoice, I18n::Message::PythonChoice, I18n::Message::PythonCommandChoice),
  ToolboxMessageTree(I18n::Message::PythonCommandRandom, I18n::Message::PythonRandom, I18n::Message::PythonCommandRandom),
  ToolboxMessageTree(I18n::Message::PythonCommandUniform, I18n::Message::PythonUniform, I18n::Message::PythonCommandUniform)
};

const ToolboxMessageTree CMathModuleChildren[CMathModuleChildrenCount] = {
  ToolboxMessageTree(I18n::Message::PythonCommandImportCmath, I18n::Message::PythonImportCmath, I18n::Message::PythonCommandImportCmath),
  ToolboxMessageTree(I18n::Message::PythonCommandImportFromCmath, I18n::Message::PythonImportFromCmath, I18n::Message::PythonCommandImportFromCmath),
  ToolboxMessageTree(I18n::Message::PythonCommandCmathFunction, I18n::Message::PythonCmathFunction, I18n::Message::PythonCommandCmathFunctionWithoutArg),
  ToolboxMessageTree(I18n::Message::PythonCommandConstantE, I18n::Message::PythonConstantE, I18n::Message::PythonCommandConstantE),
  ToolboxMessageTree(I18n::Message::PythonCommandConstantPi, I18n::Message::PythonConstantPi, I18n::Message::PythonCommandConstantPi),
  ToolboxMessageTree(I18n::Message::PythonCommandPhase, I18n::Message::PythonPhase, I18n::Message::PythonCommandPhase),
  ToolboxMessageTree(I18n::Message::PythonCommandPolar, I18n::Message::PythonPolar, I18n::Message::PythonCommandPolar),
  ToolboxMessageTree(I18n::Message::PythonCommandRect, I18n::Message::PythonRect, I18n::Message::PythonCommandRect),
  ToolboxMessageTree(I18n::Message::PythonCommandExpComplex, I18n::Message::PythonExp, I18n::Message::PythonCommandExp),
  ToolboxMessageTree(I18n::Message::PythonCommandLogComplex, I18n::Message::PythonLog, I18n::Message::PythonCommandLog),
  ToolboxMessageTree(I18n::Message::PythonCommandSqrtComplex, I18n::Message::PythonSqrt, I18n::Message::PythonCommandSqrt),
  ToolboxMessageTree(I18n::Message::PythonCommandCosComplex, I18n::Message::PythonCos, I18n::Message::PythonCommandCos),
  ToolboxMessageTree(I18n::Message::PythonCommandSinComplex, I18n::Message::PythonSin, I18n::Message::PythonCommandSin)};

const ToolboxMessageTree modulesChildren[modulesChildrenCount] = {
  ToolboxMessageTree(I18n::Message::MathModule, I18n::Message::Default, I18n::Message::Default, MathModuleChildren, MathModuleChildrenCount),
  ToolboxMessageTree(I18n::Message::CmathModule, I18n::Message::Default, I18n::Message::Default, CMathModuleChildren, CMathModuleChildrenCount),
  ToolboxMessageTree(I18n::Message::RandomModule, I18n::Message::Default, I18n::Message::Default, RandomModuleChildren, RandomModuleChildrenCount),
  ToolboxMessageTree(I18n::Message::KandinskyModule, I18n::Message::Default, I18n::Message::Default, KandinskyModuleChildren, KandinskyModuleChildrenCount)};

const ToolboxMessageTree catalogChildren[catalogChildrenCount] = {
  ToolboxMessageTree(I18n::Message::PythonCommandPound, I18n::Message::PythonPound, I18n::Message::PythonCommandPound),
  ToolboxMessageTree(I18n::Message::PythonCommandPercent, I18n::Message::PythonPercent, I18n::Message::PythonCommandPercent),
  ToolboxMessageTree(I18n::Message::PythonCommand1J, I18n::Message::Python1J, I18n::Message::PythonCommand1J),
  ToolboxMessageTree(I18n::Message::PythonCommandLF, I18n::Message::PythonLF, I18n::Message::PythonCommandLF),
  ToolboxMessageTree(I18n::Message::PythonCommandTab, I18n::Message::PythonTab, I18n::Message::PythonCommandTab),
  ToolboxMessageTree(I18n::Message::PythonCommandAmpersand, I18n::Message::PythonAmpersand, I18n::Message::PythonCommandAmpersand),
  ToolboxMessageTree(I18n::Message::PythonCommandSymbolExp, I18n::Message::PythonSymbolExp, I18n::Message::PythonCommandSymbolExp),
  ToolboxMessageTree(I18n::Message::PythonCommandVerticalBar, I18n::Message::PythonVerticalBar, I18n::Message::PythonCommandVerticalBar),
  ToolboxMessageTree(I18n::Message::PythonCommandSingleQuote, I18n::Message::PythonSingleQuote, I18n::Message::PythonCommandSingleQuote),
  ToolboxMessageTree(I18n::Message::PythonCommandAbs, I18n::Message::PythonAbs, I18n::Message::PythonCommandAbs),
  ToolboxMessageTree(I18n::Message::PythonCommandAcos, I18n::Message::PythonAcos, I18n::Message::PythonCommandAcos),
  ToolboxMessageTree(I18n::Message::PythonCommandAcosh, I18n::Message::PythonAcosh, I18n::Message::PythonCommandAcosh),
  ToolboxMessageTree(I18n::Message::PythonCommandAsin, I18n::Message::PythonAsin, I18n::Message::PythonCommandAsin),
  ToolboxMessageTree(I18n::Message::PythonCommandAsinh, I18n::Message::PythonAsinh, I18n::Message::PythonCommandAsinh),
  ToolboxMessageTree(I18n::Message::PythonCommandAtan, I18n::Message::PythonAtan, I18n::Message::PythonCommandAtan),
  ToolboxMessageTree(I18n::Message::PythonCommandAtan2, I18n::Message::PythonAtan2, I18n::Message::PythonCommandAtan2),
  ToolboxMessageTree(I18n::Message::PythonCommandAtanh, I18n::Message::PythonAtanh, I18n::Message::PythonCommandAtanh),
  ToolboxMessageTree(I18n::Message::PythonCommandBin, I18n::Message::PythonBin, I18n::Message::PythonCommandBin),
  ToolboxMessageTree(I18n::Message::PythonCommandCeil, I18n::Message::PythonCeil, I18n::Message::PythonCommandCeil),
  ToolboxMessageTree(I18n::Message::PythonCommandChoice, I18n::Message::PythonChoice, I18n::Message::PythonCommandChoice),
  ToolboxMessageTree(I18n::Message::PythonCommandCmathFunction, I18n::Message::PythonCmathFunction, I18n::Message::PythonCommandCmathFunctionWithoutArg),
  ToolboxMessageTree(I18n::Message::PythonCommandColor, I18n::Message::PythonColor, I18n::Message::PythonCommandColor),
  ToolboxMessageTree(I18n::Message::PythonCommandComplex, I18n::Message::PythonComplex, I18n::Message::PythonCommandComplex),
  ToolboxMessageTree(I18n::Message::PythonCommandCopySign, I18n::Message::PythonCopySign, I18n::Message::PythonCommandCopySign),
  ToolboxMessageTree(I18n::Message::PythonCommandCos, I18n::Message::PythonCos, I18n::Message::PythonCommandCos),
  ToolboxMessageTree(I18n::Message::PythonCommandCosh, I18n::Message::PythonCosh, I18n::Message::PythonCommandCosh),
  ToolboxMessageTree(I18n::Message::PythonCommandDegrees, I18n::Message::PythonDegrees, I18n::Message::PythonCommandDegrees),
  ToolboxMessageTree(I18n::Message::PythonCommandDivMod, I18n::Message::PythonDivMod, I18n::Message::PythonCommandDivMod),
  ToolboxMessageTree(I18n::Message::PythonCommandDrawString, I18n::Message::PythonDrawString, I18n::Message::PythonCommandDrawString),
  ToolboxMessageTree(I18n::Message::PythonCommandConstantE, I18n::Message::PythonConstantE, I18n::Message::PythonCommandConstantE),
  ToolboxMessageTree(I18n::Message::PythonCommandErf, I18n::Message::PythonErf, I18n::Message::PythonCommandErf),
  ToolboxMessageTree(I18n::Message::PythonCommandErfc, I18n::Message::PythonErfc, I18n::Message::PythonCommandErfc),
  ToolboxMessageTree(I18n::Message::PythonCommandExp, I18n::Message::PythonExp, I18n::Message::PythonCommandExp),
  ToolboxMessageTree(I18n::Message::PythonCommandExpm1, I18n::Message::PythonExpm1, I18n::Message::PythonCommandExpm1),
  ToolboxMessageTree(I18n::Message::PythonCommandFabs, I18n::Message::PythonFabs, I18n::Message::PythonCommandFabs),
  ToolboxMessageTree(I18n::Message::PythonCommandFloor, I18n::Message::PythonFloor, I18n::Message::PythonCommandFloor),
  ToolboxMessageTree(I18n::Message::PythonCommandFmod, I18n::Message::PythonFmod, I18n::Message::PythonCommandFmod),
  ToolboxMessageTree(I18n::Message::PythonCommandFrExp, I18n::Message::PythonFrExp, I18n::Message::PythonCommandFrExp),
  ToolboxMessageTree(I18n::Message::PythonCommandImportFromCmath, I18n::Message::PythonImportFromCmath, I18n::Message::PythonCommandImportFromCmath),
  ToolboxMessageTree(I18n::Message::PythonCommandImportFromKandinsky, I18n::Message::PythonImportFromKandinsky, I18n::Message::PythonCommandImportFromKandinsky),
  ToolboxMessageTree(I18n::Message::PythonCommandImportFromMath, I18n::Message::PythonImportFromMath, I18n::Message::PythonCommandImportFromMath),
  ToolboxMessageTree(I18n::Message::PythonCommandImportFromRandom, I18n::Message::PythonImportFromRandom, I18n::Message::PythonCommandImportFromRandom),
  ToolboxMessageTree(I18n::Message::PythonCommandGamma, I18n::Message::PythonGamma, I18n::Message::PythonCommandGamma),
  ToolboxMessageTree(I18n::Message::PythonCommandGetPixel, I18n::Message::PythonGetPixel, I18n::Message::PythonCommandGetPixel),
  ToolboxMessageTree(I18n::Message::PythonCommandGetrandbits, I18n::Message::PythonGetrandbits, I18n::Message::PythonCommandGetrandbits),
  ToolboxMessageTree(I18n::Message::PythonCommandHex, I18n::Message::PythonHex, I18n::Message::PythonCommandHex),
  ToolboxMessageTree(I18n::Message::PythonCommandImportCmath, I18n::Message::PythonImportCmath, I18n::Message::PythonCommandImportCmath),
  ToolboxMessageTree(I18n::Message::PythonCommandImportKandinsky, I18n::Message::PythonImportKandinsky, I18n::Message::PythonCommandImportKandinsky),
  ToolboxMessageTree(I18n::Message::PythonCommandImportMath, I18n::Message::PythonImportMath, I18n::Message::PythonCommandImportMath),
  ToolboxMessageTree(I18n::Message::PythonCommandImportRandom, I18n::Message::PythonImportRandom, I18n::Message::PythonCommandImportRandom),
  ToolboxMessageTree(I18n::Message::PythonCommandInput, I18n::Message::PythonInput, I18n::Message::PythonCommandInput),
  ToolboxMessageTree(I18n::Message::PythonCommandInt, I18n::Message::PythonInt, I18n::Message::PythonCommandInt),
  ToolboxMessageTree(I18n::Message::PythonCommandIsFinite, I18n::Message::PythonIsFinite, I18n::Message::PythonCommandIsFinite),
  ToolboxMessageTree(I18n::Message::PythonCommandIsInfinite, I18n::Message::PythonIsInfinite, I18n::Message::PythonCommandIsInfinite),
  ToolboxMessageTree(I18n::Message::PythonCommandIsNaN, I18n::Message::PythonIsNaN, I18n::Message::PythonCommandIsNaN),
  ToolboxMessageTree(I18n::Message::PythonCommandKandinskyFunction, I18n::Message::PythonKandinskyFunction, I18n::Message::PythonCommandKandinskyFunctionWithoutArg),
  ToolboxMessageTree(I18n::Message::PythonCommandLdexp, I18n::Message::PythonLdexp, I18n::Message::PythonCommandLdexp),
  ToolboxMessageTree(I18n::Message::PythonCommandLength, I18n::Message::PythonLength, I18n::Message::PythonCommandLength),
  ToolboxMessageTree(I18n::Message::PythonCommandLgamma, I18n::Message::PythonLgamma, I18n::Message::PythonCommandLgamma),
  ToolboxMessageTree(I18n::Message::PythonCommandLog, I18n::Message::PythonLog, I18n::Message::PythonCommandLog),
  ToolboxMessageTree(I18n::Message::PythonCommandLog10, I18n::Message::PythonLog10, I18n::Message::PythonCommandLog10),
  ToolboxMessageTree(I18n::Message::PythonCommandLog2, I18n::Message::PythonLog2, I18n::Message::PythonCommandLog2),
  ToolboxMessageTree(I18n::Message::PythonCommandMathFunction, I18n::Message::PythonMathFunction, I18n::Message::PythonCommandMathFunctionWithoutArg),
  ToolboxMessageTree(I18n::Message::PythonCommandMax, I18n::Message::PythonMax, I18n::Message::PythonCommandMax),
  ToolboxMessageTree(I18n::Message::PythonCommandMin, I18n::Message::PythonMin, I18n::Message::PythonCommandMin),
  ToolboxMessageTree(I18n::Message::PythonCommandModf, I18n::Message::PythonModf, I18n::Message::PythonCommandModf),
  ToolboxMessageTree(I18n::Message::PythonCommandOct, I18n::Message::PythonOct, I18n::Message::PythonCommandOct),
  ToolboxMessageTree(I18n::Message::PythonCommandPhase, I18n::Message::PythonPhase, I18n::Message::PythonCommandPhase),
  ToolboxMessageTree(I18n::Message::PythonCommandConstantPi, I18n::Message::PythonConstantPi, I18n::Message::PythonCommandConstantPi),
  ToolboxMessageTree(I18n::Message::PythonCommandPolar, I18n::Message::PythonPolar, I18n::Message::PythonCommandPolar),
  ToolboxMessageTree(I18n::Message::PythonCommandPower, I18n::Message::PythonPower, I18n::Message::PythonCommandPower),
  ToolboxMessageTree(I18n::Message::PythonCommandPrint, I18n::Message::PythonPrint, I18n::Message::PythonCommandPrint),
  ToolboxMessageTree(I18n::Message::PythonCommandRadians, I18n::Message::PythonRadians, I18n::Message::PythonCommandRadians),
  ToolboxMessageTree(I18n::Message::PythonCommandRandint, I18n::Message::PythonRandint, I18n::Message::PythonCommandRandint),
  ToolboxMessageTree(I18n::Message::PythonCommandRandom, I18n::Message::PythonRandom, I18n::Message::PythonCommandRandom),
  ToolboxMessageTree(I18n::Message::PythonCommandRandomFunction, I18n::Message::PythonRandomFunction, I18n::Message::PythonCommandRandomFunctionWithoutArg),
  ToolboxMessageTree(I18n::Message::PythonCommandRandrange, I18n::Message::PythonRandrange, I18n::Message::PythonCommandRandrange),
  ToolboxMessageTree(I18n::Message::PythonCommandRangeStartStop, I18n::Message::PythonRangeStartStop, I18n::Message::PythonCommandRangeStartStop),
  ToolboxMessageTree(I18n::Message::PythonCommandRangeStop, I18n::Message::PythonRangeStop, I18n::Message::PythonCommandRangeStop),
  ToolboxMessageTree(I18n::Message::PythonCommandRect, I18n::Message::PythonRect, I18n::Message::PythonCommandRect),
  ToolboxMessageTree(I18n::Message::PythonCommandRound, I18n::Message::PythonRound, I18n::Message::PythonCommandRound),
  ToolboxMessageTree(I18n::Message::PythonCommandSetPixel, I18n::Message::PythonSetPixel, I18n::Message::PythonCommandSetPixel),
  ToolboxMessageTree(I18n::Message::PythonCommandSeed, I18n::Message::PythonSeed, I18n::Message::PythonCommandSeed),
  ToolboxMessageTree(I18n::Message::PythonCommandSin, I18n::Message::PythonSin, I18n::Message::PythonCommandSin),
  ToolboxMessageTree(I18n::Message::PythonCommandSinh, I18n::Message::PythonSinh, I18n::Message::PythonCommandSinh),
  ToolboxMessageTree(I18n::Message::PythonCommandSorted, I18n::Message::PythonSorted, I18n::Message::PythonCommandSorted),
  ToolboxMessageTree(I18n::Message::PythonCommandSqrt, I18n::Message::PythonSqrt, I18n::Message::PythonCommandSqrt),
  ToolboxMessageTree(I18n::Message::PythonCommandSum, I18n::Message::PythonSum, I18n::Message::PythonCommandSum),
  ToolboxMessageTree(I18n::Message::PythonCommandTan, I18n::Message::PythonTan, I18n::Message::PythonCommandTan),
  ToolboxMessageTree(I18n::Message::PythonCommandTanh, I18n::Message::PythonTanh, I18n::Message::PythonCommandTanh),
  ToolboxMessageTree(I18n::Message::PythonCommandTrunc, I18n::Message::PythonTrunc, I18n::Message::PythonCommandTrunc),
  ToolboxMessageTree(I18n::Message::PythonCommandUniform, I18n::Message::PythonUniform, I18n::Message::PythonCommandUniform),
  ToolboxMessageTree(I18n::Message::PythonCommandImag, I18n::Message::PythonImag, I18n::Message::PythonCommandImagWithoutArg),
  ToolboxMessageTree(I18n::Message::PythonCommandReal, I18n::Message::PythonReal, I18n::Message::PythonCommandRealWithoutArg)
};

const ToolboxMessageTree functionsChildren[functionsChildrenCount] = {
  ToolboxMessageTree(I18n::Message::PythonCommandDefWithArg, I18n::Message::Default, I18n::Message::PythonCommandDef),
  ToolboxMessageTree(I18n::Message::PythonCommandReturn, I18n::Message::Default, I18n::Message::PythonCommandReturn)};

const ToolboxMessageTree menu[menuChildrenCount] = {
  ToolboxMessageTree(I18n::Message::LoopsAndTests, I18n::Message::Default, I18n::Message::Default, loopsAndTestsChildren, loopsAndTestsChildrenCount),
  ToolboxMessageTree(I18n::Message::Modules, I18n::Message::Default, I18n::Message::Default, modulesChildren, modulesChildrenCount),
  ToolboxMessageTree(I18n::Message::Catalog, I18n::Message::Default, I18n::Message::Default, catalogChildren, catalogChildrenCount),
  ToolboxMessageTree(I18n::Message::Functions, I18n::Message::Default, I18n::Message::Default, functionsChildren, functionsChildrenCount)};

const ToolboxMessageTree toolboxModel = ToolboxMessageTree(I18n::Message::Toolbox, I18n::Message::Default, I18n::Message::Default, menu, menuChildrenCount);


PythonToolbox::PythonToolbox() :
  Toolbox(nullptr, I18n::translate(rootModel()->label()))
{
}

bool PythonToolbox::handleEvent(Ion::Events::Event event) {
  if (Toolbox::handleEvent(event)) {
    return true;
  }
  if (event.hasText() && strlen(event.text()) == 1) {
    scrollToLetter(event.text()[0]);
    return true;
  }
  return false;
}

KDCoordinate PythonToolbox::rowHeight(int j) {
  if (typeAtLocation(0, j) == Toolbox::LeafCellType && m_messageTreeModel->label() == I18n::Message::IfStatementMenu) {
      /* To get the exact height needed for each cell, we have to compute its
       * text size, which means scan the text char by char to look for '\n'
       * chars. This is very costly and ruins the speed performance when
       * scrolling at the bottom of a long table: to compute a position on the
       * kth row, we call cumulatedHeightFromIndex(k), which calls rowHeight k
       * times.
       * We thus decided to compute the real height only for the ifStatement
       * children of the toolbox, which is the only menu that has special height
       * rows. */
    const ToolboxMessageTree * messageTree = static_cast<const ToolboxMessageTree *>(m_messageTreeModel->children(j));
    return KDText::stringSize(I18n::translate(messageTree->label()), k_fontSize).height() + 2*Metric::TableCellLabelTopMargin + (messageTree->text() == I18n::Message::Default ? 0 : Toolbox::rowHeight(j));
  }
  return Toolbox::rowHeight(j);
}

bool PythonToolbox::selectLeaf(ToolboxMessageTree * selectedMessageTree) {
  m_selectableTableView.deselectTable();
  ToolboxMessageTree * node = selectedMessageTree;
  const char * editedText = I18n::translate(node->insertedText());
  char strippedEditedText[strlen(editedText)+1];
  Shared::ToolboxHelpers::TextToInsertForCommandMessage(node->insertedText(), strippedEditedText);
  TextInput * textInput = static_cast<TextInput *>(sender());
  textInput->handleEventWithText(strippedEditedText, true);
  app()->dismissModalViewController();
  return true;
}

const ToolboxMessageTree * PythonToolbox::rootModel() {
  return &toolboxModel;
}

MessageTableCellWithMessage * PythonToolbox::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

MessageTableCellWithChevron* PythonToolbox::nodeCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_nodeCells[index];
}

int PythonToolbox::maxNumberOfDisplayedRows() {
 return k_maxNumberOfDisplayedRows;
}

void PythonToolbox::scrollToLetter(char letter) {
  char lowerLetter = tolower(letter);
  // We look for a child MessageTree that starts with the wanted letter.
  for (int i = 0; i < m_messageTreeModel->numberOfChildren(); i++) {
    char currentFirstLetterLowered = tolower(I18n::translate(m_messageTreeModel->children(i)->label())[0]);
    if (currentFirstLetterLowered == lowerLetter) {
      scrollToAndSelectChild(i);
      return;
    }
  }
  // We did not find a child MessageTree that starts with the wanted letter.
  // We scroll to the first child MessageTree that starts with a letter higher
  // than the wanted letter.
  for (int i = 0; i < m_messageTreeModel->numberOfChildren(); i++) {
    char currentFirstLetterLowered = tolower(I18n::translate(m_messageTreeModel->children(i)->label())[0]);
    if (currentFirstLetterLowered >= lowerLetter && currentFirstLetterLowered <= 'z') {
      scrollToAndSelectChild(i);
      return;
    }
  }
}

void PythonToolbox::scrollToAndSelectChild(int i) {
  assert(i >=0 && i<m_messageTreeModel->numberOfChildren());
  m_selectableTableView.deselectTable();
  m_selectableTableView.scrollToCell(0, i);
  m_selectableTableView.selectCellAtLocation(0, i);
}

}

