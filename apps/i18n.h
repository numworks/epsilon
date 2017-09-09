#ifndef APPS_I18N_H
#define APPS_I18N_H

#include <escher.h>

namespace I18n {
  constexpr static int NumberOfLanguages = 5;

  enum class Language : uint16_t {
    Default = 0,
    English = 1,
    French = 2,
    Spanish = 3,
    German = 4,
    Portuguese = 5
  };

  class StandardMessage : public Message {
  public:
    StandardMessage(const char *english, const char *french = nullptr, const char *spanish = nullptr, const char *german = nullptr, const char *portuguese = nullptr);
    const char * translate(Language l) const;
  private:
    const char *m_english;
    const char *m_french;
    const char *m_spanish;
    const char *m_german;
    const char *m_portuguese;
  };

namespace Common {
  extern const I18n::StandardMessage Warning;
  extern const I18n::StandardMessage Ok;
  extern const I18n::StandardMessage Cancel;
  extern const I18n::StandardMessage Next;
  extern const I18n::StandardMessage SyntaxError;
  extern const I18n::StandardMessage MathError;
  extern const I18n::StandardMessage LowBattery;

  /* Variables */
  extern const I18n::StandardMessage Variables;
  extern const I18n::StandardMessage Number;
  extern const I18n::StandardMessage Matrix;
  extern const I18n::StandardMessage List;
  extern const I18n::StandardMessage Empty;
  
  /* Toolbox */
  extern const I18n::StandardMessage Toolbox;
  extern const I18n::StandardMessage AbsoluteValue;
  extern const I18n::StandardMessage NthRoot;
  extern const I18n::StandardMessage BasedLogarithm;
  extern const I18n::StandardMessage Calculation;
  extern const I18n::StandardMessage ComplexNumber;
  extern const I18n::StandardMessage Probability;
  extern const I18n::StandardMessage Arithmetic;
  extern const I18n::StandardMessage Matrices;
  extern const I18n::StandardMessage Lists;
  extern const I18n::StandardMessage Approximation;
  extern const I18n::StandardMessage HyperbolicTrigonometry;
  extern const I18n::StandardMessage Fluctuation;
  extern const I18n::StandardMessage DerivateNumber;
  extern const I18n::StandardMessage Integral;
  extern const I18n::StandardMessage Sum;
  extern const I18n::StandardMessage Product;
  extern const I18n::StandardMessage ComplexAbsoluteValue;
  extern const I18n::StandardMessage Agument;
  extern const I18n::StandardMessage ReelPart;
  extern const I18n::StandardMessage ImaginaryPart;
  extern const I18n::StandardMessage Conjugate;
  extern const I18n::StandardMessage Combination;
  extern const I18n::StandardMessage Permutation;
  extern const I18n::StandardMessage GreatCommonDivisor;
  extern const I18n::StandardMessage LeastCommonMultiple;
  extern const I18n::StandardMessage Remainder;
  extern const I18n::StandardMessage Quotient;
  extern const I18n::StandardMessage Inverse;
  extern const I18n::StandardMessage Determinant;
  extern const I18n::StandardMessage Transpose;
  extern const I18n::StandardMessage Trace;
  extern const I18n::StandardMessage Dimension;
  extern const I18n::StandardMessage Sort;
  extern const I18n::StandardMessage InvSort;
  extern const I18n::StandardMessage Maximum;
  extern const I18n::StandardMessage Minimum;
  extern const I18n::StandardMessage Floor;
  extern const I18n::StandardMessage FracPart;
  extern const I18n::StandardMessage Ceiling;
  extern const I18n::StandardMessage Rounding;
  extern const I18n::StandardMessage HyperbolicCosine;
  extern const I18n::StandardMessage HyperbolicSine;
  extern const I18n::StandardMessage HyperbolicTangent;
  extern const I18n::StandardMessage InverseHyperbolicCosine;
  extern const I18n::StandardMessage InverseHyperbolicSine;
  extern const I18n::StandardMessage InverseHyperbolicTangent;
  extern const I18n::StandardMessage Prediction95;
  extern const I18n::StandardMessage Prediction;
  extern const I18n::StandardMessage Confidence;

  /* Applications */
  extern const I18n::StandardMessage Apps;
  extern const I18n::StandardMessage AppsCapital;

