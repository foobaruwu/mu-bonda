#ifndef GPIO_H
#define GPIO_H

#pragma once
#include "lib.h"

// pi gpio base addr
#define GPIO_BASE       0x1f000d0000   // gpio ref
#define GPIO_RIO_BASE   0x1f000e0000   // RIO reg
#define GPIO_PAD_BASE   0x1f000f0000   // pad control reg

//also note rpi5 does not have inherent ADC, so we need ext, so assuming we have a 3v3 ADC something like MCP3008

#define GPIO_FUNC_INPUT  0
#define GPIO_FUNC_OUTPUT 1
#define GPIO_FUNC_ALT0   4
#define GPIO_FUNC_ALT1   5
#define GPIO_FUNC_ALT2   6
#define GPIO_FUNC_ALT3   7
#define GPIO_FUNC_ALT4   3
#define GPIO_FUNC_ALT5   2

#define RIO_OUT_OFFSET     0x0000
#define RIO_OE_OFFSET      0x0004
#define RIO_IN_OFFSET      0x0008
#define RIO_INSYNC_OFFSET  0x000c
#define RIO_XOR_OFFSET     0x1000
#define RIO_SET_OFFSET     0x2000
#define RIO_CLR_OFFSET     0x3000

// spi for adc
#define SPI_CE0_PIN     8   
#define SPI_MISO_PIN    9   
#define SPI_MOSI_PIN    10  
#define SPI_SCLK_PIN    11  

//NOTE:specific to mpc3008, 3v3 
#define ADC_CHANNEL_0   0
#define ADC_CHANNEL_1   1
#define ADC_CHANNEL_2   2
#define ADC_CHANNEL_3   3
#define ADC_CHANNEL_4   4
#define ADC_CHANNEL_5   5
#define ADC_CHANNEL_6   6
#define ADC_CHANNEL_7   7

void gpio_init(void);
void gpio_set_function(uint32_t pin, uint32_t function);
void gpio_set_output(uint32_t pin);
void gpio_set_input(uint32_t pin);
void gpio_set(uint32_t pin);
void gpio_clear(uint32_t pin);
uint32_t gpio_read(uint32_t pin);

void spi_init(void);
uint32_t adc_read(uint32_t channel);

#endif 