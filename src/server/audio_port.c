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

#include "audio_port.h"
#include "../constants.h"
#include "user_list.h"
#include "session.h"
#include "voice_queue.h"
#include "session_list.h"

struct addrinfo *audio_port;
int port_d;
int sockfd_d;

void open_audio_socket(int port) {
    struct addrinfo hints, *servinfo;

    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET for IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // Using my IP

    char port_s[MAXDATASIZE];
    sprintf(port_s, "%d", port);

    if ((rv = getaddrinfo(NULL, port_s, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    // looping through all results and bind the first one available
    for (audio_port = servinfo; audio_port != NULL; audio_port = audio_port->ai_next) {
        if ((sockfd_d = socket(audio_port->ai_family, audio_port->ai_socktype,
                audio_port->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd_d, audio_port->ai_addr, audio_port->ai_addrlen) == -1) {
            close(sockfd_d);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (audio_port == NULL) {
        fprintf(stderr, "server: failed to bind socket\n");
        return;
    }

    port_d = port;
    freeaddrinfo(servinfo);
    return;
}

int hobi(int num) {
    if (!num)
        return 0;
    int ret = 1;

    while (num >>= 1)
        ret++;

    return ret;
}

void dequeue_audio_packets(Session* s) {
    AudioPacket t_packet;
    
    VoicePacketQueue* vpq = s->vpq;
    int scale = hobi(s->num_user);
    
    // TODO - mix properly using timeval, drop if time incorrect
    for (int i = 0; i < s->num_user; i++) {
        for(int j = 0; j < BUFFER_CHUNK; j++) {
            t_packet.data[j] += (vpq[i].p->data[j] >> scale);
        }
    }
    
    // Send the t_packet to everyone in the session
    int numbytes;
    for (int i = 0; i < s->num_user; i++) {
        char* client_id = s->users[i]->id;
        User_List* user_l = find_active_user(client_id);
        
        if ((numbytes = sendto(sockfd_d, &t_packet, sizeof(t_packet), 0,
                (struct sockaddr *) &user_l->udp_addr, sizeof(user_l->udp_addr))) == -1) {
            perror("client: sendto");
            exit(1);
        }
    }
}

void process_audio_packets(AudioPacket* packet, struct sockaddr_storage theiraddr) {
    // Find the user and session_id associated with the packet
    char* client_id = packet->source;
    User_List* user_l = find_active_user(client_id);
    if(user_l->udp_addr == NULL) user_l->udp_addr = &theiraddr;
    
    char* sess_id = user_l->session_id;
    Session* session = find_session(sess_id);

    // Enqueue the audio packet
    for (int i = 0; i < session->num_user; i++) {
        // Find the correct session for the user
        if (strcmp(session->users[i]->id, packet->source) == 0) {
            // Add the packet to the session Voice Packet Queue
            session->vpq[i].p = packet;
        }
    }


    // Check if the VPQ has all users's packets
    bool dequeue_audio = false;
    for (int i = 0; i < session->num_user; i++) {
        if (session->vpq[i].p != NULL) dequeue_audio = true;
    }

    // If all the users packets arrive, dequeue
    if (dequeue_audio) dequeue_audio_packets(session);
}