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
constexpr static char deviationFrenchDefinition[] = {Ion::Charset::SmallSigma, ' ', ':', ' ', 'E', 'c', 'a', 'r', 't', '-', 't', 'y', 'p', 'e', 0};
constexpr static char deviationEnglishDefinition[] = {Ion::Charset::SmallSigma, ':', ' ', 'S', 't', 'a', 'n', 'd', 'a', 'r', 'd', ' ','d','e', 'v', 'i', 'a', 't','i','o','n', 0};
constexpr static char deviationSpanishDefinition[] = {Ion::Charset::SmallSigma, ' ', ':', ' ', 'D', 'e','s','v','i','a','c','i','o','n',' ','t','i','p','i','c','a',0};

const char * messages[218][3] {
  {"Attention", "Warning", "Cuidado"},
  {"Valider", "Confirm", "Confirmar"},
  {"Annuler", "Cancel", "Cancelar"},
  {"Suivant", "Next", "Siguiente"},
  {"Attention a la syntaxe", "Syntax error", "Error sintactico"},
  {"Erreur mathematique", "Math error", "Error matematico"},
  {"Batterie faible", "Low battery", "Bateria baja"},

  /* Variables */
  {"Variables", "Variables", "Variables"},
  {"Nombres", "Numbers", "Numeros"},
  {"Matrices", "Matrices", "Matrices"},
  {"Listes", "Lists", "Listas"},

  /* Toolbox */
  {"Toolbox", "Toolbox", "Toolbox"},
  {"Valeur absolue", "Absolute value", "Valor absoluto"},
  {"Racine n-ieme", "nth-root", "Raiz enesima"},
  {"Logarithme base a", "Logarithm to base a", "Logaritmo en base a"},
  {"Calculs", "Calculation", "Calculos"},
  {"Nombres complexes", "Complex numbers", "Numeros complejos"},
  {"Denombrement", "Combinatorics", "Combinatoria"},
  {"Arithmetique", "Arithmetic", "Aritmetica"},
  {"Matrices", "Matrix", "Matriz"},
  {"Listes", "List", "Listas"},
  {"Approximation", "Approximation", "Aproximacion"},
  {"Trigonometrie hyperbolique", "Hyperbolic trigonometry", "Trigonometria hiperbolica"},
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
  {"k parmi n", "Combination", "Combinacion"},
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
  {"Arrondi n chiffres", "Rounding to n digits", "Redondeo n digitos"},
  {"Intervalle fluctuation 95% (Term)", "Prediction interval 95%", "Intervalo de prediccion 95%"},
  {"Intervalle fluctuation simple (2de)", "Simple prediction interval", "Intervalo de prediccion simple"},
  {"Intervalle confiance", "Confidence interval", "Intervalo de confianza"},

  /* Applications */
  {"Applications", "Applications", "Aplicaciones"},
  {"APPLICATIONS", "APPLICATIONS", "APLICACIONES"},

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
  {"Tracer le graphique", "Plot graph", "Dibujar el grafico"},
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
  {"Initialisation", "Preadjustment", "Inicializacion"},
  {" Deplacer : ", "Move: ", "Mover : "},
  {"Zoomer : ", "Zoom: ", "Zoom : "},
  {" ou ", " or ", " o "},
  {"Trigonometrique", "Trigonometrical", "Trigonometrico"},
  {"Abscisses entieres", "Integer", "Abscisas enteras"},
  {"Orthonorme", "Orthonormal", "Ortonormal"},
  {"Reglages de base", "Basic settings", "Ajustes basicos"},
  {"Options de la courbe", "Plot options", "Opciones de la curva"},
  {"Calculer", "Calculate", "Calcular"},
  {"Aller a", "Go to", "Ir a"},
  {"Valeur non atteinte par la fonction", "Value not reached by function", "No se alcanza este valor"},
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
  {"Colonne 0(0)", "0(0) column", "Columna 0(0)"},
  {"Colonne 0'(x)", "0'(x) column", "Columna 0'(x)"},
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
  {"Options de la suite", "Sequence options", "Opciones de la sucesion"},
  {"Couleur de la suite", "Sequence color", "Color de la sucesion"},
  {"Supprimer la suite", "Delete sequence", "Eliminar la sucesion"},
  /* Sequence: second tab */
  {"Aucune suite", "No sequence", "Ninguna sucesion"},
  {"Aucune suite activee", "No sequence is turned on", "Ninguna sucesion activada"},
  {"N debut", "N start", "N inicio"},
  {"N fin", "N end", "N fin"},
  {"Somme des termes", "Sum of terms", "Suma de terminos"},
  {"SELECTIONNER LE PREMIER TERME", "SELECT FIRST TERM", "SELECCIONAR EL PRIMER TERMINO"},
  {"SELECTIONNER LE DERNIER TERME", "SELECT LAST TERM", "SELECCIONAR EL ULTIMO TERMINO"},
  {"Valeur non atteinte par la suite", "Value not reached by sequence", "No se alcanza este valor"},
  /* Sequence: third tab */
  {"Colonne n", "n column", "Columna n"},

  /* Statistics */
  {"Statistiques", "Statistics", "Estadistica"},
  {"STATISTIQUES", "STATISTICS", "ESTADISTICA"},

  {"Donnees", "Data", "Datos"},
  {"Histogramme", "Histogram", "Histograma"},
  {"Boite", "Box", "Caja"},
  {"Stats", "Stats", "Medidas"},

  /* Statistics: first tab */
  {"Valeurs", "Values", "Valores"},
  {"Effectifs", "Sizes", "Frecuencias"},
  {"Options de la colonne", "Column options", "Opciones de la columna"},
  {"Importer une liste", "Import from a list", "Importar una lista"},
  /* Statistics: second tab */
  {"Aucune donnee a tracer", "No data to draw", "Ningunos datos que dibujar"},
  {"Intervalle ", "Interval ", "Intervalo"},
  {"Effectif", "Size", "Frecuencia"},
  {"Frequence", "Frequency", "Relativa"},
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
  {"Somme des carres", "Sum of squares", "Suma cuadrados"},

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
  {"Loi de Poisson", "Poisson distribution", "Distribucion de Poisson"},
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
  {"PARAMETRES", "SETTINGS", "CONFIGURACION"},
  {"Unite d'angle", "Angle measure", "Medida del angulo"},
  {"Format resultat", "Result format", "Formato resultado"},
  {"Forme complexe", "Complex format", "Formato complejo"},
  {"Langue", "Language", "Idioma"},
  {"Mode examen", "Exam mode", "Modo examen"},
  {"Activer le mode examen", "Activate exam mode", "Activar el modo examen"},
  {"Mode examen: actif", "Exam mode: active", "Modo examen: activo"},
  {"Toutes vos donnees seront ", "All your data will be ", "Todos sus datos se "},
  {"supprimees si vous activez ", "deleted when you activate ", "eliminarán al activar "},
  {"le mode examen.", "the exam mode.", "el modo examen."},
  {"Voulez-vous sortir ", "Exit the exam ", "Salir del modo "},
  {"du mode examen ?", "mode?", "examen ?"},
  {"A propos", "About", "Acerca"},
  {"Degres ", "Degrees ", "Grados "},
  {"Radians ", "Radians ", "Radianes "},
  {"Auto ", "Auto ", "Auto "},
  {"Scientifique ", "Scientific ", "Cientifico "},
  {"deg", "deg", "gra"},
  {"rad", "rad", "rad"},
  {"sci/", "sci/", "sci/"},
  {"Version du logiciel", "Software version", "Version de software"},
  {"Numero serie", "Serial number", "Numero serie"},
  {"Rappel mise a jour", "Update pop-up", "Pop-up de actualizacion"},

  /* On boarding */
  {"MISE A JOUR DISPONIBLE", "UPDATE AVAILABLE", "ACTUALIZACION DISPONIBLE"},
  {"Des ameliorations importantes existent", "There are important upgrades", "Hay mejoras importantes"},
  {"pour votre calculatrice.", "for your calculator.", "para su calculadora."},
  {"Connectez-vous depuis votre ordinateur", "Browse our page from your computer", "Visita nuestra pagina desde su ordenador"},
  {"www.numworks.com/update", "www.numworks.com/update", "www.numworks.com/update"},
  {"Passer", "Skip", "Saltar"},
};

