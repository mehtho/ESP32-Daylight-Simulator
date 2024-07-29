#ifndef PERSISTENCE_H_INCLUDED
#define PERSISTENCE_H_INCLUDED

#include "stdint.h"
#include "stdbool.h"
#include "esp_check.h"

void persistence_write_lights(const uint32_t r, const uint32_t g, const uint32_t b);

bool persistence_read_lights(uint32_t *r, uint32_t *g, uint32_t *b);

#endif