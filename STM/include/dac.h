/*
 * dac.h
 * Wilson Wu, EE14, Spring 2025
 *
 */


#include "ee14lib.h"
#include <math.h> 

#define MAX_DAC 4095 //adjusts horn volume. maximum is 4095

void dac_init (void);
void trigger_horn (void);
void dma_init (void);
void gpio_init (void);
void clock_init (int freq);
void clock_set (int freq);
void create_sin_table (void);

