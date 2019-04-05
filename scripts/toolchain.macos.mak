CC = clang
CXX = clang++
LD = clang++

#ARCH = x86_64
#SDK = iphonesimulator
# ARCH = arm64
# SDK = iphoneos

SYSROOT = $(shell xcrun --sdk macosx --show-sdk-path)

SFLAGS += -fPIC -arch $(ARCH) -isysroot $(SYSROOT)
LDFLAGS += -arch $(ARCH) -isysroot $(SYSROOT)
#-framework Foundation -framework OpenGLES -framework UIKit -framework AVFoundation -framework AudioToolbox -framework QuartzCore -framework GameController -framework CoreGraphics -framework CoreMotion
