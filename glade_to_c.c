/* 
*   A simple program to parse Glade-Prototype-Files with expat
*   and create some c-code.
*   Parsing the XML:
*   <http://expat.sourceforge.net/> 
*   <http://www.xml.com/pub/a/1999/09/expat/index.html> 
*   
*   
*/

/*
 *      This program is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 * 
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      You should have received a copy of the GNU General Public License
 *      along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <glib.h>
#include <glib/gprintf.h>
#include <expat.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXCHARS 1000000
#define MAXLEN 1024

#define DEBUGLEVEL1

int		widget_level = 0;	//detect a widget
int		signal_level = 0;	//detect a signal

int		Depth;	/* Keep track of the current level in the XML tree */


char	*filename_struct = "widget_structure_prototyp_from_glade.h";
FILE	*fp_struct;
    
char	*filename_widget = "widget_prototyp_from_glade.c";
FILE	*fp_widget;

char	*filename_widget_h = "widget_prototyp_from_glade.h";
FILE	*fp_widget_h;
    
char	*filename_cb = "callback_widget_prototyp_from_glade.c";
FILE	*fp_cb;
    
char	*filename_cb_h = "callback_widget_prototyp_from_glade.h";
FILE	*fp_cb_h;

char	*filename_sc_h = "signal_connect_prototyp_from_glade.h";
FILE	*fp_sc_h;

char	*filename_sc = "signal_connect_prototyp_from_glade.c";
FILE	*fp_sc;
    
char	*filename;
char	*structname;

void start(void *data, const char *el, const char **attr)	{
	char 			*str1,*str2;
	
	str1 = NULL;
	str2 = NULL;	

	#ifdef DEBUGLEVEL2
		fprintf(stderr, "Depth <%i> Element <%s>",Depth,el);
    #endif
    
    if(strncmp (el,"object",6) == 0)
		widget_level = Depth;
	else
    if(strncmp (el,"signal",6) == 0)	
		signal_level = Depth;

	if (widget_level > 0){
		if (attr[0] != NULL)
			str1 = strstr(attr[0],"class");

		if (attr[2] != NULL)
			str2 = strstr(attr[2],"id");
		
		if((str1 != NULL) && (str2 != NULL))	{

			#ifdef DEBUGLEVEL2
			fprintf(stderr, "%s %s %s %s %s\n\r",attr[0],attr[1],attr[2],attr[3],attr[4]);
			#endif

			if(strstr(attr[1],"GtkAction") == NULL)	{
				if(strstr(attr[1],"GtkEntry") != NULL)	{
					fprintf(stderr, "%s\t\t\t", attr[1]);
					fprintf(fp_struct, "%s\t\t\t", attr[1]);				
				}
				else
				if(strstr(attr[1],"GtkAdjustment") != NULL)	{
					fprintf(stderr, "%s\t", attr[1]);
					fprintf(fp_struct, "%s\t", attr[1]);
				}
				else	{
					fprintf(fp_widget, "\twdg_data->%s = GTK_WIDGET(gtk_builder_get_object(wdg_data->builder, \"%s\"));\n", attr[3], attr[3]);
					fprintf(stderr, "GtkWidget\t\t");
					fprintf(fp_struct, "GtkWidget\t\t");
				}
				fprintf(stderr, "*%s;\n", attr[3]);
				fprintf(fp_struct, "*%s;\n", attr[3]);
			} 			
		}
	}

	str1 = NULL;
	str2 = NULL;	

	if (signal_level > 0){
		if (attr[0] != NULL)
			str1 = strstr(attr[0],"name");
		if (attr[2] != NULL)
			str2 = strstr(attr[2],"handler");		
		if((str1 != NULL) && (str2 != NULL))	{
				//#ifdef DEBBUGLEVEL1
				//fprintf(stderr, "extern void *%s(GtkWidget *widget, %s_data *wdg_data);\n", attr[3]);
				//#endif
				fprintf(fp_cb_h, "extern void *%s(GtkWidget *widget, %s_data *wdg_data);\n", attr[3],structname);
				fprintf(fp_cb, "void %s(GtkWidget *widget, %s_data *wdg_data)\t{\n\tfprintf(stderr,\"%s active\\n\");\n}\n\n",attr[3], structname, attr[3]);

				//#ifdef DEBUGLEVEL1
				//fprintf(stderr,"\tif (g_strcmp0 (handler_name, \"%s\") == 0)\n",attr[3]);
				//fprintf(stderr,"\t\tg_signal_connect (object, signal_name, G_CALLBACK(%s), wdg_data);\n",attr[3]);
				//fprintf(stderr,"\telse\n");
				//#endif		
				
				fprintf(fp_sc,"\tif (g_strcmp0 (handler_name, \"%s\") == 0)\n",attr[3]);
				fprintf(fp_sc,"\t\tg_signal_connect (object, signal_name, G_CALLBACK(%s), wdg_data);\n",attr[3]);
				fprintf(fp_sc,"\telse\n");
			}
	}			
  
    Depth++;

}               /* End of start handler */

