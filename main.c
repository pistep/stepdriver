/**	
 *	Compile: g++ -o main main.c -l bcm2835
 */

//-- Includes.
#include <iostream>
#include <bcm2835.h>

//-- STD namespace.
using namespace std;

//-- Pin definitions.
#define LED			4
#define MOTOR1CH	0
#define MOTOR2CH	1
#define MOTOR1A		12
#define MOTOR1B		13
#define MOTOR2A		19
#define MOTOR2B		26

//-- Type definitions.
typedef struct{
	int val1a;
	int val1b;
	int val2a;
	int val2b;
}tStepPattern;

//-- Motor steps.
const tStepPattern steps[] = {
	{7,	0,	0,	0},
	{6,	0,	1,	0},
	{5,	0,	2,	0},
	{4,	0,	3,	0},
	{3,	0,	4,	0},
	{2,	0,	5,	0},
	{1,	0,	6,	0},
	{0,	0,	7,	0},
	{7,	1,	7,	0},
	{6,	1,	6,	0},
	{5,	1,	5,	0},
	{4,	1,	4,	0},
	{3,	1,	3,	0},
	{2,	1,	2,	0},
	{1,	1,	1,	0},
	{0,	1,	0,	0},
	{0,	1,	7,	1},
	{1,	1,	6,	1},
	{2,	1,	5,	1},
	{3,	1,	4,	1},
	{4,	1,	3,	1},
	{5,	1,	2,	1},
	{6,	1,	1,	1},
	{7,	1,	0,	1},
	{0,	0,	0,	1},
	{1,	0,	1,	1},
	{2,	0,	2,	1},
	{3,	0,	3,	1},
	{4,	0,	4,	1},
	{5,	0,	5,	1},
	{6,	0,	6,	1},
	{7,	0,	7,	1},
};

#define STEPS_PER_REV	(((int)12)*sizeof(steps))

//-- Apply a pattern to step motor coils.
void applyPattern(tStepPattern *pattern){
	bcm2835_gpio_write_mask(
		 ((pattern->val1b) << MOTOR1B)
		|((pattern->val2b) << MOTOR2B),
		 ((uint32_t)1 << MOTOR1B)
		|((uint32_t)1 << MOTOR2B));
	bcm2835_pwm_set_data(MOTOR1CH, (pattern->val1a));
	bcm2835_pwm_set_data(MOTOR2CH, (pattern->val2a));
}

//-- Apply a pattern to step motor coils.
void initStep(tStepPattern *pattern){
	//-- Set the pins as PWM.
	bcm2835_gpio_fsel(MOTOR1B, BCM2835_GPIO_FSEL_ALT0);
	bcm2835_gpio_fsel(MOTOR2A, BCM2835_GPIO_FSEL_ALT5);
	bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
	bcm2835_pwm_set_mode(MOTOR1CH, 1, 1);
	bcm2835_pwm_set_mode(MOTOR2CH, 1, 1);
	bcm2835_pwm_set_range(MOTOR1CH, 8);
	bcm2835_pwm_set_range(MOTOR2CH, 8);
	bcm2835_pwm_set_data(MOTOR1CH, 0);
	bcm2835_pwm_set_data(MOTOR2CH, 0);

	//-- Set the pins as output.
	bcm2835_gpio_fsel(MOTOR2B, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(MOTOR1A, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write_multi(
		 ((uint32_t)1 << MOTOR1B)
		|((uint32_t)1 << MOTOR2B), 0);
}

//-- Main.
int main(int argc, char **argv){
	int ratio = 0;
	int detect = 0, step = -1, rev = 0;
	if (!bcm2835_init()){
		return 1;
	}

	//-- Set the pins as output.
	bcm2835_gpio_fsel(LED, BCM2835_GPIO_FSEL_OUTP);
	
	//-- Loop.
	while (1){
		//-- Toggle.
		bcm2835_gpio_write(LED, !bcm2835_gpio_lev(LED));
	
		//-- Next step.
		if(++step >= STEPS_PER_REV){
			step = 0;
			++rev;
		}

		//-- Print.
		cout << "Revolutions: " << rev << ":" << step << endl << flush;
		
		//-- Get the pattern.
		applyPattern(&(steps[step % sizeof(steps)]));
		
		//-- Wait.
		delay(2);

		if((rev >= 3) && (step >= 32)){
			return 0;
		}
	}
	
	return 0;
}
