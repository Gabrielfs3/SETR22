/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**   @file main.c
 *    @brief Vending Machine
 *
 *    The file main.c simule an embedded application that emulates a vending
 *    machine. The vending machine accepts a subset of coins and allows the user to browse available
 *    products, buy one product and return the credit. The inputs are push-buttons and the output is done
 *    via UART/Terminal. 
 * 
 *    @author Rafael Fonseca, Gabriel Silva e Luis Almeida
 *    @date 7 May 2022
 *    @bug No known bugs.
 */



/* Includes */
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <drivers/pwm.h>
#include <sys/__assert.h>
#include <sys/util.h>
#include <sys/printk.h>
#include <timing/timing.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <drivers/gpio.h>
#include <drivers/adc.h>

/* ##################### Global Vars ##########################*/

int adc_value;
int adc_out;

int err = 0;
int ret;
long int nact = 0;

/* ##################### Threads ##########################*/

/* Size of stack area used by each thread (can be thread specific, if necessary)*/
#define STACK_SIZE 1024

/* Thread scheduling priority */
#define read_thread_prio 1
#define filter_thread_prio 1
#define out_thread_prio 1

/* Therad periodicity (in ms)*/
#define read_thread_period 1000

/* Create thread stack space */
K_THREAD_STACK_DEFINE(read_thread_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(filter_thread_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(out_thread_stack, STACK_SIZE);

/* Create variables for thread data */
struct k_thread read_thread_data;
struct k_thread filter_thread_data;
struct k_thread out_thread_data;

/* Create task IDs */
k_tid_t read_thread_tid;
k_tid_t filter_thread_tid;
k_tid_t out_thread_tid;

/* Semaphores for task synch */
struct k_sem sem_ab;
struct k_sem sem_bc;

/* Thread code prototypes */
void read_thread_code(void *argA, void *argB, void *argC);
void filter_thread_code(void *argA, void *argB, void *argC);
void out_thread_code(void *argA, void *argB, void *argC);

/* ##################### ADC ##########################*/

/*ADC definitions and includes*/
#include <hal/nrf_saadc.h>
#define ADC_NID DT_NODELABEL(adc) 
#define ADC_RESOLUTION 10
#define ADC_GAIN ADC_GAIN_1_4
#define ADC_REFERENCE ADC_REF_VDD_1_4
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 40)
#define ADC_CHANNEL_ID 1  

/* This is the actual nRF ANx input to use. Note that a channel can be assigned to any ANx. In fact a channel can */
/*    be assigned to two ANx, when differential reading is set (one ANx for the positive signal and the other one for the negative signal) */  
/* Note also that the configuration of differnt channels is completely independent (gain, resolution, ref voltage, ...) */
#define ADC_CHANNEL_INPUT NRF_SAADC_INPUT_AIN1

#define BUFFER_SIZE 1

/* Other defines */
#define TIMER_INTERVAL_MSEC 1000 /* Interval between ADC samples */

/* ADC channel configuration */
static const struct adc_channel_cfg my_channel_cfg = {
	.gain = ADC_GAIN,
	.reference = ADC_REFERENCE,
	.acquisition_time = ADC_ACQUISITION_TIME,
	.channel_id = ADC_CHANNEL_ID,
	.input_positive = ADC_CHANNEL_INPUT
};

struct k_timer my_timer;
const struct device *adc_dev = NULL;
static uint16_t adc_sample_buffer[BUFFER_SIZE];

/* ##################### PWM ##########################*/

#define PWM0_NID DT_NODELABEL(pwm0) 
#define GPIO0_NID DT_NODELABEL(gpio0)
#define NLED1 0x0d
#define NPOT  0x3

const struct device *pwm0_dev;
const struct device *gpio0_dev;

unsigned int pwmPeriod_us = 1000;

void conf();
void adc_setup();
static int adc_sample(void);

/**
 *
 *  @name   Main
 *  @brief  State machine according with state diagram.
 *          Each case of switch is a state.
 *
 */

void main(void)
{
	
    /* Processing */  
    conf();
    adc_setup();

    /* Welcome message */
    printf("\n\r Illustration of the use of shmem + semaphores\n\r");
    
    /* Create and init semaphores */
    k_sem_init(&sem_ab, 0, 1);
    k_sem_init(&sem_bc, 0, 1);
    
    /* Create tasks */
    read_thread_tid = k_thread_create(&read_thread_data, read_thread_stack,
        K_THREAD_STACK_SIZEOF(read_thread_stack), read_thread_code,
        NULL, NULL, NULL, read_thread_prio, 0, K_NO_WAIT);

    filter_thread_tid = k_thread_create(&filter_thread_data, filter_thread_stack,
        K_THREAD_STACK_SIZEOF(filter_thread_stack), filter_thread_code,
        NULL, NULL, NULL, filter_thread_prio, 0, K_NO_WAIT);

    out_thread_tid = k_thread_create(&out_thread_data, out_thread_stack,
        K_THREAD_STACK_SIZEOF(out_thread_stack), out_thread_code,
        NULL, NULL, NULL, out_thread_prio, 0, K_NO_WAIT);


}

/**
*
*  @name   Configuration
*  @brief  This function have the objetive of configure all the buttons.
*          
*
*/

void conf()
{
  /* Bind to GPIO 0 and PWM0 */
    gpio0_dev = device_get_binding(DT_LABEL(GPIO0_NID));
    if (gpio0_dev == NULL) {
        printk("Error: Failed to bind to GPIO0\n\r");        
	return;
    }
    else {
        printk("Bind to GPIO0 successfull\n\r");        
    }
    
    pwm0_dev = device_get_binding(DT_LABEL(PWM0_NID));
    if (pwm0_dev == NULL) {
	printk("Error: Failed to bind to PWM0\n\r");
	return;
    }
    else  {
        printk("Bind to PWM0 successful\n\r");            
    }

    
    /* Configure Potentiometer PIN */    
    ret = gpio_pin_configure(gpio0_dev, NPOT, GPIO_INPUT);
    if (ret < 0) {
        printk("Error %d: Failed to configure POT\n\r", ret);
	return;
    }
}

void adc_setup()
{
/* ADC setup: bind and initialize */
    adc_dev = device_get_binding(DT_LABEL(ADC_NID));
	if (!adc_dev) {
        printk("ADC device_get_binding() failed\n\r");
    } 
    err = adc_channel_setup(adc_dev, &my_channel_cfg);
    if (err) {
        printk("adc_channel_setup() failed with error code %d\n\r", err);
    }

/* It is recommended to calibrate the SAADC at least once before use, and whenever the ambient temperature has changed by more than 10 °C */
    NRF_SAADC->TASKS_CALIBRATEOFFSET = 1;
}

/* Takes one sample */
static int adc_sample(void)
{
	const struct adc_sequence sequence = {
		.channels = BIT(ADC_CHANNEL_ID),
		.buffer = adc_sample_buffer,
		.buffer_size = sizeof(adc_sample_buffer),
		.resolution = ADC_RESOLUTION,
	};

	if (adc_dev == NULL) {
            printk("adc_sample(): error, must bind to adc first \n\r");
            return -1;
	}

	ret = adc_read(adc_dev, &sequence);
	if (ret) {
            printk("adc_read() failed with code %d\n", ret);
	}	

	return ret;
}

/* Thread code implementation */
void read_thread_code(void *argA , void *argB, void *argC)
{
    /* Timing variables to control task periodicity */
    int64_t fin_time=0, release_time=0;
    
    printk("\nRead Thread init (periodic)\n");

    /* Compute next release instant */
    release_time = k_uptime_get() + read_thread_period;

    /* Thread loop */
    while(1) 
    {
        
        /* Get one sample, checks for errors and prints the values */
        err=adc_sample();
        if(err) {
            printk("adc_sample() failed with error code %d\n",err);
        }
        else {
            if(adc_sample_buffer[0] > 1023) {
                printk("adc reading out of range\n");
            }
            else {
                /* ADC is set to use gain of 1/4 and reference VDD/4, so input range is 0...VDD (3 V), with 10 bit resolution */
                adc_value = (uint16_t)(1000*adc_sample_buffer[0]*((float)3/1023));
                printk("adc reading: raw:%4u / %4u mV\n",adc_sample_buffer[0],adc_value);
            }
        }
        
        k_sem_give(&sem_ab);

       
        /* Wait for next release instant */ 
        fin_time = k_uptime_get();
        if( fin_time < release_time) {
            k_msleep(release_time - fin_time);
            release_time += read_thread_period;
        }
    }

}

void filter_thread_code(void *argA , void *argB, void *argC)
{
    int dif1, dif2, sum1, sum2, average, i, k, j;
    int values_in[10] = {0,0,0,0,0,0,0,0,0,0};

    printk("\nFilter Thread init\n");

    while(1)
    {
        k_sem_take(&sem_ab,  K_FOREVER);
       
        average = 0;
        sum1 = 0;
        sum2 = 0;
        j = 0;

        for(i = 0; i <= 9; i++)
        {
          if(i == 0)
          {
            for(k = 0; k < 9; k++)
            {
              // shift left das posições
              values_in[k] = values_in[k+1];
              // inserir valor lido na ultima posição do array
              if(k == 8)
                values_in[k+1] = adc_value;
            }
          }
          // Calcular soma dos valores do array
          sum1 += values_in[i];

          if(i == 9)
          {
            // média e diferenças para condições
            average = sum1 / 10;
            printk("average: %d\n",average);
            dif1 = average + average*0.1;
            dif2 = average - average*0.1;
        
            for(i = 0; i <= 9; i++)
            {
              // soma dos valores aprovados pelo filtro
              if((values_in[i] < dif1) && (values_in[i] > dif2))
              {
                sum2 += values_in[i];
                j++;
              }
            }
            
            // if porque é impossivel dividir por 0
            if(j != 0)
              adc_out = sum2 / j;
          }
        }

        printk("Filter Thread set the value to: %d \n",adc_out); 
         
        k_sem_give(&sem_bc);
   
    }
}

void out_thread_code(void *argA , void *argB, void *argC)
{
    printk("\nOut Thread init\n");
    int out;
    while(1)
    {
        k_sem_take(&sem_bc, K_FOREVER);
        ret = 0;
        // recálculo do valor
        out = (uint16_t)(adc_out / ((float)3 / 1023) / 1000);

        ret = pwm_pin_set_usec(pwm0_dev, NLED1,
		      pwmPeriod_us,(unsigned int)((pwmPeriod_us*out)/1023), PWM_POLARITY_NORMAL);
        if (ret)
          printk("Error %d: failed to set pulse width\n", ret);
    }
}