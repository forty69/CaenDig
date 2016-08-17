#include <stdio.h>
#include "CAENDigitizer.h"

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

#define MAXNB 1

int main(int argc, char* argv[])
{
    CAEN_DGTZ_ErrorCode ret;

    int	handle[MAXNB];

    CAEN_DGTZ_BoardInfo_t BoardInfo;
    CAEN_DGTZ_EventInfo_t eventInfo;
    CAEN_DGTZ_UINT16_EVENT_t *Evt = NULL;
    char *buffer = NULL;

    int i,b;
    int c = 0, count[MAXNB];
    char * evtptr = NULL;
    uint32_t size,bsize;
    uint32_t numEvents;
    i = sizeof(CAEN_DGTZ_TriggerMode_t);

    for(b=0; b<MAXNB; b++){
        ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,b,0,0,&handle[b]);
        if(ret != CAEN_DGTZ_Success) {
            printf("Can't open digitizer\n");
            goto QuitProgram;
        }

        ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);
        printf("\nConnected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo.ModelName, b);
        printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
        printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);

        ret = CAEN_DGTZ_Reset(handle[b]);                                               /* Reset Digitizer */
        ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);                                 /* Get Board Info */
        ret = CAEN_DGTZ_SetRecordLength(handle[b],4096);                                /* Set the lenght of each waveform (in samples) */
        ret = CAEN_DGTZ_SetChannelEnableMask(handle[b],1);                              /* Enable channel 0 */
        ret = CAEN_DGTZ_SetChannelTriggerThreshold(handle[b],0,32768);                  /* Set selfTrigger threshold */
        ret = CAEN_DGTZ_SetChannelSelfTrigger(handle[b],CAEN_DGTZ_TRGMODE_ACQ_ONLY,1);  /* Set trigger on channel 0 to be ACQ_ONLY */
        ret = CAEN_DGTZ_SetSWTriggerMode(handle[b],CAEN_DGTZ_TRGMODE_ACQ_ONLY);         /* Set the behaviour when a SW tirgger arrives */
        ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle[b],3);                                /* Set the max number of events to transfer in a sigle readout */
        ret = CAEN_DGTZ_SetAcquisitionMode(handle[b],CAEN_DGTZ_SW_CONTROLLED);          /* Set the acquisition mode */
        if(ret != CAEN_DGTZ_Success) {
            printf("Errors during Digitizer Configuration.\n");
            goto QuitProgram;
        }
    }
    printf("\n\nPress 's' to start the acquisition\n");
    printf("Press 'q' to quit  the application\n\n");
    while (1) {
        c = getchar();
        if (c == 's') break;
        if (c == 'q') return;
        Sleep(100);
    }

    ret = CAEN_DGTZ_MallocReadoutBuffer(handle[0],&buffer,&size);

    int data=0;
    for(b=0; b<MAXNB; b++)
            ret = CAEN_DGTZ_SWStartAcquisition(handle[b]);
    while(1) {
        for(b=0; b<MAXNB; b++) {
            ret = CAEN_DGTZ_SendSWtrigger(handle[b]); /* Send a SW Trigger */

            ret = CAEN_DGTZ_ReadData(handle[b],CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&bsize); /* Read the buffer from the digitizer */

            ret = CAEN_DGTZ_GetNumEvents(handle[b],buffer,bsize,&numEvents);

            count[b] +=numEvents;
            for (i=0;i<numEvents;i++) {
                for (int j=0;j<=1023;j++) {
                    /* Get the Infos and pointer to the event */
                    ret = CAEN_DGTZ_GetEventInfo(handle[b],buffer,bsize,i,&eventInfo,&evtptr);

                    /* Decode the event to get the data */
                    ret = CAEN_DGTZ_DecodeEvent(handle[b],evtptr,&Evt);
                    data = Evt->DataChannel[b][j];       /* The new line that I included is here *//*****************/
                    printf(" %d",data);                  /* It acquires the data from the DataChannel ***************/
                    printf("\n");                        /* Then, it prints it. *************************************/
                    //*************************************
                    // Event Elaboration
                    //*************************************
                    ret = CAEN_DGTZ_FreeEvent(handle[b],&Evt);
                }
            }
            c = getchar();
            if (c == 's') break;
            if (c == 'q') return;
            Sleep(100);
        }
    }

Continue:
    for(b=0; b<MAXNB; b++)
        printf("\nBoard %d: Retrieved %d Events\n",b, count[b]);
    goto QuitProgram;

QuitProgram:
    ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
    for(b=0; b<MAXNB; b++)
        ret = CAEN_DGTZ_CloseDigitizer(handle[b]);
    printf("Press 'Enter' key to exit\n");
    c = getchar();
    return 0;
}
