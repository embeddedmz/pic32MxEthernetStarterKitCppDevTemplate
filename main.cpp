/* 
 * File:   main.cpp
 * Author: Med Amine Mzoughi
 *
 * Created on 3 septembre 2017, 11:49
 */

//Dependencies
#include <cstdlib>
#include <cstdint>
#include <p32xxxx.h>
#include <sys/attribs.h>

//LEDs
#define LED1_MASK (1 << 0)
#define LED2_MASK (1 << 1)
#define LED3_MASK (1 << 2)

//Push buttons
#define SW1_MASK (1 << 6)
#define SW2_MASK (1 << 7)
#define SW3_MASK (1 << 13)

#ifndef _CP0_SET_CONFIG
#define _CP0_SET_CONFIG(val) _mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, val)
#endif

//PIC32 onfiguration settings
#pragma config USERID = 0xFFFF
#pragma config FSRSSEL = PRIORITY_7
#pragma config FMIIEN = OFF
#pragma config FETHIO = OFF
#pragma config FCANIO = OFF
#pragma config FUSBIDIO = OFF
#pragma config FVBUSONIO = OFF
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config UPLLIDIV = DIV_2
#pragma config UPLLEN = OFF
#pragma config FPLLODIV = DIV_1
#pragma config FNOSC = PRIPLL
#pragma config FSOSCEN = ON
#pragma config IESO = ON
#pragma config POSCMOD = XT
#pragma config OSCIOFNC = OFF
#pragma config FPBDIV = DIV_2
#pragma config FCKSM = CSDCMD
#pragma config WDTPS = PS1048576
#pragma config FWDTEN = OFF
//#pragma config DEBUG = ON
#pragma config ICESEL = ICS_PGx2
#pragma config PWP = OFF
#pragma config BWP = OFF
#pragma config CP = OFF

/**
 * @brief System initialization
 **/

void systemInit(void)
{
   uint32_t temp;
   
   //Execute system unlock sequence
   SYSKEY = 0xAA996655;
   SYSKEY = 0x556699AA;
   
   //Configure PBCLK clock divisor (SYSCLK / 2);
   temp = OSCCON & ~_OSCCON_PBDIV_MASK;
   OSCCON = temp | (1 << _OSCCON_PBDIV_POSITION);
   
   //Configure RAM wait states (0)
   BMXCONCLR = _BMXCON_BMXWSDRM_MASK;
   
   //Configure FLASH wait states (2)
   temp = CHECON & ~_CHECON_PFMWS_MASK;
   CHECON = temp | (2 << _CHECON_PFMWS_POSITION);
   
   //Enable predictive prefetch for both cacheable and non-cacheable regions
   temp = CHECON & ~_CHECON_PREFEN_MASK;
   CHECON = temp | (3 << _CHECON_PREFEN_POSITION);
   
   //Enable data caching (4 lines)
   temp = CHECON & ~_CHECON_DCSZ_MASK;
   CHECON = temp | (3 << _CHECON_DCSZ_POSITION);
   
   //Enable KSEG0 cache
   temp = _CP0_GET_CONFIG() & ~_CP0_CONFIG_K0_MASK;
   temp |= (3 << _CP0_CONFIG_K0_POSITION);
   _CP0_SET_CONFIG(temp);
   
   //Relock the SYSKEY
   SYSKEY = 0;
   
   //Disable interrupts
   __builtin_disable_interrupts();
   
   //Set IV
   _CP0_BIS_CAUSE(_CP0_CAUSE_IV_MASK);
   //Enable multi-vectored mode
   INTCONSET = _INTCON_MVEC_MASK;
}

/**
 * @brief I/O initialization
 **/

void ioInit(void)
{
   //Disable analog inputs
   AD1PCFG = 0xFFFF;
   
   //Configure LED1 (RD0), LED2 (RD1) and LED3 (RD3)
   TRISDCLR = LED1_MASK | LED2_MASK | LED3_MASK;
   LATDCLR = LED1_MASK | LED2_MASK | LED3_MASK;
   
   //Configure SW1 (RD6), SW2 (RD7) and SW3 (RD13)
   TRISDSET = SW1_MASK | SW2_MASK | SW3_MASK;
   //Enable pull-ups on CN15 (RD6), CN16 (RD7) and CN19 (RD13)
   CNPUESET = _CNPUE_CNPUE15_MASK | _CNPUE_CNPUE16_MASK | _CNPUE_CNPUE19_MASK;
}

/**
 * @brief Main entry point
 * @return Unused value
 **/

int32_t main(void)
{
   //System initialization
   systemInit();
   
   //Configure I/Os
   ioInit();
   
   //User code
   uint32_t uiPortValue;
   
   while (true)
   {
      uiPortValue = PORTD;
      
      //SW1 button pressed?
      if (!(uiPortValue & SW1_MASK))
      {
         //Toggle LED
         if ( (PORTD & LED1_MASK) != (uint32_t)0)
            LATDCLR = LED1_MASK;
         else
            LATDSET = LED1_MASK;
         
         //Wait for the SW1 button to be released
         while (!(PORTD & SW1_MASK));
      }
      else if (!(uiPortValue & SW2_MASK))
      {
         //Toggle LED
         if ( (PORTD & LED2_MASK) != (uint32_t)0)
            LATDCLR = LED2_MASK;
         else
            LATDSET = LED2_MASK;
         
         //Wait for the SW2 button to be released
         while (!(PORTD & SW2_MASK));
      }
      else if (!(uiPortValue & SW3_MASK))
      {
         //Toggle LED
         if ( (PORTD & LED3_MASK) != (uint32_t)0)
            LATDCLR = LED3_MASK;
         else
            LATDSET = LED3_MASK;
         
         //Wait for the SW3 button to be released
         while (!(PORTD & SW3_MASK));
      }
   }
   
   //This function should never return
   return 0;
}
