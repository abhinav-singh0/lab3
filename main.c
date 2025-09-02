#include <stdint.h>
#include "tm4c123gh6pm.h"

#define RED_LED    (1U << 1)   // PF1
#define BLUE_LED   (1U << 2)   // PF2
#define GREEN_LED  (1U << 3)   // PF3

#define SW2        (1U << 0)   // PF0
#define SW1        (1U << 4)   // PF4

void PortF_Init(void) {
    SYSCTL_RCGCGPIO_R |= 0x20;                // Enable clock to GPIOF
    while ((SYSCTL_PRGPIO_R & 0x20) == 0);    // Wait until ready
    GPIO_PORTF_LOCK_R = 0x4C4F434B;           // Unlock commit register
    GPIO_PORTF_CR_R |= 0x1F;                   // Allow changes to PF0–PF4
    GPIO_PORTF_DIR_R |= (RED_LED | BLUE_LED | GREEN_LED);  // PF1, PF2, PF3 outputs
    GPIO_PORTF_DIR_R &= ~(SW1 | SW2);          // PF0, PF4 inputs (switches)
    GPIO_PORTF_DEN_R |= 0x1F;                  // Enable digital on PF0–PF4
    GPIO_PORTF_PUR_R |= (SW1 | SW2);           // Enable internal pull-ups on PF0, PF4
}

void delayMs(int n) {
    volatile int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 3180; j++);
}

int main(void) {
    PortF_Init();
    int state = 0;
    int colors[8] = {0x00, RED_LED, BLUE_LED, GREEN_LED, RED_LED | GREEN_LED, RED_LED | BLUE_LED, BLUE_LED | GREEN_LED, RED_LED | BLUE_LED | GREEN_LED};
    // OFF, Red, Blue, Green, Yellow, Pink, Cyan, White

    while(1) {
        if((GPIO_PORTF_DATA_R & SW2) == 0) {   // SW2 pressed (PF0 low)
            delayMs(20);  // debounce delay
            while((GPIO_PORTF_DATA_R & SW2) == 0); // wait for release
            delayMs(20);  // debounce delay
            // Next color
            state = (state + 1) % 8;
            GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x0E) | colors[state]; // update LEDs only, preserve other bits
        }
    }
}
