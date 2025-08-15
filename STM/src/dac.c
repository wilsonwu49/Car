/*
 * dac.c
 * Wilson Wu, EE14, Spring 2025
 *
 */

#include "ee14lib.h"
#include "dac.h"

uint16_t sin_table[100];

// Initialize the DAC for the STM32L432KC:
// Enables the DAC for use, configures a TIM6 trigger, 
// normal output mode, external pin with buffer
// Configured for DAC channel 1, uses PA4 or A3 on board
void dac_init (void) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;

    // Enable GPIO PA4 (A3)
    gpio_init();

    // Disable DAC
    DAC1->CR &= ~DAC_CR_EN1;
    // DAC Channel 1 trigger enabled
    DAC1->CR |= DAC_CR_TEN1;
    // DAC Trigger Selection, TIM6_TRGO
    DAC1->CR &= ~DAC_CR_TSEL1_Msk;
    // Disable waveform generation, 
    DAC1->CR &= ~DAC_CR_WAVE1;

    // External for Speaker, Buffer to Amplify the Signal
    // Also, Enables normal mode for the DAC (0xx)
    // 000: DAC channel1 is connected to external pin with Buffer enabled
    DAC1->MCR &= ~DAC_MCR_MODE1;
    // Enables DAC Channel 1
    DAC1->CR |= DAC_CR_EN1;
}

// Horn, have to set delay for beep-beep?
void trigger_horn (void) {
    DMA1_Channel3->CCR |= DMA_CCR_EN;
    delay(200);
    DMA1_Channel3->CCR &= ~(DMA_CCR_EN);
}

// Initializes the DMA for DAC use, uses a 16 bit transfer for uint16_t of the
// sin table, uses memory to peripheral, goes to right holding register of DAC
// 100 number of data, Circular to allow for continuous noise production while
// car is running by enabling and disabling DMA
void dma_init (void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    // Disable 
    DMA1_Channel3->CCR &= ~(DMA_CCR_EN_Msk);

    // Channel 3, CxS[3:0] - 0110 => DAC_CH1
    DMA1_CSELR->CSELR &= ~(DMA_CSELR_C3S_Msk);
    DMA1_CSELR->CSELR |= (0b0110 << DMA_CSELR_C3S_Pos);

    // DMA Memory Size Transfer = 16 bits
    DMA1_Channel3->CCR &= ~(DMA_CCR_MSIZE_Msk);
    DMA1_Channel3->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel3->CCR &= ~(DMA_CCR_PSIZE_Msk);
    DMA1_Channel3->CCR |= DMA_CCR_PSIZE_0;
    // Memory to Peripheral
    DMA1_Channel3->CCR |= DMA_CCR_DIR;
    // Memory Increment
    DMA1_Channel3->CCR |= DMA_CCR_MINC;
    // Circular
    DMA1_Channel3->CCR |= DMA_CCR_CIRC;

    // Number of Data to Transfer
    DMA1_Channel3->CNDTR = 100;
    // 
    DMA1_Channel3->CMAR = (uint32_t)sin_table;
    DMA1_Channel3->CPAR = (uint32_t)&DAC1->DHR12R1;

    // Enable
    //DMA1_Channel3->CCR |= DMA_CCR_EN;
}

// Intialize A3 as output pin for DAC
void gpio_init (void) {
    // GPIO LOGIC
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    // Set PA4 (A3) to 11 for analog mode
    gpio_config_mode(A3, 0b11);
    // Disable Digital Logic
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD4_Msk;
}

// Write a single value to the DAC Holding Register
// Takes in a 12 bit integer and stores it left shifted
// void dac_write (int input) {
//     // If out of range of a 12 bit register
//     if (input < 0) {
//         input = 0;
//     }
//     if (input > 4095) {
//         input = 4095;
//     }

//     // Left shifted register for higher resolution
//     // Sets 12 bits of Lower 16 bits of 32 bit register
//     // Left Shifted Register = Higher resolution = * 16
//     // Right Shifted Register = Direct Input
//     DAC1->DHR12L1 = input;
//     DAC1->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
// }

// MAX FREQUENCY OF 4KHZ
void clock_init (int freq) {
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;

    freq = freq * 100;
    // TIM6 = CLK / (PSC + 1)*(ARR + 1) 
    uint16_t psc = (4000000 / (freq * 65535));
    TIM6->PSC = psc;
    TIM6->ARR = (4000000 / (freq * (psc + 1))) - 1;

    // Enable Update DMA Request, will generate a DMA request on update events
    TIM6->DIER |= TIM_DIER_UDE;

    // Enable TIM6 Master Mode - Update event is selected as TRGO
    TIM6->CR2 &= ~(TIM_CR2_MMS_Msk);
    TIM6->CR2 |= TIM_CR2_MMS_1;

    // Enable TIM6 Clock
    TIM6->CR1 |= TIM_CR1_CEN;
}

// Set the frequency of the clock while DMA/DAC are running
void clock_set (int freq) {
    // Disable TIM6 Clock
    TIM6->CR1 &= ~(TIM_CR1_CEN_Msk);
    freq = freq * 100;

    // Update TIM6 values
    // TIM6 = CLK / (PSC + 1)*(ARR + 1) 
    uint16_t psc = (40000000 / (freq * 65535)) - 1;
    TIM6->PSC = psc;
    TIM6->ARR = (40000000 / (freq * (psc + 1))) - 1;

    // Enable TIM6 Clock
    TIM6->CR1 |= TIM_CR1_CEN;
}

// Create a table for the sin function, sampling 100 times per period
void create_sin_table (void) {
    for(int i = 0; i < 300; i++) { 
        uint16_t temp = MAX_DAC / 2 * (sin (3.14159 / 50 * i) + 1);
        sin_table[i] = temp;
    }
}
