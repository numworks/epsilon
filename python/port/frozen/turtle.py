from micropython import const
import kandinsky, _turtle, math, time

_hOffset = 1.57079632679  # math.pi / 2
_hScale = 0.0174532925199 # math.pi / 180
_size = const(9)
_icons = const(8)
_tSpeed = const(60)

_x = 0
_y = 0
_color = (0, 0, 0)
_heading = _hOffset
_speed = 6
_dotSize = 3
_penUp = False
_hidden = False
_drawn = False
_carryWait = 0

def _place(x, y, dot = True):
  global _x, _y, _drawn
  if _drawn:
    _turtle.erase_turtle(round(_x), round(_y))
    _drawn = False
  if not _penUp and dot:
    _turtle.stamp(round(x), round(y), _dotSize, kandinsky.color(_color[0], _color[1], _color[2]))
  if not _hidden:
    # Compute turtle frame to show
    frame = round((_heading / (2*math.pi)) * _icons)
    if frame < 0:
      frame = _icons - ((-frame) % _icons) - 1
    else:
      frame = frame % _icons
    _turtle.draw_turtle(round(x), round(y), frame)
    _drawn = True
  _x = x
  _y = y

def _stall(progress, elapsedtime):
  global _carryWait
  if _speed > 0:
    # Accumulate difference between real time and turtle's time
    speed = _tSpeed * _speed / 10
    _carryWait += (progress / speed) - elapsedtime
    if _carryWait > 0.01:
      # Turtle is ahead of real time, stall
      delta = time.monotonic()
      time.sleep(_carryWait)
      kandinsky.wait_vblank()
      _carryWait -= time.monotonic() - delta

#
# Move and draw
#

def forward(px):
  # Compute destination coordinates
  goto(_x + px*math.sin(_heading), _y + px*math.cos(_heading))

fd = forward

def backward(px):
  forward(-px)

bk = backward
back = backward

def right(angle):
  left(-angle)

rt = right

def left(angle):
  setheading(((_heading - _hOffset) + (angle * _hScale)) / _hScale)

lt = left

def setposition(x, y=None):
  global _x, _y, _carryWait
  # Grab coordinates
  if (y == None):
    newx = x[0]
    newy = y[1]
  else:
    newx = x
    newy = y
  oldx = _x
  oldy = _y
  # Journey the turtle across the screen
  delta = time.monotonic()
  length = math.sqrt((newx - _x) * (newx - _x) + (newy - _y) * (newy - _y))
  for i in range(1, math.floor(length)):
    progress = i / length
    _place(newx * progress + oldx * (1 - progress), newy * progress + oldy * (1 - progress))
    _stall(1, time.monotonic() - delta)
    delta = time.monotonic()
  _place(newx, newy)
  _stall(length % 1, time.monotonic() - delta)

goto = setposition
setpos = setposition

def setheading(angle):
  global _heading
  _heading = angle * _hScale + _hOffset
  # Redraw turtle
  _place(_x, _y, False)

seth = setheading

def speed(speed):
  global _speed
  if (speed < 0):
    speed = 0
  elif (speed > 10):
    speed = 10
  _speed = round(speed)

#
# Tell turtle's state
#

def position():
  return (_x, _y)

pos = position

def xcor():
  return _x

def ycor():
  return _y

def heading():
  return (_heading - _hOffset) / _hScale

#
# Drawing state
#

def pendown():
  global _penUp
  _penUp = False

pd = pendown
down = pendown

def penup():
  global _penUp
  _penUp = True

pu = penup
up = penup

def pensize(size=None):
  global _dotSize
  if size == None:
    return _dotSize
  if size < 1:
    size = 1
  elif size > 10:
    size = 10
  _dotSize = round(size)

def isdown():
  return not _penUp

#
# More drawing control
#

def reset():
  global _color, _heading, _speed, _dotSize, _penUp, _hidden
  # Reset properties
  _color = (0, 0, 0)
  _heading = _hOffset
  _speed = 6
  _dotSize = 3
  _penUp = False
  _hidden = False
  # Move turtle back to origin
  _place(0, 0, False)

#
# Color control
#

def pencolor(r, g=None, b=None):
  global _color
  if g == None:
    _color = r
  else:
    _color = (r, g, b)

#
# Visibility
#

def showturtle():
  global _hidden
  _hidden = False
  _place(_x, _y, False)

st = showturtle

def hideturtle():
  global _hidden
  _hidden = True
  _place(_x, _y, False)

ht = hideturtle

def isvisible():
  return not _hidden
