#include "python_toolbox_controller.h"

#include <apps/shared/toolbox_helpers.h>
#include <assert.h>
#include <ion/keyboard/layout_events.h>
#include <ion/unicode/utf8_helper.h>
extern "C" {
#include <ctype.h>
#include <string.h>
}

using namespace Escher;

namespace Code {

constexpr ToolboxMessageTree forLoopChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::ForInRange1ArgLoopWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::ForInRange1ArgLoop),
    ToolboxMessageTree::Leaf(I18n::Message::ForInRange2ArgsLoopWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::ForInRange2ArgsLoop),
    ToolboxMessageTree::Leaf(I18n::Message::ForInRange3ArgsLoopWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::ForInRange3ArgsLoop),
    ToolboxMessageTree::Leaf(I18n::Message::ForInListLoopWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::ForInListLoop)};

constexpr ToolboxMessageTree ifStatementChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::IfElseStatementWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::IfElseStatement),
    ToolboxMessageTree::Leaf(I18n::Message::IfThenStatementWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::IfThenStatement),
    ToolboxMessageTree::Leaf(I18n::Message::IfElifElseStatementWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::IfElifElseStatement),
    ToolboxMessageTree::Leaf(I18n::Message::IfAndIfElseStatementWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::IfAndIfElseStatement),
    ToolboxMessageTree::Leaf(I18n::Message::IfOrIfElseStatementWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::IfOrIfElseStatement)};

constexpr ToolboxMessageTree whileLoopChildren[] = {ToolboxMessageTree::Leaf(
    I18n::Message::WhileLoopWithArg, I18n::Message::Default, false,
    I18n::Message::WhileLoop)};

constexpr ToolboxMessageTree conditionsChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::EqualityConditionWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::EqualityCondition),
    ToolboxMessageTree::Leaf(I18n::Message::NonEqualityConditionWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::NonEqualityCondition),
    ToolboxMessageTree::Leaf(I18n::Message::SuperiorStrictConditionWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::SuperiorStrictCondition),
    ToolboxMessageTree::Leaf(I18n::Message::InferiorStrictConditionWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::InferiorStrictCondition),
    ToolboxMessageTree::Leaf(I18n::Message::SuperiorConditionWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::SuperiorCondition),
    ToolboxMessageTree::Leaf(I18n::Message::InferiorConditionWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::InferiorCondition),
    ToolboxMessageTree::Leaf(I18n::Message::ConditionAnd,
                             I18n::Message::Default, false),
    ToolboxMessageTree::Leaf(I18n::Message::ConditionOr, I18n::Message::Default,
                             false),
    ToolboxMessageTree::Leaf(I18n::Message::NotCommandWithSpace,
                             I18n::Message::Default, false)};

constexpr ToolboxMessageTree loopsAndTestsChildren[] = {
    ToolboxMessageTree::Node(I18n::Message::ForLoopMenu, forLoopChildren),
    ToolboxMessageTree::Node(I18n::Message::IfStatementMenu,
                             ifStatementChildren),
    ToolboxMessageTree::Node(I18n::Message::WhileLoopMenu, whileLoopChildren),
    ToolboxMessageTree::Node(I18n::Message::ConditionsMenu,
                             conditionsChildren)};

