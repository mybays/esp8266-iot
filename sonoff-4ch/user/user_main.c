/*
 *  Copyright 2016 HomeACcessoryKid - HacK - homeaccessorykid@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
 
#include "esp_common.h"
#include "hkc.h"
#include "gpio.h"
#include "queue.h"
#include "uart.h"

xQueueHandle identifyQueue;

struct  gpio
{
    int aid;
    int iid;
    cJSON *value;
} relay_info1,relay_info2,relay_info3,relay_info4;

//按键1中断函数
void button0_pressed()
{
    int new;
    static uint32 oldtime;

    if ( (oldtime+200) < (oldtime=(system_get_time()/1000) ) )
    {
        //200ms debounce guard
        new = GPIO_INPUT(GPIO_Pin_12)^1; 		
        GPIO_OUTPUT(GPIO_Pin_12,new);			                //Toggle relay
        relay_info1.value->type=new;
        change_value(relay_info1.aid,relay_info1.iid,relay_info1.value);   //Save new value internally
        send_events(NULL,relay_info1.aid,relay_info1.iid);                //Propagate to HomeKit controller
    }
}

//按键2中断函数
void button9_pressed()
{
    int new;
    static uint32   oldtime;

    if ( (oldtime+200) < (oldtime=(system_get_time()/1000) ) )
    {
        //200ms debounce guard
        new = GPIO_INPUT(GPIO_Pin_5)^1;        
        GPIO_OUTPUT(GPIO_Pin_5,new);                           //Toggle relay
        relay_info2.value->type=new;
        change_value(relay_info2.aid,relay_info2.iid,relay_info2.value);   //Save new value internally
        send_events(NULL,relay_info2.aid,relay_info2.iid);                //Propagate to HomeKit controller
    }
}

//按键3中断函数
void button10_pressed()
{
    int new;
    static uint32   oldtime;

    if ( (oldtime+200) < (oldtime=(system_get_time()/1000) ) )
    {
        //200ms debounce guard
        new = GPIO_INPUT(GPIO_Pin_4)^1;        
        GPIO_OUTPUT(GPIO_Pin_4,new);                           //Toggle relay
        relay_info3.value->type=new;
        change_value(relay_info3.aid,relay_info3.iid,relay_info3.value);   //Save new value internally
        send_events(NULL,relay_info3.aid,relay_info3.iid);                //Propagate to HomeKit controller
    }
}

//按键4中断函数
void button14_pressed()
{
    int new;
    static uint32   oldtime;

    if ( (oldtime+200) < (oldtime=(system_get_time()/1000) ) )
    {
        //200ms debounce guard
        new = GPIO_INPUT(GPIO_Pin_15)^1;        
        GPIO_OUTPUT(GPIO_Pin_15,new);                           //Toggle relay
        relay_info4.value->type=new;
        change_value(relay_info4.aid,relay_info4.iid,relay_info4.value);   //Save new value internally
        send_events(NULL,relay_info4.aid,relay_info4.iid);                //Propagate to HomeKit controller
    }
}

void relay1(int aid, int iid, cJSON *value, int mode)
{
    GPIO_ConfigTypeDef gpio0_in_cfg,gpio12_in_cfg,gpio13_in_cfg;

    switch (mode)
    {
        case 1:
        { //changed by gui
            if (value)
            {
                char *out; out=cJSON_Print(value);
                os_printf("led %s\n",out);
                free(out);  // Print to text, print it, release the string.
                GPIO_OUTPUT(GPIO_Pin_12, value->type); 		//GPIO12 high -> relay on
	    }
        }
        break;
        case 0:
        {
            //init
            gpio0_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;         //Falling edge trigger
            gpio0_in_cfg.GPIO_Mode = GPIO_Mode_Input;               //Input mode
            gpio0_in_cfg.GPIO_Pin = GPIO_Pin_0;                    //Enable GPIO0
            gpio_config(&gpio0_in_cfg);                                 //Init function
            gpio_intr_callbacks[0]=button0_pressed;                      //GPIO0 callback

            //GPIO12 = relay
            gpio12_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;        //No interrupt
            gpio12_in_cfg.GPIO_Mode = GPIO_Mode_Output;             //Output mode
            gpio12_in_cfg.GPIO_Pullup = GPIO_PullUp_EN;      		//
            gpio12_in_cfg.GPIO_Pin = GPIO_Pin_12;                  //Enable GPIO12
            gpio_config(&gpio12_in_cfg);                                //Init function

            //GPIO13 = LED
            gpio13_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;        //No interrupt
            gpio13_in_cfg.GPIO_Mode = GPIO_Mode_Output;             //Output mode
            gpio13_in_cfg.GPIO_Pullup = GPIO_PullUp_EN;               //
            gpio13_in_cfg.GPIO_Pin = GPIO_Pin_13;                  //Enable GPIO13
            gpio_config(&gpio13_in_cfg);                                //Init function

            
            relay1(aid,iid,value,1);                                     //Init the outputs

            relay_info1.aid=aid;                                         //Init HomeKit related values 
	    relay_info1.iid=iid;
            relay_info1.value=cJSON_CreateBool(0); 						
        }
        break;
        case 2:
        {
            //update
            //do nothing
        }
        break;
        default:
        {
            //print an error?
        }
        break;
    }
}


void relay2(int aid, int iid, cJSON *value, int mode)
{
    GPIO_ConfigTypeDef gpio9_in_cfg,gpio5_in_cfg,gpio13_in_cfg;

    switch (mode)
    {
        case 1:
        { //changed by gui
            if (value)
            {
                char *out; out=cJSON_Print(value);
                os_printf("led:%s\n",out);
                free(out);  // Print to text, print it, release the string.
                GPIO_OUTPUT(GPIO_Pin_5, value->type);        //GPIO12 high -> relay on
            }
        }
        break;
        case 0:
        {
            //init
            gpio9_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;         //Falling edge trigger
            gpio9_in_cfg.GPIO_Mode  = GPIO_Mode_Input;               //Input mode
            gpio9_in_cfg.GPIO_Pin = GPIO_Pin_9;                    //Enable GPIO9
            gpio_config(&gpio9_in_cfg);                                 //Init function
            gpio_intr_callbacks[9]=button9_pressed;                      //GPIO9 callback

            //GPIO5 = relay
            gpio5_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;        //No interrupt
            gpio5_in_cfg.GPIO_Mode = GPIO_Mode_Output;             //Output mode
            gpio5_in_cfg.GPIO_Pullup = GPIO_PullUp_EN;           //
            gpio5_in_cfg.GPIO_Pin = GPIO_Pin_5;                  //Enable GPIO5
            gpio_config(&gpio5_in_cfg);                                //Init function

            //GPIO13 = LED
            gpio13_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;        //No interrupt
            gpio13_in_cfg.GPIO_Mode = GPIO_Mode_Output;             //Output mode
            gpio13_in_cfg.GPIO_Pullup  = GPIO_PullUp_EN;               //
            gpio13_in_cfg.GPIO_Pin = GPIO_Pin_13;                  //Enable GPIO13
            gpio_config(&gpio13_in_cfg);                                //Init function

            relay2(aid,iid,value,1);                                     //Init the outputs

            relay_info2.aid=aid;                                         //Init HomeKit related values 
            relay_info2.iid=iid;
            relay_info2.value=cJSON_CreateBool(0);                      
        }
        break;
        case 2:
        { //update
            //do nothing
        }
        break;
        default:
        {
            //print an error?
        }
        break;
    }
}

void relay3(int aid, int iid, cJSON *value, int mode)
{
    GPIO_ConfigTypeDef gpio10_in_cfg,gpio4_in_cfg,gpio13_in_cfg;

    switch (mode)
    {
        case 1:
        { //changed by gui
            if (value)
            {
                char *out; out=cJSON_Print(value);
                os_printf("led %s\n",out);
                free(out);  // Print to text, print it, release the string.
                GPIO_OUTPUT(GPIO_Pin_4, value->type);      //GPIO12 high -> relay on
            }
        }
        break;
        case 0:
        {
            //init
            gpio10_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;         //Falling edge trigger
            gpio10_in_cfg.GPIO_Mode = GPIO_Mode_Input;               //Input mode
            gpio10_in_cfg.GPIO_Pin = GPIO_Pin_10;                    //Enable GPIO10
            gpio_config(&gpio10_in_cfg);                                 //Init function
            gpio_intr_callbacks[10]=button10_pressed;                      //GPIO10 callback

            //GPIO4 = relay
            gpio4_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;        //No interrupt
            gpio4_in_cfg.GPIO_Mode = GPIO_Mode_Output;             //Output mode
            gpio4_in_cfg.GPIO_Pullup = GPIO_PullUp_EN;           //
            gpio4_in_cfg.GPIO_Pin = GPIO_Pin_4;                  //Enable GPIO4
            gpio_config(&gpio4_in_cfg);                                //Init function

            //GPIO13 = LED
            gpio13_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;        //No interrupt
            gpio13_in_cfg.GPIO_Mode = GPIO_Mode_Output;             //Output mode
            gpio13_in_cfg.GPIO_Pullup = GPIO_PullUp_EN;               //
            gpio13_in_cfg.GPIO_Pin = GPIO_Pin_13;                  //Enable GPIO13
            gpio_config(&gpio13_in_cfg);                                //Init function

            relay3(aid,iid,value,1);                                     //Init the outputs

            relay_info3.aid=aid;                                         //Init HomeKit related values 
            relay_info3.iid=iid;
            relay_info3.value=cJSON_CreateBool(0);                      
        }
        break;
        case 2:
        {
            //update
            //do nothing
        }
        break;
        default:
        {
            //print an error?
        }
        break;
    }
}

void relay4(int aid, int iid, cJSON *value, int mode)
{

    GPIO_ConfigTypeDef gpio14_in_cfg,gpio15_in_cfg,gpio13_in_cfg;

    switch (mode)
    {
        case 1:
        {
            //changed by gui
            if (value)
            {
                char *out; out=cJSON_Print(value);
                os_printf("led %s\n",out);
                free(out);  // Print to text, print it, release the string.
                GPIO_OUTPUT(GPIO_Pin_15, value->type);      //GPIO12 high -> relay on
            }
        }
        break;
        case 0:
        {
            //init
            gpio14_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_NEGEDGE;         //Falling edge trigger
            gpio14_in_cfg.GPIO_Mode = GPIO_Mode_Input;               //Input mode
            gpio14_in_cfg.GPIO_Pin  = GPIO_Pin_14;                    //Enable GPIO14
            gpio_config(&gpio14_in_cfg);                                 //Init function
            gpio_intr_callbacks[14]=button14_pressed;                      //GPIO14 callback

            //GPIO15 = relay
            gpio15_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;        //No interrupt
            gpio15_in_cfg.GPIO_Mode = GPIO_Mode_Output;             //Output mode
            gpio15_in_cfg.GPIO_Pullup  = GPIO_PullUp_EN;           //
            gpio15_in_cfg.GPIO_Pin = GPIO_Pin_15;                  //Enable GPIO15
            gpio_config(&gpio15_in_cfg);                                //Init function

            //GPIO13 = LED
            gpio13_in_cfg.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;        //No interrupt
            gpio13_in_cfg.GPIO_Mode = GPIO_Mode_Output;             //Output mode
            gpio13_in_cfg.GPIO_Pullup = GPIO_PullUp_EN;               //
            gpio13_in_cfg.GPIO_Pin = GPIO_Pin_13;                  //Enable GPIO13
            gpio_config(&gpio13_in_cfg);                                //Init function

            relay4(aid,iid,value,1);                                     //Init the outputs

            relay_info4.aid=aid;                                         //Init HomeKit related values 
            relay_info4.iid=iid;
            relay_info4.value=cJSON_CreateBool(0);                      
        }
        break;
        case 2:
        {
            //update
            //do nothing
        }
        break;
        default:
        {
            //print an error?
        }
        break;
    }
}

void identify_task(void *arg)
{
    int i,original;
    while(1)
    {
        while(!xQueueReceive(identifyQueue,NULL,10));	        //wait for a queue item
        original=GPIO_INPUT(GPIO_Pin_13); 			//get original state
        for (i=0;i<4;i++)
        {
            GPIO_OUTPUT(GPIO_Pin_13,original^1); 		//toggle
            vTaskDelay(5); 					//0.05 sec
            GPIO_OUTPUT(GPIO_Pin_13,original^0);		//toggle
            vTaskDelay(5); 					//0.05 sec
        }
    }
}

void identify(int aid, int iid, cJSON *value, int mode)
{
    switch (mode)
    {
        case 1:
        {
            //changed by gui
            xQueueSend(identifyQueue,NULL,0);
        }
        break;
        case 0:
        {
            //init
            identifyQueue = xQueueCreate( 1, 0 );
        	   xTaskCreate(identify_task,"identify",256,NULL,2,NULL);
        }
        break;
        case 2:
        {
            //update
            //do nothing
        }
        break;
        default:
        {
            //print an error?
        }
        break;
    }
}

void hkc_user_init(char *accname)
{
    //do your init thing beyond the bear minimum
    //avoid doing it in user_init else no heap left for pairing
    cJSON *accs,*sers,*chas;
    int aid=0,iid=0;

    accs=initAccessories();
    
    sers=addAccessory(accs,++aid);
    //service 0 describes the accessory
    chas=addService(sers,++iid,APPLE,ACCESSORY_INFORMATION_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,accname,NULL);
    addCharacteristic(chas,aid,++iid,APPLE,MANUFACTURER_C,"mybays",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,MODEL_C,"Rev-1",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,SERIAL_NUMBER_C,"1",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,IDENTIFY_C,NULL,identify);
    //service 1
    chas=addService(sers,++iid,APPLE,SWITCH_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,"Sonoff1",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,POWER_STATE_C,"0",relay1);        //Switched off by default, "1" if you want it on by default

    //service 2
    chas=addService(sers,++iid,APPLE,SWITCH_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,"Sonoff2",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,POWER_STATE_C,"0",relay2);        //Switched off by default, "1" if you want it on by default

    //service 3
    chas=addService(sers,++iid,APPLE,SWITCH_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,"Sonoff3",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,POWER_STATE_C,"0",relay3);        //Switched off by default, "1" if you want it on by default

    //service 4
    chas=addService(sers,++iid,APPLE,SWITCH_S);
    addCharacteristic(chas,aid,++iid,APPLE,NAME_C,"Sonoff4",NULL);
    addCharacteristic(chas,aid,++iid,APPLE,POWER_STATE_C,"0",relay4);        //Switched off by default, "1" if you want it on by default

    gpio_intr_handler_register(gpio_intr_handler,NULL);                     //Register the interrupt function
    GPIO_INTERRUPT_ENABLE;
}

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    //uart_div_modify(0, UART_CLK_FREQ / 115200);	
    UART_SetBaudrate(UART0,115200);
    os_printf("start of user_init @ %d\n",system_get_time()/1000);
    
//use this block only once to set your favorite access point or put your own selection routine
   wifi_set_opmode(STATION_MODE); 
    struct station_config *sconfig = (struct station_config *)zalloc(sizeof(struct station_config));
    sprintf(sconfig->ssid, "test"); //don't forget to set this if you use it
    sprintf(sconfig->password, "test"); //don't forget to set this if you use it
    wifi_station_set_config(sconfig);
    free(sconfig);
    wifi_station_connect(); 
    
    //try to only do the bare minimum here and do the rest in hkc_user_init
    // if not you could easily run out of stack space during pairing-setup
    hkc_init("Sonoff");
    
    os_printf("end of user_init @ %d\n",system_get_time()/1000);
}

/***********************************************************************************
 * FunctionName : user_rf_cal_sector_set forced upon us by espressif since RTOS1.4.2
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal    B : rf init data    C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
***********************************************************************************/
uint32 user_rf_cal_sector_set(void)
{
    extern char flashchip;
    SpiFlashChip *flash = (SpiFlashChip*)(&flashchip + 4);
    // We know that sector size is 4096
    //uint32_t sec_num = flash->chip_size / flash->sector_size;
    uint32_t sec_num = flash->chip_size >> 12;
    return sec_num - 5;
}
