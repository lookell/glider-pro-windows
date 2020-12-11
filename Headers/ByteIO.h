#ifndef BYTEIO_H_
#define BYTEIO_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "WinAPI.h"

// The `byteio` structure is used to perform sequential input or output,
// using a Windows HANDLE or an in-memory buffer.
typedef struct byteio {
	int (*fn_read)(struct byteio *stream, void *buffer, size_t size);
	int (*fn_write)(struct byteio *stream, const void *buffer, size_t size);
	int (*fn_seek)(struct byteio *stream, int64_t offset, int origin, int64_t *newPos);
	int (*fn_close)(struct byteio *stream);
	void *priv;
} byteio;

// All functions in this interface that return an `int` return a nonzero
// value on success, and zero on failure.

// Initialize a `byteio` structure to read bytes from a Windows file HANDLE.
int byteio_init_handle_reader(byteio *stream, HANDLE hFile);

// Initialize a `byteio` structure to write bytes into a Windows file HANDLE.
int byteio_init_handle_writer(byteio *stream, HANDLE hFile);

// Initialize a `byteio` structure to read bytes from a memory buffer.
int byteio_init_memory_reader(byteio *stream, const void *buffer, size_t size);

// Initialize a `byteio` structure to write bytes into a memory buffer.
int byteio_init_memory_writer(byteio *stream, size_t initial_capacity);

// Close an initialized `byteio` structure. The `byteio` structure must have
// been previously initialized by one of the `byteio_init_XXX` functions, and
// must not have been closed before this call.
//
// If this is called on a memory writer, the buffer will be lost. Call the
// `byteio_close_and_get_buffer` function instead to close the memory writer
// and retrieve the buffer that holds your data.
int byteio_close(byteio *stream);

// Close a memory writer and retrieve its resulting buffer. This buffer should
// be freed with the standard `free` function when you are finished with it.
// If this function is called on a `byteio` structure that isn't a memory
// writer, then the function will fail without doing anything.
int byteio_close_and_get_buffer(byteio *stream, void **bufferPtr, size_t *bufferLen);

// Read exactly `size` bytes into the array `buffer`. If the `buffer` parameter
// is NULL, then the bytes are simply skipped over in the stream.
int byteio_read(byteio *stream, void *buffer, size_t size);

// Write exactly `size` bytes from the array `buffer`.
int byteio_write(byteio *stream, const void *buffer, size_t size);

// Seek by `offset` bytes from the specified `origin` position.
// If `newPos` is not NULL, then the new position is returned to the caller.
// The `origin` parameter values are the same as for the standard `fseek`
// function (i.e., SEEK_SET, SEEK_CUR, and SEEK_END).
int byteio_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);

// Retrieve the current position in the stream. This is equivalent to
// calling `byteio_seek(stream, 0, SEEK_CUR, curPos)`.
int byteio_tell(byteio *stream, int64_t *curPos);

// Read a big-endian, 8-bit unsigned integer.
int byteio_read_be_u8(byteio *stream, uint8_t *num);

// Read a little-endian, 8-bit unsigned integer.
int byteio_read_le_u8(byteio *stream, uint8_t *num);

// Read a big-endian, 8-bit signed integer.
int byteio_read_be_i8(byteio *stream, int8_t *num);

// Read a little-endian, 8-bit signed integer.
int byteio_read_le_i8(byteio *stream, int8_t *num);

// Read a big-endian, 16-bit unsigned integer.
int byteio_read_be_u16(byteio *stream, uint16_t *num);

// Read a little-endian, 16-bit unsigned integer.
int byteio_read_le_u16(byteio *stream, uint16_t *num);

// Read a big-endian, 16-bit signed integer.
int byteio_read_be_i16(byteio *stream, int16_t *num);

// Read a little-endian, 16-bit signed integer.
int byteio_read_le_i16(byteio *stream, int16_t *num);

// Read a big-endian, 32-bit unsigned integer.
int byteio_read_be_u32(byteio *stream, uint32_t *num);

// Read a little-endian, 32-bit unsigned integer.
int byteio_read_le_u32(byteio *stream, uint32_t *num);

// Read a big-endian, 32-bit signed integer.
int byteio_read_be_i32(byteio *stream, int32_t *num);

// Read a little-endian, 32-bit signed integer.
int byteio_read_le_i32(byteio *stream, int32_t *num);

// Write a big-endian, 8-bit unsigned integer.
int byteio_write_be_u8(byteio *stream, uint8_t num);

// Write a little-endian, 8-bit unsigned integer.
int byteio_write_le_u8(byteio *stream, uint8_t num);

// Write a big-endian, 8-bit signed integer.
int byteio_write_be_i8(byteio *stream, int8_t num);

// Write a little-endian, 8-bit signed integer.
int byteio_write_le_i8(byteio *stream, int8_t num);

// Write a big-endian, 16-bit unsigned integer.
int byteio_write_be_u16(byteio *stream, uint16_t num);

// Write a little-endian, 16-bit unsigned integer.
int byteio_write_le_u16(byteio *stream, uint16_t num);

// Write a big-endian, 16-bit signed integer.
int byteio_write_be_i16(byteio *stream, int16_t num);

// Write a little-endian, 16-bit signed integer.
int byteio_write_le_i16(byteio *stream, int16_t num);

// Write a big-endian, 32-bit unsigned integer.
int byteio_write_be_u32(byteio *stream, uint32_t num);

// Write a little-endian, 32-bit unsigned integer.
int byteio_write_le_u32(byteio *stream, uint32_t num);

// Write a big-endian, 32-bit signed integer.
int byteio_write_be_i32(byteio *stream, int32_t num);

// Write a little-endian, 32-bit signed integer.
int byteio_write_le_i32(byteio *stream, int32_t num);

#endif
