#include <SDL.h>
#include <assert.h>
#include <gdiplus.h>
#include <ion/src/simulator/windows/images.h>
#include <olectl.h>
#include <windows.h>

#include "../shared/platform.h"

/* Loading images using GDI+
 * On Windows, we manipulate images using GDI+ which is widely available.
 * Note that this adds an extra runtime dependency (as compared to just SDL),
 * but this should not be an issue. */

static inline HRESULT CreateStreamOnResource(const char *name,
                                             LPSTREAM *stream) {
  HINSTANCE hInstance = GetModuleHandle(0);
  *stream = nullptr;
  HRSRC hC = FindResource(hInstance, name, RT_RCDATA);
  if (!hC) {
    SDL_Log("Could not find resource %s", name);
    return E_INVALIDARG;
  }
  // This is not really a HGLOBAL
  // http://msdn.microsoft.com/en-us/library/windows/desktop/ms648046(v=vs.85).aspx
  HGLOBAL hG = LoadResource(hInstance, hC);
  if (!hG) {
    SDL_Log("Could not load resource %s", name);
    return E_INVALIDARG;
  }
  void *bytes = LockResource(hG);
  ULONG size = SizeofResource(hInstance, hC);
  HRESULT hr = CreateStreamOnHGlobal(NULL, true, stream);
  if (SUCCEEDED(hr)) {
    ULONG written;
    hr = (*stream)->Write(bytes, size, &written);
  }
  return hr;
}

// Helper class to init/shutdown Gdiplus using RAII
class GdiplusSession {
 public:
  GdiplusSession() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
  }
  ~GdiplusSession() { Gdiplus::GdiplusShutdown(m_gdiplusToken); }

 private:
  ULONG_PTR m_gdiplusToken;
};

// Helper function from MSDN
// https://docs.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-retrieving-the-class-identifier-for-an-encoder-use
int GetEncoderClsid(const WCHAR *format, CLSID *pClsid) {
  UINT num = 0;   // number of image encoders
  UINT size = 0;  // size of the image encoder array in bytes

  Gdiplus::ImageCodecInfo *pImageCodecInfo = nullptr;
  Gdiplus::GetImageEncodersSize(&num, &size);
  if (size == 0) {
    return -1;
  }
  pImageCodecInfo = static_cast<Gdiplus::ImageCodecInfo *>(malloc(size));
  if (pImageCodecInfo == nullptr) {
    return -1;
  }

  Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

  for (UINT i = 0; i < num; i++) {
    if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
      *pClsid = pImageCodecInfo[i].Clsid;
      free(pImageCodecInfo);
      return i;  // Success
    }
  }

  free(pImageCodecInfo);
  return -1;
}

static wchar_t *createWideCharArray(const char *src) {
  int wchars_num = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
  wchar_t *wstr = new wchar_t[wchars_num];
  MultiByteToWideChar(CP_UTF8, 0, src, -1, wstr, wchars_num);
  return wstr;
}

namespace Ion {
namespace Simulator {
namespace Platform {

SDL_Texture *loadImage(SDL_Renderer *renderer, const char *identifier) {
  GdiplusSession session;

  LPSTREAM stream;
  int resourceID = -1;
  for (size_t i = 0;
       i < sizeof(resourcesIdentifiers) / sizeof(resourcesIdentifiers[0]);
       i++) {
    if (strcmp(identifier, resourcesIdentifiers[i].identifier()) == 0) {
      resourceID = resourcesIdentifiers[i].id();
    }
  }
  assert(resourceID >= 0);
  const char *resname = MAKEINTRESOURCE(resourceID);
  CreateStreamOnResource(resname, &stream);

  Gdiplus::Bitmap *image = Gdiplus::Bitmap::FromStream(stream);

  int width = (int)image->GetWidth();
  int height = (int)image->GetHeight();
  Gdiplus::Rect rc(0, 0, width, height);

  Gdiplus::BitmapData *bitmapData = new Gdiplus::BitmapData;
  image->LockBits(&rc, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB,
                  bitmapData);

  size_t bytesPerPixel = 4;

  SDL_Texture *texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STATIC, width, height);

  SDL_UpdateTexture(texture, NULL, bitmapData->Scan0, bytesPerPixel * width);

  SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

  image->UnlockBits(bitmapData);
  delete bitmapData;
  delete image;

  return texture;
}

void saveImage(const KDColor *pixels, int width, int height, const char *path) {
  static_assert(sizeof(KDColor) == 2, "KDColor expected to be RGB565");
  GdiplusSession session;

  Gdiplus::Bitmap bitmap(
      width, height, 2 * width, PixelFormat16bppRGB565,
      reinterpret_cast<BYTE *>(const_cast<KDColor *>(pixels)));

  CLSID pngClsid;
  if (GetEncoderClsid(L"image/png", &pngClsid) > 0) {
    wchar_t *widePath = createWideCharArray(path);
    bitmap.Save(widePath, &pngClsid, nullptr);
    delete[] widePath;
  }
}

void copyImageToClipboard(const KDColor *pixels, int width, int height) {
  // TODO
}

}  // namespace Platform
}  // namespace Simulator
}  // namespace Ion
