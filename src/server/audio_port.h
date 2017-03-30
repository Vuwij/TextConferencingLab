#ifndef AUDIO_PORT_H
#define AUDIO_PORT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "../audio_packet.h"

// Finds an audio (UDP) socket and opens the port
void open_audio_socket(int port);

void process_audio_packets(AudioPacket* packet, struct sockaddr_storage theiraddr);

#endif /* AUDIO_PORT_H */

