#ifndef PACKET_TYPE_H
#define	PACKET_TYPE_H

enum control_packet_t {
    LOGIN = 0,
    LO_ACK = 1,
    LO_NACK = 2,
    EXIT = 3,
    JOIN = 4,
    JN_ACK = 5,
    JN_NCK = 6,
    LEAVE_SESS = 7,
    NEW_SESS = 8,
    NS_ACK = 9,
    MESSAGE = 10,
    QUERY = 11,
    QU_ACK = 12
};

#endif	/* PACKET_TYPE_H */

