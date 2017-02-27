#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PRINT_PACKETS           1       // For testing file

#define MAXBUFSIZE              20      // For string length
#define MAXDATASIZE             1000    // max number of bytes we can get at once

// Server Parameters
#define BACKLOG                 10      // how many pending connections queue will hold
#define PACKET_TIMEOUT          100     // Timeout for packet sending (in microseconds)
#define MAX_USERS_PER_SESSION   100
#define SERVER_NACK_TIMEOUT     15000

#define DEBUG                   1

#endif

