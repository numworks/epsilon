#include "python_toolbox_controller.h"

#include <apps/shared/toolbox_helpers.h>
#include <assert.h>
#include <ion/keyboard/layout_events.h>
#include <omg/utf8_helper.h>
extern "C" {
#include <ctype.h>
#include <string.h>
}

using namespace Escher;

namespace Code {

constexpr ToolboxMessage forLoopChildren[] = {
    ToolboxMessageLeaf(I18n::Message::ForInRange1ArgLoopWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::ForInRange1ArgLoop),
    ToolboxMessageLeaf(I18n::Message::ForInRange2ArgsLoopWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::ForInRange2ArgsLoop),
    ToolboxMessageLeaf(I18n::Message::ForInRange3ArgsLoopWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::ForInRange3ArgsLoop),
    ToolboxMessageLeaf(I18n::Message::ForInListLoopWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::ForInListLoop)};

constexpr ToolboxMessage ifStatementChildren[] = {
    ToolboxMessageLeaf(I18n::Message::IfElseStatementWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::IfElseStatement),
    ToolboxMessageLeaf(I18n::Message::IfThenStatementWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::IfThenStatement),
    ToolboxMessageLeaf(I18n::Message::IfElifElseStatementWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::IfElifElseStatement),
    ToolboxMessageLeaf(I18n::Message::IfAndIfElseStatementWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::IfAndIfElseStatement),
    ToolboxMessageLeaf(I18n::Message::IfOrIfElseStatementWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::IfOrIfElseStatement)};

constexpr ToolboxMessage whileLoopChildren[] = {
    ToolboxMessageLeaf(I18n::Message::WhileLoopWithArg, I18n::Message::Default,
                       false, I18n::Message::WhileLoop)};

constexpr ToolboxMessage conditionsChildren[] = {
    ToolboxMessageLeaf(I18n::Message::EqualityConditionWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::EqualityCondition),
    ToolboxMessageLeaf(I18n::Message::NonEqualityConditionWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::NonEqualityCondition),
    ToolboxMessageLeaf(I18n::Message::SuperiorStrictConditionWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::SuperiorStrictCondition),
    ToolboxMessageLeaf(I18n::Message::InferiorStrictConditionWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::InferiorStrictCondition),
    ToolboxMessageLeaf(I18n::Message::SuperiorConditionWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::SuperiorCondition),
    ToolboxMessageLeaf(I18n::Message::InferiorConditionWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::InferiorCondition),
    ToolboxMessageLeaf(I18n::Message::ConditionAnd, I18n::Message::Default,
                       false),
    ToolboxMessageLeaf(I18n::Message::ConditionOr, I18n::Message::Default,
                       false),
    ToolboxMessageLeaf(I18n::Message::NotCommandWithSpace,
                       I18n::Message::Default, false)};

constexpr ToolboxMessage loopsAndTestsChildren[] = {
    ToolboxMessageNode(I18n::Message::ForLoopMenu, forLoopChildren),
    ToolboxMessageNode(I18n::Message::IfStatementMenu, ifStatementChildren),
    ToolboxMessageNode(I18n::Message::WhileLoopMenu, whileLoopChildren),
    ToolboxMessageNode(I18n::Message::ConditionsMenu, conditionsChildren)};

constexpr ToolboxMessage MathModuleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportMath,
                       I18n::Message::PythonImportMath, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromMath,
                       I18n::Message::PythonImportMath, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMathFunction,
                       I18n::Message::PythonMathFunction, false,
                       I18n::Message::PythonCommandMathFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::E, I18n::Message::PythonConstantE, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandConstantPi,
                       I18n::Message::PythonConstantPi, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSqrt,
                       I18n::Message::PythonSqrt),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPower,
                       I18n::Message::PythonPower),
    ToolboxMessageLeaf(I18n::Message::PythonCommandExp,
                       I18n::Message::PythonExp),
    ToolboxMessageLeaf(I18n::Message::PythonCommandExpm1,
                       I18n::Message::PythonExpm1),
    ToolboxMessageLeaf(I18n::Message::LogCommandWithArg,
                       I18n::Message::PythonLog),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLog2,
                       I18n::Message::PythonLog2),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLog10,
                       I18n::Message::PythonLog10),
    ToolboxMessageLeaf(I18n::Message::CoshCommandWithArg,
                       I18n::Message::PythonCosh),
    ToolboxMessageLeaf(I18n::Message::SinhCommandWithArg,
                       I18n::Message::PythonSinh),
    ToolboxMessageLeaf(I18n::Message::TanhCommandWithArg,
                       I18n::Message::PythonTanh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAcosh,
                       I18n::Message::PythonAcosh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAsinh,
                       I18n::Message::PythonAsinh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAtanh,
                       I18n::Message::PythonAtanh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCos,
                       I18n::Message::PythonCos),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSin,
                       I18n::Message::PythonSin),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTan,
                       I18n::Message::PythonTan),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAcos,
                       I18n::Message::PythonAcos),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAsin,
                       I18n::Message::PythonAsin),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAtan,
                       I18n::Message::PythonAtan),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAtan2,
                       I18n::Message::PythonAtan2),
    ToolboxMessageLeaf(I18n::Message::CeilCommandWithArg,
                       I18n::Message::PythonCeil),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCopySign,
                       I18n::Message::PythonCopySign),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFabs,
                       I18n::Message::PythonFabs),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFactorial,
                       I18n::Message::Factorial),
    ToolboxMessageLeaf(I18n::Message::FloorCommandWithArg,
                       I18n::Message::PythonFloor),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFmod,
                       I18n::Message::PythonFmod),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFrExp,
                       I18n::Message::PythonFrExp),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGcd,
                       I18n::Message::PythonGcd),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLdexp,
                       I18n::Message::PythonLdexp),
    ToolboxMessageLeaf(I18n::Message::PythonCommandModf,
                       I18n::Message::PythonModf),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIsFinite,
                       I18n::Message::PythonIsFinite),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIsInfinite,
                       I18n::Message::PythonIsInfinite),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIsNaN,
                       I18n::Message::PythonIsNaN),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTrunc,
                       I18n::Message::PythonTrunc),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRadians,
                       I18n::Message::PythonRadians),
    ToolboxMessageLeaf(I18n::Message::PythonCommandDegrees,
                       I18n::Message::PythonDegrees),
    ToolboxMessageLeaf(I18n::Message::PythonCommandErf,
                       I18n::Message::PythonErf),
    ToolboxMessageLeaf(I18n::Message::PythonCommandErfc,
                       I18n::Message::PythonErfc),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGamma,
                       I18n::Message::PythonGamma),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLgamma,
                       I18n::Message::PythonLgamma)};

