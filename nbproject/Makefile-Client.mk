#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Client
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/client/audio_input.o \
	${OBJECTDIR}/src/client/audio_output.o \
	${OBJECTDIR}/src/client/client.o \
	${OBJECTDIR}/src/client/client_command.o \
	${OBJECTDIR}/src/message.o \
	${OBJECTDIR}/src/print_debug.o \
	${OBJECTDIR}/src/server/audio_port.o \
	${OBJECTDIR}/src/server/message_handle.o \
	${OBJECTDIR}/src/server/server.o \
	${OBJECTDIR}/src/server/session_list.o \
	${OBJECTDIR}/src/server/user_list.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs alsa`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/textconferencinglab

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/textconferencinglab: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/textconferencinglab ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/client/audio_input.o: src/client/audio_input.c 
	${MKDIR} -p ${OBJECTDIR}/src/client
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/client/audio_input.o src/client/audio_input.c

${OBJECTDIR}/src/client/audio_output.o: src/client/audio_output.c 
	${MKDIR} -p ${OBJECTDIR}/src/client
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/client/audio_output.o src/client/audio_output.c

${OBJECTDIR}/src/client/client.o: src/client/client.c 
	${MKDIR} -p ${OBJECTDIR}/src/client
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/client/client.o src/client/client.c

${OBJECTDIR}/src/client/client_command.o: src/client/client_command.c 
	${MKDIR} -p ${OBJECTDIR}/src/client
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/client/client_command.o src/client/client_command.c

${OBJECTDIR}/src/message.o: src/message.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/message.o src/message.c

${OBJECTDIR}/src/print_debug.o: src/print_debug.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/print_debug.o src/print_debug.c

${OBJECTDIR}/src/server/audio_port.o: src/server/audio_port.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/server/audio_port.o src/server/audio_port.c

${OBJECTDIR}/src/server/message_handle.o: src/server/message_handle.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/server/message_handle.o src/server/message_handle.c

${OBJECTDIR}/src/server/server.o: src/server/server.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/server/server.o src/server/server.c

${OBJECTDIR}/src/server/session_list.o: src/server/session_list.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/server/session_list.o src/server/session_list.c

${OBJECTDIR}/src/server/user_list.o: src/server/user_list.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g `pkg-config --cflags alsa`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/server/user_list.o src/server/user_list.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/textconferencinglab

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
