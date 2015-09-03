BEGIN {
  print "#include <string.h>"
  print "#include \"symbols.h\""
  print
}

#FIXME: Is there a way to capture subexpression in awk? The following gsub is
#       kind of ugly
/QUIZ_CASE\(([a-z0-9_]+)\)/ { gsub(/(QUIZ_CASE\()|(\))/, "", $1); tests = tests ",quiz_case_" $1 }

END {
  a = tests;
  b = tests;
  gsub(/,/, "();\nvoid ", a);
  sub(/\(\);\n/, "", a);
  print a "();";
  print "";
  print "QuizCase quiz_cases[] = {";
  sub(/,/, "", b);
  gsub(/,/, ",\n", b);
  print b;
  print ",NULL"
  print "};"
}
