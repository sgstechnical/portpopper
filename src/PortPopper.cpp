/*This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
*/

#define DEBUG_ENABLED 0 //Enables printf debugging
#define WIN_FORMS_APP 1 //Set to 1,builds as a windows desktop app, 0 as a console app

#pragma comment(lib,"User32.lib") // Required for a load of windows stuff
#pragma comment(lib,"comctl32.lib") //Required for common controls
#pragma comment(lib,"shell32.lib") //Required for NotifyIcon
#pragma comment(lib,"SetupApi.lib") //Required for device enumeration


//Windows headers
#include <windows.h>
#include <windowsx.h> // For GET_X_LPARAM() etc;
#include <Knownfolders.h> //For known folder constants
#include <dbt.h>
#include <setupapi.h>
#include <initguid.h>  //Required for class GUIDs
#include <devpkey.h>

//c headers
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

//our headers
#include "helpers.h"
#include "devices.h"
#include "notify.h"

#define HIDDEN_WINDOW_CLASS_NAME "PortPopperHiddenClass"
#define SETTINGS_WINDOW_CLASS_NAME "PortPopperSettingsClass"
#define SETTINGS_WINDOW_NAME_STR "PortPopper Settings"
#define BUILD_VER 1000

//Resource Id's
#define ID_MAIN_ICON 101

//Control Ids
typedef enum{
    ID_MENU_SETTINGS = 1000,
    ID_MENU_EXIT,
}control_id_index_t;

//Custom Message Id's
typedef enum{
    WM_NOTIFY_CUST = WM_USER + 1,
    WM_SETTINGS_WINDOW_CLOSED,
}cust_message_index_t;


//GUID for this app. Windows uses this as a unique Id for notification's
//It is just randomly generated, but if you change the app name, you many need to change this GUID
static const GUID APP_GUID ={ /* 63de3f12-3af6-4ad8-a691-849881fa0be0 */
    0x63de3f41,
    0x3af6,
    0x4ad8,
    {0xa6,0x91,0x84,0x98,0x81,0xfa,0x0b,0xe0}
};


//This is the GUID for the serial port device class, from Microsoft
static const GUID serialPortInterfaceGUID  ={
    0x4d36e978,
    0xe325,
    0x11ce,
    {0xbf,0xc1,0x08,0x00,0x2b,0xe1,0x03,0x18}
};


GLOBAL_VARIABLE serial_device_list_t DeviceList;
GLOBAL_VARIABLE notify_icon_t Notify;

//The c / cpp files
#include "helpers.cpp"
#include "notify.cpp"
#include "settings_window.cpp"
#include "devices.cpp"
#include "main.cpp"

//end