void
end(void *data, const char *el)
{
    Depth--;
	if( Depth < widget_level)
		widget_level = 0;

	if( Depth < signal_level)
		signal_level = 0;
			
			
}               /* End of end handler */

int main(int argc, char **argv)
{

    FILE	*fp;
    
    struct stat sb;
        
    size_t          size;
    char           *xmltext;
    XML_Parser      parser;

	fprintf(stderr, "Glade_to_c\n");
	fprintf(stderr, "A simple CLI tool to generat some c-code from a Glade file\n");
	fprintf(stderr, "Written by Andreas Fischer 2017\n");
	fprintf(stderr, "Version 0.99rc2\n");
	
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename [optional widget_structure_name]\n", argv[0]);
        return (-1);
    }
    
    filename = argv[1];


    if(argc == 2)	{
			structname = malloc(10);
			structname = "this";
	}
    else
		structname = argv[2];
   
	fprintf(stderr,"reading %s \n\r",argv[1]);
	
    parser = XML_ParserCreate(NULL);
     if (parser == NULL) {
        fprintf(stderr, "Parser not created\n");
        return (EXIT_FAILURE);
    }

	if (stat(filename_struct, &sb) == -1) {
		fp_struct = fopen(filename_struct,"w");
	}	
	else	{
	    perror("Prototype-File for widget structure exists");
		exit(EXIT_FAILURE);
    }    
    
	if (stat(filename_widget, &sb) == -1) {
		fp_widget = fopen(filename_widget,"w");
	}
	else	{
	    perror("Prototype-File for widget function exists");
        exit(EXIT_FAILURE);
    }
    
	if (stat(filename_widget_h, &sb) == -1) {
		fp_widget_h = fopen(filename_widget_h,"w");
	}
	else	{
	    perror("Prototype-File for widget function exists");
        exit(EXIT_FAILURE);
    }

	if (stat(filename_cb, &sb) == -1) {
		fp_cb = fopen(filename_cb,"w");
	}
	else	{
	    perror("Prototype-File for callback function exists");
        exit(EXIT_FAILURE);
	}
	   
	if (stat(filename_cb_h, &sb) == -1) {
		fp_cb_h = fopen(filename_cb_h,"w");
	}
	else	{
	    perror("Prototype-File for callback function exists");
        exit(EXIT_FAILURE);
	}

	if (stat(filename_sc, &sb) == -1) {
		fp_sc = fopen(filename_sc,"w");
	}
	else	{
	    perror("Prototype-File for callback function exists");
        exit(EXIT_FAILURE);
	}
	   
	if (stat(filename_sc_h, &sb) == -1) {
		fp_sc_h = fopen(filename_sc_h,"w");
	}
	else	{
	    perror("Prototype-File for callback function exists");
        exit(EXIT_FAILURE);
	}
	
    if (parser == NULL) {
        fprintf(stderr, "Parser not created\n");
        return (-EXIT_FAILURE);
    }

	fprintf(fp_struct,"#include <gtk/gtk.h>\n\n");
	fprintf(fp_struct,"typedef struct {\n");
	fprintf(fp_struct,"GtkBuilder *builder;\n\n");
	

	fprintf(fp_cb,"#include <gtk/gtk.h>\n");
	fprintf(fp_cb,"#include <glib/gprintf.h>\n");
	fprintf(fp_cb,"#include <gtk/gtk.h>\n\n");
	fprintf(fp_cb,"#include \"widget_structure.h\"\n\n");

	fprintf(fp_cb_h,"#include <gtk/gtk.h>\n\n");
	fprintf(fp_cb_h,"#include \"widget_structure.h\"\n\n");	


	fprintf(fp_widget,"#include \"callback_widget.h\"\n");
	fprintf(fp_widget,"#include \"signal_connect.h\"\n\n");
	fprintf(fp_widget,"void wdg_main(%s_data *wdg_data)\t{\n",structname);
	fprintf(fp_widget,"\twdg_data->builder = gtk_builder_new();\n");
    fprintf(fp_widget,"\tgtk_builder_add_from_file (wdg_data->builder, \"%s\", NULL);\n",filename);


    fprintf(fp_widget_h,"extern void wdg_main(%s_data *wdg_data);",structname);
    fclose(fp_widget_h);


    fprintf(fp_sc,"#include <gtk/gtk.h>\n");
    fprintf(fp_sc,"#include <glib.h>\n");
	fprintf(fp_sc,"#include <glib/gprintf.h>\n");
	fprintf(fp_sc,"#include <glib/gprintf.h>\n\n");
	
	fprintf(fp_sc,"#include \"widget_structure.h\"\n");	
	fprintf(fp_sc,"#include \"callback_widget.h\"\n\n");

    fprintf(fp_sc,"void connection_mapper (GtkBuilder *builder, GObject *object,\n");
	fprintf(fp_sc,"\tconst gchar *signal_name, const gchar *handler_name,\n");
	fprintf(fp_sc,"\tGObject *connect_object, GConnectFlags flags, %s_data *wdg_data)\t{\n",structname);
	fprintf(fp_sc,"\tg_print (\"Verbinde %%s mit %%s\\n\", signal_name, handler_name);\n\n");

    fprintf(fp_sc_h,"#include <gtk/gtk.h>\n\n");
	fprintf(fp_sc_h,"extern void connection_mapper (GtkBuilder *builder, GObject *object,\n");
	fprintf(fp_sc_h,"\tconst gchar *signal_name, const gchar *handler_name,\n");
	fprintf(fp_sc_h,"\tGObject *connect_object, GConnectFlags flags, gpointer user_data);\n");


    /* Tell expat to use functions start() and end() each times it encounters
     * the start or end of an element. */
    XML_SetElementHandler(parser, start, end);

	fp = fopen(argv[1], "r");

	if(fp == NULL)	{
	    perror("Glade  file not exists");
        exit(EXIT_FAILURE);
	}

    xmltext = malloc(MAXCHARS);
    
    /* Slurp the XML file in the buffer xmltext */
    size = fread(xmltext, sizeof(char), MAXCHARS, fp);

    if (XML_Parse(parser, xmltext, strlen(xmltext), XML_TRUE) ==
        XML_STATUS_ERROR) {
        fprintf(stderr,
            "Cannot parse %s, file may be too large or not well-formed XML\n",
            argv[1]);
        return (1);
    }
    fclose(fp);

    fprintf(fp_struct,"} %s_data;\n",structname);
    fclose(fp_struct);

	fprintf(fp_widget,"\tgtk_builder_connect_signals_full (wdg_data->builder, connection_mapper, (gpointer*) wdg_data);\n");
	fprintf(fp_widget,"\tg_object_unref(wdg_data->builder);\n;}\n");
    fclose(fp_widget);
    
	
    fprintf(fp_cb,"\n");
    fclose(fp_cb);

    fprintf(fp_cb_h,"\n");
    fclose(fp_cb_h);
    
    fprintf(fp_sc,"\t\tg_print (\"unknown callback\\n\");\n");
    fprintf(fp_sc,"}\n");
    fclose(fp_sc);

    fprintf(fp_sc_h,"\n");
    fclose(fp_sc_h);

    XML_ParserFree(parser);
    fprintf(stdout, "Successfully parsed %li characters in file %s\n", size,
        argv[1]);
    return (0);
}
