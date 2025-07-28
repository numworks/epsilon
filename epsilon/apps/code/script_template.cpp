#include "script_template.h"

namespace Code {

constexpr ScriptTemplate emptyScriptTemplate(".py", R"(from math import *
)");

constexpr ScriptTemplate squaresScriptTemplate("squares.py",
                                               R"(from math import *
from turtle import *
def squares(angle=0.5):
  reset()
  L=330
  speed(10)
  penup()
  goto(-L/2,-L/2)
  pendown()
  for i in range(660):
    forward(L)
    left(90+angle)
    L=L-L*sin(angle*pi/180)
  hideturtle())");

constexpr ScriptTemplate mandelbrotScriptTemplate(
    "mandelbrot.py", R"(# This script draws a Mandelbrot fractal set
# N_iteration: degree of precision
import kandinsky
def mandelbrot(N_iteration):
  for x in range(320):
    for y in range(222):
# Compute the mandelbrot sequence for the point c = (c_r, c_i) with start value z = (z_r, z_i)
      z = complex(0,0)
# Rescale to fit the drawing screen 320x222
      c = complex(3.5*x/319-2.5, -2.5*y/221+1.25)
      i = 0
      while (i < N_iteration) and abs(z) < 2:
        i = i + 1
        z = z*z+c
# Choose the color of the dot from the Mandelbrot sequence
      rgb = int(255*i/N_iteration)
      col = kandinsky.color(int(rgb),int(rgb*0.75),int(rgb*0.25))
# Draw a pixel colored in 'col' at position (x,y)
      kandinsky.set_pixel(x,y,col))");

constexpr ScriptTemplate polynomialScriptTemplate("polynomial.py",
                                                  R"(from math import *
# roots(a,b,c) computes the solutions of the equation a*x**2+b*x+c=0
def roots(a,b,c):
  delta = b*b-4*a*c
  if delta == 0:
    return -b/(2*a)
  elif delta > 0:
    x_1 = (-b-sqrt(delta))/(2*a)
    x_2 = (-b+sqrt(delta))/(2*a)
    return x_1, x_2
  else:
    return None)");

constexpr ScriptTemplate parabolaScriptTemplate(
    "parabola.py", R"(from matplotlib.pyplot import *
from math import *

g=9.81

def x(t,v_0,alpha):
  return v_0*cos(alpha)*t
def y(t,v_0,alpha,h_0):
  return -0.5*g*t**2+v_0*sin(alpha)*t+h_0

def vx(v_0,alpha):
  return v_0*cos(alpha)
def vy(t,v_0,alpha):
  return -g*t+v_0*sin(alpha)

def t_max(v_0,alpha,h_0):
  return (v_0*sin(alpha)+sqrt((v_0**2)*(sin(alpha)**2)+2*g*h_0))/g

def simulation(v_0=15,alpha=pi/4,h_0=2):
  tMax=t_max(v_0,alpha,h_0)
  accuracy=1/10**(floor(log10(tMax))-1)
  T_MAX=floor(tMax*accuracy)+1
  X=[x(t/accuracy,v_0,alpha) for t in range(T_MAX)]
  Y=[y(t/accuracy,v_0,alpha,h_0) for t in range(T_MAX)]
  VX=[vx(v_0,alpha) for t in range(T_MAX)]
  VY=[vy(t/accuracy,v_0,alpha) for t in range(T_MAX)]
  for i in range(T_MAX):
    arrow(X[i],Y[i],VX[i]/accuracy,VY[i]/accuracy)
  grid()
  show())");

const ScriptTemplate* ScriptTemplate::Empty() { return &emptyScriptTemplate; }

const ScriptTemplate* ScriptTemplate::Squares() {
  return &squaresScriptTemplate;
}

const ScriptTemplate* ScriptTemplate::Mandelbrot() {
  return &mandelbrotScriptTemplate;
}

const ScriptTemplate* ScriptTemplate::Polynomial() {
  return &polynomialScriptTemplate;
}

const ScriptTemplate* ScriptTemplate::Parabola() {
  return &parabolaScriptTemplate;
}

}  // namespace Code
