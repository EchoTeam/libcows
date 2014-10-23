#ifndef BUFFERS_H
#define BUFFERS_H

#include <sys/uio.h>

typedef struct cbuf_chain cbuf_chain;
typedef struct cbuf cbuf;   /* A single chunk in the cbuf_chain */

cbuf_chain *cbuf_chain_new(size_t chunk_size);
void cbuf_chain_free(cbuf_chain *);

size_t cbuf_chain_get_buffered_data_size(cbuf_chain *chain);

/*
 * Add the specified (buf) of size (size) at the end of the chain (chain).
 * Always returns 0.
 */
int cbuf_chain_add_bytes(const void *buf, size_t size, cbuf_chain *chain);

/* Add a block of bytes, with a function to remove it later. */
void cbuf_chain_add_block(cbuf_chain *chain, void *buf, size_t size, void (*free)(void *));

/*
 * Return a contiguous space into which to write.
 * (guarantee) must be greater than zero.
 */
void *cbuf_chain_get_write_chunk(cbuf_chain *chain, size_t *size, int guarantee);

/*
 * Advance the write pointer given by cbuf_chain_get_write_chunk().
 */
void cbuf_chain_advance_write_ptr(cbuf_chain *chain, size_t written);

/*
 * Get the first chunk of contiguous space.
 * Returns 0 if it there is no data in chain, and sets the (*chunk_size) to 0.
 */
void *cbuf_chain_get_read_chunk(cbuf_chain *, size_t *chunk_size);

/*
 * Remove the (consume) number of bytes out of the head of the chain.
 */
void cbuf_chain_advance_read_ptr(cbuf_chain *, size_t consume);

/*
 * Fill the I/O vector with the next pieces of from the chain.
 * This function can be invoked over and over again, yielding new data.
 * RETURN VALUE:
 *   0: No more data in the chain or no space in the given iov.
 *   n: Number of iov slots filled up with pointers to data.
 * If given, the (opt_remaining_length) contains a size of excess data
 * which did not fit the I/O vector.
 */
size_t cbuf_chain_fill_iov(cbuf_chain *, struct iovec *vec, size_t veclen, size_t *opt_remaining_length);

/*
 * Collapse the chain into the single contiguous space.
 * Returns 1 if collapsing happened (i.e., cbuf_chain_get_read_chunk()
 * would return different chunk_size).
 * Returns 0 if collapsing not happened.
 */
int cbuf_chain_flatten(cbuf_chain *);

/*
 * Move data from one chain to another, possibly zero-copying it.
 * The optional (map_cb) callback is used to transform or verify raw byte
 * data before it gets copied. The callback might be invoked multiple times.
 */
void cbuf_chain_move(cbuf_chain *from, cbuf_chain *to, size_t length,
        void (*map_cb)(void *, size_t, void *opaque), void *opaque);

#endif  /* BUFFERS_H */
