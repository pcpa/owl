/*
 * Copyright (C) 2012  Paulo Cesar Pereira de Andrade.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Authors:
 *	Paulo Cesar Pereira de Andrade
 */

#include "owl.h"

#include <fcntl.h>
#include <sys/stat.h>

/*
 * Types
 */
typedef struct nat_open {
    ovector_t		*name;
    oint32_t		 mode;
} nat_open_t;

typedef struct nat_io {
    ostream_t		*stream;
    ovector_t		*vector;
    oword_t		 length;
} nat_io_t;

typedef struct nat_seek {
    ostream_t		*stream;
    oword_t		 offset;
    oint32_t		 whence;
} nat_seek_t;

typedef struct nat_close {
    ostream_t		*stream;
} nat_close_t;

/*
 * Prototypes
 */
static oword_t
read_buffer(ostream_t *stream);

static ssize_t
sys_read(int fd, void *data, size_t count);

static ssize_t
sys_write(int fd, void *data, size_t count);

static off_t
sys_seek(int fd, off_t offset, int whence);

static void
stream_w_flush(ostream_t *stream);

static void
native_open(oobject_t list, oint32_t size);

static void
native_read(oobject_t list, oint32_t size);

static void
native_write(oobject_t list, oint32_t size);

static void
native_seek(oobject_t list, oint32_t size);

static void
native_close(oobject_t list, oint32_t size);

/*
 * Initialization
 */
ostream_t	*std_input;
ostream_t	*std_output;
ostream_t	*std_error;

/*
 * Implementation
 */
void
init_stream(void)
{
    oadd_root((oobject_t *)&std_input);
    ofdopen((oobject_t *)&std_input, 0, S_read | S_buffered);
    std_input->name = oget_string((ouint8_t *)"<stdin>", 7);

    oadd_root((oobject_t *)&std_output);
    ofdopen((oobject_t *)&std_output, 1, S_write | S_buffered);
    std_output->name = oget_string((ouint8_t *)"<stdout>", 8);

    oadd_root((oobject_t *)&std_error);
    ofdopen((oobject_t *)&std_error, 2, S_write | S_unbuffered);
    std_error->name = oget_string((ouint8_t *)"<stderr>", 8);
}

void
init_stream_builtin(void)
{
    obuiltin_t		*builtin;

    builtin = onew_builtin("open", native_open, t_void, false);
    onew_argument(builtin, t_void);		/* name */
    onew_argument(builtin, t_int32);		/* mode */
    oend_builtin(builtin);

    builtin = onew_builtin("seek", native_seek, t_word, false);
    onew_argument(builtin, t_void);		/* stream */
    onew_argument(builtin, t_word);		/* offset */
    onew_argument(builtin, t_int32);		/* whence */
    oend_builtin(builtin);

    builtin = onew_builtin("read", native_read, t_word, false);
    onew_argument(builtin, t_void);		/* stream */
    onew_argument(builtin, t_void);		/* vector */
    onew_argument(builtin, t_word);		/* size */
    oend_builtin(builtin);

    builtin = onew_builtin("write", native_write, t_word, false);
    onew_argument(builtin, t_void);		/* stream */
    onew_argument(builtin, t_void);		/* vector */
    onew_argument(builtin, t_word);		/* size */
    oend_builtin(builtin);

    builtin = onew_builtin("close", native_close, t_void, false);
    onew_argument(builtin, t_void);		/* stream */
    oend_builtin(builtin);
}

void
finish_stream(void)
{
    std_input->fileno = std_output->fileno = std_error->fileno = -1;
    orem_root((oobject_t *)&std_input);
    orem_root((oobject_t *)&std_output);
    orem_root((oobject_t *)&std_error);
}

void
ofdopen(oobject_t *pointer, int fileno, oint32_t mode)
{
    ostream_t		*stream;
    struct stat		 st;

    onew_vector_base(pointer, t_uint8, t_stream, BUFSIZ, sizeof(ostream_t));
    stream = *pointer;

    stream->s_read = (mode & S_read) != 0;
    stream->s_write = (mode & S_write) != 0;
    stream->s_append = (mode & S_append) != 0;

    if (fstat(fileno, &st) == 0)
	stream->s_regular = S_ISREG(st.st_mode) != 0;

    /* if not explicitly set to unbuffered */
    if (!(mode & S_unbuffered) &&
	/* if regular file or explicitly set to buffered */
	(stream->s_regular || (mode & S_buffered)))
	stream->s_buffered = 1;

    stream->fileno = fileno;

    omutex_init(&stream->mutex);
}

