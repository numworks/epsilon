#include "python_toolbox.h"
#include "../shared/toolbox_helpers.h"
#include <assert.h>
extern "C" {
#include <string.h>
#include <ctype.h>
}
#include "app.h"

namespace Code {

const ToolboxMessageTree forLoopChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::ForInRange1ArgLoopWithArg, I18n::Message::Default, false, I18n::Message::ForInRange1ArgLoop),
  ToolboxMessageTree::Leaf(I18n::Message::ForInRange2ArgsLoopWithArg, I18n::Message::Default, false, I18n::Message::ForInRange2ArgsLoop),
  ToolboxMessageTree::Leaf(I18n::Message::ForInRange3ArgsLoopWithArg, I18n::Message::Default, false, I18n::Message::ForInRange3ArgsLoop),
  ToolboxMessageTree::Leaf(I18n::Message::ForInListLoopWithArg, I18n::Message::Default, false, I18n::Message::ForInListLoop)
};

const ToolboxMessageTree ifStatementChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::IfElseStatementWithArg, I18n::Message::Default, false, I18n::Message::IfElseStatement),
  ToolboxMessageTree::Leaf(I18n::Message::IfThenStatementWithArg, I18n::Message::Default, false, I18n::Message::IfThenStatement),
  ToolboxMessageTree::Leaf(I18n::Message::IfElifElseStatementWithArg, I18n::Message::Default, false, I18n::Message::IfElifElseStatement),
  ToolboxMessageTree::Leaf(I18n::Message::IfAndIfElseStatementWithArg, I18n::Message::Default, false, I18n::Message::IfAndIfElseStatement),
  ToolboxMessageTree::Leaf(I18n::Message::IfOrIfElseStatementWithArg, I18n::Message::Default, false, I18n::Message::IfOrIfElseStatement)
};

const ToolboxMessageTree whileLoopChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::WhileLoopWithArg, I18n::Message::Default, false, I18n::Message::WhileLoop)
};

const ToolboxMessageTree conditionsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::EqualityConditionWithArg, I18n::Message::Default, false, I18n::Message::EqualityCondition),
  ToolboxMessageTree::Leaf(I18n::Message::NonEqualityConditionWithArg, I18n::Message::Default, false, I18n::Message::NonEqualityCondition),
  ToolboxMessageTree::Leaf(I18n::Message::SuperiorStrictConditionWithArg, I18n::Message::Default, false, I18n::Message::SuperiorStrictCondition),
  ToolboxMessageTree::Leaf(I18n::Message::InferiorStrictConditionWithArg, I18n::Message::Default, false, I18n::Message::InferiorStrictCondition),
  ToolboxMessageTree::Leaf(I18n::Message::SuperiorConditionWithArg, I18n::Message::Default, false, I18n::Message::SuperiorCondition),
  ToolboxMessageTree::Leaf(I18n::Message::InferiorConditionWithArg, I18n::Message::Default, false, I18n::Message::InferiorCondition),
  ToolboxMessageTree::Leaf(I18n::Message::ConditionAnd, I18n::Message::Default, false),
  ToolboxMessageTree::Leaf(I18n::Message::ConditionOr, I18n::Message::Default, false),
  ToolboxMessageTree::Leaf(I18n::Message::ConditionNot, I18n::Message::Default, false)
};

const ToolboxMessageTree loopsAndTestsChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::ForLoopMenu, forLoopChildren),
  ToolboxMessageTree::Node(I18n::Message::IfStatementMenu, ifStatementChildren),
  ToolboxMessageTree::Node(I18n::Message::WhileLoopMenu, whileLoopChildren),
  ToolboxMessageTree::Node(I18n::Message::ConditionsMenu, conditionsChildren)
};

