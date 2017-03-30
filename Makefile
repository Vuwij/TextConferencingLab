MKDIR=mkdir
CP=cp
CCADMIN=CCadmin

CC=gcc

SERVER=build/Server
CLIENT=build/Client

CFLAGS = -std=gnu11
LDFLAGS += -lasound -lm -lpthread -lssl -lcrypto

# default
default: .create_dir .client .server

.create_dir:
	mkdir -p $(CLIENT) && mkdir -p $(SERVER)

.client:
	gcc $(LDFLAGS) -std=gnu11 -o $(CLIENT)/client src/*.c src/client/*.c

.server:
	gcc $(LDFLAGS) -std=gnu11 -o $(SERVER)/server src/*.c src/server/*.c


# include project implementation makefile
include nbproject/Makefile-impl.mk

# include project make variables
include nbproject/Makefile-variables.mk