  /* 1.Calculation */
  extern const I18n::StandardMessage CalculApp;
  extern const I18n::StandardMessage CalculAppCapital;

  /* 2.Function */
  extern const I18n::StandardMessage FunctionApp;
  extern const I18n::StandardMessage FunctionAppCapital;
  extern const I18n::StandardMessage FunctionTab;
  extern const I18n::StandardMessage GraphTab;
  extern const I18n::StandardMessage ValuesTab;

  /* Function: first tab */
  extern const I18n::StandardMessage Plot;
  extern const I18n::StandardMessage DisplayValues;
  extern const I18n::StandardMessage FunctionOptions;
  extern const I18n::StandardMessage AddFunction;
  extern const I18n::StandardMessage DeleteFunction;
  extern const I18n::StandardMessage NoFunctionToDelete;
  extern const I18n::StandardMessage ActivateDesactivate;
  extern const I18n::StandardMessage FunctionColor;

  /* Function: second tab */
  extern const I18n::StandardMessage NoFunction;
  extern const I18n::StandardMessage NoActivatedFunction;
  extern const I18n::StandardMessage Axis;
  extern const I18n::StandardMessage Zoom;
  extern const I18n::StandardMessage Initialization;
  extern const I18n::StandardMessage Move;
  extern const I18n::StandardMessage ToZoom;
  extern const I18n::StandardMessage Or;
  extern const I18n::StandardMessage Trigonometric;
  extern const I18n::StandardMessage RoundAbscissa;
  extern const I18n::StandardMessage Orthonormal;
  extern const I18n::StandardMessage DefaultSetting;
  extern const I18n::StandardMessage PlotOptions;
  extern const I18n::StandardMessage Compute;
  extern const I18n::StandardMessage Goto;
  extern const I18n::StandardMessage ValueNotReachedByFunction;
  extern const I18n::StandardMessage Zeros;
  extern const I18n::StandardMessage Tangent;
  extern const I18n::StandardMessage Intersection;
  extern const I18n::StandardMessage SelectLowerBound;
  extern const I18n::StandardMessage SelectUpperBound;
  extern const I18n::StandardMessage NoZeroFound;

  /* Function: third tab */
  extern const I18n::StandardMessage IntervalSet;
  extern const I18n::StandardMessage XStart;
  extern const I18n::StandardMessage XEnd;
  extern const I18n::StandardMessage Step;
  extern const I18n::StandardMessage XColumn;
  extern const I18n::StandardMessage FunctionColumn;
  extern const I18n::StandardMessage DerivativeColumn;
  extern const I18n::StandardMessage DerivativeFunctionColumn;
  extern const I18n::StandardMessage ClearColumn;
  extern const I18n::StandardMessage CopyColumnInList;
  extern const I18n::StandardMessage HideDerivativeColumn;

  /* Sequence */
  extern const I18n::StandardMessage SequenceApp;
  extern const I18n::StandardMessage SequenceAppCapital;
  extern const I18n::StandardMessage SequenceTab;
  extern const I18n::StandardMessage AddSequence;
  extern const I18n::StandardMessage ChooseSequenceType;
  extern const I18n::StandardMessage SequenceType;
  extern const I18n::StandardMessage Explicite;
  extern const I18n::StandardMessage SingleRecurrence;
  extern const I18n::StandardMessage DoubleRecurrence;
  extern const I18n::StandardMessage SequenceOptions;
  extern const I18n::StandardMessage SequenceColor;
  extern const I18n::StandardMessage DeleteSequence;
  extern const I18n::StandardMessage NoSequence;
  extern const I18n::StandardMessage NoActivatedSequence;
  extern const I18n::StandardMessage NStart;
  extern const I18n::StandardMessage NEnd;
  extern const I18n::StandardMessage TermSum;
  extern const I18n::StandardMessage SelectFirstTerm;
  extern const I18n::StandardMessage SelectLastTerm;
  extern const I18n::StandardMessage ValueNotReachedBySequence;

  /* Sequence: third tab */
  extern const I18n::StandardMessage NColumn;

  /* Statistics */
  extern const I18n::StandardMessage StatsApp;
  extern const I18n::StandardMessage StatsAppCapital;
  extern const I18n::StandardMessage DataTab;
  extern const I18n::StandardMessage HistogramTab;
  extern const I18n::StandardMessage BoxTab;
  extern const I18n::StandardMessage StatTab;