void
ofopen(oobject_t *pointer, ovector_t *path, oint32_t mode)
{
    int			 flags;
    int			 fileno;
    ostream_t		*stream;
    oword_t		 length;
    char		 name[BUFSIZ];
    gc_enter();

    gc_push(path);
    flags = O_NOCTTY;

    /* check read/write attributes */
    if ((mode & S_read) && (mode & S_write))
	flags |= O_RDWR;
    else if (mode & S_read)
	flags |= O_RDONLY;
    else if (mode & S_write)
	flags |= O_WRONLY;

    if (mode & S_nonblock)
	flags |= O_NONBLOCK;

    /* create if does not exist */
    if (mode & S_write) {
	flags |= O_CREAT;

	/* append if exists */
	if (mode & S_append)
	    flags |= O_APPEND;
	/* assume read/write mode doesn't want to truncate file */
	else if (!(mode & S_read))
	    flags |= O_TRUNC;
    }

    if ((length = path->length) >= sizeof(name) - 1) {
	owarn("pathname too long");
	length = sizeof(name) - 1;
    }
    memcpy(name, path->v.obj, length);
    name[length] = '\0';

    if ((fileno = open(name, flags, 0666)) >= 0) {
	ofdopen(pointer, fileno, mode);
	if ((stream = *(ostream_t **)pointer))
	    stream->name = path;
    }
    else
	*pointer = null;
    gc_leave();
}

off_t
oseek(ostream_t *stream, off_t position, oint32_t whence)
{
    off_t		offset;

    offset = 0;
    if (ostream_p(stream)) {
	if (stream->s_regular) {
	    if (position == 0 && whence == SEEK_CUR) {
		offset = sys_seek(stream->fileno, 0, SEEK_CUR);
		offset += stream->offset;
	    }
	    else {
		if (stream->s_r_mode) {
		    stream->offset = stream->size = 0;
		    stream->s_r_mode = 0;
		}
		else if (stream->s_w_mode)
		    stream_w_flush(stream);
		offset = sys_seek(stream->fileno, position, whence);
	    }
	}
    }
    else {
	switch (whence) {
	    case SEEK_CUR:
		position += stream->offset;
		break;
	    case SEEK_END:
		position += stream->size;
		break;
	    default:
		/* FIXME check if not SEEK_SET,
		 * or lseek "compatible" errors */
		break;
	}
	if (position < 0)			position = 0;
	else if (position > stream->length)	position = stream->length;
	stream->offset = position;
	offset = position;
    }

    return (offset);
}

ssize_t
oread(ostream_t *stream, void *data, size_t count)
{
    size_t		left;
    ssize_t		bytes;

    bytes = eof;
    if (ostream_p(stream)) {
	if (!stream->s_read)
	    goto done;
	else if (stream->s_append) {
	    bytes = 0;
	    goto done;
	}

	/* switching operation */
	if (stream->s_w_mode)
	    stream_w_flush(stream);

	/* bytes left to read */
	left = count;

	/* if there is anything in the buffer */
	if (stream->s_r_mode) {
	    bytes = stream->size - stream->offset;
	    if (bytes > left)
		bytes = left;
	    memcpy(data, stream->ptr + stream->offset, bytes);
	    /* if buffer became empty */
	    if ((stream->offset += bytes) == stream->size) {
		stream->offset = stream->size = 0;
		stream->s_r_mode = 0;
	    }
	    left -= bytes;
	}

	/* if still needs to read data */
	while (left) {
	    bytes = left - (left % BUFSIZ);

	    /* don't cache or iterate over large chunks */
	    if (bytes) {
		bytes = sys_read(stream->fileno,
				 (oint8_t *)data + (count - left), bytes);
		left -= bytes;
	    }

	    /* chunk is smaller than, or not aligned to BUFSIZ bytes */	
	    if (left) {
		/* read any padding data to buffer */
		bytes = sys_read(stream->fileno, stream->ptr,
				 stream->length);
		stream->offset = 0;
		stream->size = bytes;
		stream->s_r_mode = 1;

		/* read remaining data from buffer */
		if (bytes > left)
		    bytes = left;
		memcpy((oint8_t *)data + (count - left), stream->ptr, bytes);
		stream->offset += bytes;
		left -= bytes;
	    }
	}
	bytes = count - left;
    }
    else {
	/* number of bytes available in current buffer */
	bytes = stream->length - stream->offset;
	/* if there are excess bytes in current buffer */
	if (bytes > count)
	    bytes = count;
	/* read data */
	memcpy(data, stream->ptr + stream->offset, bytes);
	/* update stream offset */
	stream->offset += bytes;
    }

done:
    return (bytes);
}