constexpr ToolboxMessageTree MathModuleChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportMath,
                             I18n::Message::PythonImportMath, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromMath,
                             I18n::Message::PythonImportMath, false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandMathFunction,
        I18n::Message::PythonMathFunction, false,
        I18n::Message::PythonCommandMathFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::E, I18n::Message::PythonConstantE,
                             false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConstantPi,
                             I18n::Message::PythonConstantPi, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSqrt,
                             I18n::Message::PythonSqrt),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPower,
                             I18n::Message::PythonPower),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExp,
                             I18n::Message::PythonExp),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExpm1,
                             I18n::Message::PythonExpm1),
    ToolboxMessageTree::Leaf(I18n::Message::LogCommandWithArg,
                             I18n::Message::PythonLog),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog2,
                             I18n::Message::PythonLog2),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog10,
                             I18n::Message::PythonLog10),
    ToolboxMessageTree::Leaf(I18n::Message::CoshCommandWithArg,
                             I18n::Message::PythonCosh),
    ToolboxMessageTree::Leaf(I18n::Message::SinhCommandWithArg,
                             I18n::Message::PythonSinh),
    ToolboxMessageTree::Leaf(I18n::Message::TanhCommandWithArg,
                             I18n::Message::PythonTanh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAcosh,
                             I18n::Message::PythonAcosh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAsinh,
                             I18n::Message::PythonAsinh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtanh,
                             I18n::Message::PythonAtanh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCos,
                             I18n::Message::PythonCos),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSin,
                             I18n::Message::PythonSin),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTan,
                             I18n::Message::PythonTan),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAcos,
                             I18n::Message::PythonAcos),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAsin,
                             I18n::Message::PythonAsin),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtan,
                             I18n::Message::PythonAtan),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtan2,
                             I18n::Message::PythonAtan2),
    ToolboxMessageTree::Leaf(I18n::Message::CeilCommandWithArg,
                             I18n::Message::PythonCeil),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCopySign,
                             I18n::Message::PythonCopySign),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFabs,
                             I18n::Message::PythonFabs),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFactorial,
                             I18n::Message::Factorial),
    ToolboxMessageTree::Leaf(I18n::Message::FloorCommandWithArg,
                             I18n::Message::PythonFloor),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFmod,
                             I18n::Message::PythonFmod),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFrExp,
                             I18n::Message::PythonFrExp),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGcd,
                             I18n::Message::PythonGcd),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLdexp,
                             I18n::Message::PythonLdexp),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandModf,
                             I18n::Message::PythonModf),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsFinite,
                             I18n::Message::PythonIsFinite),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsInfinite,
                             I18n::Message::PythonIsInfinite),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsNaN,
                             I18n::Message::PythonIsNaN),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTrunc,
                             I18n::Message::PythonTrunc),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRadians,
                             I18n::Message::PythonRadians),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDegrees,
                             I18n::Message::PythonDegrees),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandErf,
                             I18n::Message::PythonErf),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandErfc,
                             I18n::Message::PythonErfc),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGamma,
                             I18n::Message::PythonGamma),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLgamma,
                             I18n::Message::PythonLgamma)};

constexpr ToolboxMessageTree CMathModuleChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportCmath,
                             I18n::Message::PythonImportCmath, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromCmath,
                             I18n::Message::PythonImportCmath, false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandCmathFunction,
        I18n::Message::PythonCmathFunction, false,
        I18n::Message::PythonCommandCmathFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::E, I18n::Message::PythonConstantE,
                             false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConstantPi,
                             I18n::Message::PythonConstantPi, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPhase,
                             I18n::Message::PythonPhase),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPolar,
                             I18n::Message::PythonPolar),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRect,
                             I18n::Message::PythonRect),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExpComplex,
                             I18n::Message::PythonExp),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLogComplex,
                             I18n::Message::PythonLog),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSqrtComplex,
                             I18n::Message::PythonSqrt),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCosComplex,
                             I18n::Message::PythonCos),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSinComplex,
                             I18n::Message::PythonSin)};

constexpr ToolboxMessageTree MatplotlibPyplotModuleChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportMatplotlibPyplot,
                             I18n::Message::PythonImportMatplotlibPyplot,
                             false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandImportFromMatplotlibPyplotTrimmed,
        I18n::Message::PythonImportMatplotlibPyplot, false,
        I18n::Message::PythonCommandImportFromMatplotlibPyplot),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandMatplotlibPyplotFunction,
        I18n::Message::PythonMatplotlibPyplotFunction, false,
        I18n::Message::PythonCommandMatplotlibPyplotFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArrow,
                             I18n::Message::PythonArrow),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAxis,
                             I18n::Message::PythonAxis, false,
                             I18n::Message::PythonCommandAxisWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBar,
                             I18n::Message::PythonBar),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGrid,
                             I18n::Message::PythonGrid),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandHist,
                             I18n::Message::PythonHist),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPlot,
                             I18n::Message::PythonPlot),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScatter,
                             I18n::Message::PythonScatter),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandShow,
                             I18n::Message::PythonShow),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandText,
                             I18n::Message::PythonText),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlue,
                             I18n::Message::PythonColorBlue, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorRed,
                             I18n::Message::PythonColorRed, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGreen,
                             I18n::Message::PythonColorGreen, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorYellow,
                             I18n::Message::PythonColorYellow, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBrown,
                             I18n::Message::PythonColorBrown, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlack,
                             I18n::Message::PythonColorBlack, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorWhite,
                             I18n::Message::PythonColorWhite, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPink,
                             I18n::Message::PythonColorPink, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorOrange,
                             I18n::Message::PythonColorOrange, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPurple,
                             I18n::Message::PythonColorPurple, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGray,
                             I18n::Message::PythonColorGray, false)};

