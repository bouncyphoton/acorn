#ifndef ACORN_TYPES_H
#define ACORN_TYPES_H

#include <cstdint>

typedef uint8_t u8;
typedef int8_t s8;

typedef uint16_t u16;
typedef int16_t s16;

typedef uint32_t u32;
typedef int32_t s32;

typedef uint64_t u64;
typedef int64_t s64;

typedef float f32;
typedef double f64;
static_assert(sizeof(f32) == 4, "f32 is incorrect size");
static_assert(sizeof(f64) == 8, "f64 is incorrect size");

#endif //ACORN_TYPES_H
