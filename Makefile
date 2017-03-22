MKDIR=mkdir
CP=cp
CCADMIN=CCadmin

CC=gcc
SERVER=build/Server
CLIENT=build/Client

CFLAGS = -std=gnu11
LDFLAGS += -lasound -lm -lpthread

# default
default: .create_dir .client .server

.create_dir:
	mkdir -p $(CLIENT) && mkdir -p $(SERVER)

.client:
	gcc $(CFLAGS) -o $(CLIENT)/client src/*.c src/client/*.c $(LDFLAGS)

.server:
	gcc $(CFLAGS) -o $(SERVER)/server -lasound src/*.c src/server/*.c $(LDFLAGS)


# include project implementation makefile
include nbproject/Makefile-impl.mk

# include project make variables
include nbproject/Makefile-variables.mk
