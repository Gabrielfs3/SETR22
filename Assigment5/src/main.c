/*
 * Copyright (c) 2016 Open-RnD Sp. z o.o.
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**   @file         main.c
 *    @brief        Implementing cooperative tasks in Zephyr
 *
 *                  The file main.c implement a set of cooperative real-time tasks in
 *	                Zephyr. In this file we are using threads and semaphores to execute the tasks.
 *
 * 
 *    @author       Rafael Fonseca, Gabriel Silva e Luis Almeida
 *    @date         30 May 2022
 *    @bug          No known bugs.
 */



/**
 * @{ @name         Global Includes
 *    @brief        This are the global includes to the file main.c.
 *
 */
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
#include <drivers/adc.h>
#include <console/console.h>

/**
* @}
*/


/* ##################### Global Vars ##########################*/

/**
 * @{ @name         Global Variables
 *    @brief        This are global variables used on the file main.c.
 *
 */
int adc_value;
int pwm_value;
uint16_t adc_out;
int err = 0;
int ret;
int ON_flag = 1;
int print_flag = 1;
uint16_t aux2;

/**
* @}
*/

/* ##################### Threads ##########################*/

/** @defgroup       Threads
*
*  @{
*/

#define SW0_NODE	DT_ALIAS(sw0)
#define SW1_NODE	DT_ALIAS(sw1)
#define SW2_NODE	DT_ALIAS(sw2)
#define SW3_NODE	DT_ALIAS(sw3)

static const struct gpio_dt_spec button1 = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
							      {0});
static const struct gpio_dt_spec button2 = GPIO_DT_SPEC_GET_OR(SW1_NODE, gpios,
							      {0});
static const struct gpio_dt_spec button3 = GPIO_DT_SPEC_GET_OR(SW2_NODE, gpios,
							      {0});
static const struct gpio_dt_spec button4 = GPIO_DT_SPEC_GET_OR(SW3_NODE, gpios,
							      {0});
static struct gpio_callback button1_cb_data;
static struct gpio_callback button2_cb_data;
static struct gpio_callback button3_cb_data;
static struct gpio_callback button4_cb_data;

void button1_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
  if(ON_flag == 1)
  {
    if(adc_out+93 >= 1023)
      adc_out = 1023;
    else
      adc_out += 93;
   }
}
void button2_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
  if(ON_flag == 1)
  {
    if(adc_out-93 <= 0)
      adc_out = 0;
    else
      adc_out -= 93;
  }
}
void button3_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    ON_flag = 1;
}
void button4_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    ON_flag = 0;
    print_flag = 0;
}

/**
* @addtogroup       Threads
* @name             Threads Size Constant
* @brief            Size of stack area used by each thread (can be thread specific, if necessary).
*/
#define STACK_SIZE 1024


/**
 * @{ @addtogroup   Threads
 *    @name         Threads Priority Constants
 *    @brief        Thread scheduling priority.
 *
 */
#define read_thread_prio 1
#define action_thread_prio 1
#define manual_out_thread_prio 1
#define auto_out_thread_prio 1
 /**
 * @}
 */

 /**
 * @addtogroup       Threads
 * @name             Threads Time Constant
 * @brief            Therads periodicity (in ms).
 */
#define read_thread_period 100


 /**
  * @{ @addtogroup   Threads
  *    @name         Threads Space Functions
  *    @brief        Create threads stack space.
  *
  */
