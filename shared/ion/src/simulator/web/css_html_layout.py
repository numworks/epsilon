#!/usr/bin/env python3

import argparse
import json


def css_rule(selector, *declarations):
    css = ""
    css += selector + " {\n"
    for decl in declarations:
        css += decl
    css += "}\n\n"
    return css


def css_declaration(prop, value):
    return "  %s: %s;\n" % (prop, value)


def css_percentage_declaration(prop, ratio):
    return css_declaration(prop, "%.2f%%" % (100 * ratio))


def css_rect_declarations(rect, ref=[0, 0, 100, 100]):
    css = ""
    css += css_percentage_declaration("left", rect[0] / ref[2])
    css += css_percentage_declaration("top", rect[1] / ref[3])
    css += css_percentage_declaration("width", rect[2] / ref[2])
    css += css_percentage_declaration("height", rect[3] / ref[3])
    return css


# Generate the custom css for the epsilon and scandium simulator
def css(layout):
    background = layout["background"]
    css = ""
    css += css_rule(
        ".calculator",
        css_declaration("position", "absolute"),
        css_rect_declarations(layout["calculatorPosition"]),
    )
    css += css_rule(
        ".calculator canvas, .calculator .loader",
        css_declaration("position", "absolute"),
        css_rect_declarations(layout["screen"], background),
    )
    for key, rect in enumerate(layout["keys"].values()):
        css += css_rule(
            '.calculator span[data-key="%s"]' % key,
            css_rect_declarations(rect, background),
        )
    css += css_rule(
        ".calculator .loader svg circle",
        css_declaration("stroke", f"{layout['loader_color']}"),
    )
    return css


spinner_svg = """
<svg viewBox="0 0 100 100">
  <circle cx="50" cy="50" r="46" fill="none" stroke-width="6" stroke-dasharray="72.5"></circle>
</svg>
"""


# Generate the custom html for the epsilon and scandium simulator
def html(layout):
    screen = layout["screen"]
    background = layout["background"]
    html = ""
    html += '<div class="calculator-container">\n'
    html += "  <picture>\n"
    html += '    <source srcset="background-with-shadow.webp" type="image/webp">\n'
    html += '    <img src="background.jpg" alt="NumWorks Calculator">\n'
    html += "  </picture>\n"
    html += '  <div class="calculator loading">\n'
    html += '    <div class="loader">%s</div>\n' % spinner_svg
    html += '    <canvas tabindex="1"></canvas>\n'
    for key in range(len(layout["keys"])):
        html += '    <span data-key="%s"></span>\n' % key
    html += "  </div>"
    html += "</div>"
    return html


# Argument parsing

parser = argparse.ArgumentParser(
    description="Generate HTML and CSS files from JSON layout"
)
parser.add_argument("file", type=str, help="a JSON layout file")
parser.add_argument("--css", type=str, help="an output HTML file")
parser.add_argument("--html", type=str, help="an output CSS file")
args = parser.parse_args()

with open(args.file) as f:
    layout = json.load(f)
    if args.css:
        with open(args.css, "w") as o:
            o.write(css(layout))
    if args.html:
        with open(args.html, "w") as o:
            o.write(html(layout))