const ToolboxMessageTree MathModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportMath, I18n::Message::PythonImportMath, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromMath, I18n::Message::PythonImportMath, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMathFunction, I18n::Message::PythonMathFunction, false, I18n::Message::PythonCommandMathFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConstantE, I18n::Message::PythonConstantE, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConstantPi, I18n::Message::PythonConstantPi, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSqrt, I18n::Message::PythonSqrt),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPower, I18n::Message::PythonPower),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExp, I18n::Message::PythonExp),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExpm1, I18n::Message::PythonExpm1),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog, I18n::Message::PythonLog),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog2, I18n::Message::PythonLog2),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog10, I18n::Message::PythonLog10),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCosh, I18n::Message::PythonCosh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSinh, I18n::Message::PythonSinh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTanh, I18n::Message::PythonTanh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAcosh, I18n::Message::PythonAcosh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAsinh, I18n::Message::PythonAsinh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtanh, I18n::Message::PythonAtanh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCos, I18n::Message::PythonCos),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSin, I18n::Message::PythonSin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTan, I18n::Message::PythonTan),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAcos, I18n::Message::PythonAcos),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAsin, I18n::Message::PythonAsin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtan, I18n::Message::PythonAtan),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtan2, I18n::Message::PythonAtan2),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCeil, I18n::Message::PythonCeil),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCopySign, I18n::Message::PythonCopySign),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFabs, I18n::Message::PythonFabs),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFloor, I18n::Message::PythonFloor),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFmod, I18n::Message::PythonFmod),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFrExp, I18n::Message::PythonFrExp),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLdexp, I18n::Message::PythonLdexp),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandModf, I18n::Message::PythonModf),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsFinite, I18n::Message::PythonIsFinite),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsInfinite, I18n::Message::PythonIsInfinite),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsNaN, I18n::Message::PythonIsNaN),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTrunc, I18n::Message::PythonTrunc),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRadians, I18n::Message::PythonRadians),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDegrees, I18n::Message::PythonDegrees),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandErf, I18n::Message::PythonErf),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandErfc, I18n::Message::PythonErfc),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGamma, I18n::Message::PythonGamma),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLgamma, I18n::Message::PythonLgamma)
};

const ToolboxMessageTree CMathModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportCmath, I18n::Message::PythonImportCmath, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromCmath, I18n::Message::PythonImportCmath, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCmathFunction, I18n::Message::PythonCmathFunction, false, I18n::Message::PythonCommandCmathFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConstantE, I18n::Message::PythonConstantE, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConstantPi, I18n::Message::PythonConstantPi, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPhase, I18n::Message::PythonPhase),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPolar, I18n::Message::PythonPolar),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRect, I18n::Message::PythonRect),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExpComplex, I18n::Message::PythonExp),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLogComplex, I18n::Message::PythonLog),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSqrtComplex, I18n::Message::PythonSqrt),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCosComplex, I18n::Message::PythonCos),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSinComplex, I18n::Message::PythonSin)
};

const ToolboxMessageTree MatplotlibPyplotModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportMatplotlibPyplot, I18n::Message::PythonImportMatplotlibPyplot, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromMatplotlibPyplot, I18n::Message::PythonImportMatplotlibPyplot, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMatplotlibPyplotFunction, I18n::Message::PythonMatplotlibPyplotFunction, false, I18n::Message::PythonCommandMatplotlibPyplotFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArrow, I18n::Message::PythonArrow),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAxis, I18n::Message::PythonAxis, false, I18n::Message::PythonCommandAxisWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBar, I18n::Message::PythonBar),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGrid, I18n::Message::PythonGrid),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandHist, I18n::Message::PythonHist),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPlot, I18n::Message::PythonPlot),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScatter, I18n::Message::PythonScatter),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandShow, I18n::Message::PythonShow),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandText, I18n::Message::PythonText),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlue, I18n::Message::PythonColorBlue, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorRed, I18n::Message::PythonColorRed, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGreen, I18n::Message::PythonColorGreen, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorYellow, I18n::Message::PythonColorYellow, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBrown, I18n::Message::PythonColorBrown, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlack, I18n::Message::PythonColorBlack, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorWhite, I18n::Message::PythonColorWhite, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPink, I18n::Message::PythonColorPink, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorOrange, I18n::Message::PythonColorOrange, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPurple, I18n::Message::PythonColorPurple, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGray, I18n::Message::PythonColorGray, false)
};

#if defined(INCLUDE_ULAB)

const ToolboxMessageTree NumpyNdarrayModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyArray),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyArange),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyConcatenate),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyDiag),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyZeros),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyOnes),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyEmpty),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyEye),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFull),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyLinspace),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyLogspace),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFromBuffer),  
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyCopy, I18n::Message::Default, false, I18n::Message::PythonCommandNumpyCopyWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyDtype, I18n::Message::Default, false, I18n::Message::PythonCommandNumpyDtypeWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFlat, I18n::Message::Default, false, I18n::Message::PythonCommandNumpyFlatWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFlatten, I18n::Message::Default, false, I18n::Message::PythonCommandNumpyFlattenWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyShape, I18n::Message::Default, false, I18n::Message::PythonCommandNumpyShapeWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyReshape, I18n::Message::Default, false, I18n::Message::PythonCommandNumpyReshapeWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySize, I18n::Message::Default, false, I18n::Message::PythonCommandNumpySizeWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyTranspose, I18n::Message::Default, false, I18n::Message::PythonCommandNumpyTransposeWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySort, I18n::Message::Default, false, I18n::Message::PythonCommandNumpySortWithoutArg),  
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyToBytes, I18n::Message::Default, false, I18n::Message::PythonCommandNumpyToBytesWithoutArg)
};

