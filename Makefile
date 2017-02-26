MKDIR=mkdir
CP=cp
CCADMIN=CCadmin

SERVER=build/Server
CLIENT=build/Client

# default
default: .create_dir .server .client

.create_dir:
	mkdir -p $(CLIENT) && mkdir -p $(SERVER)

.client:
	gcc -o $(CLIENT)/client src/client/*.c

.server:
	gcc -o $(SERVER)/server src/server/*.c


# include project implementation makefile
include nbproject/Makefile-impl.mk

# include project make variables
include nbproject/Makefile-variables.mk
