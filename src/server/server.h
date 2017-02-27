#ifndef SERVER_H
#define	SERVER_H

#include "../constants.h"
#include "../message.h"
#include "../packet_type.h"

#include "user.h"

// The preinitialized list of users
extern User user_list[];

void handle_client_message(char* message, int fd);

#endif	/* SERVER_H */

