# FELIX
### File Events Logging, Intercepting, and Examining

This entire project was written in C++ using QT, MS Detours and with the help of some people.

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

## Credits:
I would like to thank the following people for finding the time to help me out:
- מגנום
- iamgweej