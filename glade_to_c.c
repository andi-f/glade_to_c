/* 
*   A simple program to parse Glade-Files with expat
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

#define NODEBUGLEVEL 1

int		widget_level = 0;	//detect a widget
int		signal_level = 0;	//detect a signal

int		Depth;	/* Keep track of the current level in the XML tree */

char	*filename_struct = "prototyp_widget-structure.h";
FILE	*fp_struct;
    
char	*filename_widget = "prototyp_widget.c";
FILE	*fp_widget;
    
char	*filename_cb = "prototyp_callback_widget.c";
FILE	*fp_cb;

char	*filename_widget_h = "prototyp_widget.h";
FILE	*fp_widget_h;
    
char	*filename_cb_h = "prototyp_callback_widget.h";
FILE	*fp_cb_h;
    
char	*filename;

void start(void *data, const char *el, const char **attr)	{
	char 			*str1,*str2;
	
	str1 = NULL;
	str2 = NULL;	

    //printf("Depth <%i> Element <%s>",Depth,el);
    
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
			if(strstr(attr[1],"GtkAdjustment") != NULL)	{
				#ifdef DEBUG
					fprintf(stderr, "GtkAdjustment *%s;\n", attr[3]);
				#endif
				fprintf(fp_struct, "GtkAdjustment *%s;\n", attr[3]);
			}
			else
			{
				#ifdef DEBUG
					fprintf(stderr, "GtkWidget *%s;\n", attr[3]);		
				#endif	
				fprintf(fp_struct, "GtkWidget *%s;\n", attr[3]);			
			}
			fprintf(fp_widget, "\twg_data->%s = GTK_WIDGET(gtk_builder_get_object(wg_data->builder, \"%s\"));\n", attr[3], attr[3]);			
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
				#ifdef DEBUG			
					fprintf(stderr, "extern void *%s(GtkWidget *widget, xxxx_data *wdg_data);\n", attr[3]);
				#endif
				fprintf(fp_cb_h, "extern void *%s(GtkWidget *widget, xxxx_data *wdg_data);\n", attr[3]);
				fprintf(fp_cb, "void *%s(GtkWidget *widget, xxxx_data *wdg_data)\t{\n}\n\n", attr[3]);
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

int
main(int argc, char **argv)
{

    FILE	*fp;
    
    struct stat sb;
        
    size_t          size;
    char           *xmltext;
    XML_Parser      parser;

	fprintf(stderr, "Glade_to_c\n");
	fprintf(stderr, "A simple CLI tool to generat some c.code from a Glade file\n");
	fprintf(stderr, "Written by Andreas Fischer 2017\n");
	fprintf(stderr, "Version 0.99alpha");
	
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return (-1);
    }
    
    filename = argv[1];    
   
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
	    perror("File for widget structure exists");
        //exit(EXIT_FAILURE);
		fp_struct = fopen(filename_struct,"w");        
    }    
    
	if (stat(filename_widget, &sb) == -1) {
		fp_widget = fopen(filename_widget,"w");
	}
	else	{
	    perror("File for widget function exists");
        //exit(EXIT_FAILURE);
		fp_widget = fopen(filename_widget,"w");        
    }
    
	if (stat(filename_cb, &sb) == -1) {
		fp_cb = fopen(filename_cb,"w");
	}
	else	{
	    perror("File for callback function exists");
        //exit(EXIT_FAILURE);
        fp_cb = fopen(filename_cb,"w");
	}
	
	if (stat(filename_widget_h, &sb) == -1) {
		fp_widget_h = fopen(filename_widget_h,"w");
	}
	else	{
	    perror("File for widget function exists");
        //exit(EXIT_FAILURE);
		fp_widget_h = fopen(filename_widget_h,"w");        
    }
    
	if (stat(filename_cb_h, &sb) == -1) {
		fp_cb_h = fopen(filename_cb_h,"w");
	}
	else	{
	    perror("File for callback function exists");
        //exit(EXIT_FAILURE);
        fp_cb_h = fopen(filename_cb_h,"w");
	}
	
    if (parser == NULL) {
        fprintf(stderr, "Parser not created\n");
        return (-EXIT_FAILURE);
    }
	
	fprintf(fp_struct,"#include <stdint.h>\n");
	fprintf(fp_struct,"#include <gtk/gtk.h>\n\n");

	fprintf(fp_struct,"typedef struct {\n");
	fprintf(fp_struct,"GtkBuilder *builder;\n\n");
	
	fprintf(fp_widget,"#include \"xxxx.h\"\n");
	fprintf(fp_widget,"#include \"callback_widget.h\"\n\n");

	fprintf(fp_widget,"void wdg_main(xxxx_data *wg_data)	{\n");
	fprintf(fp_widget,"\twg_data->builder = gtk_builder_new();\n");
    fprintf(fp_widget,"\tgtk_builder_add_from_file (wg_data->builder, %s, NULL);\n",filename);
    
    fprintf(fp_cb_h,"#include <gtk/gtk.h>\n\n");
	fprintf(fp_cb_h,"extern void connection_mapper (GtkBuilder *builder, GObject *object,\n");
	fprintf(fp_cb_h,"\tconst gchar *signal_name, const gchar *handler_name,\n");
	fprintf(fp_cb_h,"\tGObject *connect_object, GConnectFlags flags, gpointer user_data);\n");
    
    /* Tell expat to use functions start() and end() each times it encounters
     * the start or end of an element. */
    XML_SetElementHandler(parser, start, end);
    
    fp = fopen(argv[1], "r");
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

    fprintf(fp_struct,"} wdg_data;\n");
    fclose(fp_struct);

	fprintf(fp_widget,"\tgtk_builder_connect_signals_full (wg_data->builder, connection_mapper, (gpointer*) wg_data);\n");
	fprintf(fp_widget,"\tg_object_unref(wg_data->builder);\n;}\n");
    fclose(fp_widget);
    
    fprintf(fp_cb,"}\n");
    fclose(fp_cb);
    
    fprintf(fp_cb_h,"\n");
    fclose(fp_cb_h);
    
    fprintf(fp_widget_h,"extern void wdg_main(xxxx_data *wg_data);");
    fclose(fp_widget_h);
    
    XML_ParserFree(parser);
    fprintf(stdout, "Successfully parsed %li characters in file %s\n", size,
        argv[1]);
    return (0);
}
