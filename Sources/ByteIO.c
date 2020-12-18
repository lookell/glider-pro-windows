#include "ByteIO.h"

#include "WinAPI.h"
#include <limits.h>
#include <stdlib.h>

#define BYTEIO_DEFAULT_BUFFER_SIZE (8 * 1024)

typedef int ASSERT_CHAR_BIT_IS_EIGHT[(CHAR_BIT == 8) ? 1 : -1];

struct byteio {
	int (*fn_read)(byteio *stream, void *buffer, size_t size);
	int (*fn_write)(byteio *stream, const void *buffer, size_t size);
	int (*fn_seek)(byteio *stream, int64_t offset, int origin, int64_t *newPos);
	int (*fn_close)(byteio *stream);
	void *priv;
};

//--------------------------------------------------------------

typedef struct handle_reader {
	HANDLE hFile;
	size_t size;
	const unsigned char *bufptr;
	unsigned char buffer[BYTEIO_DEFAULT_BUFFER_SIZE];
} handle_reader;

static handle_reader *handle_reader_init(HANDLE hFile);
static int handle_reader_read(byteio *stream, void *buffer, size_t size);
static int handle_reader_write(byteio *stream, const void *buffer, size_t size);
static int handle_reader_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);
static int handle_reader_close(byteio *stream);

typedef struct handle_writer {
	HANDLE hFile;
	size_t size;
	unsigned char *bufptr;
	unsigned char buffer[BYTEIO_DEFAULT_BUFFER_SIZE];
} handle_writer;

static handle_writer *handle_writer_init(HANDLE hFile);
static int handle_writer_read(byteio *stream, void *buffer, size_t size);
static int handle_writer_write(byteio *stream, const void *buffer, size_t size);
static int handle_writer_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);
static int handle_writer_close(byteio *stream);

typedef struct memory_reader {
	ptrdiff_t size;
	ptrdiff_t pos;
	const unsigned char *buffer;
} memory_reader;

static memory_reader *memory_reader_init(const void *buffer, size_t size);
static int memory_reader_read(byteio *stream, void *buffer, size_t size);
static int memory_reader_write(byteio *stream, const void *buffer, size_t size);
static int memory_reader_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);
static int memory_reader_close(byteio *stream);

typedef struct memory_writer {
	ptrdiff_t size;
	ptrdiff_t pos;
	ptrdiff_t capacity;
	unsigned char *buffer;
} memory_writer;

static memory_writer *memory_writer_init(size_t initial_capacity);
static int memory_writer_read(byteio *stream, void *buffer, size_t size);
static int memory_writer_write(byteio *stream, const void *buffer, size_t size);
static int memory_writer_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos);
static int memory_writer_close(byteio *stream);

//--------------------------------------------------------------

int byteio_read(byteio *stream, void *buffer, size_t size)
{
	if (stream == NULL)
		return 0;
	return stream->fn_read(stream, buffer, size);
}

int byteio_write(byteio *stream, const void *buffer, size_t size)
{
	if (stream == NULL || buffer == NULL)
		return 0;
	return stream->fn_write(stream, buffer, size);
}

int byteio_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	if (stream == NULL)
		return 0;
	if (origin != SEEK_SET && origin != SEEK_CUR && origin != SEEK_END)
		return 0;
	return stream->fn_seek(stream, offset, origin, newPos);
}

int byteio_tell(byteio *stream, int64_t *curPos)
{
	return byteio_seek(stream, 0, SEEK_CUR, curPos);
}

int byteio_close(byteio *stream)
{
	int succeeded;

	if (stream == NULL)
	{
		return 0;
	}
	succeeded = stream->fn_close(stream);
	free(stream);
	return succeeded;
}

//--------------------------------------------------------------

int byteio_read_be_u8(byteio *stream, uint8_t *num)
{
	unsigned char buffer[sizeof(uint8_t)];
	int succeeded;

	succeeded = byteio_read(stream, buffer, sizeof(buffer));
	if (succeeded && num != NULL)
	{
		*num = ((uint8_t)buffer[0]);
	}
	return succeeded;
}

