#include <nin_comm.h>

#define MAX_CONTROLLERS 3
#define NUM_RETRIES 3

#define CONTROLLER_ID_BASE 0
#define MESSAGE_SIZE 38

void processControllerReport(ControllerReport *report, unsigned int controllerID)
{
    //static char lastMessages[MAX_CONTROLLERS][MESSAGE_SIZE] = {0};
    char message[MESSAGE_SIZE] = {0};
    
    sprintf(message, "%1d%c%c%c%c%c%c%c%c%c%c%c%c%03d,%03d,%03d,%03d,%03d,%03d",
        controllerID,
        report->start ? 's' : 'S',
        report->y ? 'y' : 'Y',
        report->x ? 'x' : 'X',
        report->b ? 'b' : 'B',
        report->a ? 'a' : 'A',
        report->l ? 'l' : 'L',
        report->r ? 'r' : 'R',
        report->z ? 'z' : 'Z',
        report->up ? 'u' : 'U',
        report->down ? 'd' : 'D',
        report->right ? 'r' : 'R',
        report->left ? 'l' : 'L',
        report->jx_axis,
        report->jy_axis,
        report->cx_axis,
        report->cy_axis,
        report->l_axis,
        report->r_axis
        );

    //if (*lastMessages[controllerID] == 0 || strcmp(message, lastMessages[controllerID]) != 0)
    {
        Serial.begin(115200);
        Serial.print(message);
        Serial.print("\n");
        Serial.end();
        Serial.flush();
    }
}

static int port = CONTROLLER_ID_BASE;

void loop()
{
    int i;
    ControllerType type = INVALID_TYPE;
    ControllerReport report = {0};
    bool id[ID_LENGTH];

    // Toggle controller port
    port = (port == CONTROLLER_ID_BASE + MAX_CONTROLLERS - 1) ? CONTROLLER_ID_BASE : port + 1;

    NIN_setDataPortMask(1 << port);

    for (i = 0; type == INVALID_TYPE && i < NUM_RETRIES; i++)
    {
        type = NIN_identifyController(id);
    }

    if (type == UNKNOWN_TYPE)
    {
        int j;
    
        Serial.begin(115200);
        Serial.print("Unknown controller: ");
        for (j = 0; j <  ID_LENGTH; j++)
        {
            Serial.print(id[j]);
        }
        Serial.print("\n");
        Serial.end();
    }
    else if (type != INVALID_TYPE)
    {
        bool valid = false;
        
        for (i = 0; !valid && i < NUM_RETRIES; i++)
        {
            valid = NIN_requestControllerReport(&report, type);
        }
        
        if (valid)
        {
            processControllerReport(&report, port);
            delay(10);
        }
    }
}

void setup()
{
    NIN_arduinoSetDataPort('C');
}