constexpr ToolboxMessageTree TurtleModuleChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportTurtle,
                             I18n::Message::PythonImportTurtle, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromTurtle,
                             I18n::Message::PythonImportTurtle, false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandTurtleFunction,
        I18n::Message::PythonTurtleFunction, false,
        I18n::Message::PythonCommandTurtleFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandForward,
                             I18n::Message::PythonTurtleForward),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandBackward,
                             I18n::Message::PythonTurtleBackward),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandRight,
                             I18n::Message::PythonTurtleRight),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandLeft,
                             I18n::Message::PythonTurtleLeft),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandGoto,
                             I18n::Message::PythonTurtleGoto),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandSetheading,
                             I18n::Message::PythonTurtleSetheading),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandCircle,
                             I18n::Message::PythonTurtleCircle),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandSpeed,
                             I18n::Message::PythonTurtleSpeed),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPosition,
                             I18n::Message::PythonTurtlePosition, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandHeading,
                             I18n::Message::PythonTurtleHeading, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandDistance,
                             I18n::Message::PythonTurtleDistance),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPendown,
                             I18n::Message::PythonTurtlePendown, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPenup,
                             I18n::Message::PythonTurtlePenup, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPensize,
                             I18n::Message::PythonTurtlePensize),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandIsdown,
                             I18n::Message::PythonTurtleIsdown, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandWrite,
                             I18n::Message::PythonTurtleWrite),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandReset,
                             I18n::Message::PythonTurtleReset, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandShowturtle,
                             I18n::Message::PythonTurtleShowturtle, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandHideturtle,
                             I18n::Message::PythonTurtleHideturtle, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandColor,
                             I18n::Message::PythonTurtleColor),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandColorMode,
                             I18n::Message::PythonTurtleColorMode),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlue,
                             I18n::Message::PythonColorBlue, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorRed,
                             I18n::Message::PythonColorRed, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGreen,
                             I18n::Message::PythonColorGreen, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorYellow,
                             I18n::Message::PythonColorYellow, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBrown,
                             I18n::Message::PythonColorBrown, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlack,
                             I18n::Message::PythonColorBlack, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorWhite,
                             I18n::Message::PythonColorWhite, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPink,
                             I18n::Message::PythonColorPink, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorOrange,
                             I18n::Message::PythonColorOrange, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPurple,
                             I18n::Message::PythonColorPurple, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGray,
                             I18n::Message::PythonColorGray, false)};

constexpr ToolboxMessageTree RandomModuleChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportRandom,
                             I18n::Message::PythonImportRandom, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromRandom,
                             I18n::Message::PythonImportRandom, false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandRandomFunction,
        I18n::Message::PythonRandomFunction, false,
        I18n::Message::PythonCommandRandomFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetrandbits,
                             I18n::Message::PythonGetrandbits),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSeed,
                             I18n::Message::PythonSeed),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandrange,
                             I18n::Message::PythonRandrange),
    ToolboxMessageTree::Leaf(I18n::Message::RandintCommandWithArg,
                             I18n::Message::PythonRandint),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandChoice,
                             I18n::Message::PythonChoice),
    ToolboxMessageTree::Leaf(I18n::Message::RandomCommandWithArg,
                             I18n::Message::RandomFloat, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandUniform,
                             I18n::Message::PythonUniform)};

constexpr ToolboxMessageTree KandinskyModuleChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportKandinsky,
                             I18n::Message::PythonImportKandinsky, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromKandinsky,
                             I18n::Message::PythonImportKandinsky, false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandKandinskyFunction,
        I18n::Message::PythonKandinskyFunction, false,
        I18n::Message::PythonCommandKandinskyFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetPixel,
                             I18n::Message::PythonGetPixel),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSetPixel,
                             I18n::Message::PythonSetPixel),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColor,
                             I18n::Message::PythonColor),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDrawString,
                             I18n::Message::PythonDrawString),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFillRect,
                             I18n::Message::PythonFillRect)};

