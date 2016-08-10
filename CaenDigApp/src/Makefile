TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

USR_INCLUDES += $(CAEN_DIGITIZER_INCLUDE)

# Build an IOC support library
LIBRARY_IOC += CaenDigSupport

# Link locally-provided code into the support library,
# rather than directly into the IOC application.
# This is required for Windows DLL builds.
CaenDigSupport_SRCS += CaenDig.cpp

CaenDigSupport_LIBS += asyn
CaenDigSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

# Build the IOC application
PROD_IOC = CaenDig

# CaenDig.dbd will be created and installed
DBD += CaenDig.dbd

# CaenDig.dbd will include these files:
CaenDig_DBD += base.dbd
CaenDig_DBD += asyn.dbd
CaenDig_DBD += CaenDigSupport.dbd

# CaenDig_registerRecordDeviceDriver.cpp derives from CaenDig.dbd
CaenDig_SRCS += CaenDig_registerRecordDeviceDriver.cpp

# Build the main IOC entry point where needed
CaenDig_SRCS_DEFAULT += CaenDigMain.cpp
CaenDig_SRCS_vxWorks += -nil-

# Link in the code from our support library
CaenDig_LIBS += CaenDigSupport

# Other support libraries
CaenDig_LIBS += asyn

# Vendor libraries
CAENDigitizer_DIR = $(CAEN_DIGITIZER_LIB)
CaenDig_LIBS += CAENDigitizer

# Finally link IOC to the EPICS Base libraries
CaenDig_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE

