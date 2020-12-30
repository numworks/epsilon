#ifndef ION_DEVICE_SHARED_DRIVERS_SVCALL_H
#define ION_DEVICE_SHARED_DRIVERS_SVCALL_H

namespace Ion {
namespace Device {
namespace SVCall {

#define SVC_CLOCK_LOW_FREQUENCY 1
#define SVC_CLOCK_STANDARD_FREQUENCY 2
#define SVC_POWER_SLEEP_OR_STOP 3
#define SVC_POWER_STANDBY 4
#define SVC_DFU 5
#define SVC_RESET_CORE 6
#define SVC_EXAM_MODE_TOGGLE 7
#define SVC_PUSH_RECT 8
#define SVC_PUSH_RECT_UNIFORM 9
#define SVC_PULL_RECT 10
#define SVC_POST_PUSH_MULTICOLOR 11

void getSvcallArgs(int argc, void * argv[]);

#define SVC_ARGS_REGISTER_0 "r10"
#define SVC_ARGS_REGISTER_1 "r8"

}
}
}

#endif
