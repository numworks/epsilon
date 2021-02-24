#!/usr/bin/env python3

import argparse
import json

def css_rule(selector, *declarations):
  css = ''
  css += selector + ' {\n'
  for decl in declarations:
    css += decl
  css += '}\n\n'
  return css

def css_declaration(prop, value):
  return "  %s: %s;\n" % (prop, value)

def css_percentage_declaration(prop, ratio):
  return css_declaration(prop, "%.2f%%" % (100*ratio))

def css_rect_declarations(rect, ref):
  css = ''
  css += css_percentage_declaration("left", rect[0]/ref[2])
  css += css_percentage_declaration("top", rect[1]/ref[3])
  css += css_percentage_declaration("width", rect[2]/ref[2])
  css += css_percentage_declaration("height", rect[3]/ref[3])
  return css

def css(layout):
  background = layout["background"]
  css = ''
  css += css_rule(
    '.calculator',
    css_declaration('position', 'relative'),
  )
  css += css_rule(
    '.calculator-aspect-ratio',
    css_percentage_declaration('padding-bottom', background[3]/background[2])
  )
  css += css_rule(
    '.calculator canvas',
    css_declaration('position', 'absolute'),
    css_rect_declarations(layout['screen'], background)
  )
  css += css_rule(
    '.calculator span',
    css_declaration('position', 'absolute'),
    css_declaration('display', 'block'),
    css_declaration('border-radius', '999px'),
    css_declaration('cursor', 'pointer'),
  )
  css += css_rule(
    '.calculator span:hover',
    css_declaration('background-color', 'rgba(0, 0, 0, 0.1)')
  )
  css += css_rule(
    '.calculator span:active',
    css_declaration('background-color', 'rgba(0, 0, 0, 0.2)')
  )
  for key,rect in layout["keys"].items():
    css += css_rule(
      '.calculator span[data-key="%s"]' % key,
      css_rect_declarations(rect, background)
    )
  css += css_rule(
    '.calculator-mirror canvas',
    css_declaration('image-rendering', '-moz-crisp-edges'),
    css_declaration('image-rendering', 'pixelated'),
    css_declaration('-ms-interpolation-mode', 'nearest-neighbor')
  )
  return css

def html(layout):
  screen = layout["screen"]
  background = layout["background"]
  html = ''
  html += '<div class="calculator">\n'
  html += '  <canvas tabindex="1"></canvas>\n'
  for key in layout["keys"]:
    html += '  <span data-key="%s"></span>\n' % key
  html += '</div>'
  return html

# Argument parsing

parser = argparse.ArgumentParser(description='Generate HTML and CSS files from JSON layout')
parser.add_argument('file', type=str, help='a JSON layout file')
parser.add_argument('--css', type=str, help='an output HTML file')
parser.add_argument('--html', type=str, help='an output CSS file')
args = parser.parse_args()

with open(args.file) as f:
  layout = json.load(f)
  if args.css:
    with open(args.css, 'w') as o:
      o.write(css(layout))
  if args.html:
    with open(args.html, 'w') as o:
      o.write(html(layout))
