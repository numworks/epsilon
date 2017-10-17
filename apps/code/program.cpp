#include "program.h"

namespace Code {

Program::Program() :
  m_buffer()
{
  const char program[] = R"(# This program draws a Mandelbrot fractal set
# N_iteration: degree of precision
import kandinsky
N_iteration = 10
for x in range(320):
  for y in range(240):
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
    kandinsky.set_pixel(x,y,col))";

  constexpr size_t len = sizeof(program);
  static_assert(len < k_bufferSize, "Default program length bigger than buffer size");
  memcpy(m_buffer, program, len);
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
