TOP=..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
EPICS_BASE_PVA_CORE_LIBS = pvaClient pvAccess pvData Com

PROD_HOST += numericScalar
numericScalar_SRCS += numericScalar.cpp
numericScalar_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += alarm
alarm_SRCS += alarm.cpp
alarm_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += timeStamp
timeStamp_SRCS += timeStamp.cpp
timeStamp_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += stringScalar
stringScalar_SRCS += stringScalar.cpp
stringScalar_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += intScalar
intScalar_SRCS += intScalar.cpp
intScalar_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += enum
enum_SRCS += enum.cpp
enum_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += doubleArray
doubleArray_SRCS += doubleArray.cpp
doubleArray_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += intArray
intArray_SRCS += intArray.cpp
intArray_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += stringArray
stringArray_SRCS += stringArray.cpp
stringArray_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += intScalarNonValue
intScalarNonValue_SRCS += intScalarNonValue.cpp
intScalarNonValue_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += monitor
monitor_SRCS += monitor.cpp
monitor_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += getNoBlock
getNoBlock_SRCS += getNoBlock.cpp
getNoBlock_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_HOST += putNoBlock
putNoBlock_SRCS += putNoBlock.cpp
putNoBlock_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

PROD_SYS_LIBS_WIN32 += ws2_32


#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

