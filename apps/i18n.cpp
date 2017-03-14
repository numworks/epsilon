#include "i18n.h"
#include "global_preferences.h"
#include <assert.h>

namespace I18n {

constexpr static char lambdaExponentialFrenchDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'e', 0};
constexpr static char lambdaExponentialEnglishDefinition[] = {Ion::Charset::SmallLambda, ':', ' ', 'R', 'a', 't','e',' ', 'p', 'a', 'r', 'a', 'm', 'e', 't', 'e', 'r', 0};
constexpr static char lambdaExponentialSpanishDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'o', 0};
constexpr static char lambdaPoissonFrenchDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'e', 0};
constexpr static char lambdaPoissonEnglishDefinition[] = {Ion::Charset::SmallLambda, ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'e', 'r', 0};
constexpr static char lambdaPoissonSpanishDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'o', 0};
constexpr static char meanFrenchDefinition[] = {Ion::Charset::SmallMu, ' ', ':', ' ', 'M', 'o', 'y', 'e', 'n', 'n', 'e', 0};
constexpr static char meanEnglishDefinition[] = {Ion::Charset::SmallMu, ':', ' ', 'M', 'e', 'a', 'n', 0};
constexpr static char meanSpanishDefinition[] = {Ion::Charset::SmallMu, ' ', ':', ' ', 'M', 'e', 'd', 'i', 'a', 0};
constexpr static char deviationFrenchDefinition[] = {Ion::Charset::SmallSigma, ':', ' ', 'E', 'c', 'a', 'r', 't', '-', 't', 'y', 'p', 'e', 0};
constexpr static char deviationEnglishDefinition[] = {Ion::Charset::SmallSigma, ' ', ':', ' ', 'S', 't', 'a', 'n', 'd', 'a', 'r', 'd', ' ','d','e', 'v', 'i', 'a', 't','i','o','n', 0};
constexpr static char deviationSpanishDefinition[] = {Ion::Charset::SmallSigma, ' ', ':', ' ', 'D', 'e','s','v','i','a','c','i','o','n',' ','t','i','p','i','c','a',0};

