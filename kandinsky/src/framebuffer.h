typedef uint8_t kdpixel_t;

#define FRAMEBUFFER_WIDTH 240
#define FRAMEBUFFER_HEIGHT 320
#define FRAMEBUFFER_ADDRESS (kdpixel_t *)(0x2001D400)

#define PIXEL(x,y) *(kdpixel_t *)(FRAMEBUFFER_ADDRESS + y*FRAMEBUFFER_WIDTH + x)
