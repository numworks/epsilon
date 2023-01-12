/* SDL configuration for Emscripten */

#ifndef SDL_config_h_
#define SDL_config_h_

#include "SDL_platform.h"

/* C datatypes */
#ifdef __LP64__
#define SIZEOF_VOIDP 8
#else
#define SIZEOF_VOIDP 4
#endif
#define HAVE_GCC_ATOMICS 1
/* #undef HAVE_GCC_SYNC_LOCK_TEST_AND_SET */

/* Useful headers */
#define STDC_HEADERS 1
#define HAVE_ALLOCA_H 1
#define HAVE_CTYPE_H 1
#define HAVE_FLOAT_H 1
#define HAVE_ICONV_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_LIMITS_H 1
#define HAVE_MALLOC_H 1
#define HAVE_MATH_H 1
#define HAVE_MEMORY_H 1
#define HAVE_SIGNAL_H 1
#define HAVE_STDARG_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRINGS_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_WCHAR_H 1
/* #undef HAVE_PTHREAD_NP_H */
/* #undef HAVE_LIBUNWIND_H */

/* C library functions */
#define HAVE_MALLOC 1
#define HAVE_CALLOC 1
#define HAVE_REALLOC 1
#define HAVE_FREE 1
#define HAVE_ALLOCA 1
#define HAVE_GETENV 1
#define HAVE_SETENV 1
#define HAVE_PUTENV 1
#define HAVE_UNSETENV 1
#define HAVE_QSORT 1
/* #undef HAVE_ABS */
#define HAVE_BCOPY 1
#define HAVE_MEMSET 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MEMCMP 1
#define HAVE_WCSLEN 1
/* #undef HAVE_WCSLCPY */
/* #undef HAVE_WCSLCAT */
#define HAVE_WCSCMP 1
#define HAVE_STRLEN 1
#define HAVE_STRLCPY 1
#define HAVE_STRLCAT 1
/* #undef HAVE__STRREV */
/* #undef HAVE__STRUPR */
/* #undef HAVE__STRLWR */
/* #undef HAVE_INDEX */
/* #undef HAVE_RINDEX */
#define HAVE_STRCHR 1
#define HAVE_STRRCHR 1
#define HAVE_STRSTR 1
/* #undef HAVE_ITOA */
/* #undef HAVE__LTOA */
/* #undef HAVE__UITOA */
/* #undef HAVE__ULTOA */
#define HAVE_STRTOL 1
#define HAVE_STRTOUL 1
/* #undef HAVE__I64TOA */
/* #undef HAVE__UI64TOA */
#define HAVE_STRTOLL 1
#define HAVE_STRTOULL 1
#define HAVE_STRTOD 1
#define HAVE_ATOI 1
/* #undef HAVE_ATOF */
#define HAVE_STRCMP 1
#define HAVE_STRNCMP 1
/* #undef HAVE__STRICMP */
#define HAVE_STRCASECMP 1
/* #undef HAVE__STRNICMP */
#define HAVE_STRNCASECMP 1
/* #undef HAVE_SSCANF */
#define HAVE_VSSCANF 1
/* #undef HAVE_SNPRINTF */
#define HAVE_VSNPRINTF 1
#define HAVE_M_PI /**/
/* #undef HAVE_ACOS */
/* #undef HAVE_ACOSF */
/* #undef HAVE_ASIN */
/* #undef HAVE_ASINF */
/* #undef HAVE_ATAN */
/* #undef HAVE_ATANF */
/* #undef HAVE_ATAN2 */
/* #undef HAVE_ATAN2F */
/* #undef HAVE_CEIL */
/* #undef HAVE_CEILF */
/* #undef HAVE_COPYSIGN */
/* #undef HAVE_COPYSIGNF */
/* #undef HAVE_COS */
/* #undef HAVE_COSF */
/* #undef HAVE_EXP */
/* #undef HAVE_EXPF */
/* #undef HAVE_FABS */
/* #undef HAVE_FABSF */
/* #undef HAVE_FLOOR */
/* #undef HAVE_FLOORF */
/* #undef HAVE_FMOD */
/* #undef HAVE_FMODF */
/* #undef HAVE_LOG */
/* #undef HAVE_LOGF */
/* #undef HAVE_LOG10 */
/* #undef HAVE_LOG10F */
/* #undef HAVE_POW */
/* #undef HAVE_POWF */
/* #undef HAVE_SCALBN */
/* #undef HAVE_SCALBNF */
/* #undef HAVE_SIN */
/* #undef HAVE_SINF */
/* #undef HAVE_SQRT */
/* #undef HAVE_SQRTF */
/* #undef HAVE_TAN */
/* #undef HAVE_TANF */
#define HAVE_FOPEN64 1
#define HAVE_FSEEKO 1
#define HAVE_FSEEKO64 1
#define HAVE_SIGACTION 1
#define HAVE_SA_SIGACTION 1
/* #undef HAVE_SETJMP */
#define HAVE_NANOSLEEP 1
#define HAVE_SYSCONF 1
/* #undef HAVE_SYSCTLBYNAME */
#define HAVE_CLOCK_GETTIME 1
/* #undef HAVE_GETPAGESIZE */
#define HAVE_MPROTECT 1
#define HAVE_ICONV 1
/* #undef HAVE_PTHREAD_SETNAME_NP */
/* #undef HAVE_PTHREAD_SET_NAME_NP */
/* #undef HAVE_SEM_TIMEDWAIT */
#define HAVE_GETAUXVAL 1
#define HAVE_POLL 1
#define HAVE__EXIT 1

