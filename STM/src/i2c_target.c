/*
 * i2c_target.c
 * Alisa Yurevich and Duncan DeFonce, EE14, Spring 2025
 *
 */

#include "ee14lib.h"
#include "stm32l432xx.h"

/*
 * i2c_target_init
 * Description : Enable I2C_OA1, with address OA1, as an I2C target. D1 is clk
 * D0 is data
 * 
 */
void i2c_target_init(void){
    //ok so even tho we dont need to drive a clock, we still need to enable 
    //I2C's peripheral clock, whihc simply allows the module to run
    
    //peripheral clock enable register. basically just go in there and set the bits
    //associated wiht i2c1
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
    
    //enable the SCL and SDA as open-drain:
    gpio_config_otype(D1, OPEN_DRAIN);
    gpio_config_otype(D0, OPEN_DRAIN);
    gpio_config_pullup(D1, PULL_UP);//D1 IS CLK
    gpio_config_pullup(D0, PULL_UP);//D0 IS data
    gpio_config_alternate_function(D1, 4); // AF4 is for I2C
    gpio_config_alternate_function(D0, 4);
    
    //safety clears
    I2C1->CR1 &= ~I2C_CR1_PE; //i2c control register 1, peripheral enable
    I2C1->OAR1 &= ~I2C_OAR1_OA1EN; //own address register one

    //by default, OA1MODE is 7 bit addressing mode, we shouldn't have to change it
    
    //Bits 7:1 are the 7 bit address
    I2C1->OAR1 |= 0xF << 1; //set own address
    //OA1 is enabled by setting the OA1EN bit of the I2C_OAR1 register
    I2C1->OAR1 |= I2C_OAR1_OA1EN; //enable address recognition


    I2C1->CR1 |= I2C_CR1_ADDRIE; //address match interrupt
    I2C1->CR1 |= I2C_CR1_RXIE; //rx interrupt
    
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    
    I2C1->CR1 |= I2C_CR1_PE; // enable the peripheral

}
