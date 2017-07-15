#include "eval.h"

int32_t eval(int8_t **expr)
{
	int32_t number=0,result=0;
	int8_t operator='+';
	while(**expr != '\0')
	{
	 	if(**expr == '(')
	 	{
	 		(*expr)++;
	 		number=eval(expr);
	 	}
	 	else
	 	{
		 	 while((**expr >= '0') && (**expr <= '9'))
		 	{
		 		number=number*10 +(**expr)-'0';
		 		(*expr)++;
		 	}
	 	
	 	}
	 	switch(operator)
	 	{
	 		case '+':
	 			result=result+number;
	 			break;
	 		case '-':
	 			result=result-number;
	 			break;
	 		case '*':
	 			result=result*number;
	 			break;
	 		case '/':
	 			result=result/number;
	 			break;
	 	}
	 	if(**expr == ')')
	 	{
	 		(*expr)++;
	 		break;
	 	}
	 	if(**expr != '\0')
	 	{
	 		operator=**expr;
	 		(*expr)++;
	 	}
	 	number=0;
	}
	return result;
}