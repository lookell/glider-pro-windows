#include "ByteIO.h"
#include "WinAPI.h"
#include <limits.h>
#include <stdlib.h>

typedef int ASSERT_CHAR_BIT_IS_EIGHT[(CHAR_BIT == 8) ? 1 : -1];

//--------------------------------------------------------------

static int minimal_read(byteio *stream, void *buffer, size_t size);
static int minimal_write(byteio *stream, const void *buffer, size_t size);

typedef struct handle_reader {
	HANDLE hFile;
	const unsigned char *buf_begin;
	const unsigned char *buf_end;
	unsigned char buffer[8 * 1024];
} handle_reader;

static int handle_reader_init(byteio *stream, HANDLE hFile);
static int handle_reader_read(byteio *stream, void *buffer, size_t size);
static void handle_reader_close(byteio *stream);

typedef struct handle_writer {
	HANDLE hFile;
	unsigned char buffer[8 * 1024];
} handle_writer;

static int handle_writer_init(byteio *stream, HANDLE hFile);
static int handle_writer_write(byteio *stream, const void *buffer, size_t size);
static void handle_writer_close(byteio *stream);

typedef struct memory_reader {
	size_t size;
	size_t pos;
	const unsigned char *buffer;
} memory_reader;

static int memory_reader_init(byteio *stream, const void *buffer, size_t size);
static int memory_reader_read(byteio *stream, void *buffer, size_t size);
static int memory_reader_close(byteio *stream);

typedef struct memory_writer {
	size_t size;
	size_t pos;
	unsigned char *buffer;
} memory_writer;

static int memory_writer_init(byteio *stream, void *buffer, size_t size);
static int memory_writer_write(byteio *stream, const void *buffer, size_t size);
static int memory_writer_close(byteio *stream);

//--------------------------------------------------------------

int byteio_read(byteio *stream, void *buffer, size_t size)
{
	if (stream == NULL || buffer == NULL)
		return 0;
	return stream->fn_read(stream, buffer, size);
}

int byteio_skip(byteio *stream, size_t size)
{
	if (stream == NULL)
		return 0;
	return stream->fn_read(stream, NULL, size);
}

int byteio_write(byteio *stream, const void *buffer, size_t size)
{
	if (stream == NULL || buffer == NULL)
		return 0;
	return stream->fn_write(stream, buffer, size);
}

void byteio_close(byteio *stream)
{
	if (stream != NULL)
		stream->fn_close(stream);
}
