#ifndef _GAME_CUBE_H_
#define _GAME_CUBE_H_

#include "nin_comm.h"

#define GC_RESPONSE_LENGTH 64

bool NIN_GC_requestControllerReport(ControllerReport *controllerReport);

#endif /* _GAME_CUBE_H_ */