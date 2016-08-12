#define modelNameString     "MODEL_NAME"
#define modelNumberString   "MODEL_NUMBER"
#define numChannelsString   "NUM_CHANNELS"
#define ROCFirmwareString   "ROC_FIRMWARE"
#define AMCFirmwareString   "AMC_FIRMWARE"
#define serialNumberString  "SERIAL_NUMBER"
#define acquireString       "ACQUIRE"
#define readoutString	      "READOUT"


class CaenDig : public asynPortDriver {
public:
    CaenDig(const char *portName, int linkType, int linkNum, int conetNode, int VMEBaseAdress);

    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
    virtual void report(FILE *fp, int details);

protected:
    int modelName_;
    #define FIRST_CAENDIG_PARAM modelName_
    int modelNumber_;
    int numChannels_;
    int ROCFirmware_;
    int AMCFirmware_;
    int serialNumber_;
    int acquire_;
    int readout_;
    #define LAST_CAENDIG_PARAM acquire_

private:
    int linkType_;
    int linkNum_;
    int handle_;
    CAEN_DGTZ_BoardInfo_t boardInfo_;
};

#define NUM_PARAMS ((int)(&LAST_CAENDIG_PARAM - &FIRST_CAENDIG_PARAM + 1))
