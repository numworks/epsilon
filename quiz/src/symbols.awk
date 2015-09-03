BEGIN {
  print "#include <string.h>"
  print "#include \"symbols.h\""
  print
}

#FIXME: Is there a way to capture subexpression in awk? The following gsub is
#       kind of ugly
/TEST\(([a-z0-9_]+)\)/ { gsub(/(TEST\()|(\))/, "", $1); tests = tests "," $1 }

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
