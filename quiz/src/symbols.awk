BEGIN {
  print "#include <string.h>"
  print "#include \"symbols.h\""
  print
}

#FIXME: Is there a way to capture subexpression in awk? The following gsub is
#       kind of ugly
/QUIZ_CASE\(([a-z0-9_]+)\)/ { gsub(/(QUIZ_CASE\()|(\))/, "", $1); tests = tests "quiz_case_" $1 "," }

END {
  declarations = tests;
  gsub(/quiz_case/, "void quiz_case", declarations);
  gsub(/,/, "();\n", declarations);
  print declarations;

  symbols = tests;
  print "QuizCase quiz_cases[] = {";
  gsub(/quiz_case/, "  quiz_case", symbols);
  gsub(/,/, ",\n", symbols);
  symbols = symbols "  NULL"
  print symbols;
  print "};"
  print ""

  names = tests;
  print "char * quiz_case_names[] = {";
  gsub(/quiz_case_/, "  \"", names);
  gsub(/,/, "\",\n", names);
  names = names "  NULL"
  print names;
  print "};"
}
