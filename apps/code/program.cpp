#include "program.h"

namespace Code {

Program::Program() :
  m_buffer("")
{
  const char * program =

"# This program draws a Mandelbrot fractal set\n"
"# N_iteration: degree of precision\n"
"import kandinsky\n"
"N_iteration = 10\n"
"for x in range(320):\n"
"  for y in range(240):\n"
"# Compute the mandelbrot sequence for the point c = (c_r, c_i) with start value z = (z_r, z_i)\n"
"    z_r = 0\n"
"    z_i = 0\n"
"# Rescale to fit the drawing screen 320x222\n"
"    c_r = 2.7*x/319-2.1\n"
"    c_i = -1.87*y/221+0.935\n"
"    i = 0\n"
"    while (i < N_iteration) and ((z_r * z_r) + (z_i * z_i) < 4):\n"
"      i = i + 1\n"
"      stock = z_r\n"
"      z_r = z_r * z_r - z_i * z_i + c_r\n"
"      z_i = 2 * stock * z_i + c_i\n"
"# Choose the color of the dot from the Mandelbrot sequence\n"
"    rgb = int(255*i/N_iteration)\n"
"    col = kandinsky.color(int(rgb),int(rgb*0.75),int(rgb*0.25))\n"
"# Draw a pixel colored in 'col' at position (x,y)\n"
"    kandinsky.set_pixel(x,y,col)\n";

   memcpy(m_buffer, program, 1024);
}

const char * Program::readOnlyContent() const {
  return m_buffer;
}

char * Program::editableContent() {
  return m_buffer;
}

void Program::setContent(const char * program) {
  memcpy(m_buffer, program, k_bufferSize);
}

int Program::bufferSize() const {
  return k_bufferSize;
}

}
