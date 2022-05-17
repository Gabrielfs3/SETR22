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
#include <sys/util.h>
#include <sys/printk.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/**
 * @{ @name   Time constants
 *    @brief  Two global constants.
 *
 *    @details These constantes are used in main to set time.
 */
#define SLEEP_TIME_MS	1
#define SLEEP   	    300
/**
* @}
*/


/**
 * @{ @name   State constants
 *    @brief  Five global constants.
 *
 *    @details These constantes are used in main for the implementation of the states 
 */
#define COINS           1
#define LIST            2
#define SELECT          3
#define RETURN          4 
#define CHOOSE          5 
/**
 * @}
 */


/**
 * @{ @name   Buttons constants
 *    @brief  Nine global constants.
 *
 *    @details These constantes are used for the buttons.
 */
#define GPIO0_NID DT_NODELABEL(gpio0)
#define SW0_NODE	DT_ALIAS(sw0)
#define SW1_NODE	DT_ALIAS(sw1)
#define SW2_NODE	DT_ALIAS(sw2)
#define SW3_NODE	DT_ALIAS(sw3)
#define SW4_NODE        0x3
#define SW5_NODE        0x4
#define SW6_NODE        0x1c
#define SW7_NODE        0x1d
/**
 * @}
 */


/**
 * @{ @name Buttons structures
 */
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
static struct gpio_callback button5_cb_data;
static struct gpio_callback button6_cb_data;
static struct gpio_callback button7_cb_data;
static struct gpio_callback button8_cb_data;
const struct device *gpio0_dev;
/**
 * @}
 */

/**
 * @{  @name Flags
 *     @brief Four global variables.
 */
int f5 = 0; 
int f6 = 0; 
int f7 = 0; 
int f8 = 0; 
/**
 * @}
 */


void conf_buttons();

/**
 * @{ @name   Button Fuctions 1
 *    @brief  Four Functions for the buttons of the Nordik development kit.
 *
 *    @details These four functions aim to read impulses of the buttons of Nordik development kit.
 *
 */
void button1_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
}
void button2_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
}
void button3_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
}
void button4_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
}
/**
 * @}
 */


/**
 * @{  @name   Button Fuctions 2
 *     @brief  Four Functions for the external buttons.
 *
 *    @details These four functions have the objective of emulate the insertion of coins, when the button is press flag goes 1 (fX = 1).
 */
void button5_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    f5 = 1;
}
void button6_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    f6 = 1;
}
void button7_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    f7 = 1;
}
void button8_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    f8 = 1;
}
/**
 * @}
 */



/**
 *
 *  @name   Main
 *  @brief  State machine according with state diagram.
 *          Each case of switch is a state.
 *
 */

