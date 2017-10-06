#include "nin_comm.h"
#include "n64.h"
#include "game_cube.h"
#include <stdbool.h>
#include <string.h>

const char* NIN_controllerNames[LAST_TYPE] =
{
  "Unknown",
  "Invalid",
  "N64",
  "GameCube"
};

DATA_PORT_TYPE *g_outDataPortAddress = NULL;
DATA_PORT_TYPE *g_inDataPortAddress = NULL;
int g_dataPortMask = 0;

void NIN_setOutDataPort(DATA_PORT_TYPE *dataPort)
{
    g_outDataPortAddress = dataPort;
}

void NIN_setInDataPort(DATA_PORT_TYPE *dataPort)
{
    g_inDataPortAddress = dataPort;
}

void NIN_setDataPortMask(int dataPortMask)
{
    g_dataPortMask = dataPortMask;
}

uint8_t NIN_getAxisValue(bool *data)
{
    uint8_t out;
    int i, j;

    for (i = 0, j = 0; i < 8; i++, j++)
    {
        if (data[i])
        {
            out |= (0x80 >> j);
        }
        else
        {
            out &= ~(0x80 >> j);
        }
    }

    return out;
}

unsigned int NIN_parseData(bool *data, int dataLength, DATA_SAMPLE_TYPE *samples)
{
    int i, j, prev, maxdiff = 0, mindiff = BUF_SIZE;

    /* extract correct bit sequence (for each pad) from sampled data */
    /* locate first falling edge */
    for (i = 0; i < BUF_SIZE; i++) {
        if ((samples[i] & g_dataPortMask) == 0)
            break;
    }

    prev = i;
    j = 0;

    while ((i - prev) < (2 * mindiff) && i < BUF_SIZE) {
        i++;
        /* detect consecutive falling edges */
        if ((samples[i-1] & g_dataPortMask) && !(samples[i] & g_dataPortMask)) {
            /* update min&max diffs */
            if (i - prev > maxdiff)
            {
                maxdiff = i - prev;
            }

            if (i - prev < mindiff)
            {
                mindiff = i - prev;
            }

            /* data is taken between 2 falling edges */
            data[j] = (samples[prev+((i-prev)/2)] & g_dataPortMask) ? 1 : 0;
            j++;
            prev = i;
        }
    }

    return (maxdiff < 2 * mindiff) ? j : 0;
}

ControllerType NIN_identifyController(bool *id)
{
    ControllerType retValue = INVALID_TYPE;
    DATA_SAMPLE_TYPE samples[BUF_SIZE];
    bool data[ID_MESSAGE_LENGTH];
    
    static const bool gc_wavebird_id[ID_LENGTH] =
        {1, 1, 1, 0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0};
    static const bool gc_id[ID_LENGTH] =
        {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
    static const bool n64_id[ID_LENGTH] =
        {0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};
        
      unsigned int bitsRead;

    START_TRANSMISSION;
  
    // Send request
    ZERO;
    ZERO;
    ZERO;
    ZERO;
    
    ZERO;
    ZERO;
    ZERO;
    ZERO;
    
    STOP;
    
    STOP_TRANSMISSION;

    SAMPLE_DATA(samples);

    bitsRead = NIN_parseData(data, ID_MESSAGE_LENGTH, samples);
	
	int i;

    if (bitsRead == ID_MESSAGE_LENGTH)
    {
        if (memcmp(gc_id, data, sizeof(bool) * ID_LENGTH) == 0)
        {
            retValue = GC_TYPE;
        }
		else if (memcmp(gc_wavebird_id, data, sizeof(bool) * ID_LENGTH) == 0)
        {
            retValue = GC_WB_TYPE;
        }
        else if (memcmp(n64_id, data, sizeof(bool) * ID_LENGTH) == 0)
        {
            retValue = N64_TYPE;
        }
		else if (id != NULL)
		{
			memcpy(id, data, ID_LENGTH);
			retValue = UNKNOWN_TYPE;
		}
    }
    
    return retValue;
}

bool NIN_requestControllerReport(ControllerReport *controllerReport, ControllerType type)
{
	bool retValue = false;
	
	if (type == N64_TYPE)
	{
		retValue = NIN_N64_requestControllerReport(controllerReport);
	}
	else if (type == GC_TYPE || type == GC_WB_TYPE)
	{
		retValue = NIN_GC_requestControllerReport(controllerReport, type);
	}
	
	return retValue;
}
