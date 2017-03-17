#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

typedef enum {
    CONNECT = 0,
    LOGIN = 1,
    LO_ACK = 2,
    LO_NAK = 3,
    EXIT = 4,
    JOIN = 5,
    JN_ACK = 6,
    JN_NAK = 7,
    LEAVE_SESS = 8,
    NEW_SESS = 9,
    NS_ACK = 10,
    NS_NAK = 11,
    MESSAGE = 12,
    QUERY = 13,
    QU_ACK = 14,
    ST_CONF = 15,
    ST_CONF_INIT = 16,
    ST_CONF_INIT_ACK = 17,
    ST_CONF_ACK = 18,
    ST_CONF_NCK = 19,
    END_CONF = 20,
    END_CONF_ACK = 21,
    END_CONF_NCK = 22
} control_packet_t;

#endif /* PACKET_TYPE_H */