constexpr ToolboxMessage CMathModuleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportCmath,
                       I18n::Message::PythonImportCmath, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromCmath,
                       I18n::Message::PythonImportCmath, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCmathFunction,
                       I18n::Message::PythonCmathFunction, false,
                       I18n::Message::PythonCommandCmathFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::E, I18n::Message::PythonConstantE, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandConstantPi,
                       I18n::Message::PythonConstantPi, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPhase,
                       I18n::Message::PythonPhase),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPolar,
                       I18n::Message::PythonPolar),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRect,
                       I18n::Message::PythonRect),
    ToolboxMessageLeaf(I18n::Message::PythonCommandExpComplex,
                       I18n::Message::PythonExp),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLogComplex,
                       I18n::Message::PythonLog),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSqrtComplex,
                       I18n::Message::PythonSqrt),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCosComplex,
                       I18n::Message::PythonCos),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSinComplex,
                       I18n::Message::PythonSin)};

constexpr ToolboxMessage MatplotlibPyplotModuleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportMatplotlibPyplot,
                       I18n::Message::PythonImportMatplotlibPyplot, false),
    ToolboxMessageLeaf(
        I18n::Message::PythonCommandImportFromMatplotlibPyplotTrimmed,
        I18n::Message::PythonImportMatplotlibPyplot, false,
        I18n::Message::PythonCommandImportFromMatplotlibPyplot),
    ToolboxMessageLeaf(
        I18n::Message::PythonCommandMatplotlibPyplotFunction,
        I18n::Message::PythonMatplotlibPyplotFunction, false,
        I18n::Message::PythonCommandMatplotlibPyplotFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArrow,
                       I18n::Message::PythonArrow),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAxis,
                       I18n::Message::PythonAxis, false,
                       I18n::Message::PythonCommandAxisWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandBar,
                       I18n::Message::PythonBar),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGrid,
                       I18n::Message::PythonGrid),
    ToolboxMessageLeaf(I18n::Message::PythonCommandHist,
                       I18n::Message::PythonHist),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPlot,
                       I18n::Message::PythonPlot),
    ToolboxMessageLeaf(I18n::Message::PythonCommandScatter,
                       I18n::Message::PythonScatter),
    ToolboxMessageLeaf(I18n::Message::PythonCommandShow,
                       I18n::Message::PythonShow),
    ToolboxMessageLeaf(I18n::Message::PythonCommandText,
                       I18n::Message::PythonText),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorBlue,
                       I18n::Message::PythonColorBlue, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorRed,
                       I18n::Message::PythonColorRed, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorGreen,
                       I18n::Message::PythonColorGreen, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorYellow,
                       I18n::Message::PythonColorYellow, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorBrown,
                       I18n::Message::PythonColorBrown, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorBlack,
                       I18n::Message::PythonColorBlack, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorWhite,
                       I18n::Message::PythonColorWhite, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorPink,
                       I18n::Message::PythonColorPink, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorOrange,
                       I18n::Message::PythonColorOrange, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorPurple,
                       I18n::Message::PythonColorPurple, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorGray,
                       I18n::Message::PythonColorGray, false)};

constexpr ToolboxMessage TurtleModuleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportTurtle,
                       I18n::Message::PythonImportTurtle, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromTurtle,
                       I18n::Message::PythonImportTurtle, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTurtleFunction,
                       I18n::Message::PythonTurtleFunction, false,
                       I18n::Message::PythonCommandTurtleFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandForward,
                       I18n::Message::PythonTurtleForward),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandBackward,
                       I18n::Message::PythonTurtleBackward),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandRight,
                       I18n::Message::PythonTurtleRight),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandLeft,
                       I18n::Message::PythonTurtleLeft),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandGoto,
                       I18n::Message::PythonTurtleGoto),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandSetheading,
                       I18n::Message::PythonTurtleSetheading),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandCircle,
                       I18n::Message::PythonTurtleCircle),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandSpeed,
                       I18n::Message::PythonTurtleSpeed),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandPosition,
                       I18n::Message::PythonTurtlePosition, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandHeading,
                       I18n::Message::PythonTurtleHeading, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandDistance,
                       I18n::Message::PythonTurtleDistance),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandPendown,
                       I18n::Message::PythonTurtlePendown, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandPenup,
                       I18n::Message::PythonTurtlePenup, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandPensize,
                       I18n::Message::PythonTurtlePensize),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandIsdown,
                       I18n::Message::PythonTurtleIsdown, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandWrite,
                       I18n::Message::PythonTurtleWrite),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandReset,
                       I18n::Message::PythonTurtleReset, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandShowturtle,
                       I18n::Message::PythonTurtleShowturtle, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandHideturtle,
                       I18n::Message::PythonTurtleHideturtle, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandColor,
                       I18n::Message::PythonTurtleColor),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandColorMode,
                       I18n::Message::PythonTurtleColorMode),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorBlue,
                       I18n::Message::PythonColorBlue, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorRed,
                       I18n::Message::PythonColorRed, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorGreen,
                       I18n::Message::PythonColorGreen, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorYellow,
                       I18n::Message::PythonColorYellow, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorBrown,
                       I18n::Message::PythonColorBrown, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorBlack,
                       I18n::Message::PythonColorBlack, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorWhite,
                       I18n::Message::PythonColorWhite, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorPink,
                       I18n::Message::PythonColorPink, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorOrange,
                       I18n::Message::PythonColorOrange, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorPurple,
                       I18n::Message::PythonColorPurple, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorGray,
                       I18n::Message::PythonColorGray, false)};

