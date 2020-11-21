import sys
import unicodedata
import argparse
import io
import shutil

filename = sys.argv[1]

print("Normalization of "+filename)

output = open(filename+".tmp", "wb")

with io.open(filename, "r", encoding='utf-8') as file:    
    for line in file:
        unicodeLine = unicodedata.normalize("NFKD", line)        
        output.write(unicodeLine.encode("UTF-8"))        
output.close()
shutil.move(filename+".tmp",filename)
