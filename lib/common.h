#ifndef NUC_COMMON_H
#define NUC_COMMON_H

// Common C Libraries
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// max counts
#define UINT8_COUNT (UINT8_MAX + 1)
#define UINT16_COUNT (UINT16_MAX + 1)
#define UINT32_COUNT (UINT32_MAX + 1)

// stack allowances
#define FRAMES_MAX 1024
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

// debug defines
#define NUC_DEBUG_BYTECODE
// #define NUC_DEBUG_TRACE
// #define NUC_DEBUG_STACK_TRACE
// #define NUC_DEBUG_OP_TRACE
// #define NUC_DEBUG_GC

// native library definitions

#endif