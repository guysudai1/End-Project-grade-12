# FELIX
### File Events Logging, Intercepting, and Examining

This entire project was written in C++ using QT, MS Detours and with the help of some people.  
This project can be used to track processes, and presents basic information about events happening in the target process.
## Directory structure:

##### Prebuilt binaries + dll for x64. Comes with an example to run (pauses -> executes createfilea -> pauses) 
- Build/ 
##### Project DLL (Provides hooking functionality) 
- DLL/
##### Provided example (Currently only executes createfilea and pauses, in future builds will have more features)
- Example/
##### GUI - interesting files: inject.cpp/h, proc.cpp/h, FelixGUI.cpp/h
- FelixGUI/
##### Stub to execute desired process
- Stub/

# Usage
1. Clone the repository:
```
git clone https://github.com/guysudai1/Process-Monitor-FELIX-
```
2. Go into the `Build` Folder and execute FelixGUI.exe

![](demo.gif)

## Credits:
I would like to thank the following people for finding the time to help me out:
- מגנום
- iamgweej