int byteio_read_le_u8(byteio *stream, uint8_t *num)
{
	unsigned char buffer[sizeof(uint8_t)];
	int succeeded;

	succeeded = byteio_read(stream, buffer, sizeof(buffer));
	if (succeeded && num != NULL)
	{
		*num = ((uint8_t)buffer[0]);
	}
	return succeeded;
}

int byteio_read_be_i8(byteio *stream, int8_t *num)
{
	return byteio_read_be_u8(stream, (uint8_t *)num);
}

int byteio_read_le_i8(byteio *stream, int8_t *num)
{
	return byteio_read_le_u8(stream, (uint8_t *)num);
}

int byteio_read_be_u16(byteio *stream, uint16_t *num)
{
	unsigned char buffer[sizeof(uint16_t)];
	int succeeded;

	succeeded = byteio_read(stream, buffer, sizeof(buffer));
	if (succeeded && num != NULL)
	{
		*num = ((uint16_t)buffer[0] << 8)
			| ((uint16_t)buffer[1]);
	}
	return succeeded;
}

int byteio_read_le_u16(byteio *stream, uint16_t *num)
{
	unsigned char buffer[sizeof(uint16_t)];
	int succeeded;

	succeeded = byteio_read(stream, buffer, sizeof(buffer));
	if (succeeded && num != NULL)
	{
		*num = ((uint16_t)buffer[0])
			| ((uint16_t)buffer[1] << 8);
	}
	return succeeded;
}

int byteio_read_be_i16(byteio *stream, int16_t *num)
{
	return byteio_read_be_u16(stream, (uint16_t *)num);
}

int byteio_read_le_i16(byteio *stream, int16_t *num)
{
	return byteio_read_le_u16(stream, (uint16_t *)num);
}

int byteio_read_be_u32(byteio *stream, uint32_t *num)
{
	unsigned char buffer[sizeof(uint32_t)];
	int succeeded;

	succeeded = byteio_read(stream, buffer, sizeof(buffer));
	if (succeeded && num != NULL)
	{
		*num = ((uint32_t)buffer[0] << 24)
			| ((uint32_t)buffer[1] << 16)
			| ((uint32_t)buffer[2] << 8)
			| ((uint32_t)buffer[3]);
	}
	return succeeded;
}

int byteio_read_le_u32(byteio *stream, uint32_t *num)
{
	unsigned char buffer[sizeof(uint32_t)];
	int succeeded;

	succeeded = byteio_read(stream, buffer, sizeof(buffer));
	if (succeeded && num != NULL)
	{
		*num = ((uint32_t)buffer[0])
			| ((uint32_t)buffer[1] << 8)
			| ((uint32_t)buffer[2] << 16)
			| ((uint32_t)buffer[3] << 24);
	}
	return succeeded;
}

int byteio_read_be_i32(byteio *stream, int32_t *num)
{
	return byteio_read_be_u32(stream, (uint32_t *)num);
}

int byteio_read_le_i32(byteio *stream, int32_t *num)
{
	return byteio_read_le_u32(stream, (uint32_t *)num);
}

//--------------------------------------------------------------

