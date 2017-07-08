# glade_to_c
A simple program to parse Glade-Files with expat and create some c-code.

1. Compile

    You need the libexpat for comling the source

2. Install

    Copy the binary into a $somewhere/bin 

3. Usage:

    glade_to_c $filename
    
    The program parses the file from glade and generate the following files:
    
    
    prototyp_widget-structure.h

    Define a structure with all widget


    prototyp_widget.c prototyp_widget.h

    Use gtk-builder to generate the widget and the callback


    prototyp_callback_widget.c  prototyp_callback_widget.h

    Callback funtion for all widget with a signal handler