const ToolboxMessageTree NumpyFunctionsModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyNdinfo),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyAll),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyAny),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyArgmax),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyArgmin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyArgsort),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyClip),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyConvolve),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyDiff),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyInterp),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyDot),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyCross),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyEqual),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyNot_equal),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFlip),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyIsfinite),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyIsinf),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMean),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMax),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMedian),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMinimum),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyMaximum),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyPolyfit),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyPolyval),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyRoll),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySortWithArguments),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyStd),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySum),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyTrace),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyTrapz),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyWhere),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyVectorize),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyAcos),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyAcosh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyArctan2),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyAround),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyAsin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyAsinh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyAtan),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyAtanh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyCeil),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyCos),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyCosh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyDegrees),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyExp),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyExpm1),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFloor),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyLog),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyLog10),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyLog2),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyRadians),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySinh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySqrt),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyTan),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyTanh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyBool),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFloat),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyUint8),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyInt8),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyUint16),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyInt16),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyNan),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyInf),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyE),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyPi),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpySetPrintOptions),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyGetPrintOptions)
};

const ToolboxMessageTree NumpyFftModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFftFunction, I18n::Message::PythonNumpyFftFunction, false, I18n::Message::PythonCommandNumpyFftFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFft),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyIfft)
};

const ToolboxMessageTree NumpyLinalgModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyLinalgFunction, I18n::Message::PythonNumpyLinalgFunction, false, I18n::Message::PythonCommandNumpyLinalgFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyDet),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyEig),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyCholesky),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyInv),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyNorm)
};

const ToolboxMessageTree NumpyModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromNumpy, I18n::Message::PythonImportNumpy, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandNumpyFunction, I18n::Message::PythonNumpyFunction, false, I18n::Message::PythonCommandNumpyFunctionWithoutArg),
  ToolboxMessageTree::Node(I18n::Message::NumpyNdarray, NumpyNdarrayModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::Functions, NumpyFunctionsModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::NumpyFftModule, NumpyFftModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::NumpyLinalgModule, NumpyLinalgModuleChildren)
};

const ToolboxMessageTree ScipyLinalgModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipyLinalgFunction, I18n::Message::PythonScipyLinalgFunction, false, I18n::Message::PythonCommandScipyLinalgFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipyLinalgChoSolve),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipyLinalgSolveTriangular)
};

const ToolboxMessageTree ScipyOptimizeModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipyOptimizeFunction, I18n::Message::PythonScipyOptimizeFunction, false, I18n::Message::PythonCommandScipyOptimizeFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipyOptimizeBisect),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipyOptimizeFmin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipyOptimizeNewton)
};

const ToolboxMessageTree ScipySignalModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipySignalFunction, I18n::Message::PythonScipySignalFunction, false, I18n::Message::PythonCommandScipySignalFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipySignalSosfilt),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipySignalSpectrogram)
};

const ToolboxMessageTree ScipySpecialModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipySpecialFunction, I18n::Message::PythonScipySpecialFunction, false, I18n::Message::PythonCommandScipySpecialFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipySpecialErf),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipySpecialErfc),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipySpecialGamma),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipySpecialGammaln),
};

const ToolboxMessageTree ScipyModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromScipy, I18n::Message::PythonImportScipy, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScipyFunction, I18n::Message::PythonScipyFunction, false, I18n::Message::PythonCommandScipyFunctionWithoutArg),
  ToolboxMessageTree::Node(I18n::Message::ScipyLinalgModule, ScipyLinalgModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::ScipyOptimizeModule, ScipyOptimizeModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::ScipySignalModule, ScipySignalModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::ScipySpecialModule, ScipySpecialModuleChildren),
};

const ToolboxMessageTree UlabModuleChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::NumpyModule, NumpyModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::ScipyModule, ScipyModuleChildren),
  ToolboxMessageTree::Leaf(I18n::Message::UlabDocumentation, I18n::Message::UlabDocumentationLink)
};

#endif

