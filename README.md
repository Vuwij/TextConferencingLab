# Voice-Conferencing

#### Authors
###### Jason Wang, 1001250569, jiashen.wang@mail.utoronto.ca
###### Seunghyun Cho, 1001681325, chos.pey@gmail.com


This is a simple voice-conferencing project using ALSA and openSSL libraries. We used the UDP and the TCP ports to be able to support up to 100 clients

We implemented audio sequencing using a simple linked list. This project is meant to be only for simple demonstration purposes

To compile the project type the below command

``
make
``

To run the server type the below command

``
build/Server/server <server port to listen to>
``

To run the client type the below the command

``
build/Client/client [client number]
``

*The following commands are available for the client*

**/login <client ID> <password> <server-IP> <server-port>**
- Logs the user in to the specified server IP address and port

**/logout**

**/joinsession <session ID>**

**/leavesession**

**/createsession <session ID>**

**/startcall**

**/joincall**

**/list**

**/quit**

**<text>**