constexpr ToolboxMessageTree IonModuleChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportIon,
                             I18n::Message::PythonImportIon, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromIon,
                             I18n::Message::PythonImportIon, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIonFunction,
                             I18n::Message::PythonIonFunction, false,
                             I18n::Message::PythonCommandIonFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsKeyDown,
                             I18n::Message::PythonIsKeyDown),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyLeft,
                             I18n::Message::PythonKeyLeft, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyUp,
                             I18n::Message::PythonKeyUp, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyDown,
                             I18n::Message::PythonKeyDown, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyRight,
                             I18n::Message::PythonKeyRight, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyOk,
                             I18n::Message::PythonKeyOk, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyBack,
                             I18n::Message::PythonKeyBack, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyHome,
                             I18n::Message::PythonKeyHome, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyOnOff,
                             I18n::Message::PythonKeyOnOff, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyShift,
                             I18n::Message::PythonKeyShift, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyAlpha,
                             I18n::Message::PythonKeyAlpha, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyXnt,
                             I18n::Message::PythonKeyXnt, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyVar,
                             I18n::Message::PythonKeyVar, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyToolbox,
                             I18n::Message::PythonKeyToolbox, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyBackspace,
                             I18n::Message::PythonKeyBackspace, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyExp,
                             I18n::Message::PythonKeyExp, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyLn,
                             I18n::Message::PythonKeyLn, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyLog,
                             I18n::Message::PythonKeyLog, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyImaginary,
                             I18n::Message::PythonKeyImaginary, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyComma,
                             I18n::Message::PythonKeyComma, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyPower,
                             I18n::Message::PythonKeyPower, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeySine,
                             I18n::Message::PythonKeySine, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyCosine,
                             I18n::Message::PythonKeyCosine, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyTangent,
                             I18n::Message::PythonKeyTangent, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyPi,
                             I18n::Message::PythonKeyPi, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeySqrt,
                             I18n::Message::PythonKeySqrt, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeySquare,
                             I18n::Message::PythonKeySquare, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeySeven,
                             I18n::Message::PythonKeySeven, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyEight,
                             I18n::Message::PythonKeyEight, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyNine,
                             I18n::Message::PythonKeyNine, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyLeftParenthesis,
                             I18n::Message::PythonKeyLeftParenthesis, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyRightParenthesis,
                             I18n::Message::PythonKeyRightParenthesis, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyFour,
                             I18n::Message::PythonKeyFour, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyFive,
                             I18n::Message::PythonKeyFive, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeySix,
                             I18n::Message::PythonKeySix, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyMultiplication,
                             I18n::Message::PythonKeyMultiplication, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyDivision,
                             I18n::Message::PythonKeyDivision, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyOne,
                             I18n::Message::PythonKeyOne, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyTwo,
                             I18n::Message::PythonKeyTwo, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyThree,
                             I18n::Message::PythonKeyThree, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyPlus,
                             I18n::Message::PythonKeyPlus, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyMinus,
                             I18n::Message::PythonKeyMinus, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyZero,
                             I18n::Message::PythonKeyZero, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyDot,
                             I18n::Message::PythonKeyDot, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyEe,
                             I18n::Message::PythonKeyEe, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyAns,
                             I18n::Message::PythonKeyAns, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKeyExe,
                             I18n::Message::PythonKeyExe, false)};

constexpr ToolboxMessageTree TimeModuleChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportTime,
                             I18n::Message::PythonImportTime, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromTime,
                             I18n::Message::PythonImportTime, false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandTimeFunction,
        I18n::Message::PythonTimeFunction, false,
        I18n::Message::PythonCommandTimeFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMonotonic,
                             I18n::Message::PythonMonotonic, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSleep,
                             I18n::Message::PythonSleep)};

// TODO: Add these commands in the catalog
constexpr ToolboxMessageTree NumpyModuleChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportNumpy,
                             I18n::Message::PythonImportNumpy, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromNumpy,
                             I18n::Message::PythonImportNumpy, false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandNumpyFunction,
        I18n::Message::PythonNumpyFunction, false,
        I18n::Message::PythonCommandNumpyFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArray,
                             I18n::Message::PythonArray),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArange,
                             I18n::Message::PythonArange),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConcatenate,
                             I18n::Message::PythonConcatenate, false,
                             I18n::Message::PythonCommandConcatenateWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLinspace,
                             I18n::Message::PythonLinspace),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandOnes,
                             I18n::Message::PythonOnes),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandZeros,
                             I18n::Message::PythonZeros),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFlatten,
                             I18n::Message::PythonFlatten, false,
                             I18n::Message::PythonCommandFlattenWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandReshape,
                             I18n::Message::PythonReshape, false,
                             I18n::Message::PythonCommandReshapeWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandShape,
                             I18n::Message::PythonShape, false,
                             I18n::Message::PythonCommandShapeWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTolist,
                             I18n::Message::PythonTolist, false,
                             I18n::Message::PythonCommandTolistWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTranspose,
                             I18n::Message::PythonTranspose, false,
                             I18n::Message::PythonCommandTransposeWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArgmax,
                             I18n::Message::PythonArgmax),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArgmin,
                             I18n::Message::PythonArgmin),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDot,
                             I18n::Message::PythonDot),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCross,
                             I18n::Message::PythonCross),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMax,
                             I18n::Message::PythonMax),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMin,
                             I18n::Message::PythonMin),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMean,
                             I18n::Message::PythonMean),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMedian,
                             I18n::Message::PythonMedian),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPolyfit,
                             I18n::Message::PythonPolyfit),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPolyVal,
                             I18n::Message::PythonPolyVal),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSize,
                             I18n::Message::PythonSize),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySort,
                             I18n::Message::PythonNumpySort),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandStd,
                             I18n::Message::PythonStd),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySum,
                             I18n::Message::PythonNumpySum)};

