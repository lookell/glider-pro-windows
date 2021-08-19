#include "ByteIO.h"

#include "WinAPI.h"

#include <limits.h>
#include <stdlib.h>

#define BYTEIO_DEFAULT_BUFFER_SIZE (8 * 1024)

typedef int ASSERT_CHAR_BIT_IS_EIGHT[(CHAR_BIT == 8) ? 1 : -1];

struct byteio {
	HRESULT (*fn_read)(byteio *stream, void *buffer, size_t size);
	HRESULT (*fn_write)(byteio *stream, const void *buffer, size_t size);
	HRESULT (*fn_seek)(byteio *stream, int64_t offset, int origin, int64_t *newPos);
	HRESULT (*fn_close)(byteio *stream);
};

//--------------------------------------------------------------

typedef struct handle_reader {
	byteio base;
	HANDLE hFile;
	size_t size;
	const unsigned char *bufptr;
	unsigned char buffer[BYTEIO_DEFAULT_BUFFER_SIZE];
} handle_reader;

static HRESULT handle_reader_read(byteio *stream, void *buffer, size_t size);
static HRESULT handle_reader_write(byteio *stream, const void *buffer, size_t size);
static HRESULT handle_reader_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);
static HRESULT handle_reader_close(byteio *stream);

typedef struct handle_writer {
	byteio base;
	HANDLE hFile;
	size_t size;
	unsigned char *bufptr;
	unsigned char buffer[BYTEIO_DEFAULT_BUFFER_SIZE];
} handle_writer;

static HRESULT handle_writer_read(byteio *stream, void *buffer, size_t size);
static HRESULT handle_writer_write(byteio *stream, const void *buffer, size_t size);
static HRESULT handle_writer_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);
static HRESULT handle_writer_close(byteio *stream);

typedef struct memory_reader {
	byteio base;
	ptrdiff_t size;
	ptrdiff_t pos;
	const unsigned char *buffer;
} memory_reader;

static HRESULT memory_reader_read(byteio *stream, void *buffer, size_t size);
static HRESULT memory_reader_write(byteio *stream, const void *buffer, size_t size);
static HRESULT memory_reader_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);
static HRESULT memory_reader_close(byteio *stream);

typedef struct memory_writer {
	byteio base;
	ptrdiff_t size;
	ptrdiff_t pos;
	ptrdiff_t capacity;
	unsigned char *buffer;
} memory_writer;

static HRESULT memory_writer_read(byteio *stream, void *buffer, size_t size);
static HRESULT memory_writer_write(byteio *stream, const void *buffer, size_t size);
static HRESULT memory_writer_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);
static HRESULT memory_writer_close(byteio *stream);

//--------------------------------------------------------------

HRESULT byteio_read(byteio *stream, void *buffer, size_t size)
{
	if (stream == NULL)
	{
		return E_INVALIDARG;
	}
	return stream->fn_read(stream, buffer, size);
}

HRESULT byteio_write(byteio *stream, const void *buffer, size_t size)
{
	if (stream == NULL)
	{
		return E_INVALIDARG;
	}
	return stream->fn_write(stream, buffer, size);
}

HRESULT byteio_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	if (stream == NULL)
	{
		return E_INVALIDARG;
	}
	return stream->fn_seek(stream, offset, origin, newPos);
}

HRESULT byteio_tell(byteio *stream, int64_t *curPos)
{
	if (stream == NULL)
	{
		return E_INVALIDARG;
	}
	return stream->fn_seek(stream, 0, BYTEIO_SEEK_CUR, curPos);
}

HRESULT byteio_close(byteio *stream)
{
	if (stream == NULL)
	{
		return E_INVALIDARG;
	}
	return stream->fn_close(stream);
}

//--------------------------------------------------------------

HRESULT byteio_read_be_u8(byteio *stream, uint8_t *num)
{
	unsigned char buffer[sizeof(uint8_t)];
	HRESULT hr;

	hr = byteio_read(stream, buffer, sizeof(buffer));
	if (SUCCEEDED(hr) && num != NULL)
	{
		*num = ((uint8_t)buffer[0]);
	}
	return hr;
}

HRESULT byteio_read_le_u8(byteio *stream, uint8_t *num)
{
	unsigned char buffer[sizeof(uint8_t)];
	HRESULT hr;

	hr = byteio_read(stream, buffer, sizeof(buffer));
	if (SUCCEEDED(hr) && num != NULL)
	{
		*num = ((uint8_t)buffer[0]);
	}
	return hr;
}