int byteio_write_be_u8(byteio *stream, uint8_t num)
{
	unsigned char buffer[sizeof(uint8_t)];

	buffer[0] = (unsigned char)(num & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

int byteio_write_le_u8(byteio *stream, uint8_t num)
{
	unsigned char buffer[sizeof(uint8_t)];

	buffer[0] = (unsigned char)(num & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

int byteio_write_be_i8(byteio *stream, int8_t num)
{
	return byteio_write_be_u8(stream, (uint8_t)num);
}

int byteio_write_le_i8(byteio *stream, int8_t num)
{
	return byteio_write_le_u8(stream, (uint8_t)num);
}

int byteio_write_be_u16(byteio *stream, uint16_t num)
{
	unsigned char buffer[sizeof(uint16_t)];

	buffer[0] = (unsigned char)((num >> 8) & 0xff);
	buffer[1] = (unsigned char)(num & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

int byteio_write_le_u16(byteio *stream, uint16_t num)
{
	unsigned char buffer[sizeof(uint16_t)];

	buffer[0] = (unsigned char)(num & 0xff);
	buffer[1] = (unsigned char)((num >> 8) & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

int byteio_write_be_i16(byteio *stream, int16_t num)
{
	return byteio_write_be_u16(stream, (uint16_t)num);
}

int byteio_write_le_i16(byteio *stream, int16_t num)
{
	return byteio_write_le_u16(stream, (uint16_t)num);
}

int byteio_write_be_u32(byteio *stream, uint32_t num)
{
	unsigned char buffer[sizeof(uint32_t)];

	buffer[0] = (unsigned char)((num >> 24) & 0xff);
	buffer[1] = (unsigned char)((num >> 16) & 0xff);
	buffer[2] = (unsigned char)((num >> 8) & 0xff);
	buffer[3] = (unsigned char)(num & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

int byteio_write_le_u32(byteio *stream, uint32_t num)
{
	unsigned char buffer[sizeof(uint32_t)];

	buffer[0] = (unsigned char)(num & 0xff);
	buffer[1] = (unsigned char)((num >> 8) & 0xff);
	buffer[2] = (unsigned char)((num >> 16) & 0xff);
	buffer[3] = (unsigned char)((num >> 24) & 0xff);
	return byteio_write(stream, buffer, sizeof(buffer));
}

int byteio_write_be_i32(byteio *stream, int32_t num)
{
	return byteio_write_be_u32(stream, (uint32_t)num);
}

int byteio_write_le_i32(byteio *stream, int32_t num)
{
	return byteio_write_le_u32(stream, (uint32_t)num);
}

//--------------------------------------------------------------

static handle_reader *handle_reader_init(HANDLE hFile)
{
	handle_reader *self = (handle_reader *)malloc(sizeof(*self));
	if (self == NULL)
		return NULL;
	self->hFile = hFile;
	self->bufptr = &self->buffer[0];
	self->size = 0;
	return self;
}

static int handle_reader_read(byteio *stream, void *buffer, size_t size)
{
	unsigned char *outptr = (unsigned char *)buffer;
	size_t readsize;
	handle_reader *self = (handle_reader *)stream->priv;

	if (self == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	while (size != 0)
	{
		// Fill up the buffer, if needed
		if (self->size == 0)
		{
			DWORD numRead;
			self->bufptr = &self->buffer[0];
			if (!ReadFile(self->hFile, self->buffer, sizeof(self->buffer), &numRead, NULL))
				return 0;
			self->size = numRead;
		}
		// Read as much as we can from the buffer
		readsize = (self->size <= size) ? self->size : size;
		if (readsize == 0)
		{
			SetLastError(ERROR_HANDLE_EOF);
			return 0;
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
	return 1;
}

static int handle_reader_write(byteio *stream, const void *buffer, size_t size)
{
	(void)stream;
	(void)buffer;
	(void)size;

	return 0;
}

static int handle_reader_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	handle_reader *self = (handle_reader *)stream->priv;
	LARGE_INTEGER fileOffset, newFilePointer;
	BOOL succeeded;

	if (self == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	// "Unread" any buffered data, and seek backwards to the correct position
	fileOffset.QuadPart = -((LONGLONG)self->size);
	succeeded = SetFilePointerEx(self->hFile, fileOffset, NULL, FILE_CURRENT);
	self->bufptr = &self->buffer[0];
	self->size = 0;
	if (!succeeded)
	{
		return succeeded;
	}
	// Perform the actual seek operation
	fileOffset.QuadPart = offset;
	switch (origin)
	{
	case SEEK_SET:
		succeeded = SetFilePointerEx(self->hFile, fileOffset, &newFilePointer, FILE_BEGIN);
		break;

	case SEEK_CUR:
		succeeded = SetFilePointerEx(self->hFile, fileOffset, &newFilePointer, FILE_CURRENT);
		break;

	case SEEK_END:
		succeeded = SetFilePointerEx(self->hFile, fileOffset, &newFilePointer, FILE_END);
		break;

	default:
		SetLastError(ERROR_INVALID_PARAMETER);
		newFilePointer.QuadPart = -1;
		succeeded = 0;
		break;
	}
	// Return the new position to the caller
	if (succeeded && newPos != NULL)
	{
		*newPos = newFilePointer.QuadPart;
	}
	return succeeded;
}

static int handle_reader_close(byteio *stream)
{
	LARGE_INTEGER li;
	BOOL result;
	handle_reader *self = (handle_reader *)stream->priv;
	li.QuadPart = -((LONGLONG)self->size);
	result = SetFilePointerEx(self->hFile, li, NULL, FILE_CURRENT);
	free(stream->priv);
	stream->priv = NULL;
	if (result == FALSE)
		return 0;
	return 1;
}

byteio *byteio_init_handle_reader(HANDLE fileHandle)
{
	byteio *stream;

	if (fileHandle == NULL || fileHandle == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	stream = (byteio *)calloc(1, sizeof(*stream));
	if (stream == NULL)
	{
		return NULL;
	}
	stream->fn_read = handle_reader_read;
	stream->fn_write = handle_reader_write;
	stream->fn_seek = handle_reader_seek;
	stream->fn_close = handle_reader_close;
	stream->priv = handle_reader_init(fileHandle);
	if (stream->priv == NULL)
	{
		free(stream);
		return NULL;
	}
	return stream;
}

//--------------------------------------------------------------

static handle_writer *handle_writer_init(HANDLE hFile)
{
	handle_writer *self = (handle_writer *)malloc(sizeof(*self));
	if (self == NULL)
		return NULL;
	self->hFile = hFile;
	self->bufptr = &self->buffer[0];
	self->size = sizeof(self->buffer);
	return self;
}

static int handle_writer_read(byteio *stream, void *buffer, size_t size)
{
	(void)stream;
	(void)buffer;
	(void)size;

	return 0;
}

static int handle_writer_write(byteio *stream, const void *buffer, size_t size)
{
	const unsigned char *inptr = (const unsigned char *)buffer;
	size_t writesize;
	handle_writer *self = (handle_writer *)stream->priv;

	if (self == NULL || buffer == NULL)
		return 0;
	while (size != 0)
	{
		// Empty the buffer, if needed
		if (self->size == 0)
		{
			DWORD numWritten;
			self->bufptr = &self->buffer[0];
			if (!WriteFile(self->hFile, self->buffer, sizeof(self->buffer), &numWritten, NULL))
				return 0;
			if (numWritten != sizeof(self->buffer))
				return 0;
			self->size = sizeof(self->buffer);
		}
		// Write as much as we can into the buffer
		writesize = (self->size <= size) ? self->size : size;
		memcpy(self->bufptr, inptr, writesize);
		inptr += writesize;
		size -= writesize;
		self->bufptr += writesize;
		self->size -= writesize;
	}
	return 1;
}

static int handle_writer_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	handle_writer *self = (handle_writer *)stream->priv;
	DWORD numToWrite, numWritten;
	LARGE_INTEGER fileOffset, newFilePointer;
	BOOL succeeded;

	if (self == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return 0;
	}
	// Write out any buffered data, so that we start in the correct position
	numToWrite = (DWORD)(sizeof(self->buffer) - self->size);
	succeeded = WriteFile(self->hFile, self->buffer, numToWrite, &numWritten, NULL);
	if (succeeded)
	{
		succeeded = (numWritten == numToWrite);
	}
	self->bufptr = &self->buffer[0];
	self->size = sizeof(self->buffer);
	if (!succeeded)
	{
		return succeeded;
	}
	// Perform the actual seek operation
	fileOffset.QuadPart = offset;
	switch (origin)
	{
	case SEEK_SET:
		succeeded = SetFilePointerEx(self->hFile, fileOffset, &newFilePointer, FILE_BEGIN);
		break;
	
	case SEEK_CUR:
		succeeded = SetFilePointerEx(self->hFile, fileOffset, &newFilePointer, FILE_CURRENT);
		break;
	
	case SEEK_END:
		succeeded = SetFilePointerEx(self->hFile, fileOffset, &newFilePointer, FILE_END);
		break;
	
	default:
		SetLastError(ERROR_INVALID_PARAMETER);
		newFilePointer.QuadPart = -1;
		succeeded = 0;
		break;
	}
	// Return the new position to the caller
	if (succeeded && newPos != NULL)
	{
		*newPos = newFilePointer.QuadPart;
	}
	return succeeded;
}

static int handle_writer_close(byteio *stream)
{
	BOOL result;
	handle_writer *self = (handle_writer *)stream->priv;
	DWORD numWritten;
	DWORD numToWrite;
	numToWrite = (DWORD)(sizeof(self->buffer) - self->size);
	result = WriteFile(self->hFile, self->buffer, numToWrite, &numWritten, NULL);
	free(stream->priv);
	stream->priv = NULL;
	if (result == FALSE || numWritten != numToWrite)
		return 0;
	return 1;
}

byteio *byteio_init_handle_writer(HANDLE fileHandle)
{
	byteio *stream;

	if (fileHandle == NULL || fileHandle == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	stream = (byteio *)calloc(1, sizeof(*stream));
	if (stream == NULL)
	{
		return NULL;
	}
	stream->fn_read = handle_writer_read;
	stream->fn_write = handle_writer_write;
	stream->fn_seek = handle_writer_seek;
	stream->fn_close = handle_writer_close;
	stream->priv = handle_writer_init(fileHandle);
	if (stream->priv == NULL)
	{
		free(stream);
		return NULL;
	}
	return stream;
}

//--------------------------------------------------------------

static memory_reader *memory_reader_init(const void *buffer, size_t size)
{
	memory_reader *self;

	if (size > (size_t)PTRDIFF_MAX)
		return NULL;
	self = (memory_reader *)malloc(sizeof(*self));
	if (self == NULL)
		return NULL;
	self->size = (ptrdiff_t)size;
	self->pos = 0;
	self->buffer = (const unsigned char *)buffer;
	return self;
}

static int memory_reader_read(byteio *stream, void *buffer, size_t size)
{
	memory_reader *self = (memory_reader *)stream->priv;
	if (self == NULL || self->buffer == NULL)
		return 0;
	if (self->pos >= self->size)
		return 0;
	if (size > (size_t)PTRDIFF_MAX)
		return 0;
	if (self->size - self->pos < (ptrdiff_t)size)
	{
		self->pos = self->size;
		return 0;
	}
	if (buffer != NULL)
		memcpy(buffer, &self->buffer[self->pos], size);
	self->pos += size;
	return 1;
}

static int memory_reader_write(byteio *stream, const void *buffer, size_t size)
{
	(void)stream;
	(void)buffer;
	(void)size;

	return 0;
}

static int memory_reader_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	memory_reader *self = (memory_reader *)stream->priv;
	ptrdiff_t memoryPos;
	ptrdiff_t memoryOffset;

	if (self == NULL || offset < PTRDIFF_MIN || offset > PTRDIFF_MAX)
	{
		return 0;
	}
	memoryOffset = (ptrdiff_t)offset;
	switch (origin)
	{
	case SEEK_SET:
		memoryPos = memoryOffset;
		break;

	case SEEK_CUR:
		memoryPos = self->pos + memoryOffset;
		break;

	case SEEK_END:
		memoryPos = self->size + memoryOffset;
		break;

	default:
		return 0; // invalid parameter
	}
	if (memoryPos < 0)
	{
		return 0; // negative seek
	}
	self->pos = memoryPos;
	if (newPos)
	{
		*newPos = (int64_t)self->pos;
	}
	return 1;
}

static int memory_reader_close(byteio *stream)
{
	free(stream->priv);
	stream->priv = NULL;
	return 1;
}

byteio *byteio_init_memory_reader(const void *buffer, size_t size)
{
	byteio *stream;

	if (buffer == NULL)
	{
		return NULL;
	}
	stream = (byteio *)calloc(1, sizeof(*stream));
	if (stream == NULL)
	{
		return NULL;
	}
	stream->fn_read = memory_reader_read;
	stream->fn_write = memory_reader_write;
	stream->fn_seek = memory_reader_seek;
	stream->fn_close = memory_reader_close;
	stream->priv = memory_reader_init(buffer, size);
	if (stream->priv == NULL)
	{
		free(stream);
		return NULL;
	}
	return stream;
}

//--------------------------------------------------------------

static memory_writer *memory_writer_init(size_t initial_capacity)
{
	memory_writer *self = (memory_writer *)malloc(sizeof(*self));
	if (self == NULL || initial_capacity > (size_t)PTRDIFF_MAX)
		return NULL;
	self->size = 0;
	self->pos = 0;
	self->capacity = (ptrdiff_t)initial_capacity;
	if (self->capacity == 0)
		self->capacity = BYTEIO_DEFAULT_BUFFER_SIZE;
	self->buffer = (unsigned char *)malloc(self->capacity);
	if (self->buffer == NULL)
	{
		free(self);
		return NULL;
	}
	return self;
}

static int memory_writer_read(byteio *stream, void *buffer, size_t size)
{
	(void)stream;
	(void)buffer;
	(void)size;

	return 0;
}

static int memory_writer_write(byteio *stream, const void *buffer, size_t size)
{
	memory_writer *self = (memory_writer *)stream->priv;
	unsigned char *new_buffer;
	ptrdiff_t write_size;
	ptrdiff_t new_position;
	ptrdiff_t new_capacity;

	if (self == NULL || self->buffer == NULL || buffer == NULL)
		return 0;
	if (size > (size_t)PTRDIFF_MAX)
		return 0;
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
				return 0; // out of memory
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
	return 1;
}

static int memory_writer_seek(byteio *stream, int64_t offset, int origin, int64_t *newPos)
{
	memory_writer *self = (memory_writer *)stream->priv;
	ptrdiff_t memoryPos;
	ptrdiff_t memoryOffset;

	if (self == NULL || offset < PTRDIFF_MIN || offset > PTRDIFF_MAX)
	{
		return 0;
	}
	memoryOffset = (ptrdiff_t)offset;
	switch (origin)
	{
	case SEEK_SET:
		memoryPos = memoryOffset;
		break;

	case SEEK_CUR:
		memoryPos = self->pos + memoryOffset;
		break;

	case SEEK_END:
		memoryPos = self->size + memoryOffset;
		break;

	default:
		return 0; // invalid parameter
	}
	if (memoryPos < 0)
	{
		return 0; // negative seek
	}
	self->pos = memoryPos;
	if (newPos)
	{
		*newPos = (int64_t)self->pos;
	}
	return 1;
}

static int memory_writer_close(byteio *stream)
{
	free(((memory_writer *)stream->priv)->buffer);
	free(stream->priv);
	stream->priv = NULL;
	return 1;
}

byteio *byteio_init_memory_writer(size_t initial_capacity)
{
	byteio *stream;

	if (initial_capacity > (size_t)PTRDIFF_MAX)
	{
		return NULL;
	}
	stream = (byteio *)calloc(1, sizeof(*stream));
	if (stream == NULL)
	{
		return NULL;
	}
	stream->fn_read = memory_writer_read;
	stream->fn_write = memory_writer_write;
	stream->fn_seek = memory_writer_seek;
	stream->fn_close = memory_writer_close;
	stream->priv = memory_writer_init(initial_capacity);
	if (stream->priv == NULL)
	{
		free(stream);
		return NULL;
	}
	return stream;
}

static int byteio_is_memory_writer(byteio *stream)
{
	return (stream->fn_read == memory_writer_read) &&
		(stream->fn_write == memory_writer_write) &&
		(stream->fn_seek == memory_writer_seek) &&
		(stream->fn_close == memory_writer_close);
}

int byteio_close_and_get_buffer(byteio *stream, void **bufferPtr, size_t *bufferLen)
{
	*bufferPtr = NULL;
	*bufferLen = 0;
	if (stream == NULL)
	{
		return 0;
	}
	if (byteio_is_memory_writer(stream))
	{
		memory_writer *writer;

		writer = (memory_writer *)stream->priv;
		*bufferPtr = writer->buffer;
		*bufferLen = writer->size;
		writer->buffer = NULL;
		writer->size = 0;
	}
	return byteio_close(stream);
}