ssize_t
owrite(ostream_t *stream, void *data, size_t count)
{
    ssize_t		left;
    ssize_t		bytes;
    oword_t		length;

    bytes = eof;
    if (ostream_p(stream)) {
	if (!stream->s_write)
	    goto done;

	/* bytes left to write */
	left = count;

	if (stream->s_r_mode) {
	    assert(stream->s_r_lock == false);
	    /* switching operation, discard buffer contents */
	    sys_seek(stream->fileno, stream->offset - stream->size, SEEK_CUR);
	    stream->offset = stream->size = 0;
	    stream->s_r_mode = 0;
	}
	/* if there is anything in the buffer */
	else if (stream->s_w_mode) {
	    length = stream->size;
	    bytes = length - stream->offset;
	    if (bytes > left)
		bytes = left;
	    memcpy(stream->ptr + stream->offset, data, bytes);
	    if ((stream->offset += bytes) > stream->size)
		stream->size = stream->offset;

	    /* if write buffer is full */
	    if (stream->size >= length)
		stream_w_flush(stream);
	    left -= bytes;
	}

	/* if still need to write data */
	if (left) {
	    bytes = left - (left % BUFSIZ);

	    /* don't cache or iterate over large chunks */
	    if (bytes) {
		bytes = sys_write(stream->fileno,
				  (oint8_t *)data + (count - left), bytes);
		left -= bytes;
	    }

	    if (left) {
		/* write any padding data to buffer */
		memcpy(stream->ptr, (oint8_t *)data + (count - left), left);
		if ((stream->offset += left) > stream->size)
		    stream->size = stream->offset;
		/* only reach here if flushed buffer */
		stream->s_w_mode = 1;
	    }
	}
	bytes = count;
    }
    else {
	/* check if need more space */
	if (stream->offset + count > stream->length) {
	    bytes = stream->length + count;
	    bytes += BUFSIZ - (bytes % BUFSIZ);
	    orenew_vector((ovector_t *)stream, bytes);
	}
	memcpy(stream->ptr + stream->offset, data, count);
	stream->offset += count;
	bytes = count;
    }

done:
    return (bytes);
}

obool_t
oflush(ostream_t *stream)
{
    if (ostream_p(stream)) {
	if (stream->s_write && stream->s_w_mode) {
	    stream_w_flush(stream);

	    return (true);
	}
    }

    return (false);
}

oint32_t
ogetc(ostream_t *stream)
{
    oint32_t		 ch;

    ch = eof;
    if (ostream_p(stream)) {
	if (stream->s_read) {
	    if (stream->s_w_mode)
		stream_w_flush(stream);
	    if (stream->offset < stream->size || read_buffer(stream))
		ch = stream->ptr[stream->offset++];
	}
    }
    else if (stream->offset < stream->length)
	ch = stream->ptr[stream->offset++];

    return (ch);
}

void
oungetc(ostream_t *stream, oint32_t ch)
{
    if (ch == eof)
	return;

    if (ostream_p(stream)) {
	if (!stream->s_read)
	    return;

	if (stream->s_w_mode)
	    stream_w_flush(stream);

	if (stream->offset)
	    --stream->offset;
	else if (stream->size) {
	    if (stream->size >= stream->length)
		orenew_vector((ovector_t *)stream,
			      stream->size + BUFSIZ);
	    memmove(stream->ptr + 1, stream->ptr, stream->size);
	    *stream->ptr = ch;
	    ++stream->size;
	}
	else {
	    *stream->ptr = ch;
	    stream->size = 1;
	    stream->s_r_mode = 1;
	}
    }
    else if (stream->offset)
	--stream->offset;
}

