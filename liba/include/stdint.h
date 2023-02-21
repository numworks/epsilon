#ifndef LIBA_STDINT_H
#define LIBA_STDINT_H

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef signed long long int64_t;

typedef unsigned int uintptr_t;
typedef signed int intptr_t;

typedef uint32_t uint_fast8_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

typedef int32_t int_fast8_t;
typedef int32_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;

typedef uint8_t uint_least8_t;

#define INT8_MAX 0x7f
#define INT8_MIN (-INT8_MAX - 1)

#define UINT8_MAX 0xff
#define UINT16_MAX 0xffff
#define UINT32_MAX 0xffffffff
#define UINT64_MAX 0xffffffffffffffff

#define INT16_MAX 0x7fff
#define INT16_MIN (-INT16_MAX - 1)

#define UINT64_C(c) c##ULL
#define INT64_C(c) c##LL

#define UINT32_C(c) c##U

#endif