constexpr ToolboxMessageTree modulesChildren[] = {
    ToolboxMessageTree::Node(I18n::Message::MathModule, MathModuleChildren),
    ToolboxMessageTree::Node(I18n::Message::CmathModule, CMathModuleChildren),
    ToolboxMessageTree::Node(I18n::Message::MatplotlibPyplotModule,
                             MatplotlibPyplotModuleChildren),
    ToolboxMessageTree::Node(I18n::Message::NumpyModule, NumpyModuleChildren),
    ToolboxMessageTree::Node(I18n::Message::TurtleModule, TurtleModuleChildren),
    ToolboxMessageTree::Node(I18n::Message::RandomModule, RandomModuleChildren),
    ToolboxMessageTree::Node(I18n::Message::KandinskyModule,
                             KandinskyModuleChildren),
    ToolboxMessageTree::Node(I18n::Message::IonModule, IonModuleChildren),
    ToolboxMessageTree::Node(I18n::Message::TimeModule, TimeModuleChildren)};

constexpr ToolboxMessageTree catalogChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPound,
                             I18n::Message::PythonPound, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPercent,
                             I18n::Message::PythonPercent, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommand1J,
                             I18n::Message::Python1J, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLF,
                             I18n::Message::PythonLF, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTab,
                             I18n::Message::PythonTab, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAmpersand,
                             I18n::Message::PythonAmpersand, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSymbolExp,
                             I18n::Message::PythonSymbolExp, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandVerticalBar,
                             I18n::Message::PythonVerticalBar, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSingleQuote,
                             I18n::Message::PythonSingleQuote, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAt,
                             I18n::Message::PythonAt, false),
    ToolboxMessageTree::Leaf(I18n::Message::AbsCommandWithArg,
                             I18n::Message::PythonAbs),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAcos,
                             I18n::Message::PythonAcos),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAcosh,
                             I18n::Message::PythonAcosh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArange,
                             I18n::Message::PythonArange),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArgmax,
                             I18n::Message::PythonArgmax),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArgmin,
                             I18n::Message::PythonArgmin),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFlatten,
                             I18n::Message::PythonFlatten, false,
                             I18n::Message::PythonCommandFlattenWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandReshape,
                             I18n::Message::PythonReshape, false,
                             I18n::Message::PythonCommandReshapeWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandShape,
                             I18n::Message::PythonShape, false,
                             I18n::Message::PythonCommandShapeWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTolist,
                             I18n::Message::PythonTolist, false,
                             I18n::Message::PythonCommandTolistWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTranspose,
                             I18n::Message::PythonTranspose, false,
                             I18n::Message::PythonCommandTransposeWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArray,
                             I18n::Message::PythonArray),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArrow,
                             I18n::Message::PythonArrow),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAsin,
                             I18n::Message::PythonAsin),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAsinh,
                             I18n::Message::PythonAsinh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtan,
                             I18n::Message::PythonAtan),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtan2,
                             I18n::Message::PythonAtan2),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtanh,
                             I18n::Message::PythonAtanh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAxis,
                             I18n::Message::PythonAxis, false,
                             I18n::Message::PythonCommandAxisWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandBackward,
                             I18n::Message::PythonTurtleBackward),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBar,
                             I18n::Message::PythonBar),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBin,
                             I18n::Message::PythonBin),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlack,
                             I18n::Message::PythonColorBlack, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlue,
                             I18n::Message::PythonColorBlue, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBrown,
                             I18n::Message::PythonColorBrown, false),
    ToolboxMessageTree::Leaf(I18n::Message::CeilCommandWithArg,
                             I18n::Message::PythonCeil),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandChoice,
                             I18n::Message::PythonChoice),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandCircle,
                             I18n::Message::PythonTurtleCircle),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandCmathFunction,
        I18n::Message::PythonCmathFunction, false,
        I18n::Message::PythonCommandCmathFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColor,
                             I18n::Message::PythonColor),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandColor,
                             I18n::Message::PythonTurtleColor),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandColorMode,
                             I18n::Message::PythonTurtleColorMode),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandComplex,
                             I18n::Message::PythonComplex),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConcatenate,
                             I18n::Message::PythonConcatenate, false,
                             I18n::Message::PythonCommandConcatenateWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCopySign,
                             I18n::Message::PythonCopySign),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCos,
                             I18n::Message::PythonCos),
    ToolboxMessageTree::Leaf(I18n::Message::CoshCommandWithArg,
                             I18n::Message::PythonCosh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCross,
                             I18n::Message::PythonCross),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDegrees,
                             I18n::Message::PythonDegrees),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandDistance,
                             I18n::Message::PythonTurtleDistance),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDivMod,
                             I18n::Message::PythonDivMod),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDot,
                             I18n::Message::PythonDot),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDrawString,
                             I18n::Message::PythonDrawString),
    ToolboxMessageTree::Leaf(I18n::Message::E, I18n::Message::PythonConstantE,
                             false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandErf,
                             I18n::Message::PythonErf),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandErfc,
                             I18n::Message::PythonErfc),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandEval,
                             I18n::Message::PythonEval),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExp,
                             I18n::Message::PythonExp),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExpm1,
                             I18n::Message::PythonExpm1),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFabs,
                             I18n::Message::PythonFabs),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFactorial,
                             I18n::Message::Factorial),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFillRect,
                             I18n::Message::PythonFillRect),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFloat,
                             I18n::Message::PythonFloat),
    ToolboxMessageTree::Leaf(I18n::Message::FloorCommandWithArg,
                             I18n::Message::PythonFloor),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFmod,
                             I18n::Message::PythonFmod),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandForward,
                             I18n::Message::PythonTurtleForward),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFrExp,
                             I18n::Message::PythonFrExp),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromCmath,
                             I18n::Message::PythonImportCmath, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromIon,
                             I18n::Message::PythonImportIon, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromKandinsky,
                             I18n::Message::PythonImportKandinsky, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromMath,
                             I18n::Message::PythonImportMath, false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandImportFromMatplotlibPyplotTrimmed,
        I18n::Message::PythonImportMatplotlibPyplot, false,
        I18n::Message::PythonCommandImportFromMatplotlibPyplot),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromNumpy,
                             I18n::Message::PythonImportNumpy, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromRandom,
                             I18n::Message::PythonImportRandom, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromTime,
                             I18n::Message::PythonImportTime, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromTurtle,
                             I18n::Message::PythonImportTurtle, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGamma,
                             I18n::Message::PythonGamma),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGcd,
                             I18n::Message::PythonGcd),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetPixel,
                             I18n::Message::PythonGetPixel),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetrandbits,
                             I18n::Message::PythonGetrandbits),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandGoto,
                             I18n::Message::PythonTurtleGoto),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGray,
                             I18n::Message::PythonColorGray, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGreen,
                             I18n::Message::PythonColorGreen, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGrid,
                             I18n::Message::PythonGrid),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandHeading,
                             I18n::Message::PythonTurtleHeading, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandHex,
                             I18n::Message::PythonHex),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandHideturtle,
                             I18n::Message::PythonTurtleHideturtle, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandHist,
                             I18n::Message::PythonHist),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportCmath,
                             I18n::Message::PythonImportCmath, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportIon,
                             I18n::Message::PythonImportIon, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportKandinsky,
                             I18n::Message::PythonImportKandinsky, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportMath,
                             I18n::Message::PythonImportMath, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportMatplotlibPyplot,
                             I18n::Message::PythonImportMatplotlibPyplot,
                             false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportNumpy,
                             I18n::Message::PythonImportNumpy, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportRandom,
                             I18n::Message::PythonImportRandom, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportTime,
                             I18n::Message::PythonImportTime, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportTurtle,
                             I18n::Message::PythonImportTurtle, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandInput,
                             I18n::Message::PythonInput),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandInt,
                             I18n::Message::PythonInt),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIonFunction,
                             I18n::Message::PythonIonFunction, false,
                             I18n::Message::PythonCommandIonFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandIsdown,
                             I18n::Message::PythonTurtleIsdown, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsFinite,
                             I18n::Message::PythonIsFinite),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsInfinite,
                             I18n::Message::PythonIsInfinite),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsNaN,
                             I18n::Message::PythonIsNaN),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandKandinskyFunction,
        I18n::Message::PythonKandinskyFunction, false,
        I18n::Message::PythonCommandKandinskyFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsKeyDown,
                             I18n::Message::PythonIsKeyDown),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLdexp,
                             I18n::Message::PythonLdexp),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandLeft,
                             I18n::Message::PythonTurtleLeft),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLength,
                             I18n::Message::PythonLength),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLgamma,
                             I18n::Message::PythonLgamma),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLinspace,
                             I18n::Message::PythonLinspace),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAppend,
                             I18n::Message::PythonAppend, false,
                             I18n::Message::PythonCommandAppendWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandClear,
                             I18n::Message::PythonClear, false,
                             I18n::Message::PythonCommandClearWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCount,
                             I18n::Message::PythonCount, false,
                             I18n::Message::PythonCommandCountWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIndex,
                             I18n::Message::PythonIndex, false,
                             I18n::Message::PythonCommandIndexWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandInsert,
                             I18n::Message::PythonInsert, false,
                             I18n::Message::PythonCommandInsertWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPop,
                             I18n::Message::PythonPop, false,
                             I18n::Message::PythonCommandPopWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRemove,
                             I18n::Message::PythonRemove, false,
                             I18n::Message::PythonCommandRemoveWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandReverse,
                             I18n::Message::PythonReverse, false,
                             I18n::Message::PythonCommandReverseWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSort,
                             I18n::Message::PythonSort, false,
                             I18n::Message::PythonCommandSortWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::LogCommandWithArg,
                             I18n::Message::PythonLog),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog10,
                             I18n::Message::PythonLog10),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog2,
                             I18n::Message::PythonLog2),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandMathFunction,
        I18n::Message::PythonMathFunction, false,
        I18n::Message::PythonCommandMathFunctionWithoutArg),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandMatplotlibPyplotFunction,
        I18n::Message::PythonMatplotlibPyplotFunction, false,
        I18n::Message::PythonCommandMatplotlibPyplotFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMax,
                             I18n::Message::PythonMax),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMax,
                             I18n::Message::PythonMax),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMean,
                             I18n::Message::PythonMean),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMedian,
                             I18n::Message::PythonMedian),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMin,
                             I18n::Message::PythonMin),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMin,
                             I18n::Message::PythonMin),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandModf,
                             I18n::Message::PythonModf),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMonotonic,
                             I18n::Message::PythonMonotonic, false),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandNumpyFunction,
        I18n::Message::PythonNumpyFunction, false,
        I18n::Message::PythonCommandNumpyFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandOct,
                             I18n::Message::PythonOct),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandOnes,
                             I18n::Message::PythonOnes),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorOrange,
                             I18n::Message::PythonColorOrange, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPendown,
                             I18n::Message::PythonTurtlePendown, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPensize,
                             I18n::Message::PythonTurtlePensize),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPenup,
                             I18n::Message::PythonTurtlePenup, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPhase,
                             I18n::Message::PythonPhase),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConstantPi,
                             I18n::Message::PythonConstantPi, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPink,
                             I18n::Message::PythonColorPink, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPlot,
                             I18n::Message::PythonPlot),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPolar,
                             I18n::Message::PythonPolar),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPolyfit,
                             I18n::Message::PythonPolyfit),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPolyVal,
                             I18n::Message::PythonPolyVal),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPosition,
                             I18n::Message::PythonTurtlePosition, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPower,
                             I18n::Message::PythonPower),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPrint,
                             I18n::Message::PythonPrint),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPurple,
                             I18n::Message::PythonColorPurple, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRadians,
                             I18n::Message::PythonRadians),
    ToolboxMessageTree::Leaf(I18n::Message::RandintCommandWithArg,
                             I18n::Message::PythonRandint),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandRandomFunction,
        I18n::Message::PythonRandomFunction, false,
        I18n::Message::PythonCommandRandomFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::RandomCommandWithArg,
                             I18n::Message::RandomFloat, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandrange,
                             I18n::Message::PythonRandrange),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRangeStartStop,
                             I18n::Message::PythonRangeStartStop),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRangeStop,
                             I18n::Message::PythonRangeStop),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRect,
                             I18n::Message::PythonRect),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorRed,
                             I18n::Message::PythonColorRed, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandReset,
                             I18n::Message::PythonTurtleReset, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandRight,
                             I18n::Message::PythonTurtleRight),
    ToolboxMessageTree::Leaf(I18n::Message::RoundCommandWithArg,
                             I18n::Message::PythonRound),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScatter,
                             I18n::Message::PythonScatter),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSeed,
                             I18n::Message::PythonSeed),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSetPixel,
                             I18n::Message::PythonSetPixel),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandSetheading,
                             I18n::Message::PythonTurtleSetheading),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandShow,
                             I18n::Message::PythonShow),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandShowturtle,
                             I18n::Message::PythonTurtleShowturtle, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSin,
                             I18n::Message::PythonSin),
    ToolboxMessageTree::Leaf(I18n::Message::SinhCommandWithArg,
                             I18n::Message::PythonSinh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSize,
                             I18n::Message::PythonSize),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSleep,
                             I18n::Message::PythonSleep),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySort,
                             I18n::Message::PythonNumpySort),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSorted,
                             I18n::Message::PythonSort),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandSpeed,
                             I18n::Message::PythonTurtleSpeed),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSqrt,
                             I18n::Message::PythonSqrt),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandStd,
                             I18n::Message::PythonStd),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySum,
                             I18n::Message::PythonNumpySum),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSum,
                             I18n::Message::PythonSum),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTan,
                             I18n::Message::PythonTan),
    ToolboxMessageTree::Leaf(I18n::Message::TanhCommandWithArg,
                             I18n::Message::PythonTanh),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandText,
                             I18n::Message::PythonText),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandTimeFunction,
        I18n::Message::PythonTimeFunction, false,
        I18n::Message::PythonCommandTimeFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTrunc,
                             I18n::Message::PythonTrunc),
    ToolboxMessageTree::Leaf(
        I18n::Message::PythonCommandTurtleFunction,
        I18n::Message::PythonTurtleFunction, false,
        I18n::Message::PythonCommandTurtleFunctionWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandUniform,
                             I18n::Message::PythonUniform),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorWhite,
                             I18n::Message::PythonColorWhite, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandWrite,
                             I18n::Message::PythonTurtleWrite),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorYellow,
                             I18n::Message::PythonColorYellow, false),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImag,
                             I18n::Message::PythonImag, false,
                             I18n::Message::PythonCommandImagWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandReal,
                             I18n::Message::PythonReal, false,
                             I18n::Message::PythonCommandRealWithoutArg),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandZeros,
                             I18n::Message::PythonZeros),
};

