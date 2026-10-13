#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#define PROTOCOL_PORT 9090

/* Wire format: [type:1][length:2, big-endian][payload:length] */
#define PROTO_HEADER_SIZE 3u
#define PROTO_MAX_PAYLOAD 1024u

enum { 
    PROTO_MSG_DATA = 1, 
    PROTO_MSG_PING = 2
};

enum {
    PROTO_OK = 0,
    PROTO_ERR_NULL = -1,
    PROTO_ERR_BUF_SMALL = -2,
    PROTO_ERR_INCOMPLETE = -3,
    PROTO_ERR_TOO_LARGE = -4,
    PROTO_ERR_BAD_TYPE = -5
};

typedef struct {
    uint8_t  type;
    uint16_t length;
    uint8_t  payload[PROTO_MAX_PAYLOAD];
} proto_msg_t;

int proto_encode(const proto_msg_t *msg, uint8_t *buf, size_t buf_len, size_t *out_len);
int proto_decode(const uint8_t *buf, size_t buf_len, proto_msg_t *msg, size_t *consumed);

#endif