extern "C" {
#include "modturtle.h"
#include <py/gc.h>
#include <py/objtuple.h>
#include <py/runtime.h>
}
#include "turtle.h"
#include "../../port.h"

static Turtle sTurtle;

void modturtle_gc_collect() {
  // Mark the shared sTurtle object as a GC root
  MicroPython::collectRootsAtAddress((char *)&sTurtle, sizeof(Turtle));
}

void modturtle_view_did_disappear() {
  sTurtle.viewDidDisappear();
}

mp_obj_t modturtle___init__() {
  sTurtle = Turtle();
  /* Note: we don't even bother writing a destructor for Turtle because this
   * init function is called once, and only once, per MicroPython init cycle.
   * When the previous Turtle object is destroyed, its VM is long gone. */
  return mp_const_none;
}

mp_obj_t modturtle_reset() {
  sTurtle.reset();
  // Cf comment on modkandinsky_draw_string
  micropython_port_interrupt_if_needed();
  return mp_const_none;
}

mp_obj_t modturtle_forward(mp_obj_t px) {
  sTurtle.forward(mp_obj_get_float(px));
  return mp_const_none;
}

mp_obj_t modturtle_backward(mp_obj_t px) {
  sTurtle.backward(mp_obj_get_float(px));
  return mp_const_none;
}

mp_obj_t modturtle_right(mp_obj_t angle) {
  sTurtle.right(mp_obj_get_float(angle));
  return mp_const_none;
}

mp_obj_t modturtle_left(mp_obj_t angle) {
  sTurtle.left(mp_obj_get_float(angle));
  return mp_const_none;
}

mp_obj_t modturtle_circle(size_t n_args, const mp_obj_t *args) {
  mp_int_t radius = mp_obj_get_int(args[0]);

  if (n_args == 1) {
    sTurtle.circle(radius);
  }
  else {
    mp_float_t angle = mp_obj_get_float(args[1]);
    sTurtle.circle(radius, angle);
  }
  return mp_const_none;
}

mp_obj_t modturtle_goto(size_t n_args, const mp_obj_t *args) {
  mp_float_t x = 0;
  mp_float_t y = 0;

  if (n_args == 1) {
    mp_obj_t * mp_coords;
    mp_obj_get_array_fixed_n(args[0], 2, &mp_coords);
    x = mp_obj_get_float(mp_coords[0]);
    y = mp_obj_get_float(mp_coords[1]);
  }
  else {
    x = mp_obj_get_float(args[0]);
    y = mp_obj_get_float(args[1]);
  }

  sTurtle.goTo(x, y);

  return mp_const_none;
}

mp_obj_t modturtle_setheading(mp_obj_t angle) {
  sTurtle.setHeading(mp_obj_get_float(angle));
  return mp_const_none;
}

mp_obj_t modturtle_speed(size_t n_args, const mp_obj_t *args) {
  if (n_args == 0) {
    return MP_OBJ_NEW_SMALL_INT(sTurtle.speed());
  }
  sTurtle.setSpeed(mp_obj_get_int(args[0]));
  return mp_const_none;
}

mp_obj_t modturtle_position() {
  mp_obj_t mp_pos[2];
  mp_pos[0] = mp_obj_new_float(sTurtle.x());
  mp_pos[1] = mp_obj_new_float(sTurtle.y());
  return mp_obj_new_tuple(2, mp_pos);
}

mp_obj_t modturtle_heading() {
  return mp_obj_new_float(sTurtle.heading());
}

mp_obj_t modturtle_pendown() {
  sTurtle.setPenDown(true);
  return mp_const_none;
}

mp_obj_t modturtle_penup() {
  sTurtle.setPenDown(false);
  return mp_const_none;
}

mp_obj_t modturtle_pensize(size_t n_args, const mp_obj_t *args) {
  if (n_args == 0) {
    return MP_OBJ_NEW_SMALL_INT(sTurtle.penSize());
  }
  sTurtle.setPenSize(mp_obj_get_int(args[0]));
  return mp_const_none;
}

mp_obj_t modturtle_isdown() {
  return sTurtle.isPenDown() ? mp_const_true : mp_const_false;
}

mp_obj_t modturtle_pencolor(size_t n_args, const mp_obj_t *args) {
  switch(n_args){
    case 0:{
      // pencolor()
      KDColor c = sTurtle.color();
      mp_obj_t mp_col[3];
      if(sTurtle.colorMode() == MicroPython::ColorParser::ColorModes::MaxIntensity255){
        mp_col[0] = mp_obj_new_int_from_uint(c.red());
        mp_col[1] = mp_obj_new_int_from_uint(c.green());
        mp_col[2] = mp_obj_new_int_from_uint(c.blue());
      } else {
        mp_col[0] = mp_obj_new_float(c.red() / 255.0);
        mp_col[1] = mp_obj_new_float(c.green() / 255.0);
        mp_col[2] = mp_obj_new_float(c.blue() / 255.0);
      }
      return mp_obj_new_tuple(3, mp_col);
    }
    case 1:{
      sTurtle.setColor(MicroPython::ColorParser::ParseColor(args[0], sTurtle.colorMode()));
      break;
    }
    case 3:{
      // pencolor(r, g, b)
      if(sTurtle.colorMode() == MicroPython::ColorParser::ColorModes::MaxIntensity255){
        sTurtle.setColor(
          KDColor::RGB888(
            mp_obj_get_int(args[0]),
            mp_obj_get_int(args[1]),
            mp_obj_get_int(args[2])));
      } else {
        sTurtle.setColor(
          KDColor::RGB888(
            mp_obj_get_int(args[0]) * 255,
            mp_obj_get_int(args[1]) * 255,
            mp_obj_get_int(args[2]) * 255));
      }
      break;
    }
    default:{
      assert(n_args == 2);
      mp_raise_TypeError("pencolor() takes 0, 1 or 3 arguments");
    }
  }
  return mp_const_none;
}

mp_obj_t modturtle_colormode(size_t n_args, const mp_obj_t *args){
  if(n_args == 0){
    return (sTurtle.colorMode() == MicroPython::ColorParser::ColorModes::MaxIntensity255) ? mp_obj_new_int_from_uint(255) : mp_obj_new_int_from_uint(1);
  } else{
    int colorMode = mp_obj_get_int(args[0]);
    if(colorMode == 1){
      sTurtle.setColorMode(MicroPython::ColorParser::ColorModes::MaxIntensity1);
    } else if(colorMode == 255){
      sTurtle.setColorMode(MicroPython::ColorParser::ColorModes::MaxIntensity255);
    } else {
      mp_raise_ValueError("Colormodes can be 1 or 255");
    }
  }
  return mp_const_none;
}

mp_obj_t modturtle_showturtle() {
  sTurtle.setVisible(true);
  return mp_const_none;
}

mp_obj_t modturtle_hideturtle() {
  sTurtle.setVisible(false);
  return mp_const_none;
}

mp_obj_t modturtle_isvisible() {
  return sTurtle.isVisible() ? mp_const_true : mp_const_false;
}
