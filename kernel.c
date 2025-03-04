#include "uart.h"
#include "kvstore.h"

void kernel_main(){
    uart_init();
    kv_init();
    uart_puts("KVSTORE INIT\n");

    char buf[128];

    while(1){
        uart_puts("\n> ");
        uart_gets(buf,128);
        if(buf[0]=='p'&&buf[1]=='u'&&buf[2]=='t'){
            unsigned int key,value,is_signed=0;
            if(parse_input(buf+3,&key,&value,&is_signed)){
                kv_put(key,value,is_signed);
            }else{
                uart_puts("INVALID INPUT\n");
            }
        }
        else if(buf[0]=='g'&&buf[1]=='e'&&buf[2]=='t'){
            unsigned int key,value,is_signed=0;
            if(parse_key(buf+3,&key)){
                if(kv_get(key,&value,&is_signed)==0){
                    uart_puts("VALUE: ");
                    if(is_signed){
                        uart_puts("-");
                    }
                    uart_puts("0x");
                    uart_hex(value);
                    uart_puts("\n");
                }else{
                    uart_puts("KEY NOT FOUND\n");
                }

        }

        }else if(buf[0]=='d'&&buf[1]=='e'&&buf[2]=='l'){
            unsigned int key;
            if(parse_key(buf+3,&key)){
                kv_delete(key);
                uart_puts("KEY DELETED\n");
            }else{
                uart_puts("INVALID INPUT\n");
            }

    }}}



int parse_key(const char* str, unsigned int *key){
    *key = 0;
    while(*str>='0'&&*str<='9'){
        *key = *key*10 + (*str-'0');
        str++;
    }
    return (*str=='\0' || *str==' ');
}

int parse_input(const char* str,unsigned int *key, unsigned int *value,unsigned int* is_signed){
    *key= *value = *is_signed = 0;
    while(*str == ' ')str++;
    if (!parse_key(str,key)) return 0;

    while(*str>='0' && *str<='9') str++;
    while(*str == ' ')str++;

    if(!parse_key(str,value)) return 0;

    while(*str>='0' && *str<='9') str++;
    while(*str == ' ')str++;

    if(*str=='1'){
        *is_signed = 1;
        str++;
    }else if(*str=='0'){
        *is_signed = 0;
        str++;
    }else{
        return 0;
    }
    return (*str=='\0');

}