const ToolboxMessageTree TurtleModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportTurtle, I18n::Message::PythonImportTurtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromTurtle, I18n::Message::PythonImportTurtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTurtleFunction, I18n::Message::PythonTurtleFunction, false, I18n::Message::PythonCommandTurtleFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandForward, I18n::Message::PythonTurtleForward),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandBackward, I18n::Message::PythonTurtleBackward),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandRight, I18n::Message::PythonTurtleRight),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandLeft, I18n::Message::PythonTurtleLeft),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandGoto, I18n::Message::PythonTurtleGoto),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandSetheading, I18n::Message::PythonTurtleSetheading),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandCircle, I18n::Message::PythonTurtleCircle),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandSpeed, I18n::Message::PythonTurtleSpeed),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPosition, I18n::Message::PythonTurtlePosition, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandHeading, I18n::Message::PythonTurtleHeading, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPendown, I18n::Message::PythonTurtlePendown, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPenup, I18n::Message::PythonTurtlePenup, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPensize, I18n::Message::PythonTurtlePensize),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandIsdown, I18n::Message::PythonTurtleIsdown, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandWrite, I18n::Message::PythonTurtleWrite),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandReset, I18n::Message::PythonTurtleReset, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandShowturtle, I18n::Message::PythonTurtleShowturtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandHideturtle, I18n::Message::PythonTurtleHideturtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandColor, I18n::Message::PythonTurtleColor),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandColorMode, I18n::Message::PythonTurtleColorMode),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlue, I18n::Message::PythonColorBlue, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorRed, I18n::Message::PythonColorRed, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGreen, I18n::Message::PythonColorGreen, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorYellow, I18n::Message::PythonColorYellow, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBrown, I18n::Message::PythonColorBrown, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlack, I18n::Message::PythonColorBlack, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorWhite, I18n::Message::PythonColorWhite, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPink, I18n::Message::PythonColorPink, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorOrange, I18n::Message::PythonColorOrange, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPurple, I18n::Message::PythonColorPurple, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGray, I18n::Message::PythonColorGray, false)
};

const ToolboxMessageTree RandomModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportRandom, I18n::Message::PythonImportRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromRandom, I18n::Message::PythonImportRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandomFunction, I18n::Message::PythonRandomFunction, false, I18n::Message::PythonCommandRandomFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetrandbits, I18n::Message::PythonGetrandbits),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSeed, I18n::Message::PythonSeed),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandrange, I18n::Message::PythonRandrange),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandint, I18n::Message::PythonRandint),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandChoice, I18n::Message::PythonChoice),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandom, I18n::Message::PythonRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandUniform, I18n::Message::PythonUniform)
};

const ToolboxMessageTree KandinskyModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportKandinsky, I18n::Message::PythonImportKandinsky, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromKandinsky, I18n::Message::PythonImportKandinsky, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKandinskyFunction, I18n::Message::PythonKandinskyFunction, false, I18n::Message::PythonCommandKandinskyFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetPixel, I18n::Message::PythonGetPixel),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSetPixel, I18n::Message::PythonSetPixel),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColor, I18n::Message::PythonColor),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDrawString, I18n::Message::PythonDrawString),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDrawLine, I18n::Message::PythonDrawLine),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDrawCircle, I18n::Message::PythonDrawCircle),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFillRect, I18n::Message::PythonFillRect),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFillCircle, I18n::Message::PythonFillCircle),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFillPolygon, I18n::Message::PythonFillPolygon),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetKeys, I18n::Message::PythonGetKeys),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetPalette, I18n::Message::PythonGetPalette)};

const ToolboxMessageTree IonModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportIon, I18n::Message::PythonImportIon, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromIon, I18n::Message::PythonImportIon, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIonFunction, I18n::Message::PythonIonFunction, false, I18n::Message::PythonCommandIonFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsKeyDown, I18n::Message::PythonIsKeyDown),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBattery, I18n::Message::PythonBattery),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBatteryLevel, I18n::Message::PythonBatteryLevel),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBatteryIscharging, I18n::Message::PythonBatteryIscharging),
  ToolboxMessageTree::Leaf(I18n::Message::IonSelector, I18n::Message::IonSelector)
};

const ToolboxMessageTree TimeModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportTime, I18n::Message::PythonImportTime, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromTime, I18n::Message::PythonImportTime, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTimeFunction, I18n::Message::PythonTimeFunction, false, I18n::Message::PythonCommandTimeFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTime, I18n::Message::PythonTime),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMonotonic, I18n::Message::PythonMonotonic, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSleep, I18n::Message::PythonSleep),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLocalTime, I18n::Message::PythonLocalTime),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMktime, I18n::Message::PythonMktime),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSetLocaltime, I18n::Message::PythonSetLocaltime),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRTCmode, I18n::Message::PythonRTCmode),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSetRTCmode, I18n::Message::PythonSetRTCmode),
};

const ToolboxMessageTree OsModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportOs, I18n::Message::PythonImportOs, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromOs, I18n::Message::PythonImportOs, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonOsCommandUname, I18n::Message::PythonOsUname, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonOsCommandGetlogin, I18n::Message::PythonOsGetlogin, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonOsCommandRemove, I18n::Message::PythonOsRemove, false, I18n::Message::PythonOsCommandRemoveWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonOsCommandRename, I18n::Message::PythonOsRename, false, I18n::Message::PythonOsCommandRenameWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonOsCommandListdir, I18n::Message::PythonOsListdir, false)
};