oint32_t
oputc(ostream_t *stream, oint32_t byte)
{
    if (ostream_p(stream)) {
	if (stream->s_write) {
	    if (stream->s_r_mode) {
		assert(stream->s_r_lock == false);
		/* switching operation, discard buffer contents */
		sys_seek(stream->fileno, stream->offset - stream->size,
			 SEEK_CUR);
		stream->offset = stream->size = 0;
		stream->s_r_mode = 0;
	    }
	    else if (stream->size >= stream->length)
		stream_w_flush(stream);
	    stream->ptr[stream->offset] = byte;
	    if (++stream->offset > stream->size)
		stream->size = stream->offset;
	    stream->s_w_mode = 1;
	}
	else
	    byte = eof;
    }
    else {
	if (stream->offset >= stream->length)
	    orenew_vector((ovector_t *)stream, stream->length + BUFSIZ);
	stream->ptr[stream->offset++] = byte;
    }

    return (byte);
}

oint32_t
oclose(ostream_t *stream)
{
    oint32_t		result;

    if (ostream_p(stream) && stream->fileno != -1) {
	if (stream->s_w_mode)
	    stream_w_flush(stream);
	result = close(stream->fileno);
	stream->fileno = -1;
	stream->s_read = stream->s_write = 0;
	omutex_destroy(&stream->mutex);
    }
    else
	result = -1;

    return (result);
}

void
ocheck_buffer(ostream_t *stream, oword_t bytes)
{
    oword_t		length;

    if (stream->offset + bytes > stream->length) {
	if (ostream_p(stream) && stream->s_w_mode) {
	    stream_w_flush(stream);
	    if (bytes <= stream->length)
		return;
	}
	length = stream->offset + bytes;
	length += BUFSIZ - length % BUFSIZ;
	orenew_vector((ovector_t *)stream, length);
    }
}

void
oread_lock(ostream_t *stream)
{
    oword_t		bytes;

    if (ostream_p(stream)) {
	assert(stream->s_r_lock == false);
	stream->s_r_lock = 1;

	/* avoid possibly cyclic read_lock() call just when buffer is filled */
	if (stream->offset >= BUFSIZ) {
	    bytes = stream->size - stream->offset;
	    memmove(stream->ptr, stream->ptr + stream->offset, bytes);
	    stream->offset = 0;
	    stream->size = bytes;
	}
    }
}

void
oread_unlock(ostream_t *stream)
{
    if (ostream_p(stream)) {
	assert(stream->s_r_lock != false);
	stream->s_r_lock = 0;
    }
}

static oword_t
read_buffer(ostream_t *stream)
{
    oword_t		bytes;
    oword_t		count;
    oword_t		length;

    /* if there is anything to read */
    if ((count = stream->size - stream->offset) > 0)
	goto done;

    /* if reading from a string stream */
    if (!ostream_p(stream)) {
	count = 0;
	goto done;
    }

    /* dont block, only read more data when buffer is empty */
    length = stream->length;
    bytes = length - stream->size;
    if (bytes <= 0) {
	if (stream->s_r_lock) {
	    count = length + BUFSIZ;
	    count = count - (count % BUFSIZ);
	    orenew_vector((ovector_t *)stream, count);
	    bytes = sys_read(stream->fileno, stream->ptr + stream->offset,
			     count - length);
	    if (bytes > 0) {
		stream->size += bytes;
		count = bytes;
	    }
	    else
		count = 0;
	}
	else {
	    stream->offset = 0;
	    bytes = sys_read(stream->fileno, stream->ptr, length);
	    if (bytes > 0) {
		stream->size = count = bytes;
		stream->s_r_mode = 1;
	    }
	    else
		stream->s_r_mode = 0;
	}
    }
    else {
	bytes = sys_read(stream->fileno, stream->ptr + stream->size, bytes);
	if (bytes > 0) {
	    count = bytes;
	    stream->s_r_mode = 1;
	    stream->size += bytes;
	}
    }

done:
    return (count);
}

static ssize_t
sys_read(int fd, void *data, size_t count)
{
    ssize_t 	bytes;

    if ((bytes = read(fd, data, count)) < 0)
	owarn("I/O error");

    return (bytes);
}

static ssize_t
sys_write(int fd, void *data, size_t count)
{
    ssize_t 	bytes;

    if ((bytes = write(fd, data, count)) != count)
	owarn("I/O error");

    return (bytes);
}

static off_t
sys_seek(int fd, off_t offset, int whence)
{
    off_t	position;

    if ((position = lseek(fd, offset, whence)) < 0)
	owarn("I/O error");

    return (position);
}

static void
stream_w_flush(ostream_t *stream)
{
    sys_write(stream->fileno, stream->ptr, stream->size);
    stream->size = stream->offset = 0;
    stream->s_w_mode = 0;
}