  /* Statistics: first tab */
  extern const I18n::StandardMessage Values;
  extern const I18n::StandardMessage Sizes;
  extern const I18n::StandardMessage ColumnOptions;
  extern const I18n::StandardMessage ImportList;

  /* Statistics: second tab */
  extern const I18n::StandardMessage NoDataToPlot;
  extern const I18n::StandardMessage Interval;
  extern const I18n::StandardMessage Size;
  extern const I18n::StandardMessage Frequency;
  extern const I18n::StandardMessage HistogramSet;
  extern const I18n::StandardMessage RectangleWidth;
  extern const I18n::StandardMessage BarStart;

  /* Statistics: third tab */
  extern const I18n::StandardMessage FirstQuartile;
  extern const I18n::StandardMessage Median;
  extern const I18n::StandardMessage ThirdQuartile;

  /* Statistics: fourth tab */
  extern const I18n::StandardMessage NoValueToCompute;
  extern const I18n::StandardMessage TotalSize;
  extern const I18n::StandardMessage Range;
  extern const I18n::StandardMessage Mean;
  extern const I18n::StandardMessage StandardDeviation;
  extern const I18n::StandardMessage Deviation;
  extern const I18n::StandardMessage InterquartileRange;
  extern const I18n::StandardMessage SquareSum;

  /* Probability */
  extern const I18n::StandardMessage ProbaApp;
  extern const I18n::StandardMessage ProbaAppCapital;
  extern const I18n::StandardMessage ChooseLaw;
  extern const I18n::StandardMessage Binomial;
  extern const I18n::StandardMessage Uniforme;
  extern const I18n::StandardMessage Exponential;
  extern const I18n::StandardMessage Normal;
  extern const I18n::StandardMessage Poisson;
  extern const I18n::StandardMessage BinomialLaw;
  extern const I18n::StandardMessage UniformLaw;
  extern const I18n::StandardMessage ExponentialLaw;
  extern const I18n::StandardMessage NormalLaw;
  extern const I18n::StandardMessage PoissonLaw;
  extern const I18n::StandardMessage ChooseParameters;
  extern const I18n::StandardMessage RepetitionNumber;
  extern const I18n::StandardMessage SuccessProbability;
  extern const I18n::StandardMessage IntervalDefinition;
  extern const I18n::StandardMessage LambdaExponentialDefinition;
  extern const I18n::StandardMessage MeanDefinition;
  extern const I18n::StandardMessage DeviationDefinition;
  extern const I18n::StandardMessage LambdaPoissonDefinition;
  extern const I18n::StandardMessage ComputeProbability;
  extern const I18n::StandardMessage ForbiddenValue;
  extern const I18n::StandardMessage UndefinedValue;

  /* Regression */
  extern const I18n::StandardMessage RegressionApp;
  extern const I18n::StandardMessage RegressionAppCapital;
  extern const I18n::StandardMessage Regression;
  extern const I18n::StandardMessage NoEnoughDataForRegression;
  extern const I18n::StandardMessage Reg;
  extern const I18n::StandardMessage MeanDot;
  extern const I18n::StandardMessage RegressionSlope;
  extern const I18n::StandardMessage XPrediction;
  extern const I18n::StandardMessage YPrediction;
  extern const I18n::StandardMessage ValueNotReachedByRegression;
  extern const I18n::StandardMessage NumberOfDots;
  extern const I18n::StandardMessage Covariance;