#if MICROPY_PY_SYS
const ToolboxMessageTree SysModuleChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportSys, I18n::Message::PythonImportSys, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromSys, I18n::Message::PythonImportSys, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonSysCommandExit, I18n::Message::PythonSysExit, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonSysCommandPrintexception, I18n::Message::PythonSysPrintexception, false, I18n::Message::PythonSysCommandPrintexceptionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonSysCommandByteorder, I18n::Message::PythonSysByteorder, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonSysCommandImplementation, I18n::Message::PythonSysImplementation, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonSysCommandModules, I18n::Message::PythonSysModules, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonSysCommandVersion, I18n::Message::PythonSysVersion, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonSysCommandVersioninfo, I18n::Message::PythonSysVersioninfo, false)
};
#endif

const ToolboxMessageTree modulesChildren[] = {
  ToolboxMessageTree::Node(I18n::Message::MathModule, MathModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::CmathModule, CMathModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::MatplotlibPyplotModule, MatplotlibPyplotModuleChildren),
#if defined(INCLUDE_ULAB)
  ToolboxMessageTree::Node(I18n::Message::UlabModule, UlabModuleChildren),
#endif
  ToolboxMessageTree::Node(I18n::Message::TurtleModule, TurtleModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::RandomModule, RandomModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::KandinskyModule, KandinskyModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::IonModule, IonModuleChildren),
  ToolboxMessageTree::Node(I18n::Message::OsModule, OsModuleChildren),
#if MICROPY_PY_SYS
  ToolboxMessageTree::Node(I18n::Message::SysModule, SysModuleChildren),
#endif
  ToolboxMessageTree::Node(I18n::Message::TimeModule, TimeModuleChildren)
};

