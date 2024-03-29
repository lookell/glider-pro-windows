#ifndef BYTEIO_H_
#define BYTEIO_H_

#include <stddef.h>
#include <stdint.h>

#include "WinAPI.h"

#define BYTEIO_SEEK_SET  0
#define BYTEIO_SEEK_CUR  1
#define BYTEIO_SEEK_END  2

// The `byteio` structure is used to perform sequential input or output,
// using a Windows HANDLE or an in-memory buffer.
typedef struct byteio byteio;

// Initialize a `byteio` structure to read bytes from a Windows file HANDLE.
byteio *byteio_init_handle_reader(HANDLE fileHandle);

// Initialize a `byteio` structure to write bytes into a Windows file HANDLE.
byteio *byteio_init_handle_writer(HANDLE fileHandle);

// Initialize a `byteio` structure to read bytes from a memory buffer.
byteio *byteio_init_memory_reader(const void *buffer, size_t size);

// Initialize a `byteio` structure to write bytes into a memory buffer.
byteio *byteio_init_memory_writer(size_t initial_capacity);

// Close an initialized `byteio` structure.
//
// If this is called on a memory writer, the buffer will be lost. Call the
// `byteio_close_and_get_buffer` function instead to close the memory writer
// and retrieve the buffer that holds your data.
HRESULT byteio_close(byteio *stream);

// Close a memory writer and retrieve its resulting buffer. This buffer should
// be freed with the standard `free` function when you are finished with it.
// If this function is called on a `byteio` structure that isn't a memory
// writer, then the function will call byteio_close.
HRESULT byteio_close_and_get_buffer(byteio *stream, void **bufferPtr, size_t *bufferLen);

// Read exactly `size` bytes into the array `buffer`. If the `buffer` parameter
// is NULL, then the bytes are simply skipped over in the stream.
HRESULT byteio_read(byteio *stream, void *buffer, size_t size);

// Write exactly `size` bytes from the array `buffer`.
HRESULT byteio_write(byteio *stream, const void *buffer, size_t size);

// Seek by `offset` bytes from the specified `origin` position.
// If `newPos` is not NULL, then the new position is returned to the caller.
// The `origin` parameter values are named similarly to those for the standard `fseek`
// function (i.e., BYTEIO_SEEK_SET, BYTEIO_SEEK_CUR, and BYTEIO_SEEK_END).
HRESULT byteio_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);

// Retrieve the current position in the stream. This is equivalent to
// calling `byteio_seek(stream, 0, BYTEIO_SEEK_CUR, curPos)`.
HRESULT byteio_tell(byteio *stream, int64_t *curPos);

// *** Fixed-size integer reading routines ***

HRESULT byteio_read_be_u8(byteio *stream, uint8_t *num);
HRESULT byteio_read_le_u8(byteio *stream, uint8_t *num);

HRESULT byteio_read_be_i8(byteio *stream, int8_t *num);
HRESULT byteio_read_le_i8(byteio *stream, int8_t *num);

HRESULT byteio_read_be_u16(byteio *stream, uint16_t *num);
HRESULT byteio_read_le_u16(byteio *stream, uint16_t *num);

HRESULT byteio_read_be_i16(byteio *stream, int16_t *num);
HRESULT byteio_read_le_i16(byteio *stream, int16_t *num);

HRESULT byteio_read_be_u32(byteio *stream, uint32_t *num);
HRESULT byteio_read_le_u32(byteio *stream, uint32_t *num);

HRESULT byteio_read_be_i32(byteio *stream, int32_t *num);
HRESULT byteio_read_le_i32(byteio *stream, int32_t *num);

// *** Fixed-size integer writing routines ***

HRESULT byteio_write_be_u8(byteio *stream, uint8_t num);
HRESULT byteio_write_le_u8(byteio *stream, uint8_t num);

HRESULT byteio_write_be_i8(byteio *stream, int8_t num);
HRESULT byteio_write_le_i8(byteio *stream, int8_t num);

HRESULT byteio_write_be_u16(byteio *stream, uint16_t num);
HRESULT byteio_write_le_u16(byteio *stream, uint16_t num);

HRESULT byteio_write_be_i16(byteio *stream, int16_t num);
HRESULT byteio_write_le_i16(byteio *stream, int16_t num);

HRESULT byteio_write_be_u32(byteio *stream, uint32_t num);
HRESULT byteio_write_le_u32(byteio *stream, uint32_t num);

HRESULT byteio_write_be_i32(byteio *stream, int32_t num);
HRESULT byteio_write_le_i32(byteio *stream, int32_t num);

#endif