  /* Settings */
  extern const I18n::StandardMessage SettingsApp;
  extern const I18n::StandardMessage SettingsAppCapital;
  extern const I18n::StandardMessage AngleUnit;
  extern const I18n::StandardMessage DisplayMode;
  extern const I18n::StandardMessage ComplexFormat;
  extern const I18n::StandardMessage Language;
  extern const I18n::StandardMessage ExamMode;
  extern const I18n::StandardMessage ActivateExamMode;
  extern const I18n::StandardMessage ExamModeActive;
  extern const I18n::StandardMessage ActiveExamModeMessage1;
  extern const I18n::StandardMessage ActiveExamModeMessage2;
  extern const I18n::StandardMessage ActiveExamModeMessage3;
  extern const I18n::StandardMessage ExitExamMode1;
  extern const I18n::StandardMessage ExitExamMode2;
  extern const I18n::StandardMessage About;
  extern const I18n::StandardMessage Degres;
  extern const I18n::StandardMessage Radian;
  extern const I18n::StandardMessage Auto;
  extern const I18n::StandardMessage Scientific;
  extern const I18n::StandardMessage Deg;
  extern const I18n::StandardMessage Rad;
  extern const I18n::StandardMessage Sci;
  extern const I18n::StandardMessage Brightness;
  extern const I18n::StandardMessage SoftwareVersion;
  extern const I18n::StandardMessage SerialNumber;
  extern const I18n::StandardMessage UpdatePopUp;
  extern const I18n::StandardMessage HardwareTestLaunch1;
  extern const I18n::StandardMessage HardwareTestLaunch2;
  extern const I18n::StandardMessage HardwareTestLaunch3;
  extern const I18n::StandardMessage HardwareTestLaunch4;

  /* Code */
  extern const I18n::StandardMessage BetaVersion;
  extern const I18n::StandardMessage BetaVersionMessage1;
  extern const I18n::StandardMessage BetaVersionMessage2;
  extern const I18n::StandardMessage BetaVersionMessage3;
  extern const I18n::StandardMessage BetaVersionMessage4;
  extern const I18n::StandardMessage EditProgram;
  extern const I18n::StandardMessage ExecuteProgram;

  /* On boarding */
  extern const I18n::StandardMessage UpdateAvailable;
  extern const I18n::StandardMessage UpdateMessage1;
  extern const I18n::StandardMessage UpdateMessage2;
  extern const I18n::StandardMessage UpdateMessage3;
  extern const I18n::StandardMessage UpdateMessage4;
  extern const I18n::StandardMessage Skip;

  /* Universal messages */
  extern const I18n::UniversalMessage Default;
  extern const I18n::UniversalMessage CodeApp;
  extern const I18n::UniversalMessage CodeAppCapital;
  extern const I18n::UniversalMessage Alpha;
  extern const I18n::UniversalMessage CapitalAlpha;
  extern const I18n::UniversalMessage Shift;
  extern const I18n::UniversalMessage X;
  extern const I18n::UniversalMessage Y;
  extern const I18n::UniversalMessage N;
  extern const I18n::UniversalMessage P;
  extern const I18n::UniversalMessage Mu;
  extern const I18n::UniversalMessage Sigma;
  extern const I18n::UniversalMessage Lambda;
  extern const I18n::UniversalMessage A;
  extern const I18n::UniversalMessage B;
  extern const I18n::UniversalMessage R;
  extern const I18n::UniversalMessage Sxy;
  extern const I18n::UniversalMessage XMin;
  extern const I18n::UniversalMessage XMax;
  extern const I18n::UniversalMessage YMin;
  extern const I18n::UniversalMessage YMax;
  extern const I18n::UniversalMessage YAuto;
  extern const I18n::UniversalMessage RightIntegralFirstLegend;
  extern const I18n::UniversalMessage RightIntegralSecondLegend;
  extern const I18n::UniversalMessage LeftIntegralFirstLegend;
  extern const I18n::UniversalMessage LeftIntegralSecondLegend;
  extern const I18n::UniversalMessage FiniteIntegralLegend;
  extern const I18n::UniversalMessage DiscreteLegend;
  extern const I18n::UniversalMessage RegressionFormula;
  extern const I18n::UniversalMessage French;
  extern const I18n::UniversalMessage English;
  extern const I18n::UniversalMessage Spanish;
  extern const I18n::UniversalMessage German;
  extern const I18n::UniversalMessage Portuguese;
  extern const I18n::UniversalMessage FccId;