constexpr ToolboxMessage RandomModuleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportRandom,
                       I18n::Message::PythonImportRandom, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromRandom,
                       I18n::Message::PythonImportRandom, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRandomFunction,
                       I18n::Message::PythonRandomFunction, false,
                       I18n::Message::PythonCommandRandomFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGetrandbits,
                       I18n::Message::PythonGetrandbits),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSeed,
                       I18n::Message::PythonSeed),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRandrange,
                       I18n::Message::PythonRandrange),
    ToolboxMessageLeaf(I18n::Message::RandintCommandWithArg,
                       I18n::Message::PythonRandint),
    ToolboxMessageLeaf(I18n::Message::PythonCommandChoice,
                       I18n::Message::PythonChoice),
    ToolboxMessageLeaf(I18n::Message::RandomCommandWithArg,
                       I18n::Message::RandomFloat, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandUniform,
                       I18n::Message::PythonUniform)};

constexpr ToolboxMessage KandinskyModuleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportKandinsky,
                       I18n::Message::PythonImportKandinsky, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromKandinsky,
                       I18n::Message::PythonImportKandinsky, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKandinskyFunction,
                       I18n::Message::PythonKandinskyFunction, false,
                       I18n::Message::PythonCommandKandinskyFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGetPixel,
                       I18n::Message::PythonGetPixel),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSetPixel,
                       I18n::Message::PythonSetPixel),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColor,
                       I18n::Message::PythonColor),
    ToolboxMessageLeaf(I18n::Message::PythonCommandDrawString,
                       I18n::Message::PythonDrawString),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFillRect,
                       I18n::Message::PythonFillRect)};

constexpr ToolboxMessage IonModuleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportIon,
                       I18n::Message::PythonImportIon, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromIon,
                       I18n::Message::PythonImportIon, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIonFunction,
                       I18n::Message::PythonIonFunction, false,
                       I18n::Message::PythonCommandIonFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIsKeyDown,
                       I18n::Message::PythonIsKeyDown),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyLeft,
                       I18n::Message::PythonKeyLeft, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyUp,
                       I18n::Message::PythonKeyUp, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyDown,
                       I18n::Message::PythonKeyDown, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyRight,
                       I18n::Message::PythonKeyRight, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyOk,
                       I18n::Message::PythonKeyOk, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyBack,
                       I18n::Message::PythonKeyBack, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyHome,
                       I18n::Message::PythonKeyHome, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyOnOff,
                       I18n::Message::PythonKeyOnOff, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyShift,
                       I18n::Message::PythonKeyShift, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyAlpha,
                       I18n::Message::PythonKeyAlpha, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyXnt,
                       I18n::Message::PythonKeyXnt, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyVar,
                       I18n::Message::PythonKeyVar, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyToolbox,
                       I18n::Message::PythonKeyToolbox, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyBackspace,
                       I18n::Message::PythonKeyBackspace, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyExp,
                       I18n::Message::PythonKeyExp, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyLn,
                       I18n::Message::PythonKeyLn, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyLog,
                       I18n::Message::PythonKeyLog, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyImaginary,
                       I18n::Message::PythonKeyImaginary, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyComma,
                       I18n::Message::PythonKeyComma, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyPower,
                       I18n::Message::PythonKeyPower, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeySine,
                       I18n::Message::PythonKeySine, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyCosine,
                       I18n::Message::PythonKeyCosine, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyTangent,
                       I18n::Message::PythonKeyTangent, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyPi,
                       I18n::Message::PythonKeyPi, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeySqrt,
                       I18n::Message::PythonKeySqrt, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeySquare,
                       I18n::Message::PythonKeySquare, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeySeven,
                       I18n::Message::PythonKeySeven, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyEight,
                       I18n::Message::PythonKeyEight, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyNine,
                       I18n::Message::PythonKeyNine, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyLeftParenthesis,
                       I18n::Message::PythonKeyLeftParenthesis, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyRightParenthesis,
                       I18n::Message::PythonKeyRightParenthesis, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyFour,
                       I18n::Message::PythonKeyFour, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyFive,
                       I18n::Message::PythonKeyFive, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeySix,
                       I18n::Message::PythonKeySix, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyMultiplication,
                       I18n::Message::PythonKeyMultiplication, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyDivision,
                       I18n::Message::PythonKeyDivision, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyOne,
                       I18n::Message::PythonKeyOne, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyTwo,
                       I18n::Message::PythonKeyTwo, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyThree,
                       I18n::Message::PythonKeyThree, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyPlus,
                       I18n::Message::PythonKeyPlus, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyMinus,
                       I18n::Message::PythonKeyMinus, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyZero,
                       I18n::Message::PythonKeyZero, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyDot,
                       I18n::Message::PythonKeyDot, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyEe,
                       I18n::Message::PythonKeyEe, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyAns,
                       I18n::Message::PythonKeyAns, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKeyExe,
                       I18n::Message::PythonKeyExe, false)};

