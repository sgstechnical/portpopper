//Callback for getting an up to date list of menu items
extern void cb_notifyCreateMenuItems(void);

//
//Clear the menu items array
//
void notify_clearMenuItems(notify_icon_t* notify){
    notify->menuItemCount = 0;
}

//
//Get the next free menu item
//
notify_menu_item_t* notify_pushMenuItem(notify_icon_t* notify){
    if (notify->menuItemCount >= ARRAY_COUNT(notify->menuItems)){
        return NULL;
    }
    return &notify->menuItems[notify->menuItemCount++];
}

//
//Add a string to the menu items
//
notify_menu_item_t* notify_addMenuItemString(notify_icon_t* notify,int id, const char* str){
    
    notify_menu_item_t* item = notify_pushMenuItem(notify);
    if(item){
        string_copy(item->str,str,sizeof(item->str));
        item->id = id;
        item->type = MENU_ITEM_TYPE_STRING;
        item->enabled = true;
        return item;
    }
    return NULL;
}

//
//Add a separator to the menu items
//
notify_menu_item_t* notify_addMenuItemSeparator(notify_icon_t* notify){
    notify_menu_item_t* item = notify_pushMenuItem(notify);
    if (item){
        string_clear(item->str);
        item->id = -1;
        item->type = MENU_ITEM_TYPE_SEPARATOR;
        item->enabled = true;
        return item;
    }
    return NULL;
}

//
//Init the notify icon, set the icon and tool tip.
//Adds the icon to the notify area and sets the version number,but does not show the popup
//Returns -1 if the add or modify calls fail
//
int notify_init(notify_icon_t* NFI,HWND hWnd,const GUID* guid,const char* toolTip,UINT iconId){
    BOOL bRes;
    int rc = 0;
    NFI->hWnd = hWnd;
    NFI->ns.cbSize=sizeof(NOTIFYICONDATAA);
    NFI->ns.hWnd=hWnd;
    NFI->ns.uFlags=NIF_ICON | NIF_TIP | NIF_GUID | NIF_MESSAGE | NIF_SHOWTIP ;
    NFI->ns.uCallbackMessage=WM_NOTIFY_CUST;
    NFI->ns.szTip[0]=0;
    NFI->ns.uVersion = NOTIFYICON_VERSION_4;
    NFI->ns.guidItem=*guid;
    NFI->ns.szInfoTitle[0] = 0;
    
    LoadIconMetric(GetModuleHandle(0),MAKEINTRESOURCEW(iconId), LIM_SMALL, &(NFI->ns.hIcon));
    
    snprintf(NFI->ns.szTip,sizeof(NFI->ns.szTip),"%s",toolTip);
    
    bRes = Shell_NotifyIconA(NIM_ADD,&NFI->ns);
    if (!bRes) {
        rc = GetLastError();
        DEBUG("Shell_NotifyIcon failed %d\n",rc);
        return -1;
    }
    
    bRes = Shell_NotifyIcon(NIM_SETVERSION, &NFI->ns);
    if (!bRes) {
        DEBUG("Shell_NotifyIcon failed\n");
        return -1;
    }
    
    return 0;
    
}


//
//Shows a popup message.
//The dwInfoFlags value should be set before this is called, so do not call this directly,
//use one of the helpers below.
//
int notify_showMessage(notify_icon_t* notify, const char* title, const char* message){
    notify->ns.uFlags=NIF_ICON | NIF_TIP | NIF_GUID | NIF_INFO;
    snprintf(notify->ns.szInfoTitle,sizeof(notify->ns.szInfoTitle),"%s",title);
    snprintf(notify->ns.szInfo,sizeof(notify->ns.szInfo),"%s",message);
    if(!Shell_NotifyIconA(NIM_MODIFY,&notify->ns)){
        int err = GetLastError();
        DEBUG("Failed to show notification message %d\n",err);
        return err;
    }
    return 0;
}


//
//Shows a warning style message from the notification area
//
int notify_showWarning(notify_icon_t* notify, const char* title, const char* message){
    notify->ns.dwInfoFlags = NIIF_WARNING ;
    return notify_showMessage(notify,title,message);
}

//
//Shows an error style message from the notification area
//
int notify_showError(notify_icon_t* notify, const char* title, const char* message){
    notify->ns.dwInfoFlags = NIIF_ERROR;
    return notify_showMessage(notify,title,message);
}

//
//Shows an info style message from the notification area
//
int notify_showInfo(notify_icon_t* notify, const char* title, const char* message){
    notify->ns.dwInfoFlags = NIIF_INFO;
    return notify_showMessage(notify,title,message);
}

//
//Shows a user style message from the notification area
//
int notify_showUserMessage(notify_icon_t* notify, const char* title, const char* message){
    notify->ns.dwInfoFlags = NIIF_USER;
    return notify_showMessage(notify,title,message);
}

//
//Shows a user style message from the notification area, but with no sound
//
int notify_showUserMessageSilent(notify_icon_t* notify, const char* title, const char* message){
    notify->ns.dwInfoFlags = NIIF_USER | NIIF_NOSOUND;
    return notify_showMessage(notify,title,message);
}


