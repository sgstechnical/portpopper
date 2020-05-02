#ifndef _DEVICES_H_
#define _DEVICES_H_


typedef struct{
    char friendlyName[1024];
    char path[1024];
    char manufacturer[1024];
}serial_device_t;

#define SERIAL_DEVICES_MAX 128
typedef struct{
    int deviceCount;
    serial_device_t devices[SERIAL_DEVICES_MAX];
}serial_device_list_t;



#endif