const ToolboxMessageTree catalogChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPound, I18n::Message::PythonPound, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPercent, I18n::Message::PythonPercent, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommand1J, I18n::Message::Python1J, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLF, I18n::Message::PythonLF, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTab, I18n::Message::PythonTab, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAmpersand, I18n::Message::PythonAmpersand, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSymbolExp, I18n::Message::PythonSymbolExp, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandVerticalBar, I18n::Message::PythonVerticalBar, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSingleQuote, I18n::Message::PythonSingleQuote, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAbs, I18n::Message::PythonAbs),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAcos, I18n::Message::PythonAcos),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAcosh, I18n::Message::PythonAcosh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandArrow, I18n::Message::PythonArrow),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAsin, I18n::Message::PythonAsin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAsinh, I18n::Message::PythonAsinh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtan, I18n::Message::PythonAtan),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtan2, I18n::Message::PythonAtan2),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAtanh, I18n::Message::PythonAtanh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAxis, I18n::Message::PythonAxis, false, I18n::Message::PythonCommandAxisWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandBackward, I18n::Message::PythonTurtleBackward),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBar, I18n::Message::PythonBar),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBin, I18n::Message::PythonBin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlack, I18n::Message::PythonColorBlack, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBlue, I18n::Message::PythonColorBlue,  false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorBrown, I18n::Message::PythonColorBrown, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCeil, I18n::Message::PythonCeil),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandChoice, I18n::Message::PythonChoice),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandCircle, I18n::Message::PythonTurtleCircle),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCmathFunction, I18n::Message::PythonCmathFunction, false, I18n::Message::PythonCommandCmathFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColor, I18n::Message::PythonColor),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandColorMode, I18n::Message::PythonTurtleColorMode),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandComplex, I18n::Message::PythonComplex),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCopySign, I18n::Message::PythonCopySign),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCos, I18n::Message::PythonCos),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCosh, I18n::Message::PythonCosh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDegrees, I18n::Message::PythonDegrees),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDivMod, I18n::Message::PythonDivMod),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDrawCircle, I18n::Message::PythonDrawCircle),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDrawLine, I18n::Message::PythonDrawLine),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDrawString, I18n::Message::PythonDrawString),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConstantE, I18n::Message::PythonConstantE, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandErf, I18n::Message::PythonErf),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandErfc, I18n::Message::PythonErfc),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandEval, I18n::Message::PythonEval),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExp, I18n::Message::PythonExp),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandExpm1, I18n::Message::PythonExpm1),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFabs, I18n::Message::PythonFabs),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFillCircle, I18n::Message::PythonFillCircle),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFillPolygon, I18n::Message::PythonFillPolygon),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFillRect, I18n::Message::PythonFillRect),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFloat, I18n::Message::PythonFloat),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFloor, I18n::Message::PythonFloor),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandForward, I18n::Message::PythonTurtleForward),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFmod, I18n::Message::PythonFmod),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFrExp, I18n::Message::PythonFrExp),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromCmath, I18n::Message::PythonImportCmath, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromIon, I18n::Message::PythonImportIon, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromKandinsky, I18n::Message::PythonImportKandinsky, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromMath, I18n::Message::PythonImportMath, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromMatplotlibPyplot, I18n::Message::PythonImportMatplotlibPyplot, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromRandom, I18n::Message::PythonImportRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromTurtle, I18n::Message::PythonImportTurtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportFromTime, I18n::Message::PythonImportTime, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGamma, I18n::Message::PythonGamma),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetPixel, I18n::Message::PythonGetPixel),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGetrandbits, I18n::Message::PythonGetrandbits),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandGoto, I18n::Message::PythonTurtleGoto),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGray, I18n::Message::PythonColorGray, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorGreen, I18n::Message::PythonColorGreen, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandGrid, I18n::Message::PythonGrid),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandHeading, I18n::Message::PythonTurtleHeading, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandHex, I18n::Message::PythonHex),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandHideturtle, I18n::Message::PythonTurtleHideturtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandHist, I18n::Message::PythonHist),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportCmath, I18n::Message::PythonImportCmath, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportIon, I18n::Message::PythonImportIon, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportKandinsky, I18n::Message::PythonImportKandinsky, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportMath, I18n::Message::PythonImportMath, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportMatplotlibPyplot, I18n::Message::PythonImportMatplotlibPyplot, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportRandom, I18n::Message::PythonImportRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportTurtle, I18n::Message::PythonImportTurtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImportTime, I18n::Message::PythonImportTime, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandInput, I18n::Message::PythonInput),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandInt, I18n::Message::PythonInt),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIonFunction, I18n::Message::PythonIonFunction, false, I18n::Message::PythonCommandIonFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandIsdown, I18n::Message::PythonTurtleIsdown, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsFinite, I18n::Message::PythonIsFinite),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsInfinite, I18n::Message::PythonIsInfinite),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsNaN, I18n::Message::PythonIsNaN),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandKandinskyFunction, I18n::Message::PythonKandinskyFunction, false, I18n::Message::PythonCommandKandinskyFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIsKeyDown, I18n::Message::PythonIsKeyDown),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandBattery, I18n::Message::PythonBattery),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLdexp, I18n::Message::PythonLdexp),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandLeft, I18n::Message::PythonTurtleLeft),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLength, I18n::Message::PythonLength),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLgamma, I18n::Message::PythonLgamma),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandAppend, I18n::Message::PythonAppend, false, I18n::Message::PythonCommandAppendWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandClear, I18n::Message::PythonClear, false, I18n::Message::PythonCommandClearWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandCount, I18n::Message::PythonCount, false, I18n::Message::PythonCommandCountWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandIndex, I18n::Message::PythonIndex, false, I18n::Message::PythonCommandIndexWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandInsert, I18n::Message::PythonInsert, false, I18n::Message::PythonCommandInsertWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPop, I18n::Message::PythonPop, false, I18n::Message::PythonCommandPopWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRemove, I18n::Message::PythonRemove, false, I18n::Message::PythonCommandRemoveWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandReverse, I18n::Message::PythonReverse, false, I18n::Message::PythonCommandReverseWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSort, I18n::Message::PythonSort, false, I18n::Message::PythonCommandSortWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog, I18n::Message::PythonLog),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog10, I18n::Message::PythonLog10),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandLog2, I18n::Message::PythonLog2),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMathFunction, I18n::Message::PythonMathFunction, false, I18n::Message::PythonCommandMathFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMatplotlibPyplotFunction, I18n::Message::PythonMatplotlibPyplotFunction, false, I18n::Message::PythonCommandMatplotlibPyplotFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMax, I18n::Message::PythonMax),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMin, I18n::Message::PythonMin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandModf, I18n::Message::PythonModf),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandMonotonic, I18n::Message::PythonMonotonic, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandOct, I18n::Message::PythonOct),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorOrange, I18n::Message::PythonColorOrange, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPendown, I18n::Message::PythonTurtlePendown, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPenup, I18n::Message::PythonTurtlePenup, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPensize, I18n::Message::PythonTurtlePensize),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPhase, I18n::Message::PythonPhase),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandConstantPi, I18n::Message::PythonConstantPi, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPink, I18n::Message::PythonColorPink, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPolar, I18n::Message::PythonPolar),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandPosition, I18n::Message::PythonTurtlePosition, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPower, I18n::Message::PythonPower),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPlot, I18n::Message::PythonPlot),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandPrint, I18n::Message::PythonPrint),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorPurple, I18n::Message::PythonColorPurple, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRadians, I18n::Message::PythonRadians),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandint, I18n::Message::PythonRandint),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandom, I18n::Message::PythonRandom, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandomFunction, I18n::Message::PythonRandomFunction, false, I18n::Message::PythonCommandRandomFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRandrange, I18n::Message::PythonRandrange),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRangeStartStop, I18n::Message::PythonRangeStartStop),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRangeStop, I18n::Message::PythonRangeStop),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRect, I18n::Message::PythonRect),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorRed, I18n::Message::PythonColorRed, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandReset, I18n::Message::PythonTurtleReset, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandRight, I18n::Message::PythonTurtleRight),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandRound, I18n::Message::PythonRound),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandScatter, I18n::Message::PythonScatter),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandSetheading, I18n::Message::PythonTurtleSetheading),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSetPixel, I18n::Message::PythonSetPixel),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSeed, I18n::Message::PythonSeed),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandShow, I18n::Message::PythonShow),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandShowturtle, I18n::Message::PythonTurtleShowturtle, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSin, I18n::Message::PythonSin),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSinh, I18n::Message::PythonSinh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSleep, I18n::Message::PythonSleep),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSorted, I18n::Message::PythonSort),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandSpeed, I18n::Message::PythonTurtleSpeed),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSqrt, I18n::Message::PythonSqrt),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandSum, I18n::Message::PythonSum),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTan, I18n::Message::PythonTan),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTanh, I18n::Message::PythonTanh),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandText, I18n::Message::PythonText),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTimeFunction, I18n::Message::PythonTimeFunction, false, I18n::Message::PythonCommandTimeFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTrunc, I18n::Message::PythonTrunc),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandTurtleFunction, I18n::Message::PythonTurtleFunction, false, I18n::Message::PythonCommandTurtleFunctionWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandUniform, I18n::Message::PythonUniform),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorWhite, I18n::Message::PythonColorWhite, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonTurtleCommandWrite, I18n::Message::PythonTurtleWrite),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandColorYellow, I18n::Message::PythonColorYellow, false),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandImag, I18n::Message::PythonImag, false, I18n::Message::PythonCommandImagWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandReal, I18n::Message::PythonReal, false, I18n::Message::PythonCommandRealWithoutArg)
};

