#include "n64.h"

static void fillReport(bool *data, ControllerReport *controllerReport)
{
    // Joystick X axis
    controllerReport->jx_axis = NIN_getAxisValue(&data[16]);
	controllerReport->jx_axis += 128;

    // Joystick Y axis
    controllerReport->jy_axis = NIN_getAxisValue(&data[24]);
	controllerReport->jy_axis += 128;

	// C left and right
	controllerReport->cx_axis = data[14] ? 0 :
			  (data[15] ? 255 : 128);

	// C up and down
	controllerReport->cy_axis = data[13] ? 0 :
			  (data[12] ? 255 : 128);

	// Buttons
    controllerReport->up = data[4];
	controllerReport->down = data[5];
	controllerReport->left = data[6];
	controllerReport->right = data[7];
	
	controllerReport->start = data[3];
	
    controllerReport->a = data[0];
	controllerReport->b = data[1];
	controllerReport->z = data[2];
	
	controllerReport->l = data[10];
	controllerReport->r = data[11];
}

bool NIN_N64_requestControllerReport(ControllerReport *controllerReport)
{
    bool valid;
    unsigned int bitsRead;
    DATA_SAMPLE_TYPE samples[BUF_SIZE];
    bool data[N64_RESPONSE_LENGTH];

    // Send request
    START_TRANSMISSION;

    ZERO;
    ZERO;
    ZERO;
    ZERO;

    ZERO;
    ZERO;
    ZERO;
    ONE;

    STOP;
    
    STOP_TRANSMISSION;

    SAMPLE_DATA(samples);
    
    bitsRead = NIN_parseData(data, N64_RESPONSE_LENGTH, samples);
    
    valid = (bitsRead == N64_RESPONSE_LENGTH);
    
    if (valid)
    {
        fillReport(data, controllerReport);
    }
    
    return valid;
}