#ifndef KANDINSKY_FRAMEBUFFER_H
#define KANDINSKY_FRAMEBUFFER_H

#include <kandinsky/color.h>
#include <kandinsky/types.h>
#include <kandinsky/rect.h>

typedef struct {
  KDColor * pixels;
  KDSize size;
} KDFrameBuffer;

void KDFramePushRect(KDFrameBuffer * frameBuffer, KDRect rect, const KDColor * pixels);
void KDFramePushRectUniform(KDFrameBuffer * frameBuffer, KDRect rect, KDColor color);
void KDFramePullRect(KDFrameBuffer * frameBuffer, KDRect rect, KDColor * pixels);

#endif
