#include "game_cube.h"

static void fillReport(bool *data, ControllerReport *controllerReport)
{
    // Joystick X axis
    controllerReport->jx_axis = NIN_getAxisValue(&data[16]);

    // Joystick Y axis
    controllerReport->jy_axis = NIN_getAxisValue(&data[24]);

    // C left and right
    controllerReport->cx_axis = NIN_getAxisValue(&data[32]);

    // C up and down
    controllerReport->cy_axis = NIN_getAxisValue(&data[40]);
	
	// L trigger
    controllerReport->l_axis = NIN_getAxisValue(&data[48]);
	
	// R trigger
    controllerReport->r_axis = NIN_getAxisValue(&data[56]);

	// Buttons
    controllerReport->up = data[12];
	controllerReport->down = data[13];
	controllerReport->left = data[15];
	controllerReport->right = data[14];
	
	controllerReport->start = data[3];
	
    controllerReport->a = data[7];
	controllerReport->b = data[6];
	controllerReport->x = data[5];
	controllerReport->y = data[4];
	controllerReport->z = data[11];
	
	controllerReport->l = data[9];
	controllerReport->r = data[10];
}

bool NIN_GC_requestControllerReport(ControllerReport *controllerReport)
{
    bool valid;
    unsigned int bitsRead;
    DATA_SAMPLE_TYPE samples[BUF_SIZE];
    bool data[GC_RESPONSE_LENGTH];

    // Send request
    START_TRANSMISSION;
        
    ZERO;
    ONE;
    ZERO;
    ZERO;

    ZERO;
    ZERO;
    ZERO;
    ZERO;

    ZERO;
    ZERO;
    ZERO;
    ZERO;

    ZERO;
    ZERO;
    ONE;
    ONE;

    ZERO;
    ZERO;
    ZERO;
    ZERO;

    ZERO;
    ZERO;
    ONE;
    ZERO;

    STOP;
    
    STOP_TRANSMISSION;

    SAMPLE_DATA(samples);
    
    bitsRead = NIN_parseData(data, GC_RESPONSE_LENGTH, samples);
    
    valid = (bitsRead == GC_RESPONSE_LENGTH);
    
    if (valid)
    {
        fillReport(data, controllerReport);
    }
    
    return valid;
}