const ToolboxMessageTree functionsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandDefWithArg, I18n::Message::Default, false, I18n::Message::PythonCommandDef),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandReturn, I18n::Message::Default)
};

const ToolboxMessageTree fileChildren[] {
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileOpen, I18n::Message::PythonFileOpen, false, I18n::Message::PythonCommandFileOpenWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileClose, I18n::Message::PythonFileClose, false, I18n::Message::PythonCommandFileCloseWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileClosed, I18n::Message::PythonFileClosed, false, I18n::Message::PythonCommandFileClosedWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileMode, I18n::Message::PythonFileMode, false, I18n::Message::PythonCommandFileModeWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileName, I18n::Message::PythonFileName, false, I18n::Message::PythonCommandFileNameWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileRead, I18n::Message::PythonFileRead, false, I18n::Message::PythonCommandFileReadWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileReadable, I18n::Message::PythonFileReadable, false, I18n::Message::PythonCommandFileReadableWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileReadline, I18n::Message::PythonFileReadline, false, I18n::Message::PythonCommandFileReadlineWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileReadlines, I18n::Message::PythonFileReadlines, false, I18n::Message::PythonCommandFileReadlinesWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileSeek, I18n::Message::PythonFileSeek, false, I18n::Message::PythonCommandFileSeekWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileSeekable, I18n::Message::PythonFileSeekable, false, I18n::Message::PythonCommandFileSeekableWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileTell, I18n::Message::PythonFileTell, false, I18n::Message::PythonCommandFileTellWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileTruncate, I18n::Message::PythonFileTruncate, false, I18n::Message::PythonCommandFileTruncateWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileWrite, I18n::Message::PythonFileWrite, false, I18n::Message::PythonCommandFileWriteWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileWritable, I18n::Message::PythonFileWritable, false, I18n::Message::PythonCommandFileWritableWithoutArg),
  ToolboxMessageTree::Leaf(I18n::Message::PythonCommandFileWritelines, I18n::Message::PythonFileWritelines, false, I18n::Message::PythonCommandFileWritelinesWithoutArg),
};

const ToolboxMessageTree exceptionsChildren[] = {
  ToolboxMessageTree::Leaf(I18n::Message::TryExcept1ErrorWithArg, I18n::Message::Default, false, I18n::Message::TryExcept1Error),
  ToolboxMessageTree::Leaf(I18n::Message::TryExcept1ErrorElseWithArg, I18n::Message::Default, false, I18n::Message::TryExcept1ErrorElse),
  ToolboxMessageTree::Leaf(I18n::Message::TryExcept2ErrorWithArg, I18n::Message::Default, false, I18n::Message::TryExcept2Error),
  ToolboxMessageTree::Leaf(I18n::Message::WithInstructionWithArg, I18n::Message::Default, false, I18n::Message::WithInstruction),
};