void main(void)
{

/**
 * @param credit Variable that simulate the credit of vending machine.
 */
	double credit = 0.0; 

/**
 * @param need Variable that simulate the credit needed to buy a product.
 */
	double need = 0.0;

/**
 * @param intpart Variable that permits to print the double format.
 */
	int intpart = 0;

/**
 * @param decpart Variable that permits to print the double format.
 */
	int decpart = 0;

/**
 * @param lista List of products and price.
 */
	char lista[][40] = {"Coffee","Tuna sandwich","Beer","0.50","1.0","1.50"};

/**
 * @param lista2 List with the type of products.
 */
	char lista2[][40] = {"With Sugar","No Sugar","With Mayonese","No Mayonese","Superbock","Heineken"};

/**
 * @param prices List with the price of the products.
 */
	double prices[] = {0.50,1,1.50};

/**
 * @param state Variable that indicates the first state.
 */
	char STATE = COINS;

/**
 * @param  prod Variable that increments the product and price
 */
	int prod = 0;

  /**
 * @param k Variable to print list2.
 */
	int extra;
	
  /* Processing */  
  gpio0_dev = device_get_binding(DT_LABEL(GPIO0_NID));
  conf_buttons();
	printk("\nVending machine just started\n\n");
        

		while (1) {                        
                        /* States */                      
                        switch(STATE){
                                case COINS:
                                    intpart = (int) credit;
                                    decpart = (credit - intpart) * 10;
                                  if(f5 == 1){
                                    credit += 0.1;
                                    intpart = (int) credit;
                                    decpart = (credit - intpart) * 10;
                                    printf("\33[2K\rcredit = %d.%d EUR", intpart, decpart);
                                    STATE = COINS;
                                    f5 = 0;
                                    k_msleep(SLEEP);
                                  }
                                  if(f6 == 1){
                                    credit += 0.2;
                                    intpart = (int) credit;
                                    decpart = (credit - intpart) * 10;
                                    printf("\33[2K\rcredit = %d.%d EUR", intpart, decpart);
                                    STATE = COINS;
                                    f6 = 0;
                                    k_msleep(SLEEP);
                                  }
                                  if(f7 == 1){
                                    credit += 0.5;
                                    intpart = (int) credit;
                                    decpart = (credit - intpart) * 10;
                                    printf("\33[2K\rcredit = %d.%d EUR", intpart, decpart);
                                    STATE = COINS;
                                    f7 = 0;
                                    k_msleep(SLEEP);
                                  }
                                  if(f8 == 1){
                                    credit += 1;
                                    intpart = (int) credit;
                                    decpart = (credit - intpart) * 10;
                                    printf("\33[2K\rcredit = %d.%d EUR", intpart, decpart);
                                    STATE = COINS;
                                    f8 = 0;
                                    k_msleep(SLEEP);
                                  } 
                                  if((gpio_pin_get_dt(&button1) > 0) | (gpio_pin_get_dt(&button2) > 0)){
                                    //printf("\n\nYour credit = %d.%d\n", intpart, decpart);
                                    printf("\n\nProduct: %s - Price: %s EUR",lista[prod],lista[prod+3]);
                                    STATE = LIST;
                                    k_msleep(SLEEP);
                                  }
                                  if(gpio_pin_get_dt(&button4) > 0){
                                    STATE = RETURN;
                                  }
                                  break;
                                case LIST:
                                  if((f5 == 1) | (f6 == 1) | (f7 == 1) | (f8 == 1))
                                  {
                                    printf("\n");
                                    STATE = COINS;
                                    f5 = 0; f6 = 0; f7 = 0; f8 = 0;
                                  }
                                  if(gpio_pin_get_dt(&button1) > 0){
                                    prod++;
                                    if(prod > 2)
                                      prod = 0;
                                    printf("\33[2K\rProduct: %s - Preco: %s EUR",lista[prod],lista[prod+3]);
                                    STATE = LIST;
                                    k_msleep(SLEEP);
                                  }
                                  if(gpio_pin_get_dt(&button2) > 0){
                                    prod--;
                                    if(prod < 0)
                                      prod = 2;
                                    printf("\33[2K\rProduct: %s - Preco: %s EUR",lista[prod],lista[prod+3]);
                                    STATE = LIST;
                                    k_msleep(SLEEP);
                                  }
                                  if(gpio_pin_get_dt(&button3) > 0)
                                  {
                                    if(lista[prod]==lista[0])
                                    {
                                      extra = 0;
                                      printf("\n%s",lista2[extra]);
                                    }
                                    if(lista[prod]==lista[1])
                                    {
                                      extra = 2;
                                      printf("\n%s",lista2[extra]);
                                    }
                                    if(lista[prod]==lista[2])
                                    {
                                      extra = 4;
                                      printf("\n%s",lista2[extra]);
                                    }
                                    STATE = CHOOSE;
                                    k_msleep(SLEEP);
                                  }
                                  if(gpio_pin_get_dt(&button4) > 0 && credit > 0){
                                    STATE = RETURN;
                                  }
                                  break;
                                case SELECT:
                                  if(credit - prices[prod] < 0)
                                  {
                                    need = prices[prod] - credit;
                                    int intpart2 = (int) need;
                                    int decpart2 = (need - intpart2) * 10;
                                    printf("\n\nInsuficient credit: %d.%d\n\nYou need more: %d.%d EUR\n",intpart,decpart,intpart2,decpart2);
                                    STATE = COINS;
                                    k_msleep(SLEEP);
                                  }
                                  else{
                                    printf("\n\n%s selected\n",lista[prod]);
                                    credit = credit - prices[prod];
                                    intpart = (int) credit;
                                    decpart = (credit - intpart) * 10;
                                    STATE = RETURN;
                                    k_msleep(SLEEP);
                                  }
                                  break;
                                case RETURN:
                                  if(credit <= 0)
                                  {
                                    credit = 0;
                                    printf("\nNo credit to return\n\n");
                                    k_msleep(200);
                                    STATE = COINS;
                                  }
                                  if(credit > 0){
                                    printf("\nCredit returned: %d.%d EUR\n\n",intpart,decpart);
                                    credit = 0;
                                    STATE = COINS;
                                    k_msleep(SLEEP);
                                  }
                                  break;
                                case CHOOSE:
                                  if(lista[prod] == lista[0])
                                  {
                                    if((gpio_pin_get_dt(&button1) > 0) | (gpio_pin_get_dt(&button2) > 0))
                                    {
                                      extra++;
                                      if(extra > 1)
                                        extra = 0;
                                      printf("\33[2K\r%s",lista2[extra]);
                                      STATE = CHOOSE;
                                      k_msleep(SLEEP);
                                    }
                                    else if(gpio_pin_get_dt(&button3) > 0)
                                      STATE = SELECT;
                                  }
                                  if(lista[prod] == lista[1])
                                  {
                                    
                                    if((gpio_pin_get_dt(&button1) > 0) | (gpio_pin_get_dt(&button2) > 0))
                                    {
                                      extra++;
                                      if(extra > 3)
                                        extra = 2;
                                      printf("\33[2K\r%s",lista2[extra]);
                                      STATE = CHOOSE;
                                      k_msleep(SLEEP);
                                    }
                                    else if(gpio_pin_get_dt(&button3) > 0)
                                      STATE = SELECT;
                                  }
                                  if(lista[prod] == lista[2])
                                  {
                                    if((gpio_pin_get_dt(&button1) > 0) | (gpio_pin_get_dt(&button2) > 0))
                                    {
                                      extra++;
                                      if(extra > 5)
                                        extra = 4;
                                      printf("\33[2K\r%s",lista2[extra]);
                                      STATE = CHOOSE;
                                      k_msleep(SLEEP);
                                    }
                                    else if(gpio_pin_get_dt(&button3) > 0)
                                      STATE = SELECT;
                                  }
                                  break;
                        }
			k_msleep(SLEEP_TIME_MS);
		}
}

