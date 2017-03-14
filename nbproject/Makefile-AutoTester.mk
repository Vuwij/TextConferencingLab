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
CND_CONF=AutoTester
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES=


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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/textconferencinglab

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/textconferencinglab: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/textconferencinglab ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/src/client/client.c.gch: src/client/client.c 
	${MKDIR} -p ${OBJECTDIR}/src/client
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o "$@" src/client/client.c

${OBJECTDIR}/src/client/client_command.c.gch: src/client/client_command.c 
	${MKDIR} -p ${OBJECTDIR}/src/client
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o "$@" src/client/client_command.c

${OBJECTDIR}/src/message.c.gch: src/message.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o "$@" src/message.c

${OBJECTDIR}/src/print_debug.c.gch: src/print_debug.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o "$@" src/print_debug.c

${OBJECTDIR}/src/server/message_handle.c.gch: src/server/message_handle.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o "$@" src/server/message_handle.c

${OBJECTDIR}/src/server/server.c.gch: src/server/server.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o "$@" src/server/server.c

${OBJECTDIR}/src/server/session_list.c.gch: src/server/session_list.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o "$@" src/server/session_list.c

${OBJECTDIR}/src/server/user_list.c.gch: src/server/user_list.c 
	${MKDIR} -p ${OBJECTDIR}/src/server
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o "$@" src/server/user_list.c

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
