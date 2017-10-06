#ifndef _COMM_H_
#define _COMM_H_

#ifdef ARDUINO
#include "platform/arduino.h"
#endif

#ifndef DATA_PORT_TYPE
#error "DATA_PORT_TYPE should be defined."
#endif

#ifndef DATA_SAMPLE_TYPE
#error "DATA_SAMPLE_TYPE should be defined."
#endif

#ifndef NIN_SLEEP1
#error "NIN_SLEEP1 should be defined."
#endif

#ifndef NIN_SLEEP3
#error "NIN_SLEEP3 should be defined."
#endif

#ifndef BUF_SIZE
#error "BUF_SIZE should be defined."
#endif

#ifndef START_TRANSMISSION
#warning "START_TRANSMISSION is not defined."
#endif

#ifndef STOP_TRANSMISSION
#warning "STOP_TRANSMISSION is not defined."
#endif

#define NIN_DATA_LOW \
            *g_outDataPortAddress &= ~g_dataPortMask
            
#define NIN_DATA_HIGH \
            *g_outDataPortAddress |= g_dataPortMask

#define ZERO \
            NIN_DATA_LOW; \
            NIN_SLEEP3; \
            NIN_DATA_HIGH; \
            NIN_SLEEP1
#define ONE \
            NIN_DATA_LOW; \
            NIN_SLEEP1; \
            NIN_DATA_HIGH; \
            NIN_SLEEP3
#define STOP \
            NIN_DATA_LOW; \
            NIN_SLEEP1; \
            NIN_DATA_HIGH

#define ID_MESSAGE_LENGTH 24
#define ID_LENGTH 16
            
typedef enum _ControllerType
{
    INVALID_TYPE = 0,
    N64_TYPE,
    GC_TYPE,
    GC_WB_TYPE,
    UNKNOWN_TYPE,
    LAST_TYPE
} ControllerType;

typedef struct _ControllerReport
{
    bool up;
    bool down;
    bool left;
    bool right;
    
    bool start;
    
    bool a;
    bool b;
    bool x;
    bool y;
    bool z;
    
    bool c_up;
    bool c_down;
    bool c_left;
    bool c_right;
    
    bool l;
    bool r;
    
    uint8_t jx_axis;
    uint8_t jy_axis;
    
    uint8_t cx_axis;
    uint8_t cy_axis;
    
    uint8_t l_axis;
    uint8_t r_axis;
} ControllerReport;

extern const char* NIN_controllerNames[LAST_TYPE];
extern DATA_PORT_TYPE *g_outDataPortAddress;
extern DATA_PORT_TYPE *g_inDataPortAddress;
extern int g_dataPortMask;

void NIN_setOutDataPort(DATA_PORT_TYPE *dataPort);
void NIN_setInDataPort(DATA_PORT_TYPE *dataPort);
void NIN_setDataPortMask(int dataPortMask);

uint8_t NIN_getAxisValue(bool *data);

unsigned int NIN_parseData(bool *data, int dataLength, DATA_SAMPLE_TYPE *samples);
ControllerType NIN_identifyController(bool *id);
bool NIN_requestControllerReport(ControllerReport *controllerReport, ControllerType type);

#endif /* _COMM_H_ */