const char * messages[200][3] {
  {"Attention", "Warning", "Cuidado"},
  {"Valider", "Confirm", "Confirmar"},
  {"Suivant", "Next", "Siguiente"},
  {"Attention a la syntaxe", "Syntax error", "Error sintactico"},
  {"Error mathematique", "Math error", "Error matematico"},

  /* Variables */
  {"Variables", "Variables", "Variables"},
  {"Nombres", "Numbers", "Numeros"},
  {"Listes", "Lists", "Listas"},
  {"Matrices", "Matrices", "Matrices"},

  /* Toolbox */
  {"Toolbox", "Toolbox", "Toolbox"},
  {"Valeur absolue", "Absolute value", "Valor absoluto"},
  {"Racine n-ieme", "nth-root", "Raiz enesima"},
  {"Logarithme base a", "Logarithm to base a", "Logaritmo en base b"},
  {"Calculs", "Calculation", "Calculos"},
  {"Nombres complexes", "Complex numbers", "Numeros complejos"},
  {"Probabilites", "Probability", "Probabilidad"},
  {"Arithmetique", "Arithmetic", "Aritmetica"},
  {"Matrices", "Matrix", "Matriz"},
  {"Listes", "List", "Listas"},
  {"Approximation", "Approximation", "Aproximacion"},
  {"Trigonometrie hyperbolique", "Hyperbolic trigonometry", "Trigonometria hiberbolica"},
  {"Intervalle fluctuation", "Prediction Interval", "Interval de prediccion"},
  {"Nombre derive", "Derivative", "Derivada"},
  {"Integrale", "Integral", "Integral"},
  {"Somme", "Sum", "Suma"},
  {"Produit", "Product", "Productorio"},
  {"Module", "Absolute value", "Modulo"},
  {"Argument", "Argument", "Argumento"},
  {"Partie reelle", "Real part", "Parte real"},
  {"Partie imaginaire", "Imaginary part", "Parte imaginaria"},
  {"Conjugue", "Conjugate", "Conjugado"},
  {"Combinaison", "Combination", "Combinacion"},
  {"Arrangement", "Permutation", "Variacion"},
  {"PGCD", "GCD", "MCD"},
  {"PPCM", "LCM", "MCM"},
  {"Reste division p par q", "Remainder division p by q", "Resto division p por q"},
  {"Quotient division p par q", "Quotient division p by q", "Cociente division p por q"},
  {"Inverse", "Inverse", "Inversa"},
  {"Determinant", "Determinant", "Determinante"},
  {"Transposee", "Transpose", "Transpuesta"},
  {"Trace ", "Trace", "Traza"},
  {"Taille", "Size", "Tamano"},
  {"Tri croissant", "Sort ascending ", "Clasificacion ascendente"},
  {"Tri decroissant", "Sort descending", "Clasificacion descendente"},
  {"Maximum", "Maximum", "Maximo"},
  {"Minimum", "Minimum", "Minimo"},
  {"Partie entiere", "Floor", "Parte entera"},
  {"Partie fractionnaire", "Fractional part", "Parte fraccionaria"},
  {"Plafond", "Ceiling", "Techo"},
  {"Arrondi n chiffres apres la virgule", "Rounding to n digits of precision", "Redondeo n digitos despues de la coma"},
  {"Intervalle fluctuation 95%", "Prediction interval 95%", "Intervalo de prediccion 95%"},
  {"Intervalle fluctuation simple", "Simple prediction interval", "Intervalo de prediciion simple"},
  {"Intervalle confiance", "Confidence interval", "Intervalo de confianza"},

  /* Applications */
  {"Applications", "Applications", "Aplicacion"},
  {"APPLICATIONS", "APPLICATIONS", "APLICACION"},

  /* 1.Calculation */
  {"Calculs", "Calculation", "Calculo"},
  {"CALCULS", "CALCULATION", "CALCULO"},

  /* 2.Function */
  {"Fonctions", "Functions", "Funcion"},
  {"FONCTIONS", "FUNCTIONS", "FUNCION"},
  {"Fonctions", "Functions", "Funciones"},
  {"Graphique", "Graph", "Grafico"},
  {"Valeurs", "Table", "Tabla"},
    /* Function: first tab */
  {"Tracer", "Plot", "Dibujar"},
  {"Afficher les valeurs", "Display values", "Visualizar los valores"},
  {"Options de la fonction", "Function options", "Opciones de la funcion"},
  {"Ajouter une fonction", "Add function", "Agregar una funcion"},
  {"Supprimer la fonction", "Delete function", "Eliminar la funcion"},
  {"Pas de fonction a supprimer", "No function to delete", "Ninguna funcion que eliminar"},
  {"Activer/Desactiver", "Turn on/off", "Activar/Desactivar"},
  {"Couleur de la fonction", "Function color", "Color de la funcion"},
    /* Function: second tab */
  {"Aucune fonction", "No function", "Ninguna funcion"},
  {"Aucune fonction activee", "No function is turned on", "Ninguna funcion activada"},
  {"Axes", "Axes", "Ejes"},
  {"Zoom", "Zoom", "Zoom"},
  {"Zoom interactif", "Interactive zoom", "Zoom interactivo"},
  {"Zoom predefini", "Predefined zoom", "Zoom predefinido"},
  {"Initialisation", "Preadjustment", "Inicializacion"},
  {"HAUT", "TOP", "???"},
  {"GAUCHE", "LEFT", "???"},
  {"BAS", "BOTTM", "???"},
  {"DROITE", "RIGHT", "???"},
  {"ZOOM", "ZOOM", "ZOOM"},
  {"Se deplacer", "Move", "Mover"},
  {"Zoomer", "Zoom", "Zoom"},
  {"Trigonometrique", "Trigonometrical", "Trigonometrico"},
  {"Abscisses entieres", "Integer", "Abscisas enteras"},
  {"Orthonorme", "Orthonormal", "Ortonormal"},
  {"Reglages de base", "Basic settings", "Ajustes basicos"},
  {"Options de la courbe", "Plot options", "Opciones de la curva"},
  {"Calculer", "Calculate", "Calcular"},
  {"Aller a", "Go to", "Ir a"},
  {"Zeros", "Zeros", "Raices"},
  {"Tangente", "Tangent", "Tangente"},
  {"Intersection", "Intersection", "Interseccion"},
  {"Selectionner la borne inferieure", "Select lower bound", "Seleccionar el limite inferior"},
  {"Selectionner la borne superieure", "Select upper bound", "Seleccionar el limite superior"},
  {"Aucun zero trouve", "No zeros found", "Ninguna raiz encontrada"},
    /* Function: third tab*/
  {"Regler l'intervalle", "Set the interval", "Ajustar el intervalo"},
  {"X debut", "X start", "X inicio"},
  {"X fin", "X end", "X fin"},
  {"Pas", "Step", "Incremento"},
  {"Colonne x", "x column", "Columna x"},
  {"Colonne 0(0)", "Function 0(0)", "Columna 0(0)"},
  {"Colonne 0'(x)", "Column 0'(x)", "Columna 0'(x)"},
  {"Colonne de la fonction derivee", "Derivative function column", "Columna de la derivada"},
  {"Effacer la colonne", "Clear column", "Borrar la columna"},
  {"Copier la colonne dans une liste", "Export the column to a list", "Copiar la columna en una lista"},
  {"Masquer la fonction derivee", "Hide the derivative function", "Ocultar la derivada"},

  /* Sequence */
  {"Suites", "Sequences", "Sucesion"},
  {"SUITES", "SEQUENCES", "SUCESION"},
  {"Suites", "Sequences", "Sucesiones"},
  /* Sequence: first tab */
  {"Ajouter une suite", "Add sequence", "Agregar una sucesion"},
  {"Choisir le type de suite", "Choose sequence type", "Seleccionar el tipo de sucesion"},
  {"Type de suite", "Sequence type", "Tipo de sucesion"},
  {"Explicite", "Explicit expression", "Formula explicita"},
  {"Recurrente d'ordre 1", "Recursive first order", "Recurrencia de orden uno"},
  {"Recurrente d'ordre 2", "Recursive second order", "Recurrencia de orden dos"},
  {"Options de la suite", "Sequence options", "Opciones de sucesion"},
  {"Couleur de la suite", "Sequence color", "Color de la sucesion"},
  {"Supprimer la suite", "Delete sequence", "Eliminar la sucesion"},
  /* Sequence: second tab */
  {"Aucune suite", "No sequence", "Ninguna sucesion"},
  {"Aucune suite activee", "No sequence is turned on", "Ninguna sucesion activada"},
  {"Somme des termes", "Sum of terms", "Suma de terminos"},
  {"SELECTIONNER LE PREMIER TERME", "SELECT FIRST TERM", "SELECCIONAR EL PRIMER TERMINO"},
  {"SELECTIONNER LE DERNIER TERME", "SELECT LAST TERM", "SELECCIONAR ULTIMO TERMINO"},
  /* Sequence: third tab */
  {"Colonne n", "n column", "Columna n"},

  /* Statistics */
  {"Statistiques", "Statistics", "Estadistica"},
  {"STATISTIQUES", "STATISTICS", "ESTADISTICA"},

  {"Donnees", "Data", "Datos"},
  {"Histogramme", "Histogram", "Histograma"},
  {"Boite", "Box", "Caja"},
  {"Stats", "Statistics", "Medidas"},

  /* Statistics: first tab */
  {"Valeurs", "Values", "Valores"},
  {"Effectifs", "Sizes", "Frecuencias"},
  {"Options de la colonne", "Column options", "Opciones de columna"},
  {"Importer une liste", "Import from a list", "Importar una lista"},
  /* Statistics: second tab */
  {"Aucune donnee a tracer", "No data to draw", "Ningunos datos que dibujar"},
  {"Intervalle ", "Interval ", "Intervalo"},
  {"Effectif", "Size", "Frecuencia"},
  {"       Frequence", "       Frequency", "       Frequencia (relativa)"},
  {"Reglage de l'histogramme", "Histogram settings", "Parametros del histograma"},
  {"Largeur des rectangles", "Bin width", "Ancho del rectangulo"},
  {"Debut de la serie", "X start", "Principio de la serie"},
  /* Statistics: third tab */
  {"Premier quartile", "First quartile", "Primer cuartil"},
  {"Mediane", "Median", "Mediana"},
  {"Troisieme quartile", "Third quartile", "Tercer cuartil"},
  /* Statistics: fourth tab */
  {"Aucune grandeur a calculer", "No values to calculate", "Ninguna medida que calcular"},
  {"Effectif total", "Total size", "Poblacion"},
  {"Etendue", "Dispersion", "Rango"},
  {"Moyenne", "Mean", "Media"},
  {"Ecart type", "Standard deviation", "Desviacion tipica"},
  {"Variance", "Variance", "Varianza"},
  {"Ecart interquartile", "Interquartile range", "Rango intercuartilo"},
  {"Somme des carres", "Sum of squares", "Suma de los cuadrados"},

  /* Probability */
  {"Probabilites", "Probability", "Probabilidad"},
  {"PROBABILITES", "PROBABILITY", "PROBABILIDAD"},

  {"Choisir le type de loi", "Choose the distribution", "Seleccionar la distribucion"},
  {"Binomiale", "Binomial", "Binomial"},
  {"Uniforme", "Uniform", "Uniforme"},
  {"Exponentielle", "Exponential", "Exponencial"},
  {"Normale", "Normal", "Normal"},
  {"Poisson", "Poisson", "Poisson"},

  {"Loi binomiale", "Binomial distribution", "Distribucion binomial"},
  {"Loi uniforme", "Uniform distribution", "Distribucion uniforme"},
  {"Loi exponentielle", "Exponential distribution", "Distribucion exponencial"},
  {"Loi normale", "Normal distribution", "Distribucion normal"},
  {"Loi Poisson", "Poisson distribution", "Distribucion Poisson"},
  {"Choisir les parametres", "Choose parameters", "Seleccionar parametros"},
  {"n : Nombre de repetitions", "n: Number of trials", "n : Numero de ensayos "},
  {"p : Probabilite de succes", "p: Success probability", "p : Probabilidad de exito "},
  {"[a,b] : Intervalle", "[a,b]: Interval", "[a,b] : Intervalo"},
  {lambdaExponentialFrenchDefinition, lambdaExponentialEnglishDefinition, lambdaExponentialSpanishDefinition},
  {meanFrenchDefinition, meanEnglishDefinition, meanSpanishDefinition},
  {deviationFrenchDefinition, deviationEnglishDefinition, deviationSpanishDefinition},  {lambdaPoissonFrenchDefinition, lambdaPoissonEnglishDefinition, lambdaPoissonSpanishDefinition},

  {"Calculer les probabilites", "Calculate probabilities", "Calcular las probabilidades"},
  {"Valeur interdite", "Forbidden value", "Valor prohibido"},
  {"Valeur non definie", "Undefined value", "Valor indefinido"},

  /* Regression */
  {"Regressions", "Regression", "Regresion"},
  {"REGRESSIONS", "REGRESSION", "REGRESION"},

  {"Pas assez de donnees pour une regression", "Not enough data for regerssion", "Escasez de datos para la regresion"},
  {"Droite de regression", "Regression line", "Recta de regresion"},
  {"Prediction sachant X", "Prediction given X", "Prediccion dado X"},
  {"Prediction sachant Y", "Prediction given Y", "Prediccion dado Y"},
  {"Valeur non atteinte par la regression", "Value not reached by regression", "No se alcanza este valor"},
  {"Nombre de points", "Number of points", "Numero de puntos"},
  {"Covariance", "Covariance", "Covarianza"},

  /* Settings */
  {"Parametres", "Settings", "Configuracion"},
  {"PARAMETRES", "SETTINGS", "CONFIGURATION"},
  {"Unite d'angle", "Angle measure", "Medida del angulo"},
  {"Format resultat", "Result format", "Formato resultado"},
  {"Forme complexe", "Complex format", "Formato complejo"},
  {"Langue", "Language", "Idioma"},
  {"Degres ", "Degrees ", "Grados "},
  {"Radians ", "Radians ", "Radianes "},
  {"Auto ", "Auto ", "Auto "},
  {"Scientifique ", "Scientific ", "Cientifico "},
};