constexpr ToolboxMessage TimeModuleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportTime,
                       I18n::Message::PythonImportTime, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromTime,
                       I18n::Message::PythonImportTime, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTimeFunction,
                       I18n::Message::PythonTimeFunction, false,
                       I18n::Message::PythonCommandTimeFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMonotonic,
                       I18n::Message::PythonMonotonic, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSleep,
                       I18n::Message::PythonSleep)};

// TODO: Add these commands in the catalog
constexpr ToolboxMessage NumpyModuleChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportNumpy,
                       I18n::Message::PythonImportNumpy, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromNumpy,
                       I18n::Message::PythonImportNumpy, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpyFunction,
                       I18n::Message::PythonNumpyFunction, false,
                       I18n::Message::PythonCommandNumpyFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArray,
                       I18n::Message::PythonArray),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArange,
                       I18n::Message::PythonArange),
    ToolboxMessageLeaf(I18n::Message::PythonCommandConcatenate,
                       I18n::Message::PythonConcatenate, false,
                       I18n::Message::PythonCommandConcatenateWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLinspace,
                       I18n::Message::PythonLinspace),
    ToolboxMessageLeaf(I18n::Message::PythonCommandOnes,
                       I18n::Message::PythonOnes),
    ToolboxMessageLeaf(I18n::Message::PythonCommandZeros,
                       I18n::Message::PythonZeros),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFlatten,
                       I18n::Message::PythonFlatten, false,
                       I18n::Message::PythonCommandFlattenWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandReshape,
                       I18n::Message::PythonReshape, false,
                       I18n::Message::PythonCommandReshapeWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandShape,
                       I18n::Message::PythonShape, false,
                       I18n::Message::PythonCommandShapeWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTolist,
                       I18n::Message::PythonTolist, false,
                       I18n::Message::PythonCommandTolistWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTranspose,
                       I18n::Message::PythonTranspose, false,
                       I18n::Message::PythonCommandTransposeWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArgmax,
                       I18n::Message::PythonArgmax),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArgmin,
                       I18n::Message::PythonArgmin),
    ToolboxMessageLeaf(I18n::Message::PythonCommandDot,
                       I18n::Message::PythonDot),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCross,
                       I18n::Message::PythonCross),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpyMax,
                       I18n::Message::PythonMax),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpyMin,
                       I18n::Message::PythonMin),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMean,
                       I18n::Message::PythonMean),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMedian,
                       I18n::Message::PythonMedian),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPolyfit,
                       I18n::Message::PythonPolyfit),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPolyVal,
                       I18n::Message::PythonPolyVal),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSize,
                       I18n::Message::PythonSize),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpySort,
                       I18n::Message::PythonNumpySort),
    ToolboxMessageLeaf(I18n::Message::PythonCommandStd,
                       I18n::Message::PythonStd),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpySum,
                       I18n::Message::PythonNumpySum)};

constexpr ToolboxMessage modulesChildren[] = {
    ToolboxMessageNode(I18n::Message::MathModule, MathModuleChildren),
    ToolboxMessageNode(I18n::Message::CmathModule, CMathModuleChildren),
    ToolboxMessageNode(I18n::Message::MatplotlibPyplotModule,
                       MatplotlibPyplotModuleChildren),
    ToolboxMessageNode(I18n::Message::NumpyModule, NumpyModuleChildren),
    ToolboxMessageNode(I18n::Message::TurtleModule, TurtleModuleChildren),
    ToolboxMessageNode(I18n::Message::RandomModule, RandomModuleChildren),
    ToolboxMessageNode(I18n::Message::KandinskyModule, KandinskyModuleChildren),
    ToolboxMessageNode(I18n::Message::IonModule, IonModuleChildren),
    ToolboxMessageNode(I18n::Message::TimeModule, TimeModuleChildren)};

