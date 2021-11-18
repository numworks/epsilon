#!/usr/bin/env python3

import argparse
import base64
import fileinput
import os

parser = argparse.ArgumentParser(description='Inline resources in HTML file')
parser.add_argument('file', type=str, help='an input HTML file')
parser.add_argument('--image', type=str, action='append', help='a JPEG file to convert into a Base64 data URL')
parser.add_argument('--script', type=str, action='append', help='a JS file to inline')
args = parser.parse_args()

scripts = dict(map(lambda x: (os.path.basename(x), open(x, "r").read()), args.script or []))
images = dict(map(lambda x: (os.path.basename(x), base64.b64encode(open(x, "rb").read()).decode()), args.image or []))

for line in open(args.file).readlines():
  if '<script ' in line:
    for script,data in scripts.items():
      line = line.replace(f'<script src="{script}"></script>', f'<script>{data}</script>')
  if '<img ' in line:
    for filename, data in images.items():
      line = line.replace(f'<img src="{filename}" ', f'<img src="data:image/jpeg;base64,{data}" ')
  print(line, end = '')
