//Global Variables
static char HISTORY[1000][200];
static pid_t PID;
static pid_t PGID;
static pid_t shell_PGID;
static char **ENV;
static struct termios TMODES;
static char *input;
pid_t pid;

struct sigaction act_child;
struct sigaction act_int;


//handlers
void child_signal_HANDLER(int p);
void int_signal_HANDLER(int p);

//All functions declarations
void env_MANAGER(char *args[],int act);
void command_EXECUTER(char *args[]);
void command_HANDLER(char *args[]);
void pipe_EXECUTER(char *args[]);
void fileIO_EXECUTER(char *args[], char *inFile, char * outFile, int opt);
void echo_HANDLER(char *args[]);
void back_proc_HANDLER(char *args[],int back_stat);
void print_HISTORY(char num);

