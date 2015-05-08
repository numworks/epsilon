On what foundation do the apps rely?

 -> dynamic memory allocation -> Not obious, *maybe* a malloc implementation can be provided
 -> direct access to a framebuffer
 -> Use of a graphic library to write to the framebuffer (no "context")
 -> A way to receive keyboard input.

That's it!
