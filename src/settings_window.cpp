
typedef struct{
    HWND hParent;
    HINSTANCE instance;
    HWND hWnd;
    bool isLoaded;
    HANDLE hThread;
}settings_window_ctx_t;

INTERNAL settings_window_ctx_t Ctx = {};

//pre-declare the window proc
LRESULT WINAPI settingsWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LOCAL_VARIABLE int btnPosY = 10;

void addButton(HWND hWnd,char* text, WORD id){
    
    CreateWindowEx(0,"BUTTON",text,
                   WS_TABSTOP | WS_VISIBLE | WS_CHILD,
                   10,btnPosY,
                   150,20,
                   hWnd,
                   (HMENU)id,
                   (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                   NULL);
    btnPosY+=25;
}


//
//Worker proc, creates and handles the settings window
//
DWORD WINAPI setWin_createAndShowWorker(LPVOID lpParameter){
    TEMP_BUFF(buf);
    settings_window_ctx_t* ctx = (settings_window_ctx_t*)lpParameter;
    
    WNDCLASSEXA wc ={};
    wc.cbSize=sizeof(WNDCLASSEX);
    wc.style=CS_CLASSDC;
    wc.lpfnWndProc=settingsWndProc;
    wc.cbClsExtra=NULL;
    wc.cbWndExtra=NULL;
    wc.hInstance=ctx->instance;
    wc.hIcon=LoadIconA(ctx->instance,MAKEINTRESOURCE(ID_MAIN_ICON));
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=(HBRUSH)COLOR_WINDOW;
    wc.lpszMenuName=NULL;
    wc.lpszClassName=SETTINGS_WINDOW_CLASS_NAME;
    wc.hIconSm=NULL;
    
    RegisterClassEx(&wc);
    
    create_string(buf,"%s %d.%d",SETTINGS_WINDOW_NAME_STR,BUILD_VER/1000,BUILD_VER%1000);
    
    HWND hWnd = CreateWindowEx(0,wc.lpszClassName,
                               buf,
                               WS_VISIBLE | WS_POPUP | WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT,CW_USEDEFAULT,
                               400,600, NULL, NULL, wc.hInstance, NULL);
    
    if (!hWnd){
        //Failed
        goto exit;
    }
    
    ctx->hWnd = hWnd;
    ctx->isLoaded = true;
    
    
#if 0
    addButton(hWnd,"Info",ID_BTN_INFO);
    addButton(hWnd,"Warning",ID_BTN_WARNING);
    addButton(hWnd,"Error",ID_BTN_ERROR);
    addButton(hWnd,"User",ID_BTN_USER);
    addButton(hWnd,"No Sound",ID_BTN_NO_SOUND);
#endif
    
    
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    
    while (msg.message != WM_QUIT)
    {
        // NOTE(Simon): This blocks and only loops when we get a message
        if (GetMessage(&msg,0, 0U, 0U))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
    }
    
    exit:
    //Tell the main window we quit
    PostMessage(ctx->hParent,WM_SETTINGS_WINDOW_CLOSED,(WPARAM)ctx,NULL);
    return 0;
}


//Starts and show the settings window in it's own thread
int setWin_createAndShow(HINSTANCE instance, HWND hParent){
    settings_window_ctx_t* ctx = &Ctx;
    
    if (ctx->isLoaded){
        ShowWindow(ctx->hWnd,1);
        if (!BringWindowToTop(ctx->hWnd)){
            DEBUG("Error %d\n",GetLastError());
        }
        return 0;
    }
    
    
    assert(ctx->isLoaded == false);
    assert(ctx->hThread == NULL);
    assert(ctx->hWnd == NULL);
    
    if (ctx->isLoaded){
        // TODO(Simon): @CLEANUP handle errors
        //..//
        
    }
    
    ctx->instance = instance;
    ctx->hParent = hParent;
    
    ctx->hThread = CreateThread(NULL,0,setWin_createAndShowWorker,(LPVOID)ctx,0,NULL);
    if (ctx->hThread == INVALID_HANDLE_VALUE){
        // TODO(Simon): @CLEANUP handle errors
        //..//
        
    }
    
    return 0;
}


int setWin_closeAndDestroy(settings_window_ctx_t* ctx){
    
    if (ctx->hThread){
        WaitForSingleObject(ctx->hThread,INFINITE);
        CloseHandle(ctx->hThread);
        ctx->hThread = NULL;
    }
    
    if (ctx->hWnd){
        DestroyWindow(ctx->hWnd);
    }
    
    ctx->isLoaded = false;
    return 0;
}



//
//Settings window message proc
//
LRESULT WINAPI settingsWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case (WM_COMMAND):{
            if (lParam == 0){
                //lParam == 0, from Menu or Accelerator
                if (HIWORD(wParam) == 0){
                    //From Menu
                    //INT menuId = LOWORD(wParam);
                    //DEBUG("Menu Id %d\n",menuId);
                    
                }else if (HIWORD(wParam) == 1){
                    //From Accelerator
                    
                }
            }else{
                //Control-defined
                if (HIWORD(wParam) == BN_CLICKED){
                    switch(LOWORD(wParam)){
                        case(0): // Thanks c
                        default:{}break;
                    }
                }
            }
        }break;
        
        case (WM_DESTROY):{
            PostQuitMessage(-1);
            return 0;
        }
    }
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}


//end
