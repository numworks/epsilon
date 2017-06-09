#include "i18n.h"
#include "global_preferences.h"
#include <assert.h>

namespace I18n {

constexpr static char lambdaExponentialFrenchDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'e', 0};
constexpr static char lambdaExponentialEnglishDefinition[] = {Ion::Charset::SmallLambda, ':', ' ', 'R', 'a', 't','e',' ', 'p', 'a', 'r', 'a', 'm', 'e', 't', 'e', 'r', 0};
constexpr static char lambdaExponentialSpanishDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'o', 0};
constexpr static char lambdaExponentialGermanDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'e', 'r', 0};
constexpr static char lambdaExponentialPortugueseDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'o', 0};
constexpr static char lambdaPoissonFrenchDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'e', 0};
constexpr static char lambdaPoissonEnglishDefinition[] = {Ion::Charset::SmallLambda, ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'e', 'r', 0};
constexpr static char lambdaPoissonSpanishDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'o', 0};
constexpr static char lambdaPoissonGermanDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'e', 'r', 0};
constexpr static char lambdaPoissonPortugueseDefinition[] = {Ion::Charset::SmallLambda, ' ', ':', ' ', 'P', 'a', 'r', 'a', 'm', 'e', 't', 'r', 'o', 0};
constexpr static char meanFrenchDefinition[] = {Ion::Charset::SmallMu, ' ', ':', ' ', 'M', 'o', 'y', 'e', 'n', 'n', 'e', 0};
constexpr static char meanEnglishDefinition[] = {Ion::Charset::SmallMu, ':', ' ', 'M', 'e', 'a', 'n', 0};
constexpr static char meanSpanishDefinition[] = {Ion::Charset::SmallMu, ' ', ':', ' ', 'M', 'e', 'd', 'i', 'a', 0};
constexpr static char meanGermanDefinition[] = {Ion::Charset::SmallMu, ' ', ':', ' ', 'E', 'r', 'w', 'a', 'r', 't', 'u', 'n', 'g', 's', 'w', 'e', 'r', 't', 0};
constexpr static char meanPortugueseDefinition[] = {Ion::Charset::SmallMu, ' ', ':', ' ', 'M', 'e', 'd', 'i', 'a', 0};
constexpr static char deviationFrenchDefinition[] = {Ion::Charset::SmallSigma, ' ', ':', ' ', 'E', 'c', 'a', 'r', 't', '-', 't', 'y', 'p', 'e', 0};
constexpr static char deviationEnglishDefinition[] = {Ion::Charset::SmallSigma, ':', ' ', 'S', 't', 'a', 'n', 'd', 'a', 'r', 'd', ' ','d','e', 'v', 'i', 'a', 't','i','o','n', 0};
constexpr static char deviationSpanishDefinition[] = {Ion::Charset::SmallSigma, ' ', ':', ' ', 'D', 'e','s','v','i','a','c','i','o','n',' ','t','i','p','i','c','a',0};
constexpr static char deviationGermanDefinition[] = {Ion::Charset::SmallSigma, ' ', ':', ' ', 'S', 't', 'a', 'n', 'd', 'a', 'r', 'd', 'a', 'b', 'w', 'e', 'i', 'c', 'h', 'u', 'n', 'g', 0};
constexpr static char deviationPortugueseDefinition[] = {Ion::Charset::SmallSigma, ' ', ':', ' ', 'D', 'e','s','v','i','o',' ','p','a','d','r','a','o', 0};