constexpr ToolboxMessage catalogChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandPound,
                       I18n::Message::PythonPound, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPercent,
                       I18n::Message::PythonPercent, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommand1J, I18n::Message::Python1J,
                       false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLF, I18n::Message::PythonLF,
                       false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTab,
                       I18n::Message::PythonTab, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAmpersand,
                       I18n::Message::PythonAmpersand, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSymbolExp,
                       I18n::Message::PythonSymbolExp, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandVerticalBar,
                       I18n::Message::PythonVerticalBar, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSingleQuote,
                       I18n::Message::PythonSingleQuote, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAt, I18n::Message::PythonAt,
                       false),
    ToolboxMessageLeaf(I18n::Message::AbsCommandWithArg,
                       I18n::Message::PythonAbs),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAcos,
                       I18n::Message::PythonAcos),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAcosh,
                       I18n::Message::PythonAcosh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArange,
                       I18n::Message::PythonArange),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArgmax,
                       I18n::Message::PythonArgmax),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArgmin,
                       I18n::Message::PythonArgmin),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFlatten,
                       I18n::Message::PythonFlatten, false,
                       I18n::Message::PythonCommandFlattenWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandReshape,
                       I18n::Message::PythonReshape, false,
                       I18n::Message::PythonCommandReshapeWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandShape,
                       I18n::Message::PythonShape, false,
                       I18n::Message::PythonCommandShapeWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTolist,
                       I18n::Message::PythonTolist, false,
                       I18n::Message::PythonCommandTolistWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTranspose,
                       I18n::Message::PythonTranspose, false,
                       I18n::Message::PythonCommandTransposeWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArray,
                       I18n::Message::PythonArray),
    ToolboxMessageLeaf(I18n::Message::PythonCommandArrow,
                       I18n::Message::PythonArrow),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAsin,
                       I18n::Message::PythonAsin),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAsinh,
                       I18n::Message::PythonAsinh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAtan,
                       I18n::Message::PythonAtan),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAtan2,
                       I18n::Message::PythonAtan2),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAtanh,
                       I18n::Message::PythonAtanh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAxis,
                       I18n::Message::PythonAxis, false,
                       I18n::Message::PythonCommandAxisWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandBackward,
                       I18n::Message::PythonTurtleBackward),
    ToolboxMessageLeaf(I18n::Message::PythonCommandBar,
                       I18n::Message::PythonBar),
    ToolboxMessageLeaf(I18n::Message::PythonCommandBin,
                       I18n::Message::PythonBin),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorBlack,
                       I18n::Message::PythonColorBlack, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorBlue,
                       I18n::Message::PythonColorBlue, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorBrown,
                       I18n::Message::PythonColorBrown, false),
    ToolboxMessageLeaf(I18n::Message::CeilCommandWithArg,
                       I18n::Message::PythonCeil),
    ToolboxMessageLeaf(I18n::Message::PythonCommandChoice,
                       I18n::Message::PythonChoice),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandCircle,
                       I18n::Message::PythonTurtleCircle),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCmathFunction,
                       I18n::Message::PythonCmathFunction, false,
                       I18n::Message::PythonCommandCmathFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColor,
                       I18n::Message::PythonColor),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandColor,
                       I18n::Message::PythonTurtleColor),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandColorMode,
                       I18n::Message::PythonTurtleColorMode),
    ToolboxMessageLeaf(I18n::Message::PythonCommandComplex,
                       I18n::Message::PythonComplex),
    ToolboxMessageLeaf(I18n::Message::PythonCommandConcatenate,
                       I18n::Message::PythonConcatenate, false,
                       I18n::Message::PythonCommandConcatenateWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCopySign,
                       I18n::Message::PythonCopySign),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCos,
                       I18n::Message::PythonCos),
    ToolboxMessageLeaf(I18n::Message::CoshCommandWithArg,
                       I18n::Message::PythonCosh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCross,
                       I18n::Message::PythonCross),
    ToolboxMessageLeaf(I18n::Message::PythonCommandDegrees,
                       I18n::Message::PythonDegrees),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandDistance,
                       I18n::Message::PythonTurtleDistance),
    ToolboxMessageLeaf(I18n::Message::PythonCommandDivMod,
                       I18n::Message::PythonDivMod),
    ToolboxMessageLeaf(I18n::Message::PythonCommandDot,
                       I18n::Message::PythonDot),
    ToolboxMessageLeaf(I18n::Message::PythonCommandDrawString,
                       I18n::Message::PythonDrawString),
    ToolboxMessageLeaf(I18n::Message::E, I18n::Message::PythonConstantE, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandErf,
                       I18n::Message::PythonErf),
    ToolboxMessageLeaf(I18n::Message::PythonCommandErfc,
                       I18n::Message::PythonErfc),
    ToolboxMessageLeaf(I18n::Message::PythonCommandEval,
                       I18n::Message::PythonEval),
    ToolboxMessageLeaf(I18n::Message::PythonCommandExp,
                       I18n::Message::PythonExp),
    ToolboxMessageLeaf(I18n::Message::PythonCommandExpm1,
                       I18n::Message::PythonExpm1),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFabs,
                       I18n::Message::PythonFabs),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFactorial,
                       I18n::Message::Factorial),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFillRect,
                       I18n::Message::PythonFillRect),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFloat,
                       I18n::Message::PythonFloat),
    ToolboxMessageLeaf(I18n::Message::FloorCommandWithArg,
                       I18n::Message::PythonFloor),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFmod,
                       I18n::Message::PythonFmod),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandForward,
                       I18n::Message::PythonTurtleForward),
    ToolboxMessageLeaf(I18n::Message::PythonCommandFrExp,
                       I18n::Message::PythonFrExp),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromCmath,
                       I18n::Message::PythonImportCmath, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromIon,
                       I18n::Message::PythonImportIon, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromKandinsky,
                       I18n::Message::PythonImportKandinsky, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromMath,
                       I18n::Message::PythonImportMath, false),
    ToolboxMessageLeaf(
        I18n::Message::PythonCommandImportFromMatplotlibPyplotTrimmed,
        I18n::Message::PythonImportMatplotlibPyplot, false,
        I18n::Message::PythonCommandImportFromMatplotlibPyplot),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromNumpy,
                       I18n::Message::PythonImportNumpy, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromRandom,
                       I18n::Message::PythonImportRandom, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromTime,
                       I18n::Message::PythonImportTime, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportFromTurtle,
                       I18n::Message::PythonImportTurtle, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGamma,
                       I18n::Message::PythonGamma),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGcd,
                       I18n::Message::PythonGcd),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGetPixel,
                       I18n::Message::PythonGetPixel),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGetrandbits,
                       I18n::Message::PythonGetrandbits),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandGoto,
                       I18n::Message::PythonTurtleGoto),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorGray,
                       I18n::Message::PythonColorGray, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorGreen,
                       I18n::Message::PythonColorGreen, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandGrid,
                       I18n::Message::PythonGrid),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandHeading,
                       I18n::Message::PythonTurtleHeading, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandHex,
                       I18n::Message::PythonHex),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandHideturtle,
                       I18n::Message::PythonTurtleHideturtle, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandHist,
                       I18n::Message::PythonHist),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportCmath,
                       I18n::Message::PythonImportCmath, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportIon,
                       I18n::Message::PythonImportIon, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportKandinsky,
                       I18n::Message::PythonImportKandinsky, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportMath,
                       I18n::Message::PythonImportMath, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportMatplotlibPyplot,
                       I18n::Message::PythonImportMatplotlibPyplot, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportNumpy,
                       I18n::Message::PythonImportNumpy, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportRandom,
                       I18n::Message::PythonImportRandom, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportTime,
                       I18n::Message::PythonImportTime, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImportTurtle,
                       I18n::Message::PythonImportTurtle, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandInput,
                       I18n::Message::PythonInput),
    ToolboxMessageLeaf(I18n::Message::PythonCommandInt,
                       I18n::Message::PythonInt),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIonFunction,
                       I18n::Message::PythonIonFunction, false,
                       I18n::Message::PythonCommandIonFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandIsdown,
                       I18n::Message::PythonTurtleIsdown, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIsFinite,
                       I18n::Message::PythonIsFinite),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIsInfinite,
                       I18n::Message::PythonIsInfinite),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIsNaN,
                       I18n::Message::PythonIsNaN),
    ToolboxMessageLeaf(I18n::Message::PythonCommandKandinskyFunction,
                       I18n::Message::PythonKandinskyFunction, false,
                       I18n::Message::PythonCommandKandinskyFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIsKeyDown,
                       I18n::Message::PythonIsKeyDown),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLdexp,
                       I18n::Message::PythonLdexp),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandLeft,
                       I18n::Message::PythonTurtleLeft),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLength,
                       I18n::Message::PythonLength),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLgamma,
                       I18n::Message::PythonLgamma),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLinspace,
                       I18n::Message::PythonLinspace),
    ToolboxMessageLeaf(I18n::Message::PythonCommandAppend,
                       I18n::Message::PythonAppend, false,
                       I18n::Message::PythonCommandAppendWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandClear,
                       I18n::Message::PythonClear, false,
                       I18n::Message::PythonCommandClearWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandCount,
                       I18n::Message::PythonCount, false,
                       I18n::Message::PythonCommandCountWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandIndex,
                       I18n::Message::PythonIndex, false,
                       I18n::Message::PythonCommandIndexWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandInsert,
                       I18n::Message::PythonInsert, false,
                       I18n::Message::PythonCommandInsertWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPop,
                       I18n::Message::PythonPop, false,
                       I18n::Message::PythonCommandPopWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRemove,
                       I18n::Message::PythonRemove, false,
                       I18n::Message::PythonCommandRemoveWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandReverse,
                       I18n::Message::PythonReverse, false,
                       I18n::Message::PythonCommandReverseWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSort,
                       I18n::Message::PythonSort, false,
                       I18n::Message::PythonCommandSortWithoutArg),
    ToolboxMessageLeaf(I18n::Message::LogCommandWithArg,
                       I18n::Message::PythonLog),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLog10,
                       I18n::Message::PythonLog10),
    ToolboxMessageLeaf(I18n::Message::PythonCommandLog2,
                       I18n::Message::PythonLog2),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMathFunction,
                       I18n::Message::PythonMathFunction, false,
                       I18n::Message::PythonCommandMathFunctionWithoutArg),
    ToolboxMessageLeaf(
        I18n::Message::PythonCommandMatplotlibPyplotFunction,
        I18n::Message::PythonMatplotlibPyplotFunction, false,
        I18n::Message::PythonCommandMatplotlibPyplotFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpyMax,
                       I18n::Message::PythonMax),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMax,
                       I18n::Message::PythonMax),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMean,
                       I18n::Message::PythonMean),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMedian,
                       I18n::Message::PythonMedian),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpyMin,
                       I18n::Message::PythonMin),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMin,
                       I18n::Message::PythonMin),
    ToolboxMessageLeaf(I18n::Message::PythonCommandModf,
                       I18n::Message::PythonModf),
    ToolboxMessageLeaf(I18n::Message::PythonCommandMonotonic,
                       I18n::Message::PythonMonotonic, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpyFunction,
                       I18n::Message::PythonNumpyFunction, false,
                       I18n::Message::PythonCommandNumpyFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandOct,
                       I18n::Message::PythonOct),
    ToolboxMessageLeaf(I18n::Message::PythonCommandOnes,
                       I18n::Message::PythonOnes),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorOrange,
                       I18n::Message::PythonColorOrange, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandPendown,
                       I18n::Message::PythonTurtlePendown, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandPensize,
                       I18n::Message::PythonTurtlePensize),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandPenup,
                       I18n::Message::PythonTurtlePenup, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPhase,
                       I18n::Message::PythonPhase),
    ToolboxMessageLeaf(I18n::Message::PythonCommandConstantPi,
                       I18n::Message::PythonConstantPi, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorPink,
                       I18n::Message::PythonColorPink, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPlot,
                       I18n::Message::PythonPlot),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPolar,
                       I18n::Message::PythonPolar),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPolyfit,
                       I18n::Message::PythonPolyfit),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPolyVal,
                       I18n::Message::PythonPolyVal),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandPosition,
                       I18n::Message::PythonTurtlePosition, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPower,
                       I18n::Message::PythonPower),
    ToolboxMessageLeaf(I18n::Message::PythonCommandPrint,
                       I18n::Message::PythonPrint),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorPurple,
                       I18n::Message::PythonColorPurple, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRadians,
                       I18n::Message::PythonRadians),
    ToolboxMessageLeaf(I18n::Message::RandintCommandWithArg,
                       I18n::Message::PythonRandint),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRandomFunction,
                       I18n::Message::PythonRandomFunction, false,
                       I18n::Message::PythonCommandRandomFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::RandomCommandWithArg,
                       I18n::Message::RandomFloat, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRandrange,
                       I18n::Message::PythonRandrange),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRangeStartStop,
                       I18n::Message::PythonRangeStartStop),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRangeStop,
                       I18n::Message::PythonRangeStop),
    ToolboxMessageLeaf(I18n::Message::PythonCommandRect,
                       I18n::Message::PythonRect),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorRed,
                       I18n::Message::PythonColorRed, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandReset,
                       I18n::Message::PythonTurtleReset, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandRight,
                       I18n::Message::PythonTurtleRight),
    ToolboxMessageLeaf(I18n::Message::RoundCommandWithArg,
                       I18n::Message::PythonRound),
    ToolboxMessageLeaf(I18n::Message::PythonCommandScatter,
                       I18n::Message::PythonScatter),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSeed,
                       I18n::Message::PythonSeed),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSetPixel,
                       I18n::Message::PythonSetPixel),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandSetheading,
                       I18n::Message::PythonTurtleSetheading),
    ToolboxMessageLeaf(I18n::Message::PythonCommandShow,
                       I18n::Message::PythonShow),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandShowturtle,
                       I18n::Message::PythonTurtleShowturtle, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSin,
                       I18n::Message::PythonSin),
    ToolboxMessageLeaf(I18n::Message::SinhCommandWithArg,
                       I18n::Message::PythonSinh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSize,
                       I18n::Message::PythonSize),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSleep,
                       I18n::Message::PythonSleep),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpySort,
                       I18n::Message::PythonNumpySort),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSorted,
                       I18n::Message::PythonSort),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandSpeed,
                       I18n::Message::PythonTurtleSpeed),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSqrt,
                       I18n::Message::PythonSqrt),
    ToolboxMessageLeaf(I18n::Message::PythonCommandStd,
                       I18n::Message::PythonStd),
    ToolboxMessageLeaf(I18n::Message::PythonCommandNumpySum,
                       I18n::Message::PythonNumpySum),
    ToolboxMessageLeaf(I18n::Message::PythonCommandSum,
                       I18n::Message::PythonSum),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTan,
                       I18n::Message::PythonTan),
    ToolboxMessageLeaf(I18n::Message::TanhCommandWithArg,
                       I18n::Message::PythonTanh),
    ToolboxMessageLeaf(I18n::Message::PythonCommandText,
                       I18n::Message::PythonText),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTimeFunction,
                       I18n::Message::PythonTimeFunction, false,
                       I18n::Message::PythonCommandTimeFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTrunc,
                       I18n::Message::PythonTrunc),
    ToolboxMessageLeaf(I18n::Message::PythonCommandTurtleFunction,
                       I18n::Message::PythonTurtleFunction, false,
                       I18n::Message::PythonCommandTurtleFunctionWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandUniform,
                       I18n::Message::PythonUniform),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorWhite,
                       I18n::Message::PythonColorWhite, false),
    ToolboxMessageLeaf(I18n::Message::PythonTurtleCommandWrite,
                       I18n::Message::PythonTurtleWrite),
    ToolboxMessageLeaf(I18n::Message::PythonCommandColorYellow,
                       I18n::Message::PythonColorYellow, false),
    ToolboxMessageLeaf(I18n::Message::PythonCommandImag,
                       I18n::Message::PythonImag, false,
                       I18n::Message::PythonCommandImagWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandReal,
                       I18n::Message::PythonReal, false,
                       I18n::Message::PythonCommandRealWithoutArg),
    ToolboxMessageLeaf(I18n::Message::PythonCommandZeros,
                       I18n::Message::PythonZeros),
};

