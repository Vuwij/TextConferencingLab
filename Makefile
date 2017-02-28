MKDIR=mkdir
CP=cp
CCADMIN=CCadmin

SERVER=build/Server
CLIENT=build/Client

# default
default: .create_dir .client .server

.create_dir:
	mkdir -p $(CLIENT) && mkdir -p $(SERVER)

.client:
	gcc -std=gnu11 -o $(CLIENT)/client src/*.c src/client/*.c

.server:
	gcc -std=gnu11 -o $(SERVER)/server src/*.c src/server/*.c


# include project implementation makefile
include nbproject/Makefile-impl.mk

# include project make variables
include nbproject/Makefile-variables.mk
