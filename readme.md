# Port Popper V1.0

## Simple COM port notification app for windows 10

upon connection or removal of a serial port device, a notification is generated and shown in the windows system notifications area

## Building
Ensure visual studio C/C++ tools are installed
From a visual studio command prompt
```bash
> cl src\PortPopper.cpp res\PortPopper.res
```
### Note:
you need both of the PortPopper.cpp and the PortPopper.res files, the res file provides a windows resource file that provides the icon and the manifest

### ToDo:
- Complete the settings dialog that is currently disabled
- Persist the settings

