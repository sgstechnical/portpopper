# Port Popper V1.0

## Simple COM port notification app for windows 10

Upon connection or removal of a serial port device, a notification is generated and shown in the windows system notifications area

## Building
Ensure git command line and visual studio C/C++ tools are installed
From a visual studio command prompt
```bash
> git clone https://github.com/sgstechnical/portpopper.git
> cd portpopper
> cl src\PortPopper.cpp res\PortPopper.res
> portpopper.exe
```

### Note:
You need both PortPopper.cpp and the PortPopper.res file. The res file provides an icon and a manifest that are required for notification apps. 

If you make a change to the resource .rc file or the icon, you will need to re-build the .res file  
From a visual studio command prompt
```bash
> rc res\PortPopper.rc
```


### ToDo:
- Complete the settings dialog that is currently disabled
- Persist the settings

