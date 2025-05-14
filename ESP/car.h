
typedef struct {
    unsigned int duty;
    int direction; //1 = forward, 0 = stopped, -1 = backward
} Motor;

typedef struct {
    Motor motorL;
    Motor motorR;
} CarState;

//double check:
#define L_MOTOR 1
#define R_MOTOR 2

#define MIN_DUTY 0 // off
#define MAX_DUTY 1023// on
#define DELAY_MS 10
#define STEP 33 

//comands: 
#define DRIVE 'D'
#define ACCELERATE 'A'
#define BRAKE 'B'
#define TURN_LEFT 'L'
#define TURN_RIGHT 'R'

#define clamp(value) ((value) < (MIN_DUTY) ? (MIN_DUTY) : ((value) > (MAX_DUTY) ? (MAX_DUTY) : (value)))

//Driving functions
void motorFreeStop(int motor);//cut motor power
void motorBrake (int motor); //make motor spin backwards
void initialize_motor(void); //for PWM and motor pinout


void delay(int wait);

//Duty
#define MIN_DUTY 0
#define MAX_DUTY 1023
#define STEP 33

//Change the speed of the motor by changing its duty cycle