const ToolboxMessageTree menu[] = {
  ToolboxMessageTree::Node(I18n::Message::LoopsAndTests, loopsAndTestsChildren),
  ToolboxMessageTree::Node(I18n::Message::Modules, modulesChildren),
  ToolboxMessageTree::Node(I18n::Message::Catalog, catalogChildren),
  ToolboxMessageTree::Node(I18n::Message::Functions, functionsChildren),
  ToolboxMessageTree::Node(I18n::Message::Files, fileChildren),
  ToolboxMessageTree::Node(I18n::Message::Exceptions, exceptionsChildren)
};

const ToolboxMessageTree toolboxModel = ToolboxMessageTree::Node(I18n::Message::Toolbox, menu);


PythonToolbox::PythonToolbox() :
  Toolbox(nullptr, rootModel()->label())
{
}

const ToolboxMessageTree * PythonToolbox::moduleChildren(const char * name, int * numberOfNodes) const {
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

bool PythonToolbox::handleEvent(Ion::Events::Event event) {
  if (Toolbox::handleEvent(event)) {
    return true;
  }
  if (event.hasText() && strlen(event.text()) == 1 ) {
    char c = event.text()[0];
    if (CodePoint(c).isLatinLetter()) {
      scrollToLetter(c);
      return true;
    }
  }
  return false;
}

KDCoordinate PythonToolbox::rowHeight(int j) {
  if (typeAtLocation(0, j) == Toolbox::LeafCellType && (m_messageTreeModel->label() == I18n::Message::IfStatementMenu || m_messageTreeModel->label() == I18n::Message::Exceptions)) {
      /* To get the exact height needed for each cell, we have to compute its
       * text size, which means scan the text char by char to look for '\n'
       * chars. This is very costly and ruins the speed performance when
       * scrolling at the bottom of a long table: to compute a position on the
       * kth row, we call cumulatedHeightFromIndex(k), which calls rowHeight k
       * times.
       * We thus decided to compute the real height only for the ifStatement
       * children of the toolbox, which is the only menu that has special height
       * rows. */
    const ToolboxMessageTree * messageTree = static_cast<const ToolboxMessageTree *>(m_messageTreeModel->childAtIndex(j));
    return k_font->stringSize(I18n::translate(messageTree->label())).height() + 2*Metric::TableCellVerticalMargin + (messageTree->text() == I18n::Message::Default ? 0 : Toolbox::rowHeight(j));
  }
  return Toolbox::rowHeight(j);
}

bool PythonToolbox::selectLeaf(int selectedRow) {
  ToolboxMessageTree * node = (ToolboxMessageTree *)m_messageTreeModel->childAtIndex(selectedRow);
#if defined(INCLUDE_ULAB)
  if(node->text() == I18n::Message::UlabDocumentationLink){
    return true;
  }
#endif
  m_selectableTableView.deselectTable();
  if(node->insertedText() == I18n::Message::IonSelector){
    m_ionKeys.setSender(sender());
    Container::activeApp()->displayModalViewController(static_cast<ViewController*>(&m_ionKeys), 0.f, 0.f, Metric::PopUpTopMargin, Metric::PopUpLeftMargin, 0, Metric::PopUpRightMargin);
    return true;
  }
  const char * editedText = I18n::translate(node->insertedText());
  // strippedEditedText array needs to be in the same scope as editedText
  char strippedEditedText[k_maxMessageSize];
  if (node->stripInsertedText()) {
    int strippedEditedTextMaxLength = strlen(editedText)+1;
    assert(strippedEditedTextMaxLength <= k_maxMessageSize);
    Shared::ToolboxHelpers::TextToInsertForCommandMessage(node->insertedText(), strippedEditedText, strippedEditedTextMaxLength, true);
    editedText = strippedEditedText;
  }
  sender()->handleEventWithText(editedText, true);
  Container::activeApp()->dismissModalViewController();
  return true;
}

const ToolboxMessageTree * PythonToolbox::rootModel() const {
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
  assert(CodePoint(letter).isLatinLetter());
  /* We look for a child MessageTree that starts with the wanted letter. If we
   * do not find one, we scroll to the first child MessageTree that starts with
   * a letter higher than the wanted letter. */
  char lowerLetter = tolower(letter);
  int index = -1;
  for (int i = 0; i < m_messageTreeModel->numberOfChildren(); i++) {
    char l = tolower(I18n::translate(m_messageTreeModel->childAtIndex(i)->label())[0]);
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

void PythonToolbox::scrollToAndSelectChild(int i) {
  assert(i >=0 && i<m_messageTreeModel->numberOfChildren());
  m_selectableTableView.deselectTable();
  m_selectableTableView.scrollToCell(0, i);
  m_selectableTableView.selectCellAtLocation(0, i);
}

}