/**
*
*  @name   Buttons Configuration
*  @brief  This function have the objetive of configure all the buttons.
*          
*
*/

void conf_buttons()
{

/**
 * @param ret Variable that configure the input of the buttons.
 */

int ret;

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

        // button 5
	      ret = gpio_pin_configure(gpio0_dev, SW4_NODE, GPIO_INPUT | GPIO_PULL_UP);
        ret = gpio_pin_interrupt_configure(gpio0_dev, SW4_NODE, GPIO_INT_EDGE_TO_INACTIVE);

        gpio_init_callback(&button5_cb_data, button5_pressed, BIT(SW4_NODE));
        gpio_add_callback(gpio0_dev, &button5_cb_data);

        // button 6
	      ret = gpio_pin_configure(gpio0_dev, SW5_NODE, GPIO_INPUT | GPIO_PULL_UP);
        ret = gpio_pin_interrupt_configure(gpio0_dev, SW5_NODE, GPIO_INT_EDGE_TO_INACTIVE);

        gpio_init_callback(&button6_cb_data, button6_pressed, BIT(SW5_NODE));
        gpio_add_callback(gpio0_dev, &button6_cb_data);

        // button 7
	      ret = gpio_pin_configure(gpio0_dev, SW6_NODE, GPIO_INPUT | GPIO_PULL_UP);
        ret = gpio_pin_interrupt_configure(gpio0_dev, SW6_NODE, GPIO_INT_EDGE_TO_INACTIVE);

        gpio_init_callback(&button7_cb_data, button7_pressed, BIT(SW6_NODE));
        gpio_add_callback(gpio0_dev, &button7_cb_data);

        // button 8
	      ret = gpio_pin_configure(gpio0_dev, SW7_NODE, GPIO_INPUT | GPIO_PULL_UP);
        ret = gpio_pin_interrupt_configure(gpio0_dev, SW7_NODE, GPIO_INT_EDGE_TO_INACTIVE);

        gpio_init_callback(&button8_cb_data, button8_pressed, BIT(SW7_NODE));
        gpio_add_callback(gpio0_dev, &button8_cb_data);
}
