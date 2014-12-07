#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

//AutoComplete Variables
static char** custom_completion(const char*, int ,int);
char* custom_generator(const char*,int);
char * dupstr (char*);
void *xmalloc (int);
 
char* cmd [] ={"ls","pwd","sort","ftp","find","locate","vim","diff","export","grep","gzip","history","penv","senv","unsenv","shutdown","ps","kill","rm","mv","cp","cat","less","mount","chmod","chown","passwd","ifconfig","uname","man","tail","more","sudo","apt-get","install","ping","date","wget"};


static char** custom_completion( const char * text , int start,  int end)
{
    char **matches;
 
    matches = (char **)NULL;
 
    if (start == 0)
        matches = rl_completion_matches ((char*)text, &custom_generator);
    else
        rl_bind_key('\t',rl_abort);
 
    return (matches);
 
}
 
char* custom_generator(const char* text, int state)
{
    static int list_index, len;
    char *name;
 
    if (!state) {
        list_index = 0;
        len = strlen (text);
    }
 
    while (name = cmd[list_index]) {
        list_index++;
 
        if (strncmp (name, text, len) == 0)
            return (dupstr(name));
    }
 
    /* If no names matched, then return NULL. */
    return ((char *)NULL);
 
}
 
char * dupstr (char* s) {
  char *r;
 
  r = (char*) xmalloc ((strlen (s) + 1));
  strcpy (r, s);
  return (r);
}
 
void * xmalloc (int size)
{
    void *buf;
 
    buf = malloc (size);
    if (!buf) {
        fprintf (stderr, "Error: Out of memory. Exiting.'n");
        exit (1);
    }
 
    return buf;
}