#include "rpy.h"
#include <stdio.h>
#include <string.h>

Rpy* rpy_init() {
    Rpy* rpy = (Rpy*)malloc(sizeof(*rpy));
    if (!rpy)
        return NULL;
    rpy->unpack_fn = NULL;
    rpy->pack_fn = NULL;

    return rpy;
}

void rpy_destroy(Rpy* rpy) {
    if (!rpy)
        return;
    free(rpy);
}

size_t rpy_unpack(const Rpy* rpy, const RpyBuf* buf, RpyBuf* out) {
    if (!rpy || !rpy->unpack_fn || !buf || !buf->data || !out)
        return 0;

    // Approximate memory needed for decompressed data
    size_t ptrsize = buf->size * 10;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return 0;
    memcpy(ptr, buf->data, buf->size);

    RpyBuf tmp = {
        .data = ptr,
        .size = buf->size,
        .capacity = ptrsize,
    };
    size_t size = rpy->unpack_fn(&tmp);
    // Shrink array to data size
    tmp.data = (uint8_t*)reallocarray(tmp.data, size, 1);

    if (out->data)
        free(out->data);
    out->data = tmp.data;
    out->size = tmp.size;
    out->capacity = tmp.capacity;

    return out->size;
}

size_t rpy_pack(const Rpy* rpy, const RpyBuf* buf, RpyBuf* out) {
    if (!rpy || !rpy->pack_fn || !buf || !buf->data || !out)
        return 0;

    size_t ptrsize = buf->size;
    uint8_t* ptr = (uint8_t*)calloc(ptrsize, sizeof(*ptr));
    if (!ptr)
        return 0;
    memcpy(ptr, buf->data, buf->size);

    RpyBuf tmp = {
        .data = ptr,
        .size = buf->size,
        .capacity = ptrsize,
    };
    size_t size = rpy->pack_fn(&tmp);
    // Shrink array to data size
    tmp.data = (uint8_t*)reallocarray(tmp.data, size, 1);

    if (out->data)
        free(out->data);
    out->data = tmp.data;
    out->size = tmp.size;
    out->capacity = tmp.capacity;

    return out->size;
}

RpyBuf* rpybuf_init() {
    RpyBuf* buf = (RpyBuf*)malloc(sizeof(*buf));
    if (!buf)
        return NULL;
    buf->data = NULL;
    buf->size = 0;
    buf->capacity = 0;

    return buf;
}

size_t rpybuf_read(RpyBuf* buf, const char* file) {
    if (!buf)
        return 0;

    FILE* fp = fopen(file, "rb");
    if (!fp)
        return 0;

    size_t bytes_read = 0;

    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    if (filesize == -1)
        goto ret;
    rewind(fp);

    uint8_t* ptr = (uint8_t*)malloc(filesize);
    if (!ptr)
        return 0;

    bytes_read = fread(ptr, sizeof(*ptr), filesize, fp);
    if (buf->data)
        free(buf->data);
    buf->data = ptr;
    buf->size = filesize;
    buf->capacity = filesize;
ret:
    fclose(fp);
    return bytes_read;
}

size_t rpybuf_write(const RpyBuf* buf, const char* file) {
    if (!buf)
        return 0;

    FILE* fp = fopen(file, "wb");
    if (!fp)
        return 0;

    size_t bytes_written = fwrite(buf->data, sizeof(*buf->data), buf->size, fp);

    fclose(fp);
    return bytes_written;
}

void rpybuf_destroy(RpyBuf* buf) {
    if (!buf)
        return;
    free(buf->data);
    free(buf);
}