/*
 * File:   MetalDetector.c
 * Author: gkcol
 *
 * Created on March 5, 2025, 6:04 PM
 */


 #include <avr/io.h>
 #include <avr/interrupt.h>
 
 float value_u[5];
 uint8_t value[5];
 uint8_t count_u = 0;
 uint8_t tally_u = 0;
 uint8_t count = 0;
 uint8_t tally = 0;
 uint8_t duty = 25000;
 float distance = 0.0;
 float distance_final = 0.0;
 
 int main(void) {
     CCP = 0xd8;
     CLKCTRL.OSCHFCTRLA = 0b00010100;
     while(CLKCTRL.MCLKSTATUS & 0b00000001){
             ;
     }
     PORTA.DIRSET = 0b00000011;   //PA0 set as output
     TCA0.SINGLE.CTRLA = (0x01)<<3 | 0x01;//divide by 16 and enable clk
     TCA0.SINGLE.CTRLB = 0x03;   //single slope PWM
     TCA0.SINGLE.CTRLB |= 0x01 << 4; //PA0 enabled
     TCA0.SINGLE.PER = 31249;
     TCA0.SINGLE.CMP0 = 6; //set to very low trigg duty cycle
     EVSYS.CHANNEL0 = 0x45;    //PORTA pin PA5
     EVSYS.USERTCB2CAPT = 0x01;  //Choose channel 0
     TCB2.INTCTRL = 0x01; //Capture interupt enable
     TCB2.EVCTRL = 0x01; //enable input capture event
     TCB2.CTRLB = 0x04; //PWM measurement mode
     TCB2.CTRLA = 0x01; //TCB enable
     EVSYS.CHANNEL1 = 0x46;    //PORTA pin PA6
     EVSYS.USERTCB1CAPT = 0x02;  //Choose channel 1
     TCB1.INTCTRL = 0x01; //Capture interupt enable
     TCB1.EVCTRL = 0x01; //enable input capture event
     TCB1.CTRLB = 0x03; //Frequency measurement mode
     TCB1.CTRLA = 0x01; //TCB enable
     sei();
     while (1) {
         // Your main loop, elapsed_time will contain the measured time
     }
 }
   ISR(TCB2_INT_vect){
       TCB2.INTFLAGS = 0x01; //clears flag
       TCA0.SINGLE.CMP0 = 6;
       distance = TCB2.CCMP;
       distance_final = ((distance * 0.000000125)*(340))/2; //gives distance in m
       value_u[count_u] = distance_final;
       count_u++;
       if(value_u[count_u] <= 0.05){
         PORTA.OUT = 0b00010000;
         tally_u++;
       }
       if(count_u >= 5){
           if(tally_u == 5){
             PORTA.OUT = 0b00010000;
           }else{
             PORTA.OUT = 0b00000000;
           }
           count_u = 0;
           tally_u = 0;
     }
 
 }
   ISR(TCB1_INT_vect){
     TCB1.INTFLAGS = 0x01; //clears flag
     value[count] = TCB1.CCMP;
     if(value[count] < 200){
         tally++;
     }
     count++;
     if(count >= 5){
         if(tally == 5){
             PORTA.OUT = 0b00000010; 
         }else{
             PORTA.OUT = 0b00000000;
         }
         count = 0;
         tally = 0;
     }
 }/*
 int main(void){
     CCP = 0xd8;
     
     CLKCTRL.OSCHFCTRLA = 0b00010100;
     while(CLKCTRL.MCLKSTATUS & 0b00000001){
         ;
     }
     PORTA.DIRSET = 0b00000010;   //PA0 set as output
     EVSYS.CHANNEL0 = 0x46;    //PORTA pin PA5
     EVSYS.USERTCB2CAPT = 0x01;  //Choose channel 0
     TCB2.INTCTRL = 0x01; //Capture interupt enable
     TCB2.EVCTRL = 0x01; //enable input capture event
     TCB2.CTRLB = 0x03 ; //Frequency measurement mode
     TCB2.CTRLA = 0x01; //TCB enable
     sei();
     while (1) {
     }
 }
 ISR(TCB2_INT_vect){
     TCB2.INTFLAGS = 0x01; //clears flag
     TCA0.SINGLE.CMP1 = duty;
     value[count] = TCB2.CCMP;
     if(value[count] < 200){
         tally++;
     }
     count++;
     if(count >= 5){
         if(tally == 5){
             PORTA.OUT = 0b00000010;
             TCA0.SINGLE.CTRLA = (0x01)<<3 | 0x01;//divide by 16 and enable clk
             TCA0.SINGLE.CTRLB = 0x03;   //single slope PWM
             TCA0.SINGLE.CTRLB |= 0x01 << 5; //PA1 enabled as PWM single slope
             TCA0.SINGLE.PER = 499;//frequency set to 1khz
             TCA0.SINGLE.CMP1 = duty; //set to 50% duty cycle
         }else{
             PORTA.OUT = 0b00000000;
             TCA0.SINGLE.CTRLA = 0b00000000; //disable the clock, turning off the buzzer
         }
         count = 0;
         tally = 0;
     }
 }*/