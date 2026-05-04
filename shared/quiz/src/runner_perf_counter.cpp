#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <quiz.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

/* Count user-space retired instructions around `body` using the Linux
 * perf_event subsystem. See `man 2 perf_event_open`.
 * It requires the PERFMON capability and kernel parameter
 * perf_event_paranoid <= 2.
 */
void runCaseWithInstructionCount(Quiz::TestCase body) {
  struct perf_event_attr pe = {};
  pe.type = PERF_TYPE_HARDWARE;
  pe.size = sizeof(pe);
  pe.config = PERF_COUNT_HW_INSTRUCTIONS;
  pe.disabled = 1;
  pe.exclude_kernel = 1;
  pe.exclude_hv = 1;
  int fd = (int)syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0);
  if (fd < 0) {
    body();
    quiz_print("INSTRUCTIONS: 0");
    return;
  }
  ioctl(fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
  body();
  ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
  long long count = 0;
  read(fd, &count, sizeof(count));
  close(fd);
  char buffer[32];
  snprintf(buffer, sizeof(buffer), "INSTRUCTIONS: %lld", count);
  quiz_print(buffer);
}