const char sxy[4] = {Ion::Charset::CapitalSigma, 'x', 'y', 0};
constexpr static char mu[] = {Ion::Charset::SmallMu, 0};
constexpr static char sigma[] = {Ion::Charset::SmallSigma, 0};
constexpr static char lambda[] = {Ion::Charset::SmallLambda, 0};
constexpr static char rightIntegralSecondLegend[] = {Ion::Charset::LessEqual, 'X', ')', '=', 0};
constexpr static char leftIntegralFirstLegend[] = {'P', '(', 'X', Ion::Charset::LessEqual, 0};
constexpr static char finiteIntegralLegend[] = {Ion::Charset::LessEqual, 'X', Ion::Charset::LessEqual, 0};


const char * universalMessages[241] {
  "",
  "alpha",
  "ALPHA",
  "x",
  "y",
  "n",
  "p",
  mu,
  sigma,
  lambda,
  "a",
  "b",
  "r",
  sxy,

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

  "abs(x)",
  "root(x,n)",
  "log(a,x)",
  "diff(f(x),a)",
  "int(f(x),a,b)",
  "sum(f(n),nmin,nmax)",
  "product(f(n),nmin,nmax)",
  "arg(z)",
  "re(z)",
  "im(z)",
  "conj(z)",
  "binomial(n,k)",
  "permute(n,r)",
  "gcd(p,q)",
  "lcm(p,q)",
  "rem(p,q)",
  "quo(p,q)",
  "inverse(M)",
  "det(M)",
  "transpose(M)",
  "trace(M)",
  "dim(M)",
  "sort<(L)",
  "sort>(L)",
  "max(L)",
  "min(L)",
  "floor(x)",
  "frac(x)",
  "ceil(x)",
  "round(x,n)",
  "cosh(x)",
  "sinh(x)",
  "tanh(x)",
  "acosh(x)",
  "asinh(x)",
  "atanh(x)",
  "prediction95(p,n)",
  "prediction(p,n)",
  "confidence(f,n)",
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
  assert(((int)m*numberOfLanguages()+languageIndex-1)*sizeof(char *) < sizeof(messages));
  return messages[(int)m][languageIndex-1];
}

int numberOfLanguages() {
  return NumberOfLanguages;
}

}