HRESULT byteio_read_be_i8(byteio *stream, int8_t *num)
{
	return byteio_read_be_u8(stream, (uint8_t *)num);
}

HRESULT byteio_read_le_i8(byteio *stream, int8_t *num)
{
	return byteio_read_le_u8(stream, (uint8_t *)num);
}

HRESULT byteio_read_be_u16(byteio *stream, uint16_t *num)
{
	unsigned char buffer[sizeof(uint16_t)];
	HRESULT hr;

	hr = byteio_read(stream, buffer, sizeof(buffer));
	if (SUCCEEDED(hr) && num != NULL)
	{
		*num = ((uint16_t)buffer[0] << 8)
			| ((uint16_t)buffer[1]);
	}
	return hr;
}

HRESULT byteio_read_le_u16(byteio *stream, uint16_t *num)
{
	unsigned char buffer[sizeof(uint16_t)];
	HRESULT hr;

	hr = byteio_read(stream, buffer, sizeof(buffer));
	if (SUCCEEDED(hr) && num != NULL)
	{
		*num = ((uint16_t)buffer[0])
			| ((uint16_t)buffer[1] << 8);
	}
	return hr;
}

HRESULT byteio_read_be_i16(byteio *stream, int16_t *num)
{
	return byteio_read_be_u16(stream, (uint16_t *)num);
}

HRESULT byteio_read_le_i16(byteio *stream, int16_t *num)
{
	return byteio_read_le_u16(stream, (uint16_t *)num);
}

HRESULT byteio_read_be_u32(byteio *stream, uint32_t *num)
{
	unsigned char buffer[sizeof(uint32_t)];
	HRESULT hr;

	hr = byteio_read(stream, buffer, sizeof(buffer));
	if (SUCCEEDED(hr) && num != NULL)
	{
		*num = ((uint32_t)buffer[0] << 24)
			| ((uint32_t)buffer[1] << 16)
			| ((uint32_t)buffer[2] << 8)
			| ((uint32_t)buffer[3]);
	}
	return hr;
}

HRESULT byteio_read_le_u32(byteio *stream, uint32_t *num)
{
	unsigned char buffer[sizeof(uint32_t)];
	HRESULT hr;

	hr = byteio_read(stream, buffer, sizeof(buffer));
	if (SUCCEEDED(hr) && num != NULL)
	{
		*num = ((uint32_t)buffer[0])
			| ((uint32_t)buffer[1] << 8)
			| ((uint32_t)buffer[2] << 16)
			| ((uint32_t)buffer[3] << 24);
	}
	return hr;
}

HRESULT byteio_read_be_i32(byteio *stream, int32_t *num)
{
	return byteio_read_be_u32(stream, (uint32_t *)num);
}

HRESULT byteio_read_le_i32(byteio *stream, int32_t *num)
{
	return byteio_read_le_u32(stream, (uint32_t *)num);
}

//--------------------------------------------------------------

