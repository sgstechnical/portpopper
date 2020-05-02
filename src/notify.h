#ifndef _NOTIFY_H_
#define _NOTIFY_H_

typedef enum{
    MENU_ITEM_TYPE_STRING,
    MENU_ITEM_TYPE_SEPARATOR,
}notify_menu_item_type_t;

typedef struct{
    int id;
    notify_menu_item_type_t type;
    char str[128];
    bool enabled;
}notify_menu_item_t;

#define MENU_ITEMS_MAX 128

typedef struct{
    HWND hWnd;
    NOTIFYICONDATAA ns;
    HMENU hMenu;
    int menuItemCount;
    notify_menu_item_t menuItems[MENU_ITEMS_MAX];
}notify_icon_t;


#endif
