#include <iocsh.h>
#include <epicsString.h>
#include <asynPortDriver.h>

#include <CAENDigitizer.h>

#include <epicsExport.h>
#include "CaenDig.h"

// Maximum number of digitizer input channels
#define MAX_SIGNALS 32

static const char *driverName = "CaenDig";


CaenDig::CaenDig(const char *portName, int linkType, int linkNum, int conetNode, int VMEBaseAddress)
    : asynPortDriver(portName, MAX_SIGNALS, NUM_PARAMS, 
      asynInt32Mask | asynFloat64Mask | asynOctetMask | asynDrvUserMask,
      asynInt32Mask | asynFloat64Mask | asynOctetMask,
      ASYN_MULTIDEVICE | ASYN_CANBLOCK,
      1,0,0),
      linkType_(linkType),
      linkNum_(linkNum)

{
    CAEN_DGTZ_ErrorCode errorCode;
    static const char *functionName="CaenDig";

    createParam(modelNameString,    asynParamOctet, &modelName_);
    createParam(modelNumberString,  asynParamInt32, &modelNumber_);
    createParam(numChannelsString,  asynParamInt32, &numChannels_);
    createParam(ROCFirmwareString,  asynParamOctet, &ROCFirmware_);
    createParam(AMCFirmwareString,  asynParamOctet, &AMCFirmware_);
    createParam(serialNumberString, asynParamInt32, &serialNumber_);
    
    // Open the digitizer
    errorCode = CAEN_DGTZ_OpenDigitizer((CAEN_DGTZ_ConnectionType)linkType, linkNum, conetNode, VMEBaseAddress, &handle_);
    if (errorCode != CAEN_DGTZ_Success) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s error calling CAEN_DGTZ_OpenDigitizer, error=%d\n", 
            driverName, functionName, errorCode);
        return;
    }
    
    // Get information about the board and set parameters
    errorCode = CAEN_DGTZ_GetInfo(handle_, &boardInfo_);
    if (errorCode != CAEN_DGTZ_Success) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR, 
            "%s::%s error calling CAEN_DGTZ_GetInfo, error=%d\n", 
            driverName, functionName, errorCode);
        return;
    }
    setStringParam(modelName_,      boardInfo_.ModelName);
    setIntegerParam(modelNumber_,   boardInfo_.Model);
    setIntegerParam(numChannels_,   boardInfo_.Channels);
    setStringParam(ROCFirmware_,    boardInfo_.ROC_FirmwareRel);
    setStringParam(AMCFirmware_,    boardInfo_.AMC_FirmwareRel);
    setIntegerParam(serialNumber_,  boardInfo_.SerialNumber);
}

asynStatus CaenDig::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int addr;
    int function = pasynUser->reason;
    int status = 0;
    static const char *functionName = "writeInt32";

    this->getAddress(pasynUser, &addr);
    setIntegerParam(addr, function, value);

    callParamCallbacks(addr);
    if (status == 0) {
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
            "%s::%s, port %s, wrote %d to address %d\n",
            driverName, functionName, this->portName, value, addr);
    } else {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
            "%s:%s, port %s, ERROR writing %d to address %d, status=%d\n",
            driverName, functionName, this->portName, value, addr, status);
    }
    return (status==0) ? asynSuccess : asynError;
}

asynStatus CaenDig::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    int addr;
    int function = pasynUser->reason;
    int status = 0;
    static const char *functionName = "writeFloat64";

    this->getAddress(pasynUser, &addr);
    setDoubleParam(addr, function, value);

    callParamCallbacks(addr);
    if (status == 0) {
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
            "%s::%s, port %s, wrote %f to address %d\n",
            driverName, functionName, this->portName, value, addr);
    } else {
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
            "%s:%s, port %s, ERROR writing %f to address %d, status=%d\n",
            driverName, functionName, this->portName, value, addr, status);
    }
    return (status==0) ? asynSuccess : asynError;
}

void CaenDig::report(FILE *fp, int details)
{
    fprintf(fp, "Port: %s, linkType=%d, linkNum=%d\n", 
          this->portName, linkType_, linkNum_);
    if (details >= 1) {
        fprintf(fp, "  boardInfo.ModelName=%s\n",         boardInfo_.ModelName);
        fprintf(fp, "  boardInfo.Model=%d\n",             boardInfo_.Model);
        fprintf(fp, "  boardInfo.Channels=%d\n",          boardInfo_.Channels);
        fprintf(fp, "  boardInfo.ROC_FirmwareRel=%s\n",   boardInfo_.ROC_FirmwareRel);
        fprintf(fp, "  boardInfo.AMC_FirmwareRel=%s\n",   boardInfo_.AMC_FirmwareRel);
        fprintf(fp, "  boardInfo.SerialNumber=%d\n",      boardInfo_.SerialNumber);
    }
    asynPortDriver::report(fp, details);
}

extern "C" int CaenDigConfig(const char *portName, int linkType, int linkNum, int conetNode, int VMEBaseAddress)
{
    new CaenDig(portName, linkType, linkNum, conetNode, VMEBaseAddress);
    return(asynSuccess);
}

static const iocshArg configArg0 = { "portName",      iocshArgString};
static const iocshArg configArg1 = { "linkType",      iocshArgInt};
static const iocshArg configArg2 = { "linkNum" ,      iocshArgInt};
static const iocshArg configArg3 = { "conetNode",     iocshArgInt};
static const iocshArg configArg4 = { "VMEBaseAddress",iocshArgInt};
static const iocshArg * const configArgs[] = {&configArg0, &configArg1, &configArg2, &configArg3, &configArg4};

static const iocshFuncDef configFuncDef = {"CaenDigConfig", 5, configArgs};
static void configCallFunc(const iocshArgBuf *args)
{
    CaenDigConfig(args[0].sval, args[1].ival, args[2].ival, args[3].ival, args[4].ival);
}

void drvCaenDigRegister(void)
{
    iocshRegister(&configFuncDef, configCallFunc);
}

extern "C" {
epicsExportRegistrar(drvCaenDigRegister);
}