static void
native_open(oobject_t list, oint32_t size)
{
    GET_THREAD_SELF()
    oregister_t		*r0;
    nat_open_t		*alist;

    alist = (nat_open_t *)list;
    if (alist->name == null || otype(alist->name) != t_string)
	othrow(except_invalid_argument);
    r0 = &thread_self->r0;

    ofopen(&thread_self->obj, alist->name, alist->mode);
    if ((r0->v.o = thread_self->obj))
	r0->t = otype(r0->v.o);
    else
	r0->t = t_void;
}

static void
native_read(oobject_t list, oint32_t size)
{
    GET_THREAD_SELF()
    oregister_t		*r0;
    oint32_t		 shift;
    oword_t		 count;
    nat_io_t		*alist;

    alist = (nat_io_t *)list;
    if (alist->stream == null ||
	(otype(alist->stream) != t_string && otype(alist->stream) != t_stream))
	othrow(except_invalid_argument);

    if (alist->vector == null || (otype(alist->vector) & t_vector) != t_vector)
	othrow(except_invalid_argument);

    if (alist->length < 0)
	othrow(except_invalid_argument);

    switch (otype(alist->vector) & ~t_vector) {
	case t_int8:		case t_uint8:
	    shift = 0;
	    break;
	case t_int16:		case t_uint16:
	    shift = 1;
	    break;
	case t_int32:		case t_uint32:		case t_single:
	    shift = 2;
	    break;
	case t_int64:		case t_uint64:		case t_float:
	    shift = 3;
	    break;
	default:
	    /* Cannot allow reading pointers */
	    othrow(except_invalid_argument);
	    break;
    }

    count = alist->length << shift;

    if (count < alist->length)
	othrow(except_invalid_argument);

    if (alist->vector->length < alist->length)
	orenew_vector(alist->vector, alist->length);

    count = oread(alist->stream, alist->vector->v.ptr, count) >> shift;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = count;
}

static void
native_write(oobject_t list, oint32_t size)
{
    GET_THREAD_SELF()
    oregister_t		*r0;
    oint32_t		 shift;
    oword_t		 count;
    nat_io_t		*alist;

    alist = (nat_io_t *)list;
    if (alist->stream == null ||
	(otype(alist->stream) != t_string && otype(alist->stream) != t_stream))
	othrow(except_invalid_argument);

    if (alist->vector == null || (otype(alist->vector) & t_vector) != t_vector)
	othrow(except_invalid_argument);

    if (alist->length < 0)
	othrow(except_invalid_argument);

    switch (otype(alist->vector) & ~t_vector) {
	case t_int8:		case t_uint8:
	    shift = 0;
	    break;
	case t_int16:		case t_uint16:
	    shift = 1;
	    break;
	case t_int32:		case t_uint32:		case t_single:
	    shift = 2;
	    break;
	case t_int64:		case t_uint64:		case t_float:
	    shift = 3;
	    break;
	default:
	    /* Cannot allow writing pointers */
	    othrow(except_invalid_argument);
	    break;
    }

    count = alist->length << shift;

    if (count < alist->length || alist->length > alist->vector->length)
	othrow(except_invalid_argument);

    count = owrite(alist->stream, alist->vector->v.ptr, count) >> shift;

    r0 = &thread_self->r0;
    r0->t = t_word;
    r0->v.w = count;
}

static void
native_seek(oobject_t list, oint32_t size)
{
    GET_THREAD_SELF()
    oregister_t		*r0;
    nat_seek_t		*alist;

    alist = (nat_seek_t *)list;
    if (alist->stream == null ||
	(otype(alist->stream) != t_string && otype(alist->stream) != t_stream))
	othrow(except_invalid_argument);
    r0 = &thread_self->r0;

    r0->v.w = oseek(alist->stream, alist->offset, alist->whence);
    r0->t = t_word;
}

static void
native_close(oobject_t list, oint32_t size)
{
    GET_THREAD_SELF()
    oregister_t		*r0;
    nat_close_t		*alist;

    alist = (nat_close_t *)list;
    if (alist->stream == null ||
	(otype(alist->stream) != t_string && otype(alist->stream) != t_stream))
	othrow(except_invalid_argument);
    r0 = &thread_self->r0;
    r0->v.w = otype(alist->stream) == t_stream &&
	oclose(alist->stream) == 0 ? true : false;
    r0->t = t_word;
}