constexpr ToolboxMessage functionsChildren[] = {
    ToolboxMessageLeaf(I18n::Message::PythonCommandDefWithArg,
                       I18n::Message::Default, false,
                       I18n::Message::PythonCommandDef),
    ToolboxMessageLeaf(I18n::Message::PythonCommandReturn,
                       I18n::Message::Default)};

constexpr ToolboxMessage menu[] = {
    ToolboxMessageNode(I18n::Message::LoopsAndTests, loopsAndTestsChildren),
    ToolboxMessageNode(I18n::Message::Modules, modulesChildren),
    ToolboxMessageNode(I18n::Message::Catalog, catalogChildren),
    ToolboxMessageNode(I18n::Message::Functions, functionsChildren)};

constexpr ToolboxMessage toolboxModel =
    ToolboxMessageNode(I18n::Message::Toolbox, menu);

constexpr static bool catalogContainsAllChildren(
    const ToolboxMessage* list, int numberOfChildren,
    bool sameCaptionIsEnough = false) {
  constexpr int nCatalog = std::size(catalogChildren);
  for (int i = 0; i < numberOfChildren; i++) {
    bool isInCatalog = false;
    for (int j = 0; j < nCatalog; j++) {
      if (catalogChildren[j].leaf.text() == list[i].leaf.text() &&
          (sameCaptionIsEnough ||
           catalogChildren[j].leaf.label() == list[i].leaf.label())) {
        isInCatalog = true;
        break;
      }
    }
    if (!isInCatalog) {
      return false;
    }
  }
  return true;
}

