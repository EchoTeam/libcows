#ifndef LIBCOWS_FRAME_H
#define LIBCOWS_FRAME_H

#include <sys/types.h>
#include <inttypes.h>

/*
 * RFC6455, Section 5.2. Base Framing Protocol
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *   +-+-+-+-+-------+-+-------------+-------------------------------+
 *   |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
 *   |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
 *   |N|V|V|V|       |S|             |   (if payload len==126/127)   |
 *   | |1|2|3|       |K|             |                               |
 *   +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
 *   |     Extended payload length continued, if payload len == 127  |
 *   + - - - - - - - - - - - - - - - +-------------------------------+
 *   |                               |Masking-key, if MASK set to 1  |
 *   +-------------------------------+-------------------------------+
 *   | Masking-key (continued)       |          Payload Data         |
 *   +-------------------------------- - - - - - - - - - - - - - - - +
 *   :                     Payload Data continued ...                :
 *   + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
 *   |                     Payload Data continued ...                |
 *   +---------------------------------------------------------------+
 *
 */

struct libcows_ws_frame {
    enum {
        WSOP_CONTINUATION = 0x0,
        WSOP_TEXT         = 0x1,
        WSOP_BINARY       = 0x2,
        WSOP_CLOSE        = 0x8,
        WSOP_PING         = 0x9,
        WSOP_PONG         = 0xA,
    } opcode:4;
    unsigned int rsvs:3;
    unsigned int fin:1;
    uint8_t mask[4];
    uint64_t payload_length;
};

/*
 * Fill the frame structure based off given buffer and its size.
 * Return values:
 *   -1:     Frame is permanently broken.
 *    0:     Buffer is not filled: not enough data to fill the frame.
 *   n>0:   Frame is filled, consuming n bytes off the buffer.
 */
ssize_t libcows_ws_frame_fill(struct libcows_ws_frame *, void *buf, size_t size);

#endif  /* LIBCOWS_FRAME_H */
