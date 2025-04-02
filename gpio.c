#include "gpio.h"
#include "timer.h"

static volatile uint32_t* gpio_regs = (uint32_t*)GPIO_BASE;
static volatile uint32_t* rio_regs = (uint32_t*)GPIO_RIO_BASE;
static volatile uint32_t* pad_regs = (uint32_t*)GPIO_PAD_BASE;

typedef struct {
    uint32_t status;
    uint32_t ctrl;
} gpio_pin_regs_t;

typedef struct {
    uint32_t out;
    uint32_t oe;
    uint32_t in;
    uint32_t in_sync;
} rio_regs_t;

#define GPIO_PINS ((volatile gpio_pin_regs_t*)gpio_regs)
#define RIO ((volatile rio_regs_t*)rio_regs)
#define RIO_XOR ((volatile rio_regs_t*)(rio_regs + (RIO_XOR_OFFSET/4)))
#define RIO_SET ((volatile rio_regs_t*)(rio_regs + (RIO_SET_OFFSET/4)))
#define RIO_CLR ((volatile rio_regs_t*)(rio_regs + (RIO_CLR_OFFSET/4)))
#define PADS ((volatile uint32_t*)pad_regs)

void gpio_init(void) {
///no init needed i think
//needed if we have to set up pads, but not needed for rpi5
}

void gpio_set_function(uint32_t pin, uint32_t function) {
    if (pin < 30) { 
        GPIO_PINS[pin].ctrl = function;
    }
}

void gpio_set_output(uint32_t pin) {
    if (pin < 30) {
        gpio_set_function(pin, GPIO_FUNC_OUTPUT);
        RIO_SET->oe |= (1 << pin);  
    }
}

void gpio_set_input(uint32_t pin) {
    if (pin < 30) {
        gpio_set_function(pin, GPIO_FUNC_INPUT);
        RIO_CLR->oe &= ~(1 << pin);  
    }
}

void gpio_set(uint32_t pin) {
    if (pin < 30) {
        RIO_SET->out = (1 << pin);
    }
}

void gpio_clear(uint32_t pin) {
    if (pin < 30) {
        RIO_CLR->out = (1 << pin);
    }
}

uint32_t gpio_read(uint32_t pin) {
    if (pin < 30) {
        return (RIO->in & (1 << pin)) ? 1 : 0;
    }
    return 0;
}
//bit mapped spi for adc
void spi_delay(void) {
    for (volatile int i = 0; i < 10; i++);
}

void spi_init(void) {
    gpio_set_output(SPI_CE0_PIN);
    gpio_set_output(SPI_MOSI_PIN);
    gpio_set_output(SPI_SCLK_PIN);
    gpio_set_input(SPI_MISO_PIN);
    
    gpio_set(SPI_CE0_PIN);    
    gpio_clear(SPI_SCLK_PIN); 
    gpio_clear(SPI_MOSI_PIN); 
}

uint32_t spi_transfer(uint8_t data_out) {
    uint8_t data_in = 0;
    
    for (int i = 7; i >= 0; i--) {
        if (data_out & (1 << i)) {
            gpio_set(SPI_MOSI_PIN);
        } else {
            gpio_clear(SPI_MOSI_PIN);
        }
        
        spi_delay();
        gpio_set(SPI_SCLK_PIN); 
        
        spi_delay();
        if (gpio_read(SPI_MISO_PIN)) {
            data_in |= (1 << i);
        }
        
        gpio_clear(SPI_SCLK_PIN); 
    }
    
    return data_in;
}


//NOTE: assuming 10b adc like MPC3008, if not we will have to write code for that
uint32_t adc_read(uint32_t channel) {
    uint32_t result = 0;
    
    if (channel > 7) return 0; 
    
    gpio_clear(SPI_CE0_PIN); 
    

    uint8_t command = 0b11000000 | (channel << 3);
    
    spi_transfer(command);
    
    uint8_t high_byte = spi_transfer(0);
    uint8_t low_byte = spi_transfer(0);
    
    gpio_set(SPI_CE0_PIN);
    
    result = ((high_byte & 0x03) << 8) | low_byte;
    
    return result;
}