HRESULT byteio_write_be_u8(byteio *stream, uint8_t num)
{
	unsigned char buffer[sizeof(uint8_t)];

	buffer[0] = (unsigned char)(num & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

HRESULT byteio_write_le_u8(byteio *stream, uint8_t num)
{
	unsigned char buffer[sizeof(uint8_t)];

	buffer[0] = (unsigned char)(num & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

HRESULT byteio_write_be_i8(byteio *stream, int8_t num)
{
	return byteio_write_be_u8(stream, (uint8_t)num);
}

HRESULT byteio_write_le_i8(byteio *stream, int8_t num)
{
	return byteio_write_le_u8(stream, (uint8_t)num);
}

HRESULT byteio_write_be_u16(byteio *stream, uint16_t num)
{
	unsigned char buffer[sizeof(uint16_t)];

	buffer[0] = (unsigned char)((num >> 8) & 0xff);
	buffer[1] = (unsigned char)(num & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

HRESULT byteio_write_le_u16(byteio *stream, uint16_t num)
{
	unsigned char buffer[sizeof(uint16_t)];

	buffer[0] = (unsigned char)(num & 0xff);
	buffer[1] = (unsigned char)((num >> 8) & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

HRESULT byteio_write_be_i16(byteio *stream, int16_t num)
{
	return byteio_write_be_u16(stream, (uint16_t)num);
}

HRESULT byteio_write_le_i16(byteio *stream, int16_t num)
{
	return byteio_write_le_u16(stream, (uint16_t)num);
}

HRESULT byteio_write_be_u32(byteio *stream, uint32_t num)
{
	unsigned char buffer[sizeof(uint32_t)];

	buffer[0] = (unsigned char)((num >> 24) & 0xff);
	buffer[1] = (unsigned char)((num >> 16) & 0xff);
	buffer[2] = (unsigned char)((num >> 8) & 0xff);
	buffer[3] = (unsigned char)(num & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

HRESULT byteio_write_le_u32(byteio *stream, uint32_t num)
{
	unsigned char buffer[sizeof(uint32_t)];

	buffer[0] = (unsigned char)(num & 0xff);
	buffer[1] = (unsigned char)((num >> 8) & 0xff);
	buffer[2] = (unsigned char)((num >> 16) & 0xff);
	buffer[3] = (unsigned char)((num >> 24) & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

HRESULT byteio_write_be_i32(byteio *stream, int32_t num)
{
	return byteio_write_be_u32(stream, (uint32_t)num);
}

HRESULT byteio_write_le_i32(byteio *stream, int32_t num)
{
	return byteio_write_le_u32(stream, (uint32_t)num);
}

//--------------------------------------------------------------

static HRESULT handle_reader_read(byteio *stream, void *buffer, size_t size)
{
	handle_reader *self = (handle_reader *)stream;
	unsigned char *outptr = (unsigned char *)buffer;
	size_t readsize;
	DWORD numRead;
	DWORD lastError;

	if (self == NULL)
	{
		return E_INVALIDARG;
	}
	while (size != 0)
	{
		// Fill up the buffer, if needed
		if (self->size == 0)
		{
			self->bufptr = &self->buffer[0];
			SetLastError(0);
			if (!ReadFile(self->hFile, self->buffer, sizeof(self->buffer), &numRead, NULL))
			{
				lastError = GetLastError();
				if (lastError == 0)
				{
					return E_FAIL;
				}
				return HRESULT_FROM_WIN32(lastError);
			}
			self->size = numRead;
		}
		// Read as much as we can from the buffer
		readsize = (self->size <= size) ? self->size : size;
		if (readsize == 0)
		{
			return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
		}
		if (outptr != NULL)
		{
			memcpy(outptr, self->bufptr, readsize);
			outptr += readsize;
		}
		size -= readsize;
		self->bufptr += readsize;
		self->size -= readsize;
	}
	return S_OK;
}

static HRESULT handle_reader_write(byteio *stream, const void *buffer, size_t size)
{
	(void)stream;
	(void)buffer;
	(void)size;

	return E_NOTIMPL;
}

static HRESULT handle_reader_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	handle_reader *self = (handle_reader *)stream;
	LARGE_INTEGER fileOffset;
	LARGE_INTEGER newFilePointer;
	DWORD moveMethod;
	DWORD lastError;

	if (self == NULL)
	{
		return E_INVALIDARG;
	}
	// "Unread" any buffered data, and seek backwards to the correct position
	fileOffset.QuadPart = -((LONGLONG)self->size);
	self->bufptr = &self->buffer[0];
	self->size = 0;
	SetLastError(0);
	if (!SetFilePointerEx(self->hFile, fileOffset, NULL, FILE_CURRENT))
	{
		lastError = GetLastError();
		if (lastError == 0)
		{
			return E_FAIL;
		}
		return HRESULT_FROM_WIN32(lastError);
	}
	// Perform the actual seek operation
	fileOffset.QuadPart = offset;
	switch (origin)
	{
	case BYTEIO_SEEK_SET:
		moveMethod = FILE_BEGIN;
		break;

	case BYTEIO_SEEK_CUR:
		moveMethod = FILE_CURRENT;
		break;

	case BYTEIO_SEEK_END:
		moveMethod = FILE_END;
		break;

	default:
		return E_INVALIDARG;
	}
	SetLastError(0);
	if (!SetFilePointerEx(self->hFile, fileOffset, &newFilePointer, moveMethod))
	{
		lastError = GetLastError();
		if (lastError == 0)
		{
			return E_FAIL;
		}
		return HRESULT_FROM_WIN32(lastError);
	}
	// Return the new position to the caller
	if (newPos != NULL)
	{
		*newPos = newFilePointer.QuadPart;
	}
	return S_OK;
}

static HRESULT handle_reader_close(byteio *stream)
{
	handle_reader *self = (handle_reader *)stream;
	LARGE_INTEGER fileOffset;
	HANDLE fileHandle;
	DWORD lastError;

	fileOffset.QuadPart = -((LONGLONG)self->size);
	fileHandle = self->hFile;
	free(self);
	SetLastError(0);
	if (!SetFilePointerEx(fileHandle, fileOffset, NULL, FILE_CURRENT))
	{
		lastError = GetLastError();
		if (lastError != 0)
		{
			return E_FAIL;
		}
		return HRESULT_FROM_WIN32(lastError);
	}
	return S_OK;
}

byteio *byteio_init_handle_reader(HANDLE fileHandle)
{
	handle_reader *self;

	if (fileHandle == NULL || fileHandle == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	self = (handle_reader *)calloc(1, sizeof(*self));
	if (self == NULL)
	{
		return NULL;
	}
	self->base.fn_read = handle_reader_read;
	self->base.fn_write = handle_reader_write;
	self->base.fn_seek = handle_reader_seek;
	self->base.fn_close = handle_reader_close;
	self->hFile = fileHandle;
	self->bufptr = &self->buffer[0];
	self->size = 0;
	return &self->base;
}

//--------------------------------------------------------------

static HRESULT handle_writer_read(byteio *stream, void *buffer, size_t size)
{
	(void)stream;
	(void)buffer;
	(void)size;

	return E_NOTIMPL;
}

static HRESULT handle_writer_write(byteio *stream, const void *buffer, size_t size)
{
	handle_writer *self = (handle_writer *)stream;
	const unsigned char *inptr = (const unsigned char *)buffer;
	size_t writesize;
	DWORD numWritten;
	DWORD lastError;

	if (self == NULL || buffer == NULL)
	{
		return E_INVALIDARG;
	}
	while (size != 0)
	{
		// Empty the buffer, if needed
		if (self->size == 0)
		{
			self->bufptr = &self->buffer[0];
			self->size = sizeof(self->buffer);
			SetLastError(0);
			if (!WriteFile(self->hFile, self->buffer, sizeof(self->buffer), &numWritten, NULL))
			{
				lastError = GetLastError();
				if (lastError == 0)
				{
					return E_FAIL;
				}
				return HRESULT_FROM_WIN32(lastError);
			}
			if (numWritten != sizeof(self->buffer))
			{
				return E_FAIL;
			}
		}
		// Write as much as we can into the buffer
		writesize = (self->size <= size) ? self->size : size;
		memcpy(self->bufptr, inptr, writesize);
		inptr += writesize;
		size -= writesize;
		self->bufptr += writesize;
		self->size -= writesize;
	}
	return S_OK;
}

static HRESULT handle_writer_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	handle_writer *self = (handle_writer *)stream;
	DWORD numToWrite;
	DWORD numWritten;
	LARGE_INTEGER fileOffset;
	LARGE_INTEGER newFilePointer;
	DWORD moveMethod;
	DWORD lastError;

	if (self == NULL)
	{
		return E_INVALIDARG;
	}
	// Write out any buffered data, so that we start in the correct position
	numToWrite = (DWORD)(sizeof(self->buffer) - self->size);
	self->bufptr = &self->buffer[0];
	self->size = sizeof(self->buffer);
	SetLastError(0);
	if (!WriteFile(self->hFile, self->buffer, numToWrite, &numWritten, NULL))
	{
		lastError = GetLastError();
		if (lastError == 0)
		{
			return E_FAIL;
		}
		return HRESULT_FROM_WIN32(lastError);
	}
	if (numWritten != numToWrite)
	{
		return E_FAIL;
	}
	// Perform the actual seek operation
	fileOffset.QuadPart = offset;
	switch (origin)
	{
	case BYTEIO_SEEK_SET:
		moveMethod = FILE_BEGIN;
		break;
	
	case BYTEIO_SEEK_CUR:
		moveMethod = FILE_CURRENT;
		break;
	
	case BYTEIO_SEEK_END:
		moveMethod = FILE_END;
		break;
	
	default:
		return E_INVALIDARG;
	}
	SetLastError(0);
	if (!SetFilePointerEx(self->hFile, fileOffset, &newFilePointer, moveMethod))
	{
		lastError = GetLastError();
		if (lastError == 0)
		{
			return E_FAIL;
		}
		return HRESULT_FROM_WIN32(lastError);
	}
	// Return the new position to the caller
	if (newPos != NULL)
	{
		*newPos = newFilePointer.QuadPart;
	}
	return S_OK;
}

static HRESULT handle_writer_close(byteio *stream)
{
	handle_writer *self = (handle_writer *)stream;
	BOOL succeeded;
	DWORD numWritten;
	DWORD numToWrite;
	DWORD lastError;

	numToWrite = (DWORD)(sizeof(self->buffer) - self->size);
	SetLastError(0);
	succeeded = WriteFile(self->hFile, self->buffer, numToWrite, &numWritten, NULL);
	lastError = GetLastError();
	free(self);
	if (!succeeded)
	{
		if (lastError == 0)
		{
			return E_FAIL;
		}
		return HRESULT_FROM_WIN32(lastError);
	}
	if (numWritten != numToWrite)
	{
		return E_FAIL;
	}
	return S_OK;
}

byteio *byteio_init_handle_writer(HANDLE fileHandle)
{
	handle_writer *self;

	if (fileHandle == NULL || fileHandle == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	self = (handle_writer *)calloc(1, sizeof(*self));
	if (self == NULL)
	{
		return NULL;
	}
	self->base.fn_read = handle_writer_read;
	self->base.fn_write = handle_writer_write;
	self->base.fn_seek = handle_writer_seek;
	self->base.fn_close = handle_writer_close;
	self->hFile = fileHandle;
	self->bufptr = &self->buffer[0];
	self->size = sizeof(self->buffer);
	return &self->base;
}

//--------------------------------------------------------------

static HRESULT memory_reader_read(byteio *stream, void *buffer, size_t size)
{
	memory_reader *self = (memory_reader *)stream;
	size_t numBytesAvailable;

	if (self == NULL)
	{
		return E_INVALIDARG;
	}
	if (self->pos >= self->size)
	{
		return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
	}
	numBytesAvailable = (size_t)self->size - (size_t)self->pos;
	if (numBytesAvailable < size)
	{
		self->pos = self->size;
		return HRESULT_FROM_WIN32(ERROR_HANDLE_EOF);
	}
	if (buffer != NULL)
	{
		memcpy(buffer, &self->buffer[self->pos], size);
	}
	self->pos += size;
	return S_OK;
}

static HRESULT memory_reader_write(byteio *stream, const void *buffer, size_t size)
{
	(void)stream;
	(void)buffer;
	(void)size;

	return E_NOTIMPL;
}

static HRESULT memory_reader_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	memory_reader *self = (memory_reader *)stream;
	ptrdiff_t memoryPos;
	ptrdiff_t memoryOffset;

	if (self == NULL || offset < PTRDIFF_MIN || offset > PTRDIFF_MAX)
	{
		return E_INVALIDARG;
	}
	memoryOffset = (ptrdiff_t)offset;
	switch (origin)
	{
	case BYTEIO_SEEK_SET:
		memoryPos = memoryOffset;
		break;

	case BYTEIO_SEEK_CUR:
		memoryPos = self->pos + memoryOffset;
		break;

	case BYTEIO_SEEK_END:
		memoryPos = self->size + memoryOffset;
		break;

	default:
		return E_INVALIDARG;
	}
	if (memoryPos < 0)
	{
		return HRESULT_FROM_WIN32(ERROR_NEGATIVE_SEEK);
	}
	self->pos = memoryPos;
	if (newPos)
	{
		*newPos = (int64_t)self->pos;
	}
	return S_OK;
}

static HRESULT memory_reader_close(byteio *stream)
{
	memory_reader *self = (memory_reader *)stream;

	free(self);
	return S_OK;
}

byteio *byteio_init_memory_reader(const void *buffer, size_t size)
{
	memory_reader *self;

	if (buffer == NULL || size > (size_t)PTRDIFF_MAX)
	{
		return NULL;
	}
	self = (memory_reader *)calloc(1, sizeof(*self));
	if (self == NULL)
	{
		return NULL;
	}
	self->base.fn_read = memory_reader_read;
	self->base.fn_write = memory_reader_write;
	self->base.fn_seek = memory_reader_seek;
	self->base.fn_close = memory_reader_close;
	self->size = (ptrdiff_t)size;
	self->pos = 0;
	self->buffer = (const unsigned char *)buffer;
	return &self->base;
}

//--------------------------------------------------------------

static HRESULT memory_writer_read(byteio *stream, void *buffer, size_t size)
{
	(void)stream;
	(void)buffer;
	(void)size;

	return E_NOTIMPL;
}

static HRESULT memory_writer_write(byteio *stream, const void *buffer, size_t size)
{
	memory_writer *self = (memory_writer *)stream;
	unsigned char *new_buffer;
	ptrdiff_t write_size;
	ptrdiff_t new_position;
	ptrdiff_t new_capacity;

	if (self == NULL || self->buffer == NULL || buffer == NULL)
	{
		return E_INVALIDARG;
	}
	if (size > (size_t)PTRDIFF_MAX)
	{
		return E_INVALIDARG;
	}
	write_size = (ptrdiff_t)size;
	new_position = self->pos + write_size;
	// Does the buffer size need to grow?
	if (self->capacity < new_position)
	{
		new_capacity = self->capacity;
		while (new_capacity < new_position)
		{
			new_capacity *= 2;
		}
		new_buffer = (unsigned char *)realloc(self->buffer, new_capacity);
		if (new_buffer == NULL)
		{
			// Try again with just enough memory to complete this write
			new_capacity = new_position;
			new_buffer = (unsigned char *)realloc(self->buffer, new_capacity);
			if (new_buffer == NULL)
			{
				return E_OUTOFMEMORY;
			}
		}
		self->buffer = new_buffer;
		self->capacity = new_capacity;
	}
	// Write the incoming buffer at the current position
	memcpy(&self->buffer[self->pos], buffer, write_size);
	self->pos = new_position;
	// Increase the buffer's total size appropriately
	if (self->size < new_position)
	{
		self->size = new_position;
	}
	return S_OK;
}

static HRESULT memory_writer_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	memory_writer *self = (memory_writer *)stream;
	ptrdiff_t memoryPos;
	ptrdiff_t memoryOffset;

	if (self == NULL || offset < PTRDIFF_MIN || offset > PTRDIFF_MAX)
	{
		return E_INVALIDARG;
	}
	memoryOffset = (ptrdiff_t)offset;
	switch (origin)
	{
	case BYTEIO_SEEK_SET:
		memoryPos = memoryOffset;
		break;

	case BYTEIO_SEEK_CUR:
		memoryPos = self->pos + memoryOffset;
		break;

	case BYTEIO_SEEK_END:
		memoryPos = self->size + memoryOffset;
		break;

	default:
		return E_INVALIDARG;
	}
	if (memoryPos < 0)
	{
		return HRESULT_FROM_WIN32(ERROR_NEGATIVE_SEEK);
	}
	self->pos = memoryPos;
	if (newPos)
	{
		*newPos = (int64_t)self->pos;
	}
	return S_OK;
}

static HRESULT memory_writer_close(byteio *stream)
{
	memory_writer *self = (memory_writer *)stream;

	free(self->buffer);
	free(self);
	return S_OK;
}

byteio *byteio_init_memory_writer(size_t initial_capacity)
{
	memory_writer *self;

	if (initial_capacity > (size_t)PTRDIFF_MAX)
	{
		return NULL;
	}
	self = (memory_writer *)calloc(1, sizeof(*self));
	if (self == NULL)
	{
		return NULL;
	}
	self->base.fn_read = memory_writer_read;
	self->base.fn_write = memory_writer_write;
	self->base.fn_seek = memory_writer_seek;
	self->base.fn_close = memory_writer_close;
	self->size = 0;
	self->pos = 0;
	self->capacity = (ptrdiff_t)initial_capacity;
	if (self->capacity == 0)
	{
		self->capacity = BYTEIO_DEFAULT_BUFFER_SIZE;
	}
	self->buffer = (unsigned char *)malloc(self->capacity);
	if (self->buffer == NULL)
	{
		free(self);
		return NULL;
	}
	return &self->base;
}

static int byteio_is_memory_writer(byteio *stream)
{
	return (stream->fn_read == memory_writer_read) &&
		(stream->fn_write == memory_writer_write) &&
		(stream->fn_seek == memory_writer_seek) &&
		(stream->fn_close == memory_writer_close);
}

HRESULT byteio_close_and_get_buffer(byteio *stream, void **bufferPtr, size_t *bufferLen)
{
	*bufferPtr = NULL;
	*bufferLen = 0;
	if (stream == NULL)
	{
		return E_INVALIDARG;
	}
	if (byteio_is_memory_writer(stream))
	{
		memory_writer *self = (memory_writer *)stream;

		*bufferPtr = self->buffer;
		*bufferLen = self->size;
		self->buffer = NULL;
		self->size = 0;
	}
	return byteio_close(stream);
}
