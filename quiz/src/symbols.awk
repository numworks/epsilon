BEGIN {
  print "void * pointers[] = {"
}

#FIXME: Is there a way to capture subexpression in awk? The following gsub is
#       kind of ugly
/TEST\(([a-z0-9_]+)\)/ { gsub(/(TEST\()|(\))/, "", $1); print "test_" $1 "," }

END {
  print "};"
}