const char Sxy[4] = {Ion::Charset::CapitalSigma, 'x', 'y', 0};
constexpr static char Mu[] = {Ion::Charset::SmallMu, 0};
constexpr static char Sigma[] = {Ion::Charset::SmallSigma, 0};
constexpr static char Lambda[] = {Ion::Charset::SmallLambda, 0};
constexpr static char rightIntegralSecondLegend[] = {Ion::Charset::LessEqual, 'X', ')', '=', 0};
constexpr static char leftIntegralFirstLegend[] = {'P', '(', 'X', Ion::Charset::LessEqual, 0};
constexpr static char finiteIntegralLegend[] = {Ion::Charset::LessEqual, 'X', Ion::Charset::LessEqual, 0};


const char * universalMessages[200] {
  "",
  "x",
  "y",
  "n",
  "p",
  Mu,
  Sigma,
  Lambda,
  "a",
  "b",
  "r",
  Sxy,

  "cosh",
  "sinh",
  "tanh",
  "acosh",
  "asinh",
  "atanh",

  "Xmin",
  "Xmax",
  "Ymin",
  "Ymax",
  "Y auto",

  "P(",
  rightIntegralSecondLegend,
  leftIntegralFirstLegend,
  ")=",
  finiteIntegralLegend,

  "   y=ax+b   ",

  "Francais ",
  "English ",
  "Espanol ",

   /* Toolbox: commands */
  "abs()",
  "root(,)",
  "log(,)",
  "diff(,)",
  "int(,,)",
  "sum(,,)",
  "product(,,)",
  "arg()",
  "re()",
  "im()",
  "conj()",
  "binomial(,)",
  "permute(,)",
  "gcd(,)",
  "lcm(,)",
  "rem(,)",
  "quo(,)",
  "inverse()",
  "det()",
  "transpose()",
  "trace()",
  "dim()",
  "sort<()",
  "sort>()",
  "max()",
  "min()",
  "floor()",
  "frac()",
  "ceil()",
  "round(,)",
  "cosh()",
  "sinh()",
  "tanh()",
  "acosh()",
  "asinh()",
  "atanh()",
  "prediction95(,)",
  "prediction(,)",
  "confidence(,)",
};

const char * translate(Message m, Language l) {
  if ((int)m >= 0x8000) {
    assert(universalMessages[(int)m - 0X8000] != nullptr);
    return universalMessages[(int)m - 0x8000];
  }
  int languageIndex = (int)l;
  if (l == Language::Default) {
    languageIndex = (int) GlobalPreferences::sharedGlobalPreferences()->language();
  }
  assert(languageIndex > 0);
  assert(messages[(int)m][languageIndex-1] != nullptr);
  return messages[(int)m][languageIndex-1];
}

}
