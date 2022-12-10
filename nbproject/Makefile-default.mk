#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile

# Environment
SHELL=cmd.exe
# Adding MPLAB X bin directory to path
PATH:=C:/Program Files (x86)/Microchip/MPLABX/mplab_ide/mplab_ide/modules/../../bin/:$(PATH)
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/TKLL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/TKLL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/button_matrix/button.o ${OBJECTDIR}/lcd/lcd.o ${OBJECTDIR}/interrupt/interrupt.o ${OBJECTDIR}/timer/timer.o ${OBJECTDIR}/i2c/i2c.o ${OBJECTDIR}/main.o ${OBJECTDIR}/menu/menu.o ${OBJECTDIR}/uart/uart.o
POSSIBLE_DEPFILES=${OBJECTDIR}/button_matrix/button.o.d ${OBJECTDIR}/lcd/lcd.o.d ${OBJECTDIR}/interrupt/interrupt.o.d ${OBJECTDIR}/timer/timer.o.d ${OBJECTDIR}/i2c/i2c.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/menu/menu.o.d ${OBJECTDIR}/uart/uart.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/button_matrix/button.o ${OBJECTDIR}/lcd/lcd.o ${OBJECTDIR}/interrupt/interrupt.o ${OBJECTDIR}/timer/timer.o ${OBJECTDIR}/i2c/i2c.o ${OBJECTDIR}/main.o ${OBJECTDIR}/menu/menu.o ${OBJECTDIR}/uart/uart.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH="C:\Program Files\Java\jre6/bin/"
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC="C:\Program Files (x86)\Microchip\mplabc18\v3.47\bin\mcc18.exe"
# MP_BC is not defined
MP_AS="C:\Program Files (x86)\Microchip\mplabc18\v3.47\bin\..\mpasm\MPASMWIN.exe"
MP_LD="C:\Program Files (x86)\Microchip\mplabc18\v3.47\bin\mplink.exe"
MP_AR="C:\Program Files (x86)\Microchip\mplabc18\v3.47\bin\mplib.exe"
DEP_GEN=${MP_JAVA_PATH}java -jar "C:/Program Files (x86)/Microchip/MPLABX/mplab_ide/mplab_ide/modules/../../bin/extractobjectdependencies.jar" 
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps
MP_CC_DIR="C:\Program Files (x86)\Microchip\mplabc18\v3.47\bin"
# MP_BC_DIR is not defined
MP_AS_DIR="C:\Program Files (x86)\Microchip\mplabc18\v3.47\bin\..\mpasm"
MP_LD_DIR="C:\Program Files (x86)\Microchip\mplabc18\v3.47\bin"
MP_AR_DIR="C:\Program Files (x86)\Microchip\mplabc18\v3.47\bin"
# MP_BC_DIR is not defined

.build-conf:  ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/TKLL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F4620
MP_PROCESSOR_OPTION_LD=18f4620
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0xfdc0 -u_DEBUGCODELEN=0x240 -u_DEBUGDATASTART=0xef4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/button_matrix/button.o: button_matrix/button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/button_matrix 
	@${RM} ${OBJECTDIR}/button_matrix/button.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/button_matrix/button.o   button_matrix/button.c 
	@${DEP_GEN} -d ${OBJECTDIR}/button_matrix/button.o 
	
${OBJECTDIR}/lcd/lcd.o: lcd/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/lcd 
	@${RM} ${OBJECTDIR}/lcd/lcd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/lcd/lcd.o   lcd/lcd.c 
	@${DEP_GEN} -d ${OBJECTDIR}/lcd/lcd.o 
	
${OBJECTDIR}/interrupt/interrupt.o: interrupt/interrupt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/interrupt 
	@${RM} ${OBJECTDIR}/interrupt/interrupt.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/interrupt/interrupt.o   interrupt/interrupt.c 
	@${DEP_GEN} -d ${OBJECTDIR}/interrupt/interrupt.o 
	
${OBJECTDIR}/timer/timer.o: timer/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/timer 
	@${RM} ${OBJECTDIR}/timer/timer.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/timer/timer.o   timer/timer.c 
	@${DEP_GEN} -d ${OBJECTDIR}/timer/timer.o 
	
${OBJECTDIR}/i2c/i2c.o: i2c/i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/i2c 
	@${RM} ${OBJECTDIR}/i2c/i2c.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/i2c/i2c.o   i2c/i2c.c 
	@${DEP_GEN} -d ${OBJECTDIR}/i2c/i2c.o 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/main.o   main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/main.o 
	
${OBJECTDIR}/menu/menu.o: menu/menu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/menu 
	@${RM} ${OBJECTDIR}/menu/menu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/menu/menu.o   menu/menu.c 
	@${DEP_GEN} -d ${OBJECTDIR}/menu/menu.o 
	
${OBJECTDIR}/uart/uart.o: uart/uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/uart 
	@${RM} ${OBJECTDIR}/uart/uart.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/uart/uart.o   uart/uart.c 
	@${DEP_GEN} -d ${OBJECTDIR}/uart/uart.o 
	
else
${OBJECTDIR}/button_matrix/button.o: button_matrix/button.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/button_matrix 
	@${RM} ${OBJECTDIR}/button_matrix/button.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/button_matrix/button.o   button_matrix/button.c 
	@${DEP_GEN} -d ${OBJECTDIR}/button_matrix/button.o 
	
${OBJECTDIR}/lcd/lcd.o: lcd/lcd.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/lcd 
	@${RM} ${OBJECTDIR}/lcd/lcd.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/lcd/lcd.o   lcd/lcd.c 
	@${DEP_GEN} -d ${OBJECTDIR}/lcd/lcd.o 
	
${OBJECTDIR}/interrupt/interrupt.o: interrupt/interrupt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/interrupt 
	@${RM} ${OBJECTDIR}/interrupt/interrupt.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/interrupt/interrupt.o   interrupt/interrupt.c 
	@${DEP_GEN} -d ${OBJECTDIR}/interrupt/interrupt.o 
	
${OBJECTDIR}/timer/timer.o: timer/timer.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/timer 
	@${RM} ${OBJECTDIR}/timer/timer.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/timer/timer.o   timer/timer.c 
	@${DEP_GEN} -d ${OBJECTDIR}/timer/timer.o 
	
${OBJECTDIR}/i2c/i2c.o: i2c/i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/i2c 
	@${RM} ${OBJECTDIR}/i2c/i2c.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/i2c/i2c.o   i2c/i2c.c 
	@${DEP_GEN} -d ${OBJECTDIR}/i2c/i2c.o 
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR} 
	@${RM} ${OBJECTDIR}/main.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/main.o   main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/main.o 
	
${OBJECTDIR}/menu/menu.o: menu/menu.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/menu 
	@${RM} ${OBJECTDIR}/menu/menu.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/menu/menu.o   menu/menu.c 
	@${DEP_GEN} -d ${OBJECTDIR}/menu/menu.o 
	
${OBJECTDIR}/uart/uart.o: uart/uart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/uart 
	@${RM} ${OBJECTDIR}/uart/uart.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"C:/Users/PC/Desktop/Digital Clock"  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/uart/uart.o   uart/uart.c 
	@${DEP_GEN} -d ${OBJECTDIR}/uart/uart.o 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/TKLL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" -l"C:/Program Files (x86)/Microchip/mplabc18/v3.47/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PICKIT2=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/TKLL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/TKLL.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w  -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" -l"C:/Program Files (x86)/Microchip/mplabc18/v3.47/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/TKLL.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
