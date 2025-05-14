/*
 * motor.c
 * Alisa Yurevich, Wilson Wu, Duncan DeFonce, EE14, Spring 2025
 *
 */


#include "ee14lib.h"
#include "car.h"
#include "dac.h"
#include <stdio.h>

CarState car = { // intial global state
    .motorL = {.duty = 0, .direction = 0},
    .motorR = {.duty = 0, .direction = 0}
};

/*
    helper function that updates the duty to the passe value
*/
void set_motor_duty(motor_t motor, unsigned int duty) {
    if (motor == LEFT) {
        car.motorL.duty = clamp(duty);
        timer_config_channel_pwm(TIM1, L_ENABLE, car.motorL.duty);
    } else {
        car.motorR.duty = clamp(duty);
        timer_config_channel_pwm(TIM1, R_ENABLE, car.motorR.duty);
    }
}

/*
    simultaneous duty change of BOTH motors at once. 
    can be different duties, but will increment by the same amoumt
    will end up in different states if they intially were in different states
    a negative passed here will decrease the speed
*/
void transition_speed(int duty_change) {
    int l_target = clamp(car.motorL.duty + duty_change);
    int r_target = clamp(car.motorR.duty + duty_change);

    int steps = abs(duty_change / STEP); 

    for (int i = 0; i <= steps; i++) {
        int step_size = (duty_change > 0) ? STEP : -STEP;

        int current_left = clamp(car.motorL.duty + (step_size * i)); 
        int current_right = clamp(car.motorR.duty + (step_size * i));

        set_motor_duty(LEFT, current_left);
        set_motor_duty(RIGHT, current_right);
    }

    set_motor_duty(LEFT, l_target);
    set_motor_duty(RIGHT, r_target);

}

/*
 * Initialize the pins for the motor and set up the PWM with 100Hz
 * 
 * */
void initialize_motor() {
    timer_config_pwm(TIM1, FREQ_HZ); //set up the pwm of timer 1
    
    gpio_config_mode(L_ENABLE, OUTPUT); //set enable pins output
    gpio_config_mode(R_ENABLE, OUTPUT);

    gpio_config_mode(L_LOGIC_1, OUTPUT); //set logic motor pins as output
    gpio_config_mode(L_LOGIC_2, OUTPUT);
    gpio_config_mode(R_LOGIC_1, OUTPUT);
    gpio_config_mode(R_LOGIC_2, OUTPUT);

    set_motor_direction(LEFT, STOPPED); //initializes pins to global state , kind f werid bc i alreayd luke updated it but idkl
    set_motor_direction(RIGHT, STOPPED);
    set_motor_duty(LEFT, 0); // configures the duty to 0 for both motors
    set_motor_duty(RIGHT, 0); 

}
void set_motor_direction(motor_t motor, direction direction) {
    if (motor == LEFT) {
        car.motorL.direction = direction;
        switch(direction) {
            case FORWARD: 
                gpio_write(L_LOGIC_1, 1); gpio_write(L_LOGIC_2, 0); break;
            case BACK: 
                gpio_write(L_LOGIC_1, 0); gpio_write(L_LOGIC_2, 1); break;
            case STOPPED: 
                gpio_write(L_LOGIC_1, 0); gpio_write(L_LOGIC_2, 0); break;
        }
    } else {
        car.motorR.direction = direction;
        switch(direction) {
            case FORWARD:  
                gpio_write(R_LOGIC_1, 1); gpio_write(R_LOGIC_2, 0); break;
            case BACK: 
                gpio_write(R_LOGIC_1, 0); gpio_write(R_LOGIC_2, 1); break;
            case STOPPED: 
                gpio_write(R_LOGIC_1, 0); gpio_write(R_LOGIC_2, 0); break;
        }
    }
}


/*
 * drive 
 * Purpose : Set the car into forward drive mode at base PWM duty (speed).
 * Returns : always true so we can set performed to true in main
 */
bool drive(){
    set_motor_direction(LEFT, FORWARD); 
    set_motor_duty(LEFT, BASE_DUTY);

    set_motor_direction(RIGHT, FORWARD); 
    set_motor_duty(RIGHT, BASE_DUTY);
    return true;
}

/*
 * accelerate
 * Purpose : Increase the PWM duty of both wheels by 100 to make them spin faster
 * Returns : always true so we can set performed to true in main
 */
bool accelerate(){
    transition_speed(100);

    return true;
}

/*
 * brake 
 * Purpose : Decrease the PWM duty of both wheels by 100 to make them spin slower
 * Returns : always true so we can set performed to true in main
 */
bool brake(){
    transition_speed(-100);

    return true;
}

/*
 * stop
 * Purpose : Cut power to both wheels through the H-bridge. Wheels immediately
 * stop spinning
 * Returns : always true so we can set performed to true in main
 */

bool stop(){
    set_motor_direction(LEFT, STOPPED);
    set_motor_direction(RIGHT, STOPPED);

    set_motor_duty(LEFT, 0);
    set_motor_duty(RIGHT, 0);
    
    return true;
}

/*
 * turn
 * Purpose : Make one wheel spin faster than the other so we turn in the oppposite
 * direction. Turn occurs at a fixed speed (which can be changed by accel/brake)
 * Arguments : which direction do you want to turn (L/R)
 * Returns : always true so we can set performed to true in main
 */
bool turn(motor_t direction){
    if (direction == LEFT){
        set_motor_duty(LEFT, 600);
        set_motor_duty(RIGHT, 800);
    } else{
        set_motor_duty(RIGHT, 600);
        set_motor_duty(LEFT, 800);
    }

    return 1;
}
/*
 * reverse
 * Purpose : Change direction of both motors' spin through the H-bridge.
 * Returns : always true so we can set performed to true in main
 */

bool reverse(){
    set_motor_direction(LEFT, BACK); 
    set_motor_direction(RIGHT, BACK);  

    set_motor_duty(RIGHT, BASE_DUTY);
    set_motor_duty(LEFT, BASE_DUTY );
    return true;
}

/*
 * honk
 * Purpose : Enable the DAC output so we can honk the horn. It automatically turns off
 * Returns : always true so we can set performed to true in main
 */
bool honk(){
    //clock_set(440);
    trigger_horn();
    //clock_set(370);
    delay(100);
    trigger_horn();

    return true;
}