constexpr ToolboxMessageTree functionsChildren[] = {
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDefWithArg,
                             I18n::Message::Default, false,
                             I18n::Message::PythonCommandDef),
    ToolboxMessageTree::Leaf(I18n::Message::PythonCommandReturn,
                             I18n::Message::Default)};

constexpr ToolboxMessageTree menu[] = {
    ToolboxMessageTree::Node(I18n::Message::LoopsAndTests,
                             loopsAndTestsChildren),
    ToolboxMessageTree::Node(I18n::Message::Modules, modulesChildren),
    ToolboxMessageTree::Node(I18n::Message::Catalog, catalogChildren),
    ToolboxMessageTree::Node(I18n::Message::Functions, functionsChildren)};

constexpr ToolboxMessageTree toolboxModel =
    ToolboxMessageTree::Node(I18n::Message::Toolbox, menu);

constexpr static bool catalogContainsAllChildren(
    const ToolboxMessageTree *list, int numberOfChildren,
    bool sameCaptionIsEnough = false) {
  constexpr int nCatalog = std::size(catalogChildren);
  for (int i = 0; i < numberOfChildren; i++) {
    bool isInCatalog = false;
    for (int j = 0; j < nCatalog; j++) {
      if (catalogChildren[j].text() == list[i].text() &&
          (sameCaptionIsEnough ||
           catalogChildren[j].label() == list[i].label())) {
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

const ToolboxMessageTree *PythonToolboxController::moduleChildren(
    const char *name, int *numberOfNodes) const {
  for (ToolboxMessageTree t : modulesChildren) {
    if (strcmp(I18n::translate(t.label()), name) == 0) {
      const int childrenCount = t.numberOfChildren();
      if (numberOfNodes != nullptr) {
        *numberOfNodes = childrenCount;
      }
      assert(childrenCount > 0);
      return static_cast<const ToolboxMessageTree *>(t.childAtIndex(0));
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
  const ToolboxMessageTree *node = static_cast<const ToolboxMessageTree *>(
      m_messageTreeModel->childAtIndex(selectedRow));
  const char *editedText = I18n::translate(node->insertedText());
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

const ToolboxMessageTree *PythonToolboxController::rootModel() const {
  return &toolboxModel;
}

ToolboxLeafCell *PythonToolboxController::leafCellAtIndex(int index) {
  assert(index >= 0 && index < k_maxNumberOfDisplayedRows);
  return &m_leafCells[index];
}

NestedMenuController::NodeCell *PythonToolboxController::nodeCellAtIndex(
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

void PythonToolboxController::fillCellForRow(HighlightCell *cell, int row) {
  const ToolboxMessageTree *messageTree =
      static_cast<const ToolboxMessageTree *>(
          m_messageTreeModel->childAtIndex(row));
  // Message is leaf
  if (typeAtRow(row) == k_leafCellType) {
    ToolboxLeafCell *myCell = static_cast<ToolboxLeafCell *>(cell);
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
