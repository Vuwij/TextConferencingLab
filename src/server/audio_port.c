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
#include <sys/time.h>
#include <time.h>

#include "audio_port.h"
#include "../constants.h"
#include "user_list.h"
#include "session.h"
#include "voice_queue.h"
#include "session_list.h"
#include "server.h"

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

AudioPacket t_packet = {
    .source = "Server\n"
};

void dequeue_audio_packets(Session* s) {
    VoicePacketQueue** vpq = s->vpq;
    int num_user = s->num_user;
    t_packet.start_time = vpq[0]->p->start_time;
    t_packet.packet_num = vpq[0]->p->packet_num;
    
    for (int j = 0; j < PACKET_SIZE; j++) {
        t_packet.data[j] = vpq[j % num_user]->p->data[j];
    }
    
    // Send the t_packet to everyone in the session
    int numbytes;
    for (int i = 0; i < s->num_user; i++) {
        char* client_id = s->users[i]->id;
        User_List* user_l = find_active_user(client_id);
        socklen_t length = sizeof (*(user_l->udp_addr));
        if ((numbytes = sendto(sockfd_d, &t_packet, sizeof (AudioPacket), 0,
                (struct sockaddr *) (user_l->udp_addr), length)) == -1) {
            perror("fail: sendto");
            exit(1);
        }
    }

    // Dequeue the audio packets
    for (int i = 0; i < s->num_user; i++) {
        VoicePacketQueue* erase = s->vpq[i];
        s->vpq[i] = s->vpq[i]->next;
        free(erase);
    }
}

void process_audio_packets(AudioPacket* packet, struct sockaddr_storage theiraddr) {

    // Find the user and session_id associated with the packet
    if(packet->source == NULL) return;
    char* client_id = packet->source;
    User_List* user_l = find_active_user(packet->source);
    //PRINT("%s %d\n\n",packet->source, packet->packet_num);
    if (user_l->udp_addr == NULL) user_l->udp_addr = &theiraddr;
    char* sess_id = user_l->session_id;
    Session* session = find_session(sess_id);
    
    //PRINT("R %s-%d\n",packet->source, packet->packet_num);
    
    // Enqueue the audio packet
    for (int i = 0; i < session->num_user; i++) {
        // Find the correct session for the user
        if (strcmp(session->users[i]->id, packet->source) == 0) {
            // Add the packet to the session Voice Packet Queue
            if (session->vpq[i] == NULL) {
                session->vpq[i] = (VoicePacketQueue*) malloc(sizeof (VoicePacketQueue));
                session->vpq[i]->p = packet;
                session->vpq[i]->next = NULL;
            }

            // Go through the linked list until a empty list has been reached
            VoicePacketQueue* head = session->vpq[i];
            while (head->next != NULL) {
                head = head->next;
            }
            head->next = (VoicePacketQueue*) malloc(sizeof (VoicePacketQueue));
            head->next->p = packet;
            head->next->next = NULL;
            
            break;
        }
    }
    
    // Check if the VPQ has all users's packets
    bool dequeue_audio = true;
    for (int i = 0; i < session->num_user; i++) {
        if (session->vpq[i] == NULL) dequeue_audio = false;
    }

    // If all the users packets arrive, dequeue
    if (dequeue_audio) dequeue_audio_packets(session);
}