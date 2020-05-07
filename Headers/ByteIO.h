#ifndef BYTEIO_H_
#define BYTEIO_H_

#include <stddef.h>
#include <stdint.h>

// The `byteio` structure is used to perform sequential input or output,
// using a Windows HANDLE or an in-memory buffer.
typedef struct byteio {
	int (*fn_read)(struct byteio *stream, void *buffer, size_t size);
	int (*fn_write)(struct byteio *stream, const void *buffer, size_t size);
	int (*fn_close)(struct byteio *stream);
	void *priv;
} byteio;

// All functions in this interface that return an `int` return a nonzero
// value on success, and zero on failure.

// Initialize a `byteio` structure to read bytes from a Windows file HANDLE.
int byteio_init_handle_reader(byteio *stream, void *hFile);

// Initialize a `byteio` structure to write bytes into a Windows file HANDLE.
int byteio_init_handle_writer(byteio *stream, void *hFile);

// Initialize a `byteio` structure to read bytes from a memory buffer.
int byteio_init_memory_reader(byteio *stream, const void *buffer, size_t size);

// Initialize a `byteio` structure to write bytes into a memory buffer.
int byteio_init_memory_writer(byteio *stream, void *buffer, size_t size);

// Close an initialized `byteio` structure. The `byteio` structure must have
// been previously initialized by one of the `byteio_init_XXX` functions, and
// must not have been closed before this call.
int byteio_close(byteio *stream);

// Read exactly `size` bytes into the array `buffer`. If the `buffer` parameter
// is NULL, then the bytes are simply skipped over in the stream.
int byteio_read(byteio *stream, void *buffer, size_t size);

// Skip past `size` bytes in a `byteio` reader.
int byteio_skip(byteio *stream, size_t size);

// Write exactly `size` bytes from the array `buffer`.
int byteio_write(byteio *stream, const void *buffer, size_t size);

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