const char * messages[232][5] {
  {"Attention", "Warning", "Cuidado", "Achtung", "Atencao"},
  {"Valider", "Confirm", "Confirmar", "Bestatigen", "Confirmar"},
  {"Annuler", "Cancel", "Cancelar", "Abbrechen", "Cancelar"},
  {"Suivant", "Next", "Siguiente", "Nachste", "Seguinte"},
  {"Attention a la syntaxe", "Syntax error", "Error sintactico", "Syntaxfehler", "Erro de sintaxe"},
  {"Erreur mathematique", "Math error", "Error matematico", "Mathematischen Fehler", "Erro matematico"},
  {"Batterie faible", "Low battery", "Bateria baja", "Batterie leer", "Bateria fraca"},

  /* Variables */
  {"Variables", "Variables", "Variables", "Variablen", "Variaveis"},
  {"Nombres", "Numbers", "Numeros", "Zahlen", "Numeros"},
  {"Matrices", "Matrices", "Matrices", "Matrizen", "Matrizes"},
  {"Listes", "Lists", "Listas", "Listen", "Listas"},
  {"Vide", "Empty", "Vacio", "Leer", "Vacuo"},

  /* Toolbox */
  {"Toolbox", "Toolbox", "Toolbox", "Toolbox", "Toolbox"},
  {"Valeur absolue", "Absolute value", "Valor absoluto", "Betragsfunktion", "Valor absoluto"},
  {"Racine n-ieme", "nth-root", "Raiz enesima", "n-te Wurzel", "Radiciacao"},
  {"Logarithme base a", "Logarithm to base a", "Logaritmo en base a", "Logarithmus zur Basis a", "Logaritmo na base a"},
  {"Calculs", "Calculation", "Calculos", "Berechnung", "Calculo"},
  {"Nombres complexes", "Complex numbers", "Numeros complejos", "Komplexen Zahlen", "Numeros complexos"},
  {"Denombrement", "Combinatorics", "Combinatoria", "Kombinatorik", "Combinatoria"},
  {"Arithmetique", "Arithmetic", "Aritmetica", "Arithmetisch", "Aritmetica"},
  {"Matrices", "Matrix", "Matriz", "Matrizen", "Matrizes"},
  {"Listes", "List", "Listas", "Listen", "Listas"},
  {"Approximation", "Approximation", "Aproximacion", "Approximation", "Aproximacao"},
  {"Trigonometrie hyperbolique", "Hyperbolic trigonometry", "Trigonometria hiperbolica", "Hyperbelfunktionen", "Funcoes hiperbolicas"},
  {"Intervalle fluctuation", "Prediction Interval", "Interval de prediccion", "Konfidenzintervall", "Intervalo de confianca"},
  {"Nombre derive", "Derivative", "Derivada", "Ableitung", "Derivada"},
  {"Integrale", "Integral", "Integral", "Integral", "Integral"},
  {"Somme", "Sum", "Suma", "Summe", "Somatorio"},
  {"Produit", "Product", "Productorio", "Produkt", "Produto"},
  {"Module", "Absolute value", "Modulo", "Betrag", "Modulo"},
  {"Argument", "Argument", "Argumento", "Argument", "Argumento"},
  {"Partie reelle", "Real part", "Parte real", "Realer Teil", "Parte real"},
  {"Partie imaginaire", "Imaginary part", "Parte imaginaria", "Imaginarer Teil", "Parte imaginaria"},
  {"Conjugue", "Conjugate", "Conjugado", "Konjugiert", "Conjugado"},
  {"k parmi n", "Combination", "Combinacion", "Kombination", "Combinacao"},
  {"Arrangement", "Permutation", "Variacion", "Permutation", "Permutacao"},
  {"PGCD", "GCD", "MCD", "ggT", "MDC"},
  {"PPCM", "LCM", "MCM", "kgV", "MMC"},
  {"Reste division p par q", "Remainder division p by q", "Resto division p por q", "Rest", "Resto da divisao inteira"},
  {"Quotient division p par q", "Quotient division p by q", "Cociente division p por q", "Ganzzahlquotient", "Quociente"},
  {"Inverse", "Inverse", "Inversa", "Inverse", "Matriz inversa"},
  {"Determinant", "Determinant", "Determinante", "Determinante", "Determinante"},
  {"Transposee", "Transpose", "Transpuesta", "Transponierte", "Matriz transposta"},
  {"Trace ", "Trace", "Traza", "Spur", "Traco"},
  {"Taille", "Size", "Tamano", "Grosse", "Tamanho"},
  {"Tri croissant", "Sort ascending ", "Clasificacion ascendente", "Sortieren aufsteigend", "Ordem crescente"},
  {"Tri decroissant", "Sort descending", "Clasificacion descendente", "Sortieren absteigend", "Ordem decrescente"},
  {"Maximum", "Maximum", "Maximo", "Maximalwert", "Maximo"},
  {"Minimum", "Minimum", "Minimo", "Mindestwert", "Minimo"},
  {"Partie entiere", "Floor", "Parte entera", "Untergrenze", "Parte inteira"},
  {"Partie fractionnaire", "Fractional part", "Parte fraccionaria", "Bruchanteil" , "Parte fraccionaria"},
  {"Plafond", "Ceiling", "Techo", "Obergrenze", "Teto"},
  {"Arrondi n chiffres", "Rounding to n digits", "Redondeo n digitos", "Runden", "Arredondar"},
  {"Cosinus hyperbolique", "Hyperbolic cosine", "Coseno hiperbolico", "Kosinus hyperbolicus", "Cosseno hiperbolico"},
  {"Sinus hyperbolique", "Hyperbolic sine", "Seno hiperbolico", "Sinus hyperbolicus", "Seno hiperbolico"},
  {"Tangente hyperbolique", "Hyperbolic tangent", "Tangente hiperbolica", "Tangens hyperbolicus", "Tangente hiperbolica"},
  {"Argument cosinus hyperbolique", "Inverse hyperbolic cosine", "Argumento coseno hiperbolico", "Areakosinus hyperbolicus", "Arco cosseno hiperbolico"},
  {"Argument sinus hyperbolique", "Inverse hyperbolic sine", "Argumento seno hiperbolico", "Areasinus hyperbolicus", "Arco seno hiperbolico"},
  {"Argument tangente hyperbolique", "Inverse hyperbolic tangent", "Argumento tangente hiperbolica", "Areatangens hyperbolicus", "Arco tangente hiperbolica"},
  {"Intervalle fluctuation 95% (Term)", "Prediction interval 95%", "Intervalo de prediccion 95%", "Schwankungsbereich 95%", "Intervalo de previsao 95%"},
  {"Intervalle fluctuation simple (2de)", "Simple prediction interval", "Intervalo de prediccion simple", "Einfaches Schwankungsbereich", "Intervalo de previsao simples"},
  {"Intervalle confiance", "Confidence interval", "Intervalo de confianza", "Konfidenzintervall", "Intervalo de confianca"},

  /* Applications */
  {"Applications", "Applications", "Aplicaciones", "Anwendungen", "Aplicações"},
  {"APPLICATIONS", "APPLICATIONS", "APLICACIONES", "ANWENDUNGEN", "APLICACOES"},

  /* 1.Calculation */
  {"Calculs", "Calculation", "Calculo", "Berechnung", "Calculo"},
  {"CALCULS", "CALCULATION", "CALCULO", "BERECHNUNG", "CALCULO"},

  /* 2.Function */
  {"Fonctions", "Functions", "Funcion", "Funktionen", "Funcao"},
  {"FONCTIONS", "FUNCTIONS", "FUNCION", "FUNKTIONEN", "FUNCAO"},
  {"Fonctions", "Functions", "Funciones", "Funktionen", "Funcoes"},
  {"Graphique", "Graph", "Grafico", "Graph", "Grafico"},
  {"Tableau", "Table", "Tabla", "Tabelle", "Tabela"},
    /* Function: first tab */
  {"Tracer le graphique", "Plot graph", "Dibujar el grafico", "Graphen zeichnen", "Tracar o grafico"},
  {"Afficher les valeurs", "Display values", "Visualizar los valores", "Werte anzeigen", "Exibir os valores"},
  {"Options de la fonction", "Function options", "Opciones de la funcion", "Optionen Funktion", "Opcoes de funcao"},
  {"Ajouter une fonction", "Add function", "Agregar una funcion", "Funktion hinzuzufugen", "Adicionar uma funcao"},
  {"Supprimer la fonction", "Delete function", "Eliminar la funcion", "Funktion loschen", "Eliminar a funcao"},
  {"Pas de fonction a supprimer", "No function to delete", "Ninguna funcion que eliminar", "Keine Funktion zu loschen", "Sem funcao para eliminar"},
  {"Activer/Desactiver", "Turn on/off", "Activar/Desactivar", "Aktivieren/Deaktivieren", "Activar/Desactivar"},
  {"Couleur de la fonction", "Function color", "Color de la funcion", "Farbe der Funktion", "Cor da funcao"},
    /* Function: second tab */
  {"Aucune fonction", "No function", "Ninguna funcion", "Keine funktion", "Nenhuma funcao"},
  {"Aucune fonction activee", "No function is turned on", "Ninguna funcion activada", "Keine aktive Funktion", "Sem funcao ativada"},
  {"Axes", "Axes", "Ejes", "Achsen", "Eixos"},
  {"Zoom", "Zoom", "Zoom", "Zoom", "Zoom"},
  {"Initialisation", "Preadjustment", "Inicializacion", "Initialisierung", "Inicializacao"},
  {" Deplacer : ", " Move: ", " Mover : ", " Verschieben: ", " Mover : "},
  {"Zoomer : ", "Zoom: ", "Zoom : ", "Zoom: ", "Zoom : "},
  {" ou ", " or ", " o ", " oder ", " ou "},
  {"Trigonometrique", "Trigonometrical", "Trigonometrico", "Trigonometrisch", "Trigonometrico"},
  {"Abscisses entieres", "Integer", "Abscisas enteras", "Ganzzahl", "Inteiro"},
  {"Orthonorme", "Orthonormal", "Ortonormal", "Orthonormal", "Ortonormado"},
  {"Reglages de base", "Basic settings", "Ajustes basicos", "Grundeinstellungen", "Configuracoes basicas"},
  {"Options de la courbe", "Plot options", "Opciones de la curva", "Plotoptionen", "Opcoes da curva"},
  {"Calculer", "Calculate", "Calcular", "Berechnen", "Calcular"},
  {"Aller a", "Go to", "Ir a", "Gehe zu", "Ir a"},
  {"Valeur non atteinte par la fonction", "Value not reached by function", "No se alcanza este valor", "Der Wert wird nicht von der Funktion erreicht", "O valor nao e alcancado pela funcao"},
  {"Zeros", "Zeros", "Raices", "Nullstellen", "Raizes"},
  {"Tangente", "Tangent", "Tangente", "Tangente", "Tangente"},
  {"Intersection", "Intersection", "Interseccion", "Schnittmenge", "Interseccao"},
  {"Selectionner la borne inferieure", "Select lower bound", "Seleccionar el limite inferior", "Untere Integrationsgrenze", "Selecionar limite superior"},
  {"Selectionner la borne superieure", "Select upper bound", "Seleccionar el limite superior", "Obere Integrationsgrenze", "Selecionar limite inferior"},
  {"Aucun zero trouve", "No zeros found", "Ninguna raiz encontrada", "Keine Nullstelle gefunden", "Nenhuma encontrado de raiz"},
    /* Function: third tab*/
  {"Regler l'intervalle", "Set the interval", "Ajustar el intervalo", "Tabelleneinstell", "Ajustar o intervalo"},
  {"X debut", "X start", "X inicio", "Startwert", "X inicio"},
  {"X fin", "X end", "X fin", "Endwert", "X fim"},
  {"Pas", "Step", "Incremento", "Schrittwert", "Passo"},
  {"Colonne x", "x column", "Columna x", "X Spalte", "Coluna X"},
  {"Colonne 0(0)", "0(0) column", "Columna 0(0)", "0(0) Spalte", "Coluna 0(0)"},
  {"Colonne 0'(x)", "0'(x) column", "Columna 0'(x)", "0'(x) Spalte", "Coluna 0'(x)"},
  {"Colonne de la fonction derivee", "Derivative function column", "Columna de la derivada", "Spalte der Ableitungsfunktion", "Coluna da funcao derivada"},
  {"Effacer la colonne", "Clear column", "Borrar la columna", "Spalte loschen", "Excluir coluna"},
  {"Copier la colonne dans une liste", "Export the column to a list", "Copiar la columna en una lista", "Die Spalte in einer Liste kopieren", "Copie a coluna em uma lista"},
  {"Masquer la fonction derivee", "Hide the derivative function", "Ocultar la derivada", "Die Ableitungsfunktion ausblenden", "Esconder funcao derivada"},
  /* Sequence */
  {"Suites", "Sequences", "Sucesion", "Folge", "Sequencia"},
  {"SUITES", "SEQUENCES", "SUCESION", "FOLGE", "SEQUENCIA"},
  {"Suites", "Sequences", "Sucesiones", "Folgen", "Sequencias"},
  /* Sequence: first tab */
  {"Ajouter une suite", "Add sequence", "Agregar una sucesion", "Folge hinzuzufugen", "Adicionar uma sequencia"},
  {"Choisir le type de suite", "Choose sequence type", "Seleccionar el tipo de sucesion", "Das Bildungsgesetz der Folge auswahlen", "Escolha o tipo de sequencia"},
  {"Type de suite", "Sequence type", "Tipo de sucesion", "Bildungsgesetz der Folge", "Tipo de sequencia"},
  {"Explicite", "Explicit expression", "Formula explicita", "Explizit", "Explicita"},
  {"Recurrente d'ordre 1", "Recursive first order", "Recurrencia de orden uno", "Rekursiv, Ordnung 1", "Recorrencia de Primeira Ordem"},
  {"Recurrente d'ordre 2", "Recursive second order", "Recurrencia de orden dos", "Rekursiv, Ordnung 2", "Recorrencia de Segunda Ordem"},
  {"Options de la suite", "Sequence options", "Opciones de la sucesion", "Optionen Folge", "Opcoes de sequencia"},
  {"Couleur de la suite", "Sequence color", "Color de la sucesion", "Farbe der Folge", "Cor da sequencia"},
  {"Supprimer la suite", "Delete sequence", "Eliminar la sucesion", "Folge loschen", "Eliminar a sequencia"},
  /* Sequence: second tab */
  {"Aucune suite", "No sequence", "Ninguna sucesion", "Keine Folge", "Sem sequencia"},
  {"Aucune suite activee", "No sequence is turned on", "Ninguna sucesion activada", "Keine aktive Folge", "Sem sequencia ativada"},
  {"N debut", "N start", "N inicio", "Startwert", "N inicio"},
  {"N fin", "N end", "N fin", "Endwert", "N fim"},
  {"Somme des termes", "Sum of terms", "Suma de terminos", "Summe der Glieder", "Soma dos termos"},
  {"Selectionner le premier terme ", "Select First Term ", "Seleccionar el premer termino ", "Erster Glied ", "Selecionar primeiro termo " },
  {"Selectionner le dernier terme ", "Select last term ", "Seleccionar el ultimo termino ", "Letztes Glied ", "Selecionar ultimo termo "},
  {"Valeur non atteinte par la suite", "Value not reached by sequence", "No se alcanza este valor", "Der Wert wird nicht von der Folge erreicht", "O valor nao e alcancado pela sequencia"},
  /* Sequence: third tab */
  {"Colonne n", "n column", "Columna n", "n Spalte", "Coluna n"},

  /* Statistics */
  {"Statistiques", "Statistics", "Estadistica", "Statistiken", "Estatistica"},
  {"STATISTIQUES", "STATISTICS", "ESTADISTICA", "STATISTIKEN", "ESTATISTICA"},

  {"Donnees", "Data", "Datos", "Daten", "Dados"},
  {"Histogramme", "Histogram", "Histograma", "Histogramm", "Histograma"},
  {"Boite", "Box", "Caja", "Boxplot", "Caixa"},
  {"Stats", "Stats", "Medidas", "Stats", "Estat"},

  /* Statistics: first tab */
  {"Valeurs", "Values", "Valores", "Werte", "Valores"},
  {"Effectifs", "Sizes", "Frecuencias", "Haufigkeiten", "Frequencias"},
  {"Options de la colonne", "Column options", "Opciones de la columna", "Optionen Spalte", "Opcoes de coluna"},
  {"Importer une liste", "Import from a list", "Importar una lista", "Laden eine Liste", "Importar de uma lista"},
  /* Statistics: second tab */
  {"Aucune donnee a tracer", "No data to draw", "Ningunos datos que dibujar", "Keine Daten zu zeichnen", "Nao ha dados para desenhar"},
  {" Intervalle ", " Interval ", " Intervalo", " Intervall", " Intervalo"},
  {" Effectif", " Size", " Frecuencia", " Haufigkeit", " Frequencia"},
  {"Frequence", "Frequency", "Relativa", "Relative", "Relativa"},
  {"Reglage de l'histogramme", "Histogram settings", "Parametros del histograma", "Einstellung des Histogramms", "Configurando o histograma"},
  {"Largeur des rectangles", "Bin width", "Ancho del rectangulo", "Breite der Rechtecken", "Largura dos rectangulos"},
  {"Debut de la serie", "X start", "Principio de la serie", "Beginn der Serie", "Inicio da serie"},
  /* Statistics: third tab */
  {"Premier quartile", "First quartile", "Primer cuartil", "Unteres Quartil", "Quartil inferior"},
  {"Mediane", "Median", "Mediana", "Median", "Mediana"},
  {"Troisieme quartile", "Third quartile", "Tercer cuartil", "Oberes Quartil", "Quartil superior"},
  /* Statistics: fourth tab */
  {"Aucune grandeur a calculer", "No values to calculate", "Ninguna medida que calcular", "Keine Grosse zu berechnen", "Nenhuma quantidade para calcular"},
  {"Effectif total", "Total size", "Poblacion", "Anzahl der Elemente", "Numero de itens"},
  {"Etendue", "Range", "Rango", "Spannweite", "Amplitude"},
  {"Moyenne", "Mean", "Media", "Mittelwert", "Media"},
  {"Ecart type", "Standard deviation", "Desviacion tipica", "Standardabweichung", "Desvio padrao"},
  {"Variance", "Variance", "Varianza", "Varianz", "Variancia"},
  {"Ecart interquartile", "Interquartile range", "Rango intercuartilo", "Interquartilsabstand", "Interquartil"},
  {"Somme des carres", "Sum of squares", "Suma cuadrados", "Quadratsumme", "Soma dos quadrados"},

  /* Probability */
  {"Probabilites", "Probability", "Probabilidad", "Wahrsch.", "Probabilidade"},
  {"PROBABILITES", "PROBABILITY", "PROBABILIDAD", "WAHRSCHEINLICHKEIT", "PROBABILIDADE"},

  {"Choisir le type de loi", "Choose the distribution", "Seleccionar la distribucion", "Wahlen Sie Verteilung", "Selecionar a distribuicao"},
  {"Binomiale", "Binomial", "Binomial", "Binomial", "Binomial"},
  {"Uniforme", "Uniform", "Uniforme", "Uniform", "Uniforme"},
  {"Exponentielle", "Exponential", "Exponencial", "Exponential", "Exponencial"},
  {"Normale", "Normal", "Normal", "Normal", "Normal"},
  {"Poisson", "Poisson", "Poisson", "Poisson", "Poisson"},

  {"Loi binomiale", "Binomial distribution", "Distribucion binomial", "Binomialverteilung", "Distribuicao binomial"},
  {"Loi uniforme", "Uniform distribution", "Distribucion uniforme", "Uniformverteilung", "Distribuicao uniforme"},
  {"Loi exponentielle", "Exponential distribution", "Distribucion exponencial", "Exponentialverteilung", "Distribuicao exponencial"},
  {"Loi normale", "Normal distribution", "Distribucion normal", "Normalverteilung", "Distribuicao normal"},
  {"Loi de Poisson", "Poisson distribution", "Distribucion de Poisson", "Poisson-Verteilung", "Distribuicao de Poisson"},
  {"Choisir les parametres", "Choose parameters", "Seleccionar parametros", "Parameter auswahlen", "Selecionar os parametros"},
  {"n : Nombre de repetitions", "n: Number of trials", "n : Numero de ensayos ", "n : Anzahl der Versuche", "n : Numero de ensaios"},
  {"p : Probabilite de succes", "p: Success probability", "p : Probabilidad de exito ", "p : Erfolgswahrscheinlichkeit", "p : Probabilidade de sucesso"},
  {"[a,b] : Intervalle", "[a,b]: Interval", "[a,b] : Intervalo", "[a,b] : Intervall", "[a,b] : Intervalo"},
  {lambdaExponentialFrenchDefinition, lambdaExponentialEnglishDefinition, lambdaExponentialSpanishDefinition, lambdaExponentialGermanDefinition, lambdaExponentialPortugueseDefinition},
  {meanFrenchDefinition, meanEnglishDefinition, meanSpanishDefinition, meanGermanDefinition, meanPortugueseDefinition},
  {deviationFrenchDefinition, deviationEnglishDefinition, deviationSpanishDefinition, deviationGermanDefinition, deviationPortugueseDefinition},  {lambdaPoissonFrenchDefinition, lambdaPoissonEnglishDefinition, lambdaPoissonSpanishDefinition, lambdaPoissonGermanDefinition, lambdaPoissonPortugueseDefinition},

  {"Calculer les probabilites", "Calculate probabilities", "Calcular las probabilidades", "Die Wahrscheinlichkeiten berechnen", "Calcular probabilidades"},
  {"Valeur interdite", "Forbidden value", "Valor prohibido","Verbotener Wert", "Valor proibida"},
  {"Valeur non definie", "Undefined value", "Valor indefinido", "Undefinierter Wert", "Valor indefinido"},

  /* Regression */
  {"Regressions", "Regression", "Regresion", "Regression", "Regressao"},
  {"REGRESSIONS", "REGRESSION", "REGRESION", "REGRESSION", "REGRESSAO"},

  {"Regression", "Regression", "Regresion", "Regression", "Regressao"},
  {"Pas assez de donnees pour une regression", "Not enough data for regerssion", "Escasez de datos para la regresion", "Nicht genug Daten fur eine Regression", "Nao ha dados suficientes para uma regressao"},
  {"reg", "reg", "reg", "reg", "reg"},
  {"moyen", "mean", "media", "mittel", "media"},
  {"Droite de regression", "Regression line", "Recta de regresion", "Regressionsgerade", "Linha de regressao"},
  {"Prediction sachant X", "Prediction given X", "Prediccion dado X", "Berechne Y", "Predicao dado X"},
  {"Prediction sachant Y", "Prediction given Y", "Prediccion dado Y", "Berechne X", "Predicao dado Y"},
  {"Valeur non atteinte par la regression", "Value not reached by regression", "No se alcanza este valor", "Wert wird nicht von der Regression erreicht", "Valor nao alcancado pela regressao"},
  {"Nombre de points", "Number of points", "Numero de puntos", "Punktanzahl", "Numero de pontos"},
  {"Covariance", "Covariance", "Covarianza", "Kovarianz", "Covariancia"},

  /* Settings */
  {"Parametres", "Settings", "Configuracion", "Einstellungen", "Configuracao"},
  {"PARAMETRES", "SETTINGS", "CONFIGURACION", "EINSTELLUNGEN", "CONFIGURACAO"},
  {"Unite d'angle", "Angle measure", "Medida del angulo", "Winkeleinheit", "Valor do angulo"},
  {"Format resultat", "Result format", "Formato resultado", "Zahlenformat", "Formato numerico"},
  {"Forme complexe", "Complex format", "Formato complejo", "Komplex", "Complexos"},
  {"Langue", "Language", "Idioma", "Sprache", "Idioma"},
  {"Mode examen", "Exam mode", "Modo examen", "Testmodus", "Modo de Exame"},
  {"Activer le mode examen", "Activate exam mode", "Activar el modo examen", "Start Testmodus", "Inicio modo de exame"},
  {"Mode examen: actif", "Exam mode: active", "Modo examen: activo", "Testmodus : aktiv", "Modo de exame : ativo"},
  {"Toutes vos donnees seront ", "All your data will be ", "Todos sus datos se ", "Alle Ihre Daten werden ", "Todos os seus dados serao "},
  {"supprimees si vous activez ", "deleted when you activate ", "eliminaran al activar ", "geloscht, wenn Sie den ", "apagados se voce ligar "},
  {"le mode examen.", "the exam mode.", "el modo examen.", "Testmodus einschalten.", "o modo de exame."},
  {"Voulez-vous sortir ", "Exit the exam ", "Salir del modo ", "Wollen Sie den Testmodus ", "Voce quer sair do modo de "},
  {"du mode examen ?", "mode?", "examen ?", "verlassen?", "exame ?"},
  {"A propos", "About", "Acerca", "Uber", "Acerca"},
  {"Degres ", "Degrees ", "Grados ", "Grad ", "Graus "},
  {"Radians ", "Radians ", "Radianes ", "Bogenmass ", "Radianos "},
  {"Auto ", "Auto ", "Auto ", "Auto ", "Auto "},
  {"Scientifique ", "Scientific ", "Cientifico ", "Wissenschaftlich ", "Cientifico "},
  {"deg", "deg", "gra", "gra", "gra"},
  {"rad", "rad", "rad", "rad", "rad"},
  {"sci/", "sci/", "sci/", "sci/", "sci/"},
  {"Version du logiciel", "Software version", "Version de software", "Softwareversion", "Versao do software"},
  {"Numero serie", "Serial number", "Numero serie", "Seriennummer", "Numero serie"},
  {"Rappel mise a jour", "Update pop-up", "Pop-up de actualizacion", "Erinnerung: Update", "Alertas de atualizacao"},

  {"Vous allez lancer le test usine.", "You are starting the hardware", "Esta iniciando la prueba de", "Sie werden den Hardwaretest", "Voce vai executar o teste da planta."},
  {"Pour en sortir vous devrez", " test. At the end of the test, you", "fabrica. Para quitar la prueba,", "starten. Um den zu verlassen, mussen", "Para sair voce tem que executar"},
  {"effectuer un reset qui supprimera", "will have to reset the device and", "debera resetear su equipo.", "Sie einen Reset durchfuhren, der", "uma redefinicao, que ira apagar"},
  {"vos donnees.", "all your data will be deleted.", "", "Ihre Daten loschen werden.", "seus dados."},

  /* On boarding */
  {"MISE A JOUR DISPONIBLE", "UPDATE AVAILABLE", "ACTUALIZACION DISPONIBLE", "UPDATE VERFUGBAR", "ATUALIZACAO DISPONIVEL"},
  {"Des ameliorations importantes existent", "There are important upgrades", "Hay mejoras importantes", "Wichtige Verbesserungen fur Ihren", "Existem melhorias significativas"},
  {"pour votre calculatrice.", "for your calculator.", "para su calculadora.", "Rechner stehen zur Verfugung.", "para a sua calculadora."},
  {"Connectez-vous depuis votre ordinateur", "Browse our page from your computer", "Visita nuestra pagina desde su ordenador", "Melden Sie sich von Ihrem Computer an", "Navegue na nossa pagina do seu computador"},
  {"www.numworks.com/update", "www.numworks.com/update", "www.numworks.com/update", "www.numworks.com/update", "www.numworks.com/update"},
  {"Passer", "Skip", "Saltar", "Uberspringen", "Pular"},
};

const char sxy[4] = {Ion::Charset::CapitalSigma, 'x', 'y', 0};
constexpr static char mu[] = {Ion::Charset::SmallMu, 0};
constexpr static char sigma[] = {Ion::Charset::SmallSigma, 0};
constexpr static char lambda[] = {Ion::Charset::SmallLambda, 0};
constexpr static char rightIntegralSecondLegend[] = {Ion::Charset::LessEqual, 'X', ')', '=', 0};
constexpr static char leftIntegralFirstLegend[] = {'P', '(', 'X', Ion::Charset::LessEqual, 0};
constexpr static char finiteIntegralLegend[] = {Ion::Charset::LessEqual, 'X', Ion::Charset::LessEqual, 0};


const char * universalMessages[238] {
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

  " D: y=ax+b ",

  "Francais ",
  "English ",
  "Espanol ",
  "Deutsch ",
  "Portugues ",

  "FCC ID",

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