//
//Shows the context menu
//
int notify_showMenu(notify_icon_t* notify){
    int rc =0;
    cb_notifyCreateMenuItems();
    if (notify->menuItemCount == 0)return -1;
    
    POINT point;
    GetCursorPos(&point);
    notify->hMenu = CreatePopupMenu();
    if (!notify->hMenu){
        DEBUG("Failed to create menu %d\n",GetLastError());
        return -1;
    }
    
    for (int i=0; i<notify->menuItemCount; ++i){
        notify_menu_item_t* ptr = &notify->menuItems[i];
        
        MENUITEMINFOA MII = {};
        
        MII.cbSize= sizeof(MENUITEMINFOA);
        //MII.fState= ;
        MII.wID= ptr->id;
        MII.hSubMenu= NULL;
        MII.hbmpChecked= NULL;
        MII.hbmpUnchecked= NULL;
        MII.dwItemData= NULL;
        MII.hbmpItem= NULL;
        
        switch(ptr->type){
            case(MENU_ITEM_TYPE_STRING):{
                MII.fMask= MIIM_ID | MIIM_STRING;
                MII.fType= MFT_STRING;
                MII.dwTypeData= ptr->str;
                MII.cch= (INT)string_length(ptr->str);
            }break;
            case(MENU_ITEM_TYPE_SEPARATOR):{
                MII.fMask= MIIM_ID;
                MII.fType= MFT_SEPARATOR ;
            }break;
        }
        
        if (!ptr->enabled){
            MII.fMask |= MIIM_STATE;
            MII.fState= MFS_DISABLED;
        }
        
        if(!InsertMenuItemA(notify->hMenu,
                            (UINT)-1,TRUE,&MII)){
            rc = GetLastError();
            DEBUG("Failed to InsertMenu %d\n",rc);
            goto error;
        }
        
    }
    
    SetForegroundWindow(notify->hWnd);
    if(!TrackPopupMenu(notify->hMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_BOTTOMALIGN,
                       point.x,point.y,0,notify->hWnd,NULL)){
        rc = GetLastError();
        DEBUG("Failed to InsertMenu %d\n",rc);
        goto error;
    }
    
    return 0;
    
    error:
    DestroyMenu(notify->hMenu);
    return rc;
}


//
//Removes the icon from the notification area
//If this is not called before the application quits, there is some
//delay before the icon can be re-shown.
//
void notify_remove(notify_icon_t* notify){
    if (notify){
        DEBUG("Remove the Icon\n");
        Shell_NotifyIcon(NIM_DELETE,&notify->ns);
    }
}


//Handle the WM_NOIFY_CUST message from the notify icon
LRESULT WINAPI notify_proc(notify_icon_t* notify,HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
    
    assert(msg == WM_NOTIFY_CUST);
    
    UINT event = LOWORD(lParam);
    //UINT iconId = HIWORD(lParam);
    //UINT xAnchor = GET_X_LPARAM(wParam);
    //UINT yAnchor = GET_Y_LPARAM(wParam);
    switch(event){
        case(NIN_BALLOONSHOW):{
            DEBUG("NIN_BALLOONSHOW\n");
        }break;
        case(NIN_BALLOONHIDE):{
            DEBUG("NIN_BALLOONHIDE\n");
        }break;
        case(NIN_BALLOONTIMEOUT):{
            DEBUG("NIN_BALLOONTIMEOUT\n");
        }break;
        case(NIN_BALLOONUSERCLICK):{
            DEBUG("NIN_BALLOONUSERCLICK\n");
        }break;
        case(NIN_POPUPOPEN):{
            DEBUG("NIN_POPUPOPEN\n");
        }break;
        case(NIN_POPUPCLOSE):{
            DEBUG("NIN_POPUPCLOSE\n");
        }break;
        case(WM_RBUTTONDOWN):{
            DEBUG("WM_RBUTTONDOWN\n");
        }break;
        case(WM_CONTEXTMENU):{
            notify_showMenu(notify);
            DEBUG("WM_CONTEXTMENU\n");
        }break;
        case(WM_RBUTTONUP):{
            DEBUG("WM_RBUTTONUP\n");
        }break;
        case(WM_LBUTTONUP):{
            DEBUG("WM_LBUTTONUP\n");
        }break;
        case(WM_LBUTTONDOWN):{
            DEBUG("WM_LBUTTONDOWN\n");
        }break;
        case(WM_RBUTTONDBLCLK):{
            DEBUG("WM_RBUTTONDBLCLK\n");
        }break;
        case(WM_LBUTTONDBLCLK):{
            DEBUG("WM_LBUTTONDBLCLK\n");
        }break;
        case(WM_MOUSEMOVE):{
            DEBUG("WM_MOUSEMOVE\n");
        }break;
        case(WM_USER):{
            DEBUG("WM_USER\n");
        }break;
        default: {
            DEBUG("Event %d 0x%x iconId %d x %d, y %d\n",event,event,iconId,xAnchor,yAnchor);
        }
    }
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


//end
