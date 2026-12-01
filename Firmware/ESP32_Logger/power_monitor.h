#pragma once
#include "data_logger.h"
void PowerMonitor_Begin();
PowerSample PowerMonitor_Read(uint32_t now);