K_THREAD_STACK_DEFINE(read_thread_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(manual_out_thread_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(action_thread_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(auto_out_thread_stack, STACK_SIZE);

/**
* @}
*/

/**
 * @{ @addtogroup   Threads
 *    @name         Threads Structures
 *    @brief        Create variables for thread data.
 *
 */
struct k_thread read_thread_data;
struct k_thread manual_out_thread_data;
struct k_thread action_thread_data;
struct k_thread auto_out_thread_data;
/**
* @}
*/

/**
 * @{ @addtogroup   Threads
 *    @name         Threads IDs
 *    @brief        Creating task IDs.
 *
 */
k_tid_t read_thread_tid;
k_tid_t manual_out_thread_tid;
k_tid_t action_thread_tid;
k_tid_t auto_out_thread_tid;
/**
* @}
*/

/**
 * @{ @addtogroup   Threads
 *    @name         Threads Semaphores
 *    @brief        Semaphores for task synch.
 *
 */
struct k_sem sem_manual;
struct k_sem sem_auto;
struct k_sem sem_auto2;
/**
* @}
*/

/**
 * @{ @addtogroup   Threads
 *    @name         Read Thread
 *    @brief        This thread have the objective to read ADC, get one sample, check if are errors and print the value.
 *
 *
 */
void read_thread_code(void* argA, void* argB, void* argC);
/**
* @}
*/

/**
 * @{ @addtogroup   Threads
 *    @name         Action Thread
 *    @brief        This thread is a moving average filter, with a window size of 10 samples. Removes the
  *                 outliers (10% or high deviation from average) and computes the average of the remaining
 *                  samples.
 *
 *
 *
 */
void action_thread_code(void* argA, void* argB, void* argC);
/**
* @}
*/


/**
 * @{ @addtogroup   Threads
 *    @name         Output Thread
 *    @brief        This thread send a pwm signal to one of the DevKit led.
 *
 *
 */
void manual_out_thread_code(void* argA, void* argB, void* argC);
/**
* @}
*/

void auto_out_thread_code(void* argA, void* argB, void* argC);

/** @}
*/

/* ##################### ADC ##########################*/

/** @defgroup       ADC
*
*  @{
*/

 /**
  * @{  @name       ADC Constants
  *     @brief      ADC definitions and includes.
  */
#include <hal/nrf_saadc.h>
#define ADC_NID DT_NODELABEL(adc) 
#define ADC_RESOLUTION 10
#define ADC_GAIN ADC_GAIN_1_4
#define ADC_REFERENCE ADC_REF_VDD_1_4
#define ADC_ACQUISITION_TIME ADC_ACQ_TIME(ADC_ACQ_TIME_MICROSECONDS, 40)
#define ADC_CHANNEL_ID 1  
  /**
  * @}
  */

  /**
   * @name          ADC Channel Constant
   * @brief         This is the actual nRF ANx input to use. Note that a channel can be assigned to any ANx. In fact a channel can */
   /*               be assigned to two ANx, when differential reading is set (one ANx for the positive signal and the other one for the negative signal)
    */
#define ADC_CHANNEL_INPUT NRF_SAADC_INPUT_AIN1

    /**
     * @name        ADC Buffer Size Constant
     * @brief       This initialize the buffer size with the value of 1.
     */
#define BUFFER_SIZE 1

     /**
      * @name        ADC Time Constant
      * @brief       Interval between ADC samples
      */
#define TIMER_INTERVAL_MSEC 100

      /**
       * @{  @struct   ADC Structures
       *     @brief    ADC channel configuration.
       */
static const struct adc_channel_cfg my_channel_cfg = {
    .gain = ADC_GAIN,
    .reference = ADC_REFERENCE,
    .acquisition_time = ADC_ACQUISITION_TIME,
    .channel_id = ADC_CHANNEL_ID,
    .input_positive = ADC_CHANNEL_INPUT
};

struct k_timer my_timer;
const struct device* adc_dev = NULL;
static uint16_t adc_sample_buffer[BUFFER_SIZE];
/**
* @}
*/

/**
 * @{ @addtogroup   ADC
 *    @name         ADC Configuration Function
 *    @brief        This function give us the information if ADC configuration was successful or not.
 *
 */
void adc_setup();
/**
* @}
*/

/**
 * @{ @addtogroup   ADC
 *    @name         ADC Sample Function
 *    @brief        This function reading and geting one sample of the potenciometer.
 *
 */
static int adc_sample(void);
/**
* @}
*/

/** @}
*/

/* ##################### PWM ##########################*/

/** @defgroup       PWM
*
*  @{
*/

/**
 * @{ @addtogroup   PWM
 *    @name         PWM Constants
 *    @brief        This constants are related to the configuration of PWM ports.
 *
 */
#define PWM0_NID DT_NODELABEL(pwm0) 
#define GPIO0_NID DT_NODELABEL(gpio0)
 /**
 * @}
 */


 /**
  * @{ @addtogroup   PWM
  *    @name         PWM Structures
  *    @brief        This variables are related to the configuration of PWM ports.
  *
  */
const struct device* pwm0_dev;
const struct device* gpio0_dev;
/**
* @}
*/

/**
 * @{ @addtogroup   PWM
 *    @name         PWM Period Variable
 *    @brief        This variable set the period of PWM.
 *
 */
unsigned int pwmPeriod_us = 100;
/**
* @}
*/

/**
 * @{ @addtogroup   PWM
 *    @name         PWM Configuration Function
 *    @brief        This function give us the information if the ports configuration was successful or not.
 *
 *
 */
void conf();
/**
* @}
*/

/** @}
*/

/* ##################### LED ##########################*/

/** @defgroup       LED
 *
 *  @{
 */

 /**
  * @{ @addtogroup   LED
  *    @name         LED Constants
  *    @brief        This constants are related to the configuration of LED output.
  *
  */
#define NLED1 0x0d
#define NPOT  0x3
  /**
  * @}
  */

  /** @}
  */

  /* ##################### Main ##########################*/

/**
 *
 *  @name   Main
 *  @brief  The main initialize the configurations, create threads and semaphores.
 *
 */
 
static const char prompt[] = "Character echo started ...\r\n";

void main(void)
{
	
    /* Initialization of the functions */
    conf();
    adc_setup();

    /* Welcome message */
    printf("\n\r Buenos Dias\n\r");
    
    /* Create and init semaphores */
    k_sem_init(&sem_manual, 0, 1);
    k_sem_init(&sem_auto, 0, 1);
    k_sem_init(&sem_auto2, 0, 1);
    
    /* Create tasks */
    read_thread_tid = k_thread_create(&read_thread_data, read_thread_stack,
        K_THREAD_STACK_SIZEOF(read_thread_stack), read_thread_code,
        NULL, NULL, NULL, read_thread_prio, 0, K_NO_WAIT);

    manual_out_thread_tid = k_thread_create(&manual_out_thread_data, manual_out_thread_stack,
        K_THREAD_STACK_SIZEOF(manual_out_thread_stack), manual_out_thread_code,
        NULL, NULL, NULL, manual_out_thread_prio, 0, K_NO_WAIT);

    action_thread_tid = k_thread_create(&action_thread_data, action_thread_stack,
        K_THREAD_STACK_SIZEOF(action_thread_stack), action_thread_code,
        NULL, NULL, NULL, action_thread_prio, 0, K_NO_WAIT);
    
    auto_out_thread_tid = k_thread_create(&auto_out_thread_data, auto_out_thread_stack,
        K_THREAD_STACK_SIZEOF(auto_out_thread_stack), auto_out_thread_code,
        NULL, NULL, NULL, auto_out_thread_prio, 0, K_NO_WAIT);
    


}

void conf()
{
  
    /* Init console service */
    console_init();
    
    /* Output a string*/ 
    console_write(NULL, prompt, sizeof(prompt) - 1);

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

    // button 1
    ret = gpio_pin_configure_dt(&button1, GPIO_INPUT);
    ret = gpio_pin_interrupt_configure_dt(&button1, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button1_cb_data, button1_pressed, BIT(button1.pin));
    gpio_add_callback(button1.port, &button1_cb_data);

    // button 2
    ret = gpio_pin_configure_dt(&button2, GPIO_INPUT);
    ret = gpio_pin_interrupt_configure_dt(&button2, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button2_cb_data, button2_pressed, BIT(button2.pin));
    gpio_add_callback(button2.port, &button2_cb_data);

    // button 3
    ret = gpio_pin_configure_dt(&button3, GPIO_INPUT);
    ret = gpio_pin_interrupt_configure_dt(&button3, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button3_cb_data, button3_pressed, BIT(button3.pin));
    gpio_add_callback(button3.port, &button3_cb_data);

    // button 4
    ret = gpio_pin_configure_dt(&button4, GPIO_INPUT);
    ret = gpio_pin_interrupt_configure_dt(&button4, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button4_cb_data, button4_pressed, BIT(button4.pin));
    gpio_add_callback(button4.port, &button4_cb_data);
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
    int amostra = 0;
    printk("\nRead Thread init\n");

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
                if(print_flag == 1)
                  printk("\33[2K\rAdc reading: raw:%4u / %4u mV",1023-adc_sample_buffer[0],3000-adc_value);
                amostra++;
                //printk("Amostra: %d\n\n",amostra);
            }
        }
        
        if (ON_flag == 1)
          k_sem_give(&sem_manual);
        else if (ON_flag == 0)
          k_sem_give(&sem_auto);
       
        /* Wait for next release instant */ 
        fin_time = k_uptime_get();
        if( fin_time < release_time) {
            k_msleep(release_time - fin_time);
            release_time += read_thread_period;
        }
    }

}