  /* Toolbox: commands */
  extern const I18n::UniversalMessage AbsCommand;
  extern const I18n::UniversalMessage RootCommand;
  extern const I18n::UniversalMessage LogCommand;
  extern const I18n::UniversalMessage DiffCommand;
  extern const I18n::UniversalMessage IntCommand;
  extern const I18n::UniversalMessage SumCommand;
  extern const I18n::UniversalMessage ProductCommand;
  extern const I18n::UniversalMessage ArgCommand;
  extern const I18n::UniversalMessage ReCommand;
  extern const I18n::UniversalMessage ImCommand;
  extern const I18n::UniversalMessage ConjCommand;
  extern const I18n::UniversalMessage BinomialCommand;
  extern const I18n::UniversalMessage PermuteCommand;
  extern const I18n::UniversalMessage GcdCommand;
  extern const I18n::UniversalMessage LcmCommand;
  extern const I18n::UniversalMessage RemCommand;
  extern const I18n::UniversalMessage QuoCommand;
  extern const I18n::UniversalMessage InverseCommand;
  extern const I18n::UniversalMessage DeterminantCommand;
  extern const I18n::UniversalMessage TransposeCommand;
  extern const I18n::UniversalMessage TraceCommand;
  extern const I18n::UniversalMessage DimensionCommand;
  extern const I18n::UniversalMessage SortCommand;
  extern const I18n::UniversalMessage InvSortCommand;
  extern const I18n::UniversalMessage MaxCommand;
  extern const I18n::UniversalMessage MinCommand;
  extern const I18n::UniversalMessage FloorCommand;
  extern const I18n::UniversalMessage FracCommand;
  extern const I18n::UniversalMessage CeilCommand;
  extern const I18n::UniversalMessage RoundCommand;
  extern const I18n::UniversalMessage CoshCommand;
  extern const I18n::UniversalMessage SinhCommand;
  extern const I18n::UniversalMessage TanhCommand;
  extern const I18n::UniversalMessage AcoshCommand;
  extern const I18n::UniversalMessage AsinhCommand;
  extern const I18n::UniversalMessage AtanhCommand;
  extern const I18n::UniversalMessage Prediction95Command;
  extern const I18n::UniversalMessage PredictionCommand;
  extern const I18n::UniversalMessage ConfidenceCommand;
  extern const I18n::UniversalMessage AbsCommandWithArg;
  extern const I18n::UniversalMessage RootCommandWithArg;
  extern const I18n::UniversalMessage LogCommandWithArg;
  extern const I18n::UniversalMessage DiffCommandWithArg;
  extern const I18n::UniversalMessage IntCommandWithArg;
  extern const I18n::UniversalMessage SumCommandWithArg;
  extern const I18n::UniversalMessage ProductCommandWithArg;
  extern const I18n::UniversalMessage ArgCommandWithArg;
  extern const I18n::UniversalMessage ReCommandWithArg;
  extern const I18n::UniversalMessage ImCommandWithArg;
  extern const I18n::UniversalMessage ConjCommandWithArg;
  extern const I18n::UniversalMessage BinomialCommandWithArg;
  extern const I18n::UniversalMessage PermuteCommandWithArg;
  extern const I18n::UniversalMessage GcdCommandWithArg;
  extern const I18n::UniversalMessage LcmCommandWithArg;
  extern const I18n::UniversalMessage RemCommandWithArg;
  extern const I18n::UniversalMessage QuoCommandWithArg;
  extern const I18n::UniversalMessage InverseCommandWithArg;
  extern const I18n::UniversalMessage DeterminantCommandWithArg;
  extern const I18n::UniversalMessage TransposeCommandWithArg;
  extern const I18n::UniversalMessage TraceCommandWithArg;
  extern const I18n::UniversalMessage DimensionCommandWithArg;
  extern const I18n::UniversalMessage SortCommandWithArg;
  extern const I18n::UniversalMessage InvSortCommandWithArg;
  extern const I18n::UniversalMessage MaxCommandWithArg;
  extern const I18n::UniversalMessage MinCommandWithArg;
  extern const I18n::UniversalMessage FloorCommandWithArg;
  extern const I18n::UniversalMessage FracCommandWithArg;
  extern const I18n::UniversalMessage CeilCommandWithArg;
  extern const I18n::UniversalMessage RoundCommandWithArg;
  extern const I18n::UniversalMessage CoshCommandWithArg;
  extern const I18n::UniversalMessage SinhCommandWithArg;
  extern const I18n::UniversalMessage TanhCommandWithArg;
  extern const I18n::UniversalMessage AcoshCommandWithArg;
  extern const I18n::UniversalMessage AsinhCommandWithArg;
  extern const I18n::UniversalMessage AtanhCommandWithArg;
  extern const I18n::UniversalMessage Prediction95CommandWithArg;
  extern const I18n::UniversalMessage PredictionCommandWithArg;
  extern const I18n::UniversalMessage ConfidenceCommandWithArg;
}
}

#endif
