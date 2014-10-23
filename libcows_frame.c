#include <string.h>

#include "libcows_frame.h"

ssize_t
libcows_ws_frame_fill(struct libcows_ws_frame *frame, void *buf, size_t size) {
    const unsigned char *b = buf;

    if(size < 2) return 0;

    *(unsigned char *)frame = b[0];
    int mask_len = (b[1] & 0x80) >> 5;
    int len7 = b[1] & 0x7f;
    int extra_payload_len = ((len7<126)?0:(len7==127?8:2));
    int frame_hdr_len = 2 + extra_payload_len + mask_len;

    if(size < frame_hdr_len) {
        return 0;
    } else {
        switch(extra_payload_len) {
        case 0: frame->payload_length = len7; break;
        case 2: frame->payload_length = (b[2] << 8) | b[3]; break;
        case 4:
            frame->payload_length =
                  ((uint64_t)b[2] << 56) | ((uint64_t)b[3] << 48)
                | ((uint64_t)b[4] << 40) | ((uint64_t)b[5] << 32)
                | (b[6] << 24) | (b[7] << 16) | (b[8] << 8)  | (b[9] << 0);
        }
        if(mask_len)
            memcpy(frame->mask, b + 2 + extra_payload_len, 4);
        else
            memset(frame->mask, 0, 4);

        return frame_hdr_len;
    }
}
