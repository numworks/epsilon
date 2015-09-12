extern char _framebuffer_start;
extern char _framebuffer_end;

#define FRAMEBUFFER_ADDRESS (&_framebuffer_start)
#define FRAMEBUFFER_LENGTH (&_framebuffer_end-&_framebuffer_start)
#define FRAMEBUFFER_WIDTH 160
#define FRAMEBUFFER_HEIGHT 160
#define FRAMEBUFFER_BITS_PER_PIXEL 4
