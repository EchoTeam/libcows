#ifndef BASE64_H
#define BASE64_H

#include <sys/types.h>

/*
 * Encode or decode the given string using the base64 decoder.
 * If size is not specified, strlen(str) will be used.
 * The resulting data size will be placed into *size.
 */
char *base64_encode(const void *data,   size_t *size);	/* leak-safe */
void *base64_decode(const char *string, size_t *size);	/* leak-safe */
void *base64_detach(size_t *size);		/* Detach the internal buffer */

#endif	/* BASE64_H */
