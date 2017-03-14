#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#define PRINT_PACKETS           1       // For testing file

#define MAXBUFSIZE              20      // For string length
#define MAXDATASIZE             1000    // max number of bytes we can get at once

// Port Parmaeters
#define MIN_PORTNUM             0
#define MAX_PORTNUM             65535
#define PORT_BUF_SIZE           5

// Server Parameters
#define BACKLOG                 10      // how many pending connections queue will hold
#define PACKET_TIMEOUT          100     // Timeout for packet sending (in microseconds)
#define MAX_USERS_PER_SESSION   100
#define SERVER_NACK_TIMEOUT     15000

// Debugging Print Section
#define DEBUG                   1
#define DEBUG_MSG               0       // Prints packet information on deliver

#define COMM_LOGIN              "/login"
#define COMM_LOGOUT             "/logout"
#define COMM_JOINSESSION        "/joinsession"
#define COMM_LEAVESESSION       "/leavesession"
#define COMM_CREATESESSION      "/createsession"
#define COMM_LIST               "/list"
#define COMM_QUIT               "/quit"

#define H_COMM_LOGIN            36862910
#define H_COMM_LOGOUT           153414711
#define H_COMM_JOINSESSION      1838018493
#define H_COMM_LEAVESESSION     3521579419
#define H_COMM_CREATESESSION    3091944101
#define H_COMM_LIST             2083414413
#define H_COMM_QUIT             1790193820

typedef enum {
    SERVER = 0,
    CLIENT1 = 1,
    CLIENT2 = 2,
    CLIENT3 = 3,
    CLIENT4 = 4
} PRINT_SRC;

extern PRINT_SRC print_src;

void PRINT(const char * pString, ...);

#endif

