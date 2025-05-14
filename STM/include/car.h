/*
 * car.h
 * Alisa Yurevich, Duncan DeFonce, EE14, Spring 2025
 *
 */

#include "ee14lib.h"

typedef struct {
    unsigned int duty;
    int direction; //1 = forward, 0 = stopped, -1 = backward
} Motor;

typedef struct {
    Motor motorL;
    Motor motorR;
} CarState;

//0, 1
typedef enum {LEFT, RIGHT} motor_t;
typedef enum {STOPPED, FORWARD, BACK} direction;

//motor pins:
// A0 - A3 = left motor: IN1, IN2 right motor: IN3,IN4 
// D9 = ENA (left), TIM1
// D10 = ENB (right), TIM1
#define L_ENABLE D9
#define R_ENABLE D10

#define L_LOGIC_1 A0
#define L_LOGIC_2 A1
#define R_LOGIC_1 A2
#define R_LOGIC_2 A4

#define DAC_PIN A3

#define DELAY_MS 10
#define FREQ_HZ 100000

//commands: 
#define DRIVE 'W'
#define ACCELERATE 'E'
#define BRAKE 'S' //decrease
#define STOP 'F'
#define TURN_LEFT 'A'
#define TURN_RIGHT 'D'
#define REVERSE 'R'
#define HONK 'H'

//Driving functions


bool drive();
bool accelerate();
bool brake();
bool stop();
bool turn(motor_t direction);
bool reverse();
bool honk();

void initialize_motor(void); //for PWM and motor pinout
void set_motor_direction(motor_t motor, direction direction);
void set_motor_duty(motor_t motor, unsigned int duty);
void transition_speed(int duty_change);
int clamp(int value);

//PWM
#define BASE_DUTY 600
#define MIN_DUTY 300
#define MAX_DUTY 1023
#define STEP 30

