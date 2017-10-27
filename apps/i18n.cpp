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
constexpr static char approximateEqual[] = {Ion::Charset::ApproximateEqual, 0};


const char * messages[240][5] {
  {"Warning", "Attention", "Cuidado", "Achtung", "Atencao"},
  {"Confirm", "Valider", "Confirmar", "Bestatigen", "Confirmar"},
  {"Cancel", "Annuler", "Cancelar", "Abbrechen", "Cancelar"},
  {"Next", "Suivant", "Siguiente", "Nachste", "Seguinte"},
  {"Syntax error", "Attention a la syntaxe", "Error sintactico", "Syntaxfehler", "Erro de sintaxe"},
  {"Math error", "Erreur mathematique", "Error matematico", "Mathematischen Fehler", "Erro matematico"},
  {"Low battery", "Batterie faible", "Bateria baja", "Batterie leer", "Bateria fraca"},

  /* Variables */
  {"Variables", "Variables", "Variables", "Variablen", "Variaveis"},
  {"Numbers", "Nombres", "Numeros", "Zahlen", "Numeros"},
  {"Matrices", "Matrices", "Matrices", "Matrizen", "Matrizes"},
  {"Lists", "Listes", "Listas", "Listen", "Listas"},
  {"Empty", "Vide", "Vacio", "Leer", "Vacuo"},

  /* Toolbox */
  {"Toolbox", "Toolbox", "Toolbox", "Toolbox", "Toolbox"},
  {"Absolute value", "Valeur absolue", "Valor absoluto", "Betragsfunktion", "Valor absoluto"},
  {"nth-root", "Racine n-ieme", "Raiz enesima", "n-te Wurzel", "Radiciacao"},
  {"Logarithm to base a", "Logarithme base a", "Logaritmo en base a", "Logarithmus zur Basis a", "Logaritmo na base a"},
  {"Calculation", "Calculs", "Calculos", "Berechnung", "Calculo"},
  {"Complex numbers", "Nombres complexes", "Numeros complejos", "Komplexen Zahlen", "Numeros complexos"},
  {"Combinatorics", "Denombrement", "Combinatoria", "Kombinatorik", "Combinatoria"},
  {"Arithmetic", "Arithmetique", "Aritmetica", "Arithmetisch", "Aritmetica"},
  {"Matrix", "Matrices", "Matriz", "Matrizen", "Matrizes"},
  {"List", "Listes", "Listas", "Listen", "Listas"},
  {"Approximation", "Approximation", "Aproximacion", "Approximation", "Aproximacao"},
  {"Hyperbolic trigonometry", "Trigonometrie hyperbolique", "Trigonometria hiperbolica", "Hyperbelfunktionen", "Funcoes hiperbolicas"},
  {"Prediction Interval", "Intervalle fluctuation", "Interval de prediccion", "Konfidenzintervall", "Intervalo de confianca"},
  {"Derivative", "Nombre derive", "Derivada", "Ableitung", "Derivada"},
  {"Integral", "Integrale", "Integral", "Integral", "Integral"},
  {"Sum", "Somme", "Suma", "Summe", "Somatorio"},
  {"Product", "Produit", "Productorio", "Produkt", "Produto"},
  {"Absolute value", "Module", "Modulo", "Betrag", "Modulo"},
  {"Argument", "Argument", "Argumento", "Argument", "Argumento"},
  {"Real part", "Partie reelle", "Parte real", "Realer Teil", "Parte real"},
  {"Imaginary part", "Partie imaginaire", "Parte imaginaria", "Imaginarer Teil", "Parte imaginaria"},
  {"Conjugate", "Conjugue", "Conjugado", "Konjugiert", "Conjugado"},
  {"Combination", "k parmi n", "Combinacion", "Kombination", "Combinacao"},
  {"Permutation", "Arrangement", "Variacion", "Permutation", "Permutacao"},
  {"GCD", "PGCD", "MCD", "ggT", "MDC"},
  {"LCM", "PPCM", "MCM", "kgV", "MMC"},
  {"Remainder division p by q", "Reste division p par q", "Resto division p por q", "Rest", "Resto da divisao inteira"},
  {"Quotient division p by q", "Quotient division p par q", "Cociente division p por q", "Ganzzahlquotient", "Quociente"},
  {"Inverse", "Inverse", "Inversa", "Inverse", "Matriz inversa"},
  {"Determinant", "Determinant", "Determinante", "Determinante", "Determinante"},
  {"Transpose", "Transposee", "Transpuesta", "Transponierte", "Matriz transposta"},
  {"Trace", "Trace ", "Traza", "Spur", "Traco"},
  {"Size", "Taille", "Tamano", "Grosse", "Tamanho"},
  {"Sort ascending ", "Tri croissant", "Clasificacion ascendente", "Sortieren aufsteigend", "Ordem crescente"},
  {"Sort descending", "Tri decroissant", "Clasificacion descendente", "Sortieren absteigend", "Ordem decrescente"},
  {"Maximum", "Maximum", "Maximo", "Maximalwert", "Maximo"},
  {"Minimum", "Minimum", "Minimo", "Mindestwert", "Minimo"},
  {"Floor", "Partie entiere", "Parte entera", "Untergrenze", "Parte inteira"},
  {"Fractional part", "Partie fractionnaire", "Parte fraccionaria", "Bruchanteil" , "Parte fraccionaria"},
  {"Ceiling", "Plafond", "Techo", "Obergrenze", "Teto"},
  {"Rounding to n digits", "Arrondi n chiffres", "Redondeo n digitos", "Runden", "Arredondar"},
  {"Hyperbolic cosine", "Cosinus hyperbolique", "Coseno hiperbolico", "Kosinus hyperbolicus", "Cosseno hiperbolico"},
  {"Hyperbolic sine", "Sinus hyperbolique", "Seno hiperbolico", "Sinus hyperbolicus", "Seno hiperbolico"},
  {"Hyperbolic tangent", "Tangente hyperbolique", "Tangente hiperbolica", "Tangens hyperbolicus", "Tangente hiperbolica"},
  {"Inverse hyperbolic cosine", "Argument cosinus hyperbolique", "Argumento coseno hiperbolico", "Areakosinus hyperbolicus", "Arco cosseno hiperbolico"},
  {"Inverse hyperbolic sine", "Argument sinus hyperbolique", "Argumento seno hiperbolico", "Areasinus hyperbolicus", "Arco seno hiperbolico"},
  {"Inverse hyperbolic tangent", "Argument tangente hyperbolique", "Argumento tangente hiperbolica", "Areatangens hyperbolicus", "Arco tangente hiperbolica"},
  {"Prediction interval 95%", "Intervalle fluctuation 95% (Term)", "Intervalo de prediccion 95%", "Schwankungsbereich 95%", "Intervalo de previsao 95%"},
  {"Simple prediction interval", "Intervalle fluctuation simple (2de)", "Intervalo de prediccion simple", "Einfaches Schwankungsbereich", "Intervalo de previsao simples"},
  {"Confidence interval", "Intervalle confiance", "Intervalo de confianza", "Konfidenzintervall", "Intervalo de confianca"},

  /* Applications */
  {"Applications", "Applications", "Aplicaciones", "Anwendungen", "Aplicações"},
  {"APPLICATIONS", "APPLICATIONS", "APLICACIONES", "ANWENDUNGEN", "APLICACOES"},

  /* 1.Calculation */
  {"Calculation", "Calculs", "Calculo", "Berechnung", "Calculo"},
  {"CALCULATION", "CALCULS", "CALCULO", "BERECHNUNG", "CALCULO"},

  /* 2.Function */
  {"Functions", "Fonctions", "Funcion", "Funktionen", "Funcao"},
  {"FUNCTIONS", "FONCTIONS", "FUNCION", "FUNKTIONEN", "FUNCAO"},
  {"Functions", "Fonctions", "Funciones", "Funktionen", "Funcoes"},
  {"Graph", "Graphique", "Grafico", "Graph", "Grafico"},
  {"Table", "Tableau", "Tabla", "Tabelle", "Tabela"},
    /* Function: first tab */
  {"Plot graph", "Tracer le graphique", "Dibujar el grafico", "Graphen zeichnen", "Tracar o grafico"},
  {"Display values", "Afficher les valeurs", "Visualizar los valores", "Werte anzeigen", "Exibir os valores"},
  {"Function options", "Options de la fonction", "Opciones de la funcion", "Optionen Funktion", "Opcoes de funcao"},
  {"Add function", "Ajouter une fonction", "Agregar una funcion", "Funktion hinzuzufugen", "Adicionar uma funcao"},
  {"Delete function", "Supprimer la fonction", "Eliminar la funcion", "Funktion loschen", "Eliminar a funcao"},
  {"No function to delete", "Pas de fonction a supprimer", "Ninguna funcion que eliminar", "Keine Funktion zu loschen", "Sem funcao para eliminar"},
  {"Turn on/off", "Activer/Desactiver", "Activar/Desactivar", "Aktivieren/Deaktivieren", "Activar/Desactivar"},
  {"Function color", "Couleur de la fonction", "Color de la funcion", "Farbe der Funktion", "Cor da funcao"},
    /* Function: second tab */
  {"No function", "Aucune fonction", "Ninguna funcion", "Keine funktion", "Nenhuma funcao"},
  {"No function is turned on", "Aucune fonction activee", "Ninguna funcion activada", "Keine aktive Funktion", "Sem funcao ativada"},
  {"Axes", "Axes", "Ejes", "Achsen", "Eixos"},
  {"Zoom", "Zoom", "Zoom", "Zoom", "Zoom"},
  {"Preadjustment", "Initialisation", "Inicializacion", "Initialisierung", "Inicializacao"},
  {" Move: ", " Deplacer : ", " Mover : ", " Verschieben: ", " Mover : "},
  {"Zoom: ", "Zoomer : ", "Zoom : ", "Zoom: ", "Zoom : "},
  {" or ", " ou ", " o ", " oder ", " ou "},
  {"Trigonometrical", "Trigonometrique", "Trigonometrico", "Trigonometrisch", "Trigonometrico"},
  {"Integer", "Abscisses entieres", "Abscisas enteras", "Ganzzahl", "Inteiro"},
  {"Orthonormal", "Orthonorme", "Ortonormal", "Orthonormal", "Ortonormado"},
  {"Basic settings", "Reglages de base", "Ajustes basicos", "Grundeinstellungen", "Configuracoes basicas"},
  {"Plot options", "Options de la courbe", "Opciones de la curva", "Plotoptionen", "Opcoes da curva"},
  {"Calculate", "Calculer", "Calcular", "Berechnen", "Calcular"},
  {"Go to", "Aller a", "Ir a", "Gehe zu", "Ir a"},
  {"Value not reached by function", "Valeur non atteinte par la fonction", "No se alcanza este valor", "Der Wert wird nicht von der Funktion erreicht", "O valor nao e alcancado pela funcao"},
  {"Zeros", "Zeros", "Raices", "Nullstellen", "Raizes"},
  {"Tangent", "Tangente", "Tangente", "Tangente", "Tangente"},
  {"Intersection", "Intersection", "Interseccion", "Schnittmenge", "Interseccao"},
  {"Select lower bound", "Selectionner la borne inferieure", "Seleccionar el limite inferior", "Untere Integrationsgrenze", "Selecionar limite superior"},
  {"Select upper bound", "Selectionner la borne superieure", "Seleccionar el limite superior", "Obere Integrationsgrenze", "Selecionar limite inferior"},
  {"No zeros found", "Aucun zero trouve", "Ninguna raiz encontrada", "Keine Nullstelle gefunden", "Nenhuma encontrado de raiz"},
    /* Function: third tab*/
  {"Set the interval", "Regler l'intervalle", "Ajustar el intervalo", "Tabelleneinstell", "Ajustar o intervalo"},
  {"X start", "X debut", "X inicio", "Startwert", "X inicio"},
  {"X end", "X fin", "X fin", "Endwert", "X fim"},
  {"Step", "Pas", "Incremento", "Schrittwert", "Passo"},
  {"x column", "Colonne x", "Columna x", "X Spalte", "Coluna X"},
  {"0(0) column", "Colonne 0(0)", "Columna 0(0)", "0(0) Spalte", "Coluna 0(0)"},
  {"0'(x) column", "Colonne 0'(x)", "Columna 0'(x)", "0'(x) Spalte", "Coluna 0'(x)"},
  {"Derivative function column", "Colonne de la fonction derivee", "Columna de la derivada", "Spalte der Ableitungsfunktion", "Coluna da funcao derivada"},
  {"Clear column", "Effacer la colonne", "Borrar la columna", "Spalte loschen", "Excluir coluna"},
  {"Export the column to a list", "Copier la colonne dans une liste", "Copiar la columna en una lista", "Die Spalte in einer Liste kopieren", "Copie a coluna em uma lista"},
  {"Hide the derivative function", "Masquer la fonction derivee", "Ocultar la derivada", "Die Ableitungsfunktion ausblenden", "Esconder funcao derivada"},
  /* Sequence */
  {"Sequences", "Suites", "Sucesion", "Folge", "Sequencia"},
  {"SEQUENCES", "SUITES", "SUCESION", "FOLGE", "SEQUENCIA"},
  {"Sequences", "Suites", "Sucesiones", "Folgen", "Sequencias"},
  /* Sequence: first tab */
  {"Add sequence", "Ajouter une suite", "Agregar una sucesion", "Folge hinzuzufugen", "Adicionar uma sequencia"},
  {"Choose sequence type", "Choisir le type de suite", "Seleccionar el tipo de sucesion", "Das Bildungsgesetz der Folge auswahlen", "Escolha o tipo de sequencia"},
  {"Sequence type", "Type de suite", "Tipo de sucesion", "Bildungsgesetz der Folge", "Tipo de sequencia"},
  {"Explicit expression", "Explicite", "Formula explicita", "Explizit", "Explicita"},
  {"Recursive first order", "Recurrente d'ordre 1", "Recurrencia de orden uno", "Rekursiv, Ordnung 1", "Recorrencia de Primeira Ordem"},
  {"Recursive second order", "Recurrente d'ordre 2", "Recurrencia de orden dos", "Rekursiv, Ordnung 2", "Recorrencia de Segunda Ordem"},
  {"Sequence options", "Options de la suite", "Opciones de la sucesion", "Optionen Folge", "Opcoes de sequencia"},
  {"Sequence color", "Couleur de la suite", "Color de la sucesion", "Farbe der Folge", "Cor da sequencia"},
  {"Delete sequence", "Supprimer la suite", "Eliminar la sucesion", "Folge loschen", "Eliminar a sequencia"},
  /* Sequence: second tab */
  {"No sequence", "Aucune suite", "Ninguna sucesion", "Keine Folge", "Sem sequencia"},
  {"No sequence is turned on", "Aucune suite activee", "Ninguna sucesion activada", "Keine aktive Folge", "Sem sequencia ativada"},
  {"N start", "N debut", "N inicio", "Startwert", "N inicio"},
  {"N end", "N fin", "N fin", "Endwert", "N fim"},
  {"Sum of terms", "Somme des termes", "Suma de terminos", "Summe der Glieder", "Soma dos termos"},
  {"Select First Term ", "Selectionner le premier terme ", "Seleccionar el premer termino ", "Erster Glied ", "Selecionar primeiro termo " },
  {"Select last term ", "Selectionner le dernier terme ", "Seleccionar el ultimo termino ", "Letztes Glied ", "Selecionar ultimo termo "},
  {"Value not reached by sequence", "Valeur non atteinte par la suite", "No se alcanza este valor", "Der Wert wird nicht von der Folge erreicht", "O valor nao e alcancado pela sequencia"},
  /* Sequence: third tab */
  {"n column", "Colonne n", "Columna n", "n Spalte", "Coluna n"},

  /* Statistics */
  {"Statistics", "Statistiques", "Estadistica", "Statistiken", "Estatistica"},
  {"STATISTICS", "STATISTIQUES", "ESTADISTICA", "STATISTIKEN", "ESTATISTICA"},

  {"Data", "Donnees", "Datos", "Daten", "Dados"},
  {"Histogram", "Histogramme", "Histograma", "Histogramm", "Histograma"},
  {"Box", "Boite", "Caja", "Boxplot", "Caixa"},
  {"Stats", "Stats", "Medidas", "Stats", "Estat"},

  /* Statistics: first tab */
  {"Values", "Valeurs", "Valores", "Werte", "Valores"},
  {"Sizes", "Effectifs", "Frecuencias", "Haufigkeiten", "Frequencias"},
  {"Column options", "Options de la colonne", "Opciones de la columna", "Optionen Spalte", "Opcoes de coluna"},
  {"Import from a list", "Importer une liste", "Importar una lista", "Laden eine Liste", "Importar de uma lista"},
  /* Statistics: second tab */
  {"No data to draw", "Aucune donnee a tracer", "Ningunos datos que dibujar", "Keine Daten zu zeichnen", "Nao ha dados para desenhar"},
  {" Interval ", " Intervalle ", " Intervalo", " Intervall", " Intervalo"},
  {" Size", " Effectif", " Frecuencia", " Haufigkeit", " Frequencia"},
  {"Frequency", "Frequence", "Relativa", "Relative", "Relativa"},
  {"Histogram settings", "Reglage de l'histogramme", "Parametros del histograma", "Einstellung des Histogramms", "Configurando o histograma"},
  {"Bin width", "Largeur des rectangles", "Ancho del rectangulo", "Breite der Rechtecken", "Largura dos rectangulos"},
  {"X start", "Debut de la serie", "Principio de la serie", "Beginn der Serie", "Inicio da serie"},
  /* Statistics: third tab */
  {"First quartile", "Premier quartile", "Primer cuartil", "Unteres Quartil", "Quartil inferior"},
  {"Median", "Mediane", "Mediana", "Median", "Mediana"},
  {"Third quartile", "Troisieme quartile", "Tercer cuartil", "Oberes Quartil", "Quartil superior"},
  /* Statistics: fourth tab */
  {"No values to calculate", "Aucune grandeur a calculer", "Ninguna medida que calcular", "Keine Grosse zu berechnen", "Nenhuma quantidade para calcular"},
  {"Total size", "Effectif total", "Poblacion", "Anzahl der Elemente", "Numero de itens"},
  {"Range", "Etendue", "Rango", "Spannweite", "Amplitude"},
  {"Mean", "Moyenne", "Media", "Mittelwert", "Media"},
  {"Standard deviation", "Ecart type", "Desviacion tipica", "Standardabweichung", "Desvio padrao"},
  {"Variance", "Variance", "Varianza", "Varianz", "Variancia"},
  {"Interquartile range", "Ecart interquartile", "Rango intercuartilo", "Interquartilsabstand", "Interquartil"},
  {"Sum of squares", "Somme des carres", "Suma cuadrados", "Quadratsumme", "Soma dos quadrados"},

  /* Probability */
  {"Probability", "Probabilites", "Probabilidad", "Wahrsch.", "Probabilidade"},
  {"PROBABILITY", "PROBABILITES", "PROBABILIDAD", "WAHRSCHEINLICHKEIT", "PROBABILIDADE"},

  {"Choose the distribution", "Choisir le type de loi", "Seleccionar la distribucion", "Wahlen Sie Verteilung", "Selecionar a distribuicao"},
  {"Binomial", "Binomiale", "Binomial", "Binomial", "Binomial"},
  {"Uniform", "Uniforme", "Uniforme", "Uniform", "Uniforme"},
  {"Exponential", "Exponentielle", "Exponencial", "Exponential", "Exponencial"},
  {"Normal", "Normale", "Normal", "Normal", "Normal"},
  {"Poisson", "Poisson", "Poisson", "Poisson", "Poisson"},

  {"Binomial distribution", "Loi binomiale", "Distribucion binomial", "Binomialverteilung", "Distribuicao binomial"},
  {"Uniform distribution", "Loi uniforme", "Distribucion uniforme", "Uniformverteilung", "Distribuicao uniforme"},
  {"Exponential distribution", "Loi exponentielle", "Distribucion exponencial", "Exponentialverteilung", "Distribuicao exponencial"},
  {"Normal distribution", "Loi normale", "Distribucion normal", "Normalverteilung", "Distribuicao normal"},
  {"Poisson distribution", "Loi de Poisson", "Distribucion de Poisson", "Poisson-Verteilung", "Distribuicao de Poisson"},
  {"Choose parameters", "Choisir les parametres", "Seleccionar parametros", "Parameter auswahlen", "Selecionar os parametros"},
  {"n: Number of trials", "n : Nombre de repetitions", "n : Numero de ensayos ", "n : Anzahl der Versuche", "n : Numero de ensaios"},
  {"p: Success probability", "p : Probabilite de succes", "p : Probabilidad de exito ", "p : Erfolgswahrscheinlichkeit", "p : Probabilidade de sucesso"},
  {"[a,b]: Interval", "[a,b] : Intervalle", "[a,b] : Intervalo", "[a,b] : Intervall", "[a,b] : Intervalo"},
  {lambdaExponentialEnglishDefinition, lambdaExponentialFrenchDefinition, lambdaExponentialSpanishDefinition, lambdaExponentialGermanDefinition, lambdaExponentialPortugueseDefinition},
  {meanEnglishDefinition, meanFrenchDefinition, meanSpanishDefinition, meanGermanDefinition, meanPortugueseDefinition},
  {deviationEnglishDefinition, deviationFrenchDefinition, deviationSpanishDefinition, deviationGermanDefinition, deviationPortugueseDefinition},
  {lambdaPoissonEnglishDefinition, lambdaPoissonFrenchDefinition, lambdaPoissonSpanishDefinition, lambdaPoissonGermanDefinition, lambdaPoissonPortugueseDefinition},

  {"Calculate probabilities", "Calculer les probabilites", "Calcular las probabilidades", "Die Wahrscheinlichkeiten berechnen", "Calcular probabilidades"},
  {"Forbidden value", "Valeur interdite", "Valor prohibido","Verbotener Wert", "Valor proibida"},
  {"Undefined value", "Valeur non definie", "Valor indefinido", "Undefinierter Wert", "Valor indefinido"},

  /* Regression */
  {"Regression", "Regressions", "Regresion", "Regression", "Regressao"},
  {"REGRESSION", "REGRESSIONS", "REGRESION", "REGRESSION", "REGRESSAO"},

  {"Regression", "Regression", "Regresion", "Regression", "Regressao"},
  {"Not enough data for regerssion", "Pas assez de donnees pour une regression", "Escasez de datos para la regresion", "Nicht genug Daten fur eine Regression", "Nao ha dados suficientes para uma regressao"},
  {"reg", "reg", "reg", "reg", "reg"},
  {"mean", "moyen", "media", "mittel", "media"},
  {"Regression line", "Droite de regression", "Recta de regresion", "Regressionsgerade", "Linha de regressao"},
  {"Prediction given X", "Prediction sachant X", "Prediccion dado X", "Berechne Y", "Predicao dado X"},
  {"Prediction given Y", "Prediction sachant Y", "Prediccion dado Y", "Berechne X", "Predicao dado Y"},
  {"Value not reached by regression", "Valeur non atteinte par la regression", "No se alcanza este valor", "Wert wird nicht von der Regression erreicht", "Valor nao alcancado pela regressao"},
  {"Number of points", "Nombre de points", "Numero de puntos", "Punktanzahl", "Numero de pontos"},
  {"Covariance", "Covariance", "Covarianza", "Kovarianz", "Covariancia"},

  /* Settings */
  {"Settings", "Parametres", "Configuracion", "Einstellungen", "Configuracao"},
  {"SETTINGS", "PARAMETRES", "CONFIGURACION", "EINSTELLUNGEN", "CONFIGURACAO"},
  {"Angle measure", "Unite d'angle", "Medida del angulo", "Winkeleinheit", "Valor do angulo"},
  {"Result format", "Format resultat", "Formato resultado", "Zahlenformat", "Formato numerico"},
  {"Complex format", "Forme complexe", "Formato complejo", "Komplex", "Complexos"},
  {"Language", "Langue", "Idioma", "Sprache", "Idioma"},
  {"Exam mode", "Mode examen", "Modo examen", "Testmodus", "Modo de Exame"},
  {"Activate exam mode", "Activer le mode examen", "Activar el modo examen", "Start Testmodus", "Inicio modo de exame"},
  {"Exam mode: active", "Mode examen: actif", "Modo examen: activo", "Testmodus : aktiv", "Modo de exame : ativo"},
  {"All your data will be ", "Toutes vos donnees seront ", "Todos sus datos se ", "Alle Ihre Daten werden ", "Todos os seus dados serao "},
  {"deleted when you activate ", "supprimees si vous activez ", "eliminaran al activar ", "geloscht, wenn Sie den ", "apagados se voce ligar "},
  {"the exam mode.", "le mode examen.", "el modo examen.", "Testmodus einschalten.", "o modo de exame."},
  {"Exit the exam ", "Voulez-vous sortir ", "Salir del modo ", "Wollen Sie den Testmodus ", "Voce quer sair do modo de "},
  {"mode?", "du mode examen ?", "examen ?", "verlassen?", "exame ?"},
  {"About", "A propos", "Acerca", "Uber", "Acerca"},
  {"Degrees ", "Degres ", "Grados ", "Grad ", "Graus "},
  {"Radians ", "Radians ", "Radianes ", "Bogenmass ", "Radianos "},
  {"Auto ", "Auto ", "Auto ", "Auto ", "Auto "},
  {"Scientific ", "Scientifique ", "Cientifico ", "Wissenschaftlich ", "Cientifico "},
  {"deg", "deg", "gra", "gra", "gra"},
  {"rad", "rad", "rad", "rad", "rad"},
  {"sci/", "sci/", "sci/", "sci/", "sci/"},
  {"Brightness", "Luminosite", "Brillo", "Helligkeit", "Brilho"},
  {"Software version", "Version du logiciel", "Version de software", "Softwareversion", "Versao do software"},
  {"Serial number", "Numero serie", "Numero serie", "Seriennummer", "Numero serie"},
  {"Update pop-up", "Rappel mise a jour", "Pop-up de actualizacion", "Erinnerung: Update", "Alertas de atualizacao"},

  {"You are starting the hardware", "Vous allez lancer le test usine.", "Esta iniciando la prueba de", "Sie werden den Hardwaretest", "Voce vai executar o teste da planta."},
  {" test. At the end of the test, you", "Pour en sortir vous devrez", "fabrica. Para quitar la prueba,", "starten. Um den zu verlassen, mussen", "Para sair voce tem que executar"},
  {"will have to reset the device and", "effectuer un reset qui supprimera", "debera resetear su equipo.", "Sie einen Reset durchfuhren, der", "uma redefinicao, que ira apagar"},
  {"all your data will be deleted.", "vos donnees.", "", "Ihre Daten loschen werden.", "seus dados."},

  /* Code */
  {"BETA VERSION", "VERSION BETA", "BETA VERSION", "BETA VERSION", "BETA VERSION"},
  {"You are using a pre-release and", "La version de Python que vous utilisez", "No esta utilizando una version", "You are using a pre-release and", "You are using a pre-release and"},
  {"feature-limited version of Python.", "n'est pas une version definitive.", "definitiva de Python.", "feature-limited version of Python.", "feature-limited version of Python."},
  {"You may encounter some", "Il est possible que son utilisation soit", "Es posible que encuentre varios bugs", "You may encounter", "You may encounter"},
  {"unexpected bugs.", "limitee et que certains bugs apparaissent.", "y que su utilizacion sea limitada.", "unexpected bugs.", "unexpected bugs."},
  {"Edit program", "Editer le programme", "Editar el programa", "Programm bearbeiten", "Editar programa"},
  {"Execute program", "Executer le programme", "Ejecutar el programa", "Programm ausfuhren", "Executar programa"},

  /* On boarding */
  {"UPDATE AVAILABLE", "MISE A JOUR DISPONIBLE", "ACTUALIZACION DISPONIBLE", "UPDATE VERFUGBAR", "ATUALIZACAO DISPONIVEL"},
  {"There are important upgrades", "Des ameliorations importantes existent", "Hay mejoras importantes", "Wichtige Verbesserungen fur Ihren", "Existem melhorias significativas"},
  {"for your calculator.", "pour votre calculatrice.", "para su calculadora.", "Rechner stehen zur Verfugung.", "para a sua calculadora."},
  {"Browse our page from your computer", "Connectez-vous depuis votre ordinateur", "Visita nuestra pagina desde su ordenador", "Melden Sie sich von Ihrem Computer an", "Navegue na nossa pagina do seu computador"},
  {"www.numworks.com/update", "www.numworks.com/update", "www.numworks.com/update", "www.numworks.com/update", "www.numworks.com/update"},
  {"Skip", "Passer", "Saltar", "Uberspringen", "Pular"},
};

const char sxy[4] = {Ion::Charset::CapitalSigma, 'x', 'y', 0};
constexpr static char mu[] = {Ion::Charset::SmallMu, 0};
constexpr static char sigma[] = {Ion::Charset::SmallSigma, 0};
constexpr static char lambda[] = {Ion::Charset::SmallLambda, 0};
constexpr static char rightIntegralSecondLegend[] = {Ion::Charset::LessEqual, 'X', ')', '=', 0};
constexpr static char leftIntegralFirstLegend[] = {'P', '(', 'X', Ion::Charset::LessEqual, 0};
constexpr static char finiteIntegralLegend[] = {Ion::Charset::LessEqual, 'X', Ion::Charset::LessEqual, 0};


const char * universalMessages[243] {
  "",
  approximateEqual,
  "Python",
  "PYTHON (BETA)",
  "alpha",
  "ALPHA",
  "shift",
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
  "P(X=",

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
