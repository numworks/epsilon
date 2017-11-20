#include "script_template.h"

namespace Code {

constexpr ScriptTemplate emptyScriptTemplate(".py", R"(from math import *
)");

constexpr ScriptTemplate factorialScriptTemplate("factorial.py", R"(def factorial(n):
  if n == 0:
    return 1
  else:
    return n * factorial(n-1))");

constexpr ScriptTemplate fibonacciScriptTemplate("fibonacci.py", R"(def fibo(n):
  a=0
  b=1
  for i in range(1,n+1):
    c=a+b
    a=b
    b=c
  return a

def fibo2(n):
  if n==0:
    return 0
  elif n==1 or n==2:
    return 1
  return fibo2(n-1)+fibo2(n-2))");

constexpr ScriptTemplate mandelbrotScriptTemplate("mandelbrot.py", R"(# This script draws a Mandelbrot fractal set
# N_iteration: degree of precision
import kandinsky
N_iteration = 10
def drawMandlebrot():
  for x in range(320):
    for y in range(222):
# Compute the mandelbrot sequence for the point c = (c_r, c_i) with start value z = (z_r, z_i)
      z_r = 0
      z_i = 0
# Rescale to fit the drawing screen 320x222
      c_r = 2.7*x/319-2.1
      c_i = -1.87*y/221+0.935
      i = 0
      while (i < N_iteration) and ((z_r * z_r) + (z_i * z_i) < 4):
        i = i + 1
        stock = z_r
        z_r = z_r * z_r - z_i * z_i + c_r
        z_i = 2 * stock * z_i + c_i
# Choose the color of the dot from the Mandelbrot sequence
      rgb = int(255*i/N_iteration)
      col = kandinsky.color(int(rgb),int(rgb*0.75),int(rgb*0.25))
# Draw a pixel colored in 'col' at position (x,y)
      kandinsky.set_pixel(x,y,col))");

const ScriptTemplate * ScriptTemplate::Empty() {
  return &emptyScriptTemplate;
}

const ScriptTemplate * ScriptTemplate::Factorial() {
  return &factorialScriptTemplate;
}

const ScriptTemplate * ScriptTemplate::Fibonacci() {
  return &fibonacciScriptTemplate;
}

const ScriptTemplate * ScriptTemplate::Mandelbrot() {
  return &mandelbrotScriptTemplate;
}

}


