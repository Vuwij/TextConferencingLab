#ifndef PACKET_TYPE_H
#define	PACKET_TYPE_H

typedef enum {
    CONNECT = 0,
    LOGIN = 1,
    LO_ACK = 2,
    LO_NACK = 3,
    EXIT = 4,
    JOIN = 5,
    JN_ACK = 6,
    JN_NCK = 7,
    LEAVE_SESS = 8,
    NEW_SESS = 9,
    NS_ACK = 10,
    MESSAGE = 11,
    QUERY = 12,
    QU_ACK = 13
}control_packet_t;

#endif	/* PACKET_TYPE_H */

