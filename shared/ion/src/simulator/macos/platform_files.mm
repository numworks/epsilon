#include "../shared/platform.h"
#include <AppKit/AppKit.h>
#include <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

namespace Ion {
namespace Simulator {
namespace Platform {

static const char * getPath(NSSavePanel * panel, const char * extension) {
  NSString * ext = [NSString stringWithUTF8String:extension];
  [panel setAllowedContentTypes:[NSArray arrayWithObject:[UTType typeWithFilenameExtension:ext]]];
  [panel setAllowsOtherFileTypes:NO];
  static NSString * path = nil;
  [path release]; path = nil; // Discard previous path if any
  NSWindow * mainWindow = [[NSApplication sharedApplication] keyWindow];
  if ([panel runModal] == NSModalResponseOK) {
    path = [[[panel URL] path] retain];
  }
  [mainWindow makeKeyAndOrderFront:nil];
  return [path UTF8String];
}

const char * filePathForReading(const char * extension) {
  NSOpenPanel * panel = [NSOpenPanel openPanel];
  return getPath(panel, extension);
}

const char * filePathForWriting(const char * extension) {
  NSSavePanel * panel = [NSSavePanel savePanel];
  NSString * ext = [NSString stringWithUTF8String:extension];
  [panel setNameFieldStringValue:[@"scenario." stringByAppendingString:ext]];
  return getPath(panel, extension);
}

}
}
}
