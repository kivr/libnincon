#ifndef _N64_H_
#define _N64_H_

#include "nin_comm.h"

#define N64_RESPONSE_LENGTH 32

bool NIN_N64_requestControllerReport(ControllerReport *controllerReport);

#endif /* _N64_H_ */