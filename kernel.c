#include "kvstore.h"
#include "lib.h"
#include "uart.h"
#include "timer.h" 
#include "gpio.h"


//NOTE:test not called in main
void test_led_blink(void) {
  const uint32_t LED_PIN = 14;  // You can change this to any available GPIO pin
  char buf[128];
  
  uart_puts("\r\nInitializing LED on GPIO ");
  itoa(LED_PIN, buf, 10);
  uart_puts(buf);
  uart_puts("...\r\n");
  
  gpio_init();
  
  // Set LED pin as output
  gpio_set_output(LED_PIN);
  
  uart_puts("disco time..\r\n");
  
  while (1) {
      gpio_set(LED_PIN);
      uart_puts("LED ON\r\n");
      
      for (volatile int i = 0; i < 5000000; i++);
      
      gpio_clear(LED_PIN);
      uart_puts("LED OFF\r\n");
      
      for (volatile int i = 0; i < 5000000; i++);
  }
}

void test_temperature_sensor(void) {
  char buf[128];
  uart_puts("\r\ninit sensor?\r\n");
  
  gpio_init();
  spi_init();
  
  uart_puts("Reading temperature values...\r\n");
  
  while (1) {
    //NOTE:specific to MCP3008, 3v3
      uint32_t raw_value = adc_read(ADC_CHANNEL_0);
      
      uart_puts("Raw ADC value: ");
      itoa(raw_value, buf, 10);
      uart_puts(buf);
      
      // converting to V and assuming 3v3
      uint32_t voltage_mv = (raw_value * 3300) / 1023;
      uart_puts(" | Voltage: ");
      itoa(voltage_mv, buf, 10);
      uart_puts(buf);
      uart_puts(" mV");
      
      //convert to temp, need to check the sensor data sheet for its particular formula
      int32_t temperature = (voltage_mv - 500) / 10;
      uart_puts(" | Temperature: ");
      itoa(temperature, buf, 10);
      uart_puts(buf);
      uart_puts(" C\r\n");
      
      //Delay between readings
      for (volatile int i = 0; i < 10000000; i++);
  }
}

int parse_key(const char *str, unsigned int *key);
int parse_input(const char *str, unsigned int *key, unsigned int *value,
                unsigned int *is_signed);

void kernel_main() {
  kv_init();
  uart_puts("KVSTORE INIT\r\n");

  char buf[128];
  char clock_buf[128];

  while (1) {
    uart_puts("\r\n>");
    uart_gets(buf, 128);
    uart_puts("\r\n");
    
    uint64_t elapsed_us;
    timer_init();
    uint32_t timer_freq = timer_get_frequency();
    uart_puts("TIMER FREQUENCY: ");
    itoa(timer_freq, clock_buf, 10);
    uart_puts(clock_buf);
    uart_puts(" Hz\r\n");
    time_start();

    if (buf[0] == 'p' && buf[1] == 'u' && buf[2] == 't') {
      unsigned int key, value, is_signed = 0;
      if (parse_input(buf + 3, &key, &value, &is_signed)) {
        int result = kv_put(key, value, is_signed);
        if (result == 0) {
          uart_puts("\r\nKEY SET SUCCESSFULLY\r\n");
        } else {
          uart_puts("\r\nFAILED TO SET KEY\r\n");
        }
      } else {
        uart_puts("\r\nINVALID INPUT\r\n");
      }
    } else if (buf[0] == 'g' && buf[1] == 'e' && buf[2] == 't') {
      unsigned int key, value, is_signed = 0;
      if (parse_key(buf + 4, &key)) {
        int result = kv_get((int)key, &value, &is_signed);
        if (result == 0) {
          uart_puts("\r\nVALUE: ");
          if (is_signed) {
            uart_puts("-");
            itoa(-value, buf, 10);
            uart_puts(buf);
          } else {
            itoa(value, buf, 10);
            uart_puts(buf);
          }
          uart_puts("\r\n");
        } else {
          uart_puts("\r\nKEY NOT FOUND\r\n");
        }
      } else {
        uart_puts("\r\nINVALID INPUT\r\n");
      }
    } else if (buf[0] == 'd' && buf[1] == 'e' && buf[2] == 'l') {
      unsigned int key;
      if (parse_key(buf + 4, &key)) {
        int result = kv_delete(key);
        if (result == 0) {
          uart_puts("\r\nKEY DELETED\r\n");
        } else {
          uart_puts("\r\nFAILED TO DELETE KEY\r\n");
        }
      } else {
        uart_puts("\r\nINVALID INPUT\r\n");
      }
    } else if (buf[0] == 'l' && buf[1] == 'o' && buf[2] == 'g') {
      int count = 10;
      char *param = buf + 3;

      while (*param == ' ')
        param++;

      if (*param == '*') {
        count = -1;
      } else if (*param >= '0' && *param <= '9') {
        count = 0;
        while (*param >= '0' && *param <= '9') {
          count = count * 10 + (*param - '0');
          param++;
        }
      }

      int result = kv_print_log(count);
      if (result == -1) {
        uart_puts("\r\nNO ENTRIES TO DISPLAY\r\n");
      }
    }
    
    elapsed_us = time_end();
    uart_puts("ELAPSED TIME: ");
    itoa(elapsed_us, clock_buf, 10);
    uart_puts(clock_buf);
    uart_puts(" us\r\n");
    

  }
}

int parse_key(const char *str, unsigned int *key) {
  *key = 0;
  while (*str >= '0' && *str <= '9') {
    *key = *key * 10 + (*str - '0');
    str++;
  }
  // TEST:
  // char buf[128];
  // int temp = *key;
  // itoa(temp, buf, 10);
  // uart_puts("\r\nparsed key: ");
  // uart_puts(buf);
  // uart_puts("\r\n");
  return (*str == '\0' || *str == ' ');
}

int parse_input(const char *str, unsigned int *key, unsigned int *value,
                unsigned int *is_signed) {
  *key = *value = *is_signed = 0;
  while (*str == ' ')
    str++;
  if (!parse_key(str, key))
    return 0;

  while (*str >= '0' && *str <= '9')
    str++;
  while (*str == ' ')
    str++;

  if (!parse_key(str, value))
    return 0;

  while (*str >= '0' && *str <= '9')
    str++;
  while (*str == ' ')
    str++;

  if (*str == '1') {
    *is_signed = 1;
    str++;
  } else if (*str == '0') {
    *is_signed = 0;
    str++;
  } else {
    return 0;
  }
  return (*str == '\0');
}