void manual_out_thread_code(void *argA , void *argB, void *argC)
{
    printk("\nOut Thread init\n");
    //int out;
    while(1)
    {
        k_sem_take(&sem_manual, K_FOREVER);

        ret = 0;

        pwm_value = 1023-adc_out;

        ret = pwm_pin_set_usec(pwm0_dev, NLED1,
		      pwmPeriod_us,(unsigned int)((pwmPeriod_us*pwm_value)/1023), PWM_POLARITY_NORMAL);
        if (ret)
          printk("Error %d: failed to set pulse width\n", ret);
    }
}


void action_thread_code(void *argA , void *argB, void *argC)
{
    printk("\nAction Thread init\n");
    //int adc_out_per;
    int dif1, dif2, sum1, sum2, average, i, k, j;
    int values_in[10] = {0,0,0,0,0,0,0,0,0,0};

    while(1)
    {
        k_sem_take(&sem_auto,  K_FOREVER);
        

        // FILTRO 

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
              // shift left das posi??es
              values_in[k] = values_in[k+1];
              // inserir valor lido na ultima posi??o do array
              if(k == 8)
                values_in[k+1] = 1023 - adc_sample_buffer[0];
            }
          }
          // Calcular soma dos valores do array
          sum1 += values_in[i];

          if(i == 9)
          {
            // m?dia e diferen?as para condi??es
            average = sum1 / 10;
            //printk("\naverage: %d\n",average);
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
            
            // if porque ? impossivel dividir por 0
            if(j != 0)
              aux2 = sum2 / j;
          }
        }


        // Leitura do terminal

        uint8_t c;
        uint16_t aux = 0;
        
        if(print_flag == 0)
        {
          printk("\nInsira a intensidade em %%: ");
          for(int i = 0; i <= 2; i++)
          {
            c = console_getchar();
            console_putchar(c);
            c = c-48;
            
            if(c >= 0 && c <= 9)
              aux = aux*10 + c;
          }
          adc_out = (uint16_t)(aux*1023/100);
          printk("avg: %4u", aux2);
          printk("\n");
        }
        print_flag = 1;

        k_sem_give(&sem_auto2);
    }
}


void auto_out_thread_code(void *argA , void *argB, void *argC)
{
    printk("\nOut Thread init\n");
    //int out;
    while(1)
    {
        k_sem_take(&sem_auto2, K_FOREVER);

        ret = 0;

        pwm_value = 1023-adc_out;

        ret = pwm_pin_set_usec(pwm0_dev, NLED1,
		      pwmPeriod_us,(unsigned int)((pwmPeriod_us*pwm_value)/1023), PWM_POLARITY_NORMAL);
        if (ret)
          printk("Error %d: failed to set pulse width\n", ret);
    }
}