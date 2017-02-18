========================================================================
    MAKEFILE PROJECT : AVR_Template Project Overview
(Daniel Veneziano's Notes)
========================================================================

This file contains a summary of what you can do with your AVR_Template project.

You can use this template to write programs in C developed for the ATMEGA328P Atmel microchip. This custom built Visual Studio (VS) template compiles C/C++ code using a modified version of the WinAVR default Makefile, which invokes the gcc complier and references the avrlibc library.

I modified the WinAVR default Makefile in the following 3 ways:

1. I set the TARGET macro equal to "Main". Please note that this macro TARGET must be equal to the name of your program file that contains the main() function. Note I have already added a file called 'Main.c' with an empty main() function by default.

2. I setup the AVRDUDE macros to download the '.hex'output to the Romeo board (Makefile lines 336-340):

SERIAL_AVRDUDE = "C:\WinAVR-20100110\bin"
SERIAL_AVRDUDE_CONFIG = "C:\WinAVR-20100110\bin"
SERIAL_AVRDUDE_PORT = COM3
SERIAL_AVRDUDE_SPEED = 115200
SERIAL_AVRDUDE_PROGRAMMER = stk500v1

These macros may need to be updated for your computer, specifically the WinAVR directory and the COM Port.

3. I applied a hack that enables the intellisense features built into VS, which include with auto-complete and error prediction.

This template is based off a standard VS C++ Makefile project that I set up to call call NMake, which references  'Makefile' and generates a '.hex' output file.

To fully enable the intellisense features I added precompiler definitions into the project configuration properties, which include defining our board, the processor speed (F_CPU) and a dummy variabe (__DOXYGEN__) that has to do with automatic commenting in some library files. So, these values do not need to be defined elsewhere in your program. If you ever use another board these properties will require updating.

I also created a new solution configuration called "Upload" such that when you are in this configuration and build your project it will upload the program to your board, IF the code compiles without error and your board is connected via USB (please note aforementioned AVRDUDE settings). If you are in the "Debug" or "Release" solution configurations then your project will build but VS will not upload the output '.hex' file. Also note that "Clean Build" will remove the unecessary output files after compilation and "Rebuild All" forces the VS to recompile all of your project files.

I have added two source files to the template: 'Main.c' and 'Main.h'. These files are left blank except for an empty Main() function. Simply create your program code in these and other source files and this VS template will take care of the rest (CTRL+SHIFT+B to build).

Happy Coding :)

////////////////////////////////////////////////////////////////////////////
