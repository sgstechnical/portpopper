
//Pre-declare the windows proc
LRESULT WINAPI mainWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//
//Main entry point
//
#if WIN_FORMS_APP
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
#else
int main (int argc, char** argv)
#endif
{
    TEMP_BUFF(buf);
    SETBUF(); // Sets the stdout buffer to flush per character
    
    WNDCLASSEXA wc ={};
    wc.cbSize=sizeof(WNDCLASSEX);
    wc.style=CS_CLASSDC;
    wc.lpfnWndProc=mainWindowProc;
    wc.cbClsExtra=NULL;
    wc.cbWndExtra=NULL;
    wc.hInstance=GetModuleHandle(NULL);
    wc.hIcon=LoadIconA(GetModuleHandle(NULL),MAKEINTRESOURCE(ID_MAIN_ICON));
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)COLOR_WINDOW;
    wc.lpszMenuName=NULL;
    wc.lpszClassName=HIDDEN_WINDOW_CLASS_NAME;
    wc.hIconSm=NULL;
    
    RegisterClassEx(&wc);
    
    // NOTE(Simon): V6 Common controls setup
    INITCOMMONCONTROLSEX ctrls = {sizeof(INITCOMMONCONTROLSEX)};
    ctrls.dwICC = ICC_STANDARD_CLASSES | ICC_TAB_CLASSES | ICC_UPDOWN_CLASS |
        ICC_USEREX_CLASSES |  ICC_LINK_CLASS;
    
    if (!InitCommonControlsEx(&ctrls)){
        DEBUG("Common Controls Failed\n");
    }
    
    //This is a 'dummy' window that we should never show, we need this for a notify icon and
    //device notifications
    HWND hWnd = CreateWindowEx(0,wc.lpszClassName,NULL,0,CW_USEDEFAULT,CW_USEDEFAULT,
                               0,0, NULL, NULL, wc.hInstance, NULL);
    
    devices_registerForNotifications(hWnd);
    
    devices_getAvailableSerialPorts(hWnd,&DeviceList);
    
    RegisterHotKey(hWnd,1,MOD_ALT,VK_F5);
    
    if (notify_init(&Notify,hWnd,&APP_GUID,"PortPopper",ID_MAIN_ICON)){
        DEBUG("Failed to create notify icon\n");
        return -1;
    }
    
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // NOTE(Simon): This blocks and only loops when we get a message
        if (GetMessage(&msg, NULL, 0U, 0U))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
    }
    
    
    notify_remove(&Notify);
    return 0;
    
}


//Callback from the notify icon helper, create the items needed for the context menu
void cb_notifyCreateMenuItems(void){
    TEMP_BUFF(buf);
    notify_clearMenuItems(&Notify);
    notify_menu_item_t* item = notify_addMenuItemString(&Notify,ID_MENU_SETTINGS,"Settings");
    if (item){
        item->enabled = false;
    }
    notify_addMenuItemSeparator(&Notify);
    
    for(int i=0; i<DeviceList.deviceCount; ++i){
        serial_device_t* ptr = &DeviceList.devices[i];
        create_string(buf,"%s - %s",ptr->manufacturer,ptr->friendlyName);
        item = notify_addMenuItemString(&Notify,-1,buf);
        if (item){
            item->enabled = false;
        }
    }
    
    notify_addMenuItemSeparator(&Notify);
    notify_addMenuItemString(&Notify,ID_MENU_EXIT,"Exit");
    
}


void updateDeviceConnected(HWND hWnd,PDEV_BROADCAST_HDR ptr, WPARAM msg){
    TEMP_BUFF(buf);
    
    if (ptr->dbch_devicetype == DBT_DEVTYP_PORT){
        PDEV_BROADCAST_PORT_A device = (PDEV_BROADCAST_PORT_A)ptr;
        DEBUG ("Device Type: 0x%08X\n",ptr->dbch_devicetype);
        DEBUG("Device Name %s\n",device->dbcp_name);
        switch(msg){
            case(DBT_DEVICEARRIVAL):{
                DEBUG("Device Arrived\n");
                create_string(buf,"%s Connected",device->dbcp_name);
                notify_showUserMessage(&Notify,"Device Connected",buf);
                devices_getAvailableSerialPorts(hWnd,&DeviceList);
            }break;
            case(DBT_DEVICEREMOVECOMPLETE):{
                DEBUG("Device Removed\n");
                create_string(buf,"%s Removed",device->dbcp_name);
                notify_showUserMessage(&Notify,"Device Removed",buf);
                devices_getAvailableSerialPorts(hWnd,&DeviceList);
            }break;
        }
    }
    
}


LRESULT WINAPI mainWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case(WM_NOTIFY_CUST):{
            //The messages from the notify icon, arrive via a custom message number
            return notify_proc(&Notify,hWnd,msg,wParam,lParam);
        }break;
        
        case(WM_SETTINGS_WINDOW_CLOSED):{
            //Settings window closed, clean up
            setWin_closeAndDestroy((settings_window_ctx_t*)wParam);
        }break;
        
        case(WM_HOTKEY):{
            //Hot-key triggered
            notify_showMenu(&Notify);
        }break;
        
        case (WM_COMMAND):{
            if (lParam == 0){
                //lParam == 0, from Menu or Accelerator
                if (HIWORD(wParam) == 0){
                    DEBUG("Menu CMD\n");
                    //Menu
                    INT menuId = LOWORD(wParam);
                    DEBUG("Menu Id %d\n",menuId);
                    
                    switch(menuId){
                        case(ID_MENU_SETTINGS):{
                            setWin_createAndShow(GetModuleHandle(0),hWnd);
                        }break;
                        case(ID_MENU_EXIT):{
                            PostQuitMessage(1);
                        }break;
                        
                    }
                }else if (HIWORD(wParam) == 1){
                    //Accelerator
                    
                }
            }else{
                //Non 0 lParam, everything else
                //Control-defined
            }
            
        }break;
        
        case(WM_DEVICECHANGE):{
            //Device change messages, we only care about arrived and disconnected.
            //Fair Warning - if we dont return TRUE to some of the messages,
            //windows will never accept them as a new device. See MSDN for more
            
            switch(wParam){
                case(DBT_DEVNODES_CHANGED):{
                    //DEBUG("WM_DEVICECHANGE\r\n");
                    return TRUE;
                }
                
                case(DBT_DEVICEREMOVECOMPLETE):
                case(DBT_DEVICEARRIVAL):{
                    //lParam is a ptr to a DEV_BROADCAST_HDR
                    PDEV_BROADCAST_HDR ptr = (PDEV_BROADCAST_HDR)lParam;
                    updateDeviceConnected(hWnd,ptr,wParam);
                    return TRUE;
                }
                
                break;
                
                default:{
                    DEBUG("DBT_???\r\n");
                }break;
            }
        }break;
        
        case (WM_DESTROY):{
            PostQuitMessage(0);
            return 0;
        }
    }
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

//end
