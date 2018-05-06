%option noyywrap
%option never-interactive

%{
#define YY_DECL int yylex(char *attr, int position)

#define YY_INPUT
#define ECHO

#undef YY_BUF_SIZE
#define YY_BUF_SIZE 256

#define fprintf(...) ((void)0)
#define exit(...) abort()
%}

KEYWORD    "False"|"None"|"True"|"and"|"as"|"assert"|"break"|"class"|"continue"|"def"|"del"|"elif"|"else"|"except"|"finally"|"for"|"from"|"global"|"if"|"import"|"in"|"is"|"lambda"|"nonlocal"|"not"|"or"|"pass"|"raise"|"return"|"try"|"while"|"with"|"yield"
BUILTIN    "ArithmeticError"|"AssertionError"|"AttributeError"|"BaseException"|"BlockingIOError"|"BrokenPipeError"|"BufferError"|"BytesWarning"|"ChildProcessError"|"ConnectionAbortedError"|"ConnectionError"|"ConnectionRefusedError"|"ConnectionResetError"|"DeprecationWarning"|"EOFError"|"Ellipsis"|"EnvironmentError"|"Exception"|"FileExistsError"|"FileNotFoundError"|"FloatingPointError"|"FutureWarning"|"GeneratorExit"|"IOError"|"ImportError"|"ImportWarning"|"IndentationError"|"IndexError"|"InterruptedError"|"IsADirectoryError"|"KeyError"|"KeyboardInterrupt"|"LookupError"|"MemoryError"|"ModuleNotFoundError"|"NameError"|"NotADirectoryError"|"NotImplemented"|"NotImplementedError"|"OSError"|"OverflowError"|"PendingDeprecationWarning"|"PermissionError"|"ProcessLookupError"|"RecursionError"|"ReferenceError"|"ResourceWarning"|"RuntimeError"|"RuntimeWarning"|"StopAsyncIteration"|"StopIteration"|"SyntaxError"|"SyntaxWarning"|"SystemError"|"SystemExit"|"TabError"|"TimeoutError"|"TypeError"|"UnboundLocalError"|"UnicodeDecodeError"|"UnicodeEncodeError"|"UnicodeError"|"UnicodeTranslateError"|"UnicodeWarning"|"UserWarning"|"ValueError"|"Warning"|"ZeroDivisionError"|"abs"|"all"|"any"|"ascii"|"bin"|"bool"|"bytearray"|"bytes"|"callable"|"chr"|"classmethod"|"compile"|"complex"|"copyright"|"credits"|"delattr"|"dict"|"dir"|"divmod"|"enumerate"|"eval"|"exec"|"exit"|"filter"|"float"|"format"|"frozenset"|"getattr"|"globals"|"hasattr"|"hash"|"help"|"hex"|"id"|"input"|"int"|"isinstance"|"issubclass"|"iter"|"len"|"license"|"list"|"locals"|"map"|"max"|"memoryview"|"min"|"next"|"object"|"oct"|"open"|"ord"|"pow"|"print"|"property"|"quit"|"range"|"repr"|"reversed"|"round"|"set"|"setattr"|"slice"|"sorted"|"staticmethod"|"str"|"sum"|"super"|"tuple"|"type"|"vars"|"zip"
IDENTIFIER [a-zA-Z_][a-zA-Z0-9_]*
COMMENT    "#"[^\n]*
SP         (?i:r|u|f|fr|rf|b|br|rb)?
SQSTRING   '[^'\\\n]*(\\.[^'\\\n]*)*'?
DQSTRING   \"[^"\\\n]*(\\.[^"\\\n]*)*\"?
TSQSTRING  '''[^'\\]*((\\.|'[^']|''[^'])[^'\\]*)*(''')?
TDQSTRING  \"\"\"[^"\\]*((\\.|\"[^"]|\"\"[^"])[^"\\]*)*(\"\"\")?

%%

def\ {IDENTIFIER}                   { memset(attr+position, 6, 4); memset(attr+position+4, 1, yyleng-4); position += yyleng; }
class\ {IDENTIFIER}                 { memset(attr+position, 6, 6); memset(attr+position+6, 1, yyleng-6); position += yyleng; }
{KEYWORD}                           { memset(attr+position, 6, yyleng); position += yyleng; }
{BUILTIN}                           { memset(attr+position, 5, yyleng); position += yyleng; }
{COMMENT}                           { memset(attr+position, 4, yyleng); position += yyleng; }
({SP}{TDQSTRING})|({SP}{TDQSTRING}) { memset(attr+position, 2, yyleng); position += yyleng; }
({SP}{DQSTRING})|({SP}{SQSTRING})   { memset(attr+position, 2, yyleng); position += yyleng; }
{IDENTIFIER}                        { memset(attr+position, 0, yyleng); position += yyleng; }
.|\n                                { attr[position] = 0; position += yyleng; }

%%
