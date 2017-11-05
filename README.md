# glade_to_c
A simple program to parse Glade-Files with expat and create some c-code.

1. Compile

    You need the libexpat for comling the source

2. Install

    sudo make install
    
2. Remove

    sudo make distclean
    
4. Usage:

    glade_to_c $filename [optional] widget structure name
    
    The program parses the file from glade and generate the following files:


    widget-structure_prototyp_from_glade.h

    Define a structure with all widget


    widget_prototyp_from_glade.c widget_prototyp_from_glade.h

    Use gtk-builder to generate the widget and the callback


    callback_widget_prototyp_from_glade.c  callback_widget_prototyp_from_glade.h

    Callback function for all widget with a signal handler
    
    signal_prototyp_from_glade.h signal_prototyp_from_glade.c
    
    connection_mapper for signal
    
