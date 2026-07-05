

#ifndef ALBERT_SRC_MAIN_H
#define ALBERT_SRC_MAIN_H

#include <cstdint>
#include <cstddef>

/** 32-bit address type used for ROM offsets and memory indices. */
using Addr = uint32_t;

/** Total emulated ROM memory size in bytes. */
constexpr Addr k8MB = 8 * 1024 * 1024; // 8 MB ROM size
/** Byte offset where payload starts in AIF image files. */
constexpr size_t kAifFileDataStart = 0x00000080; // Offset by header size
/** Total AIF payload size loaded into ROM memory (RO + RW). */
constexpr Addr kAifDataSize = 0x0071A95C + 0x000052F0; // RO size + R/W size, 0x71FCCC
/** Start address of symbol list in AIF file. */
constexpr size_t kAifSymList  = 0x0071fccc;
/** Start address of symbol name table in AIF file. */
constexpr size_t kAifSymTable = 0x00786d68;

/** Start offset of REx data in the merged ROM image. */
constexpr Addr kRexDataOffset = kAifDataSize; // Offset in binary ROM image
/** Size of the REx extension block in bytes. */
constexpr Addr kRexDataSize = 0x000CE3FC; // REx size
/** End offset (exclusive) of the REx data block in ROM image space. */
constexpr Addr kRexDataEnd = kRexDataOffset + kRexDataSize; // 0x7EE0C8

#endif // ALBERT_SRC_MAIN_H