/* Allow disabling of core subsystems */
/* #undef SDL_ATOMIC_DISABLED */
#define SDL_AUDIO_DISABLED 1
#define SDL_CPUINFO_DISABLED 1
/* #undef SDL_EVENTS_DISABLED */
/* #undef SDL_FILE_DISABLED */
#define SDL_JOYSTICK_DISABLED 1
#define SDL_HAPTIC_DISABLED 1
#define SDL_SENSOR_DISABLED 1
/* #undef SDL_LOADSO_DISABLED */
/* #undef SDL_RENDER_DISABLED */
#define SDL_THREADS_DISABLED 1
/* #undef SDL_TIMERS_DISABLED */
/* #undef SDL_VIDEO_DISABLED */
#define SDL_POWER_DISABLED 1
#define SDL_FILESYSTEM_DISABLED 1

#ifndef SDL_AUDIO_DISABLED
#define SDL_AUDIO_DRIVER_DISK 1
#define SDL_AUDIO_DRIVER_DUMMY 1
#define SDL_AUDIO_DRIVER_EMSCRIPTEN 1
#endif

#ifndef SDL_JOYSTICK_DISABLED
#define SDL_JOYSTICK_EMSCRIPTEN 1
#endif

#ifndef SDL_HAPTIC_DISABLED
#define SDL_HAPTIC_DUMMY 1
#endif

/* Enable various sensor drivers */
/* #undef SDL_SENSOR_ANDROID */
#ifndef SDL_SENSOR_DISABLED
#define SDL_SENSOR_DUMMY 1
#endif

/* Enable various shared object loading systems */
#define SDL_LOADSO_DLOPEN 1
/* #undef SDL_LOADSO_DUMMY */
/* #undef SDL_LOADSO_LDG */
/* #undef SDL_LOADSO_WINDOWS */

/* Enable various timer systems */
/* #undef SDL_TIMER_HAIKU */
/* #undef SDL_TIMER_DUMMY */
#define SDL_TIMER_UNIX 1
/* #undef SDL_TIMER_WINDOWS */

#ifndef SDL_VIDEO_DISABLED
// #define SDL_VIDEO_DRIVER_DUMMY 1
#define SDL_VIDEO_DRIVER_EMSCRIPTEN 1
#define SDL_VIDEO_RENDER_OGL_ES2 0
#define SDL_VIDEO_OPENGL_ES2 0
#define SDL_VIDEO_OPENGL_EGL 0
#endif

#ifndef SDL_POWER_DISABLED
#define SDL_POWER_EMSCRIPTEN 1
#endif

#ifndef SDL_FILESYSTEM_DISABLED
#define SDL_FILESYSTEM_EMSCRIPTEN 1
#endif

#endif
