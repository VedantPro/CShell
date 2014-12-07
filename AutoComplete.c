 

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