static_assert(catalogContainsAllChildren(MathModuleChildren,
                                         std::size(MathModuleChildren)),
              "Some functions of math module are not in the python catalog.");
// cos(z) is same as cos(x) so having the same caption is enough for cmath
static_assert(catalogContainsAllChildren(CMathModuleChildren,
                                         std::size(CMathModuleChildren), true),
              "Some functions of cmath module are not in the python catalog.");
static_assert(
    catalogContainsAllChildren(MatplotlibPyplotModuleChildren,
                               std::size(MatplotlibPyplotModuleChildren)),
    "Some functions of matplotlib module are not in the python catalog.");
static_assert(catalogContainsAllChildren(NumpyModuleChildren,
                                         std::size(NumpyModuleChildren)),
              "Some functions of numpy module are not in the python catalog.");
static_assert(catalogContainsAllChildren(TurtleModuleChildren,
                                         std::size(TurtleModuleChildren)),
              "Some functions of turtle module are not in the python catalog.");
static_assert(catalogContainsAllChildren(RandomModuleChildren,
                                         std::size(RandomModuleChildren)),
              "Some functions of random module are not in the python catalog.");
static_assert(
    catalogContainsAllChildren(KandinskyModuleChildren,
                               std::size(KandinskyModuleChildren)),
    "Some functions of kandinsky module are not in the python catalog.");
static_assert(catalogContainsAllChildren(TimeModuleChildren,
                                         std::size(TimeModuleChildren)),
              "Some functions of time module are not in the python catalog.");
/* Most of Ion key functions are not in catalog so this is not asserted for
 * Ion submodule. */

PythonToolboxController::PythonToolboxController()
    : Toolbox(nullptr, rootModel()->label()) {}

const ToolboxMessageTree* PythonToolboxController::moduleChildren(
    const char* name, int* numberOfNodes) const {
  for (const ToolboxMessage& tm : modulesChildren) {
    const ToolboxMessageTree& t =
        reinterpret_cast<const ToolboxMessageTree&>(tm);
    if (strcmp(I18n::translate(t.label()), name) == 0) {
      const int childrenCount = t.numberOfChildren();
      if (numberOfNodes != nullptr) {
        *numberOfNodes = childrenCount;
      }
      assert(childrenCount > 0);
      return static_cast<const ToolboxMessageTree*>(t.childAtIndex(0));
    }
  }
  return nullptr;
}

bool PythonToolboxController::handleEvent(Ion::Events::Event event) {
  if (Toolbox::handleEvent(event)) {
    return true;
  }
  char buffer[Ion::Events::EventData::k_maxDataSize] = {0};
  size_t length = Ion::Events::copyText(static_cast<uint8_t>(event), buffer,
                                        Ion::Events::EventData::k_maxDataSize);
  if (length == 1) {
    char c = buffer[0];
    if (CodePoint(c).isLatinLetter()) {
      scrollToLetter(c);
      return true;
    }
  }
  return false;
}

bool PythonToolboxController::selectLeaf(int selectedRow) {
  const ToolboxMessageTree* node = static_cast<const ToolboxMessageTree*>(
      m_messageTreeModel->childAtIndex(selectedRow));
  const char* editedText = I18n::translate(node->insertedText());
  // strippedEditedText array needs to be in the same scope as editedText
  char strippedEditedText[k_maxMessageSize];
  if (node->stripInsertedText()) {
    int strippedEditedTextMaxLength = strlen(editedText) + 1;
    assert(strippedEditedTextMaxLength <= k_maxMessageSize);
    Shared::ToolboxHelpers::TextToInsertForCommandMessage(
        node->insertedText(), strippedEditedText, strippedEditedTextMaxLength,
        true);
    editedText = strippedEditedText;
  }
  App::app()->modalViewController()->dismissModal();
  sender()->handleEventWithText(editedText, true);
  return true;
}

const ToolboxMessageTree* PythonToolboxController::rootModel() const {
  return toolboxModel.toMessageTree();
}

ToolboxLeafCell* PythonToolboxController::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

NestedMenuController::NodeCell* PythonToolboxController::nodeCellAtIndex(
    int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_nodeCells[index];
}

int PythonToolboxController::maxNumberOfDisplayedRows() const {
  return k_maxNumberOfDisplayedRows;
}

KDCoordinate PythonToolboxController::nonMemoizedRowHeight(int row) {
  if (m_messageTreeModel->childAtIndex(row)->numberOfChildren() == 0) {
    ToolboxLeafCell tempCell;
    return protectedNonMemoizedRowHeight(&tempCell, row);
  }
  return Toolbox::nonMemoizedRowHeight(row);
}

void PythonToolboxController::fillCellForRow(HighlightCell* cell, int row) {
  const ToolboxMessageTree* messageTree =
      static_cast<const ToolboxMessageTree*>(
          m_messageTreeModel->childAtIndex(row));
  // Message is leaf
  if (typeAtRow(row) == k_leafCellType) {
    ToolboxLeafCell* myCell = static_cast<ToolboxLeafCell*>(cell);
    if (messageTree->text() == I18n::Message::Default &&
        UTF8Helper::HasCodePoint(I18n::translate(messageTree->label()), '\n')) {
      // Leaf node with a multiple row label and no subLabel have a small font.
      myCell->label()->setFont(KDFont::Size::Small);
    } else {
      // Reset cell's font (to prevent a small font from being memoized)
      myCell->label()->setFont(KDFont::Size::Large);
    }
    myCell->label()->setMessage(messageTree->label());
    myCell->subLabel()->setMessage(messageTree->text());
    return;
  }
  assert(typeAtRow(row) == k_nodeCellType);
  Toolbox::fillCellForRow(cell, row);
}

void PythonToolboxController::scrollToLetter(char letter) {
  assert(CodePoint(letter).isLatinLetter());
  /* We look for a child MessageTree that starts with the wanted letter. If we
   * do not find one, we scroll to the first child MessageTree that starts with
   * a letter higher than the wanted letter. */
  char lowerLetter = tolower(letter);
  int index = -1;
  int childrenNumber = m_messageTreeModel->numberOfChildren();
  for (int i = 0; i < childrenNumber; i++) {
    char l = tolower(
        I18n::translate(m_messageTreeModel->childAtIndex(i)->label())[0]);
    if (l == lowerLetter) {
      index = i;
      break;
    }
    if (index < 0 && l >= lowerLetter && CodePoint(l).isLatinSmallLetter()) {
      index = i;
    }
  }
  if (index >= 0) {
    scrollToAndSelectChild(index);
  }
}

void PythonToolboxController::scrollToAndSelectChild(int i) {
  assert(i >= 0 && i < m_messageTreeModel->numberOfChildren());
  m_selectableListView.scrollToCell(i);
  m_selectableListView.selectCell(i);
}

}  // namespace Code
