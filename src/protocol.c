#include <string.h>
#include "protocol.h"

static int type_is_valid(uint8_t type) {
    return type == PROTO_MSG_DATA || type == PROTO_MSG_PING;
}

int proto_encode(const proto_msg_t *msg, uint8_t *buf, size_t buf_len, size_t *out_len) {
    if (msg == NULL || buf == NULL || out_len == NULL) {
        return PROTO_ERR_NULL;
    }
    if (msg->length > PROTO_MAX_PAYLOAD) {
        return PROTO_ERR_TOO_LARGE;
    }
    if (!type_is_valid(msg->type)) {
        return PROTO_ERR_BAD_TYPE;
    }

    size_t total = PROTO_HEADER_SIZE + msg->length;
    if (buf_len < total) {
        return PROTO_ERR_BUF_SMALL;
    }

    buf[0] = msg->type;
    buf[1] = (uint8_t)(msg->length >> 8);
    buf[2] = (uint8_t)(msg->length & 0xFF);
    memcpy(buf + PROTO_HEADER_SIZE, msg->payload, msg->length);

    *out_len = total;
    return PROTO_OK;
}

int proto_decode(const uint8_t *buf, size_t buf_len, proto_msg_t *msg, size_t *consumed) {
    if (buf == NULL || msg == NULL || consumed == NULL) {
        return PROTO_ERR_NULL;
    }
    if (buf_len < PROTO_HEADER_SIZE) {
        return PROTO_ERR_INCOMPLETE;
    }

    uint8_t  type   = buf[0];
    uint16_t length = (uint16_t)((buf[1] << 8) | buf[2]);

    if (!type_is_valid(type)) {
        return PROTO_ERR_BAD_TYPE;
    }
    if (length > PROTO_MAX_PAYLOAD) {
        return PROTO_ERR_TOO_LARGE;
    }

    size_t total = PROTO_HEADER_SIZE + length;
    if (buf_len < total) {
        return PROTO_ERR_INCOMPLETE;
    }

    msg->type = type;
    msg->length = length;
    memcpy(msg->payload, buf + PROTO_HEADER_SIZE, length);

    *consumed = total;
    return PROTO_OK;
}