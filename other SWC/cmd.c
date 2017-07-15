#include "shell.h"
#include "cmd.h"
#include "eval.h"

static char lasc[13];
static char* ltoa(int32_t v)
{
    int32_t i;
    int sign = 0;
    uint32_t absv;
    
    if(v < 0)
    {
    	sign = 1;
    	absv = -v;
    }
    else
    {
    	sign = 0;
    	absv = v;    	
    }
    
    for(i=11;i>0;i--)
    {
    	lasc[i] = '0' + (absv%10);
    	absv = absv/10;
    	
    	if(0 == absv) break;
    }
    
    if(sign)
    {
    	lasc[--i] = '-';
    }
    
    lasc[12] = '\0';
	return &lasc[i];
}

static int evalFunc(int argc, char *argv[] ) {
	char *expr = argv[1];
    int32_t result;

	result = eval(&expr);
	expr = ltoa(result);
	puts(expr);

	return 0;
}

static ShellCmdT evalInfo= {
		evalFunc,
		2,2,
		"eval",
		"eval expression",
		"evaluate a expression\n",
		{NULL,NULL}
};


void Cmd_Init(void)
{
	SHELL_AddCmd(&evalInfo);
}