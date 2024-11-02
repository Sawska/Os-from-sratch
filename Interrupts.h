#ifndef INTERRUPTS_H

#define INTERRUPTS_H

void InitializeIDT();

extern "C" void isr_handler();

extern "C" void IRQ1_Handler();


extern "C" void ISR0();

#endif //INTERRUPTS_H