
//
//Register the app for device notifications, using the serial port device class
//
INTERNAL BOOL devices_registerForNotifications(HWND hWnd){
    
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    
    memory_clear(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    
    HDEVNOTIFY hDeviceNotify;
    hDeviceNotify = RegisterDeviceNotificationW(hWnd,
                                                &NotificationFilter,
                                                DEVICE_NOTIFY_WINDOW_HANDLE|
                                                DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
    if (!hDeviceNotify){
        return FALSE;
    }else{
        return TRUE;
    }
}


//
//Adds a device to our list
//
serial_device_t* devices_pushDevice(serial_device_list_t* list){
    if (list->deviceCount >= ARRAY_COUNT(list->devices)){
        return NULL;
    }
    return &list->devices[list->deviceCount++];
}


//
//Walks over the available serial port devices on the system and builts a list of the results
//
int devices_getAvailableSerialPorts(HWND hWnd, serial_device_list_t* list){
    int err;
    int rc = 0;
    HDEVINFO hDevInfo;
    
    hDevInfo = SetupDiGetClassDevsA(&serialPortInterfaceGUID,NULL,hWnd,DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE){
        DEBUG("SetupDiCreateDeviceInfoList failed %d\n",GetLastError());
        return -1;
    }
    
    //Clear the old list
    list->deviceCount = 0;
    
    DWORD index = 0;
    SP_DEVINFO_DATA dev;
    clearStruct(dev);
    dev.cbSize = sizeof(SP_DEVINFO_DATA);
    while(SetupDiEnumDeviceInfo(hDevInfo,index,&dev)){
        TEMP_PATH_BUFF(buf);
        
        serial_device_t* myDevice = devices_pushDevice(list);
        
        DEBUG("Device %d\n",index);
        DWORD size;
        if (SetupDiGetDeviceInstanceIdA(hDevInfo,&dev,myDevice->path,sizeof(myDevice->path),&size)){
            DEBUG("Path %s\n",myDevice->path);
        }
        
        //This buffer should be big enough for most devices, if not we will just have to go and make it bigger
        BYTE buffer[1204];
        DEVPROPTYPE ulPropertyType;
        
        if (!SetupDiGetDevicePropertyW(hDevInfo,&dev,&DEVPKEY_Device_FriendlyName,&ulPropertyType,
                                       buffer,sizeof(buffer),
                                       &size,0)){
            err = GetLastError();
            DEBUG("SetupDiGetDevicePropertyW failed %d\n",GetLastError());
        }
        snprintf(myDevice->friendlyName,sizeof(myDevice->friendlyName),"%ws",(WCHAR*)buffer);
        DEBUG("Name: %s\n",myDevice->friendlyName);
        
        
        if (!SetupDiGetDevicePropertyW(hDevInfo,&dev,&DEVPKEY_Device_Manufacturer,&ulPropertyType,
                                       buffer,sizeof(buffer),
                                       &size,0)){
            err = GetLastError();
            DEBUG("SetupDiGetDevicePropertyW failed %d\n",GetLastError());
        }
        snprintf(myDevice->manufacturer,sizeof(myDevice->manufacturer),"%ws",(WCHAR*)buffer);
        DEBUG("Manufacturer: %s\n",myDevice->manufacturer);
        
        ++index;
    }
    
    err = GetLastError();
    if (err != ERROR_NO_MORE_ITEMS){
        DEBUG("SetupDiEnumDeviceInfo failed %d\n",err);
        rc = -1;
        goto exit;
    }
    
    exit:
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return rc;
    
}


//end
