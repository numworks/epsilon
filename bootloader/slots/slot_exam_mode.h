#ifndef BOOTLOADER_SLOTS_EXAM_MODE_H
#define BOOTLOADER_SLOTS_EXAM_MODE_H

extern "C" {
    #include <stdint.h>
  }

namespace Bootloader {
namespace ExamMode {

static const uint32_t SlotAExamModeBufferStartOldVersions = 0x90001000;
static const uint32_t SlotAExamModeBufferEndOldVersions = 0x90003000;

static const uint32_t SlotAExamModeBufferStartNewVersions = 0x903f0000;
static const uint32_t SlotAExamModeBufferEndNewVersions = 0x90400000;

static const uint32_t SlotBExamModeBufferStartOldVersions = 0x90401000;
static const uint32_t SlotBExamModeBufferEndOldVersions = 0x90403000;

static const uint32_t SlotBExamModeBufferStartNewVersions = 0x907f0000;
static const uint32_t SlotBExamModeBufferEndNewVersions = 0x90800000;

static const uint32_t SlotKhiExamModeBufferStart = 0x90181000;
static const uint32_t SlotKhiExamModeBufferEnd = 0x90183000;

class SlotsExamMode{
  public:
    static uint8_t FetchSlotAExamMode(bool newVersion);
    static uint8_t FetchSlotBExamMode(bool newVerion);
    static uint8_t FetchSlotKhiExamMode();
    
    static uint32_t getSlotAStartExamAddress(bool newVersion);
    static uint32_t getSlotAEndExamAddress(bool newVersion);
    static uint32_t getSlotBStartExamAddress(bool newVersion);
    static uint32_t getSlotBEndExamAddress(bool newVersion);
    static uint32_t getSlotKhiStartExamAddress();
    static uint32_t getSlotKhiEndExamAddress();

};

enum class ExamMode : int8_t {
    Unknown = -1,
    Off = 0,
    Standard = 1,
    NoSym = 2,
    NoSymNoText = 3,
    Dutch = 4,
  };

}
}

#endif
