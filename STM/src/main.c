/*
 * timer.c
 * Alisa Yurevich, Wilson Wu, Duncan DeFonce, EE14, Spring 2025
 *
 */

#include "ee14lib.h"
#include "car.h"
#include "dac.h"
#include <stdio.h>
volatile int INTERRUPT_COUNTER;
volatile bool performed = 0;//we only want to perform each command once 
unsigned char data[1]; 

int clamp(int value){
    if(value < MIN_DUTY){
        return MIN_DUTY-99;
    }else if (value > MAX_DUTY){
        return MAX_DUTY;
    }else{
        return value;
    }
}

int _write(int file, char *data, int len) {
    serial_write(USART2, data, len);
    return len;
}

// delay
// Input: integer of a certain amount of time to wait
// No output
// The function takes in a certain integer and runs a for loop for 1000 * wait
// to delay the clock cycle for a certain amount.
void delay(int ms) {
    volatile int target = INTERRUPT_COUNTER + ms;
    while(INTERRUPT_COUNTER <= target);
}

/*I2C1_EV
* Called whenever a) we receive an I2C address or b) if we 
* receive something else over I2C
**/
void I2C1_EV_IRQHandler(void){
    if(I2C1->ISR & I2C_ISR_ADDR){
        volatile uint32_t temp = I2C1->ISR; //read isr
        (void)temp;
        I2C1->ICR |= I2C_ICR_ADDRCF; //clear addr flag
    }

    if(I2C1->ISR & I2C_ISR_RXNE){ //recieved data
        data[0] = I2C1->RXDR;
        performed = false;
    }

    //clear nack 
    if(I2C1->ISR & I2C_ISR_NACKF){
        I2C1->ICR = I2C_ICR_NACKCF;
    }  
}

/*
 * SysTick_initialize
 * purpose : generate an interrupt which goes high every 1ms
 */
void SysTick_initialize(void) {
    SysTick->CTRL = 0;//turn enable off so we can configure it

    //What value do we want to count down from?
    SysTick->LOAD = 3999; //to make 4MHz down to 100kHz (T = 1ms)

    // This sets the priority of the interrupt to 15 (2^4 - 1), which is the
    // largest supported value (aka lowest priority)
    NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
    
    //set the current value of the counter to 0
    SysTick->VAL = 0;
    //clear all values of the register to 0, but keep following vals the same
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;//which timer are we using (AHB or processor)
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;//Systick exception request enable
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//counter enable on
}

/*
 * SysTick_Handler
 * purpose : count how many times the Systick interrupt has occured in a global
 * variable. IT MUST BE NAMED SysTick_Handler for linker to automatically use it
 */
void  SysTick_Handler(void) {
    INTERRUPT_COUNTER++;
}

/**
 * perform_command
 * Description : main logic for performing each function. Performs each action 
 * exactly once by using the performed bool. */

void perform_command(){
    if(performed == false){
        if(data[0] == DRIVE){
            performed = drive();
        }else if (data[0] == ACCELERATE){
            performed = accelerate();
        }else if(data[0] == BRAKE){// brake = decelerate
            performed = brake();
        }else if(data[0] == STOP){//come to a full stop
            performed = stop();
        }else if(data[0] == TURN_LEFT){
            performed = turn(LEFT);
        }else if (data[0] == TURN_RIGHT){
            performed = turn(RIGHT);
        } else if (data[0] == REVERSE){
            performed = reverse();
        }else if(data[0] == HONK){
            performed = honk();
        }
    }
}

//Max frequency : 4KHz. 
void config_dac_all(int frequency_hz){
    clock_init(frequency_hz); //440 Hz is an A
    create_sin_table();//generate a discrete sin f. for the DAC to spit out

    gpio_init(); //set A3 as output DAC pin
    dma_init(); //we'll turn DMA on/off to sound horn
    dac_init(); //config A3 further
}





int main(){
    initialize_motor();
    //host_serial_init(); //only for testing purposes
   
    SysTick_initialize();
    i2c_target_init();
    performed = true;

    config_dac_all(440);


    //if want to change f. while running we should use clock_set

    while(1){
        perform_command();
    }
}
