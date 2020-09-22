#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <float.h>
#include <sys/stat.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>
#include "npl.h"

//-----------JUMP project(builtin for compiler)------------

int b_reconsult_predicate(int arglist, int rest){
	int n,arg1,lis,save1;

    n = length(arglist);
    if(n == 1){
        save1 = sp;
    	arg1 = deref(car(arglist));
        if(!wide_variable_p(arg1))
            error(NOT_VAR,"n_reconsult_predicate ",arg1);

        lis = reverse(reconsult_list);
        while(!nullp(lis)){
            unify(arg1,car(lis));
            if(prove_all(rest,sp,0) == YES)
                return(YES);

            lis = cdr(lis);
            unbind(save1);
        }
        unbind(save1);
        return(NO);
    }
    return(NO);
}

int b_filename(int arglist, int rest){
	int n,arg1,arg2,pos,len;
    char str1[STRSIZE];

    n = length(arglist);
    if(n == 2){
    	arg1 = deref(car(arglist));
        arg2 = cadr(arglist);
    	strcpy(str1,GET_NAME(arg1));
        len = strlen(GET_NAME(arg1));
        for(pos=0;pos<len;pos++)
        	if(str1[pos] == '.'){
            	str1[pos] = NUL;
                return(unify(arg2,makeconst(str1)));
            }
        return(unify(arg1,arg2));
    }
    return(NO);
}

//convert atom for C language function name
int b_atom_convert(int arglist, int rest){
    int n,arg1,arg2,pos1,pos2;
    char str1[ATOMSIZE],str2[ATOMSIZE];

    n = length(arglist);
    if(n == 2){
        arg1 = deref(car(arglist));
        arg2 = deref(cadr(arglist));

        strcpy(str1,GET_NAME(arg1));
        pos1 = 0;
        pos2 = 0;
        while(str1[pos1] != NUL){
            if(str1[pos1] == ':'){
                str2[pos2] = '_';
                pos2++;
            }
            else if(str1[pos1] == '&'){
                str2[pos2] = 'a';
                pos2++;
                str2[pos2] = 'n';
                pos2++;
                str2[pos2] = 'd';
                pos2++;
            }
            else if(str1[pos1] == '?'){
                str2[pos2] = 'm';
                pos2++;
                str2[pos2] = 'a';
                pos2++;
                str2[pos2] = 'g';
                pos2++;
            }
            else if(str1[pos1] == '+'){
                str2[pos2] = 'p';
                pos2++;
                str2[pos2] = 'l';
                pos2++;
                str2[pos2] = 's';
                pos2++;
            }
            else if(str1[pos1] == '-'){
                str2[pos2] = 'm';
                pos2++;
                str2[pos2] = 'n';
                pos2++;
                str2[pos2] = 's';
                pos2++;
            }
            else if(str1[pos1] == '*'){
                str2[pos2] = 'a';
                pos2++;
                str2[pos2] = 's';
                pos2++;
                str2[pos2] = 't';
                pos2++;
            }
            else if(str1[pos1] == '/'){
                str2[pos2] = 'd';
                pos2++;
                str2[pos2] = 'i';
                pos2++;
                str2[pos2] = 'v';
                pos2++;
            }
            else if(str1[pos1] == '>'){
                str2[pos2] = 'r';
                pos2++;
                str2[pos2] = 'i';
                pos2++;
                str2[pos2] = 'g';
                pos2++;
            }
            else if(str1[pos1] == '<'){
                str2[pos2] = 'l';
                pos2++;
                str2[pos2] = 'e';
                pos2++;
                str2[pos2] = 'f';
                pos2++;
            }
            else{
                str2[pos2] = str1[pos1];
                pos2++;
            }
            pos1++;
        }
        str2[pos2] = NUL;
        unify(arg2,makeconst(str2));
        return(YES);
    }
    return(NO);
}

int b_arity_count(int arglist, int rest){
	int n,arg1,arg2;

    n = length(arglist);
    if(n == 2){
    	arg1 = deref(car(arglist));
        arg2 = cadr(arglist);

        return(unify(arg2,listreverse(GET_ARITY(arg1))));
    }
    return(NO);
}


int b_generate_all_variable(int arglist, int rest){
	int n,arg1,arg2;

    n = length(arglist);
    if(n == 2){
    	arg1 = deref(car(arglist));
        arg2 = cadr(arglist);
        return(unify(arg2,generate_all_variable(GET_CAR(arg1))));
    }
    return(NO);
}

int b_generate_variable(int arglist, int rest){
	int n,arg1,arg2;

    n = length(arglist);
    if(n == 2){
    	arg1 = deref(car(arglist));
        arg2 = cadr(arglist);
        return(unify(arg2,generate_variable(arg1)));
    }
    return(NO);
}

int generate_all_variable(int x){
    int y,res;

    res = NIL;
    while(!nullp(x)){
    	y = variable_convert1(car(x));
    	if(clausep(y))
    		res = generate_variable1(caddr(y),
                   generate_variable1(cadr(y),res));
    	else
    		res = generate_variable1(y,res);

        x = cdr(x);
    }
    return(res);
}

int generate_variable(int x){
	if(clausep(x))
    	return(generate_variable1(caddr(x),
                   generate_variable1(cadr(x),NIL)));
    else
    	return(generate_variable1(x,NIL));
}

int generate_variable1(int x, int y){
    int temp;

    if(nullp(x))
    	return(y);
    else if(numberp(x))
    	return(y);
    else if(compiler_variable_p(x) && !memq(x,y)){
    	temp = cons(x,y);
        SET_AUX(temp,LIST);
        return(temp);
    }
    else if(singlep(x))
    	return(y);
    else if(car(x) == AND)
    	return(generate_variable1(cadr(x),generate_variable1(caddr(x),y)));
    else if(car(x) == OR)
    	return(generate_variable1(cadr(x),generate_variable1(caddr(x),y)));
    else
    	return(generate_variable1(car(x),generate_variable1(cdr(x),y)));
}

int b_compiler_anoymous(int arglist, int rest){
    int n,arg1;

    n = length(arglist);
    if(n == 1){
        arg1 = deref(car(arglist));
        if(!!structurep(arg1))
            return(NO);
        else if(numberp(arg1))
        	return(NO);

        if(strcmp(GET_NAME(arg1),"var_") == 0)
            return(YES);
        else
            return(NO);
    }
    return(NO);
}

int b_compiler_variable(int arglist, int rest){
	int n,arg1;

    n = length(arglist);
    if(n == 1){
    	arg1 = deref(car(arglist));
        if(compiler_variable_p(arg1))
        	return(YES);
        else
        	return(NO);
    }
    return(NO);
}

int compiler_variable_p(int x){
	char str[256];

    if(numberp(x))
    	return(0);
    if(!!structurep(x))
    	return(0);
    strcpy(str,GET_NAME(x));
    str[3] = NUL;
    if(strcmp(str,"var") == 0)
    	return(1);
    else
    	return(0);
}

int b_variable_convert(int arglist, int rest){
    int n,arg1,arg2;

    n = length(arglist);
    if(n == 2){
        arg1 = deref(car(arglist));
        arg2 = cadr(arglist);

        return(unify(arg2,variable_convert1(arg1)));
    }
    return(NO);
}

int variable_convert1(int x){
    int y;
    variable_convert4(NIL); //initialize anoymous N
    y = variable_convert2(x);
    if(clausep(x)){
    	SET_AUX(y,CLAUSE);
        SET_OPT(y,GET_OPT(x));
    }
    return(y);
}

int variable_convert2(int x){
    int y,z;

    if(nullp(x))
        return(NIL);
    else if(anoymousp(x))
        return(variable_convert4(x));
    else if(variablep(x))
        return(variable_convert3(x));
    else if(!structurep(x))
        return(x);
    else{
        y = GET_AUX(x);
        z = cons(variable_convert2(car(x)),
                variable_convert2(cdr(x)));
        SET_AUX(z,y);
        return(z);
    }

}

//normal variable X -> varX
int variable_convert3(int x){
    int res;
    char str[STRSIZE];

    strcpy(str,"var");
    strcat(str,GET_NAME(x));
    res = makeconst(str);
    return(res);
}

//anoymous variable _ -> varN (N=1...)
int variable_convert4(int x){
    int res;
    static int n;
    char str1[STRSIZE],str2[STRSIZE];

    //initialize
    if(x == NIL){
        n = 1;
        return(0);
    }

    strcpy(str1,"var");
    sprintf(str2,"%d",n);
    strcat(str1,str2);
    res = makeconst(str1);
    n++;
    return(res);
}

int b_clause_with_arity(int arglist, int rest){
    int n,arg1,arg2,arg3,l,clause,clauses,res;

    n = length(arglist);
    if(n == 3){
        arg1 = deref(car(arglist));
        arg2 = deref(cadr(arglist));
        arg3 = deref(caddr(arglist));
        if(!singlep(arg1))
            error(NOT_ATOM, "n_clause_with_arity", arg1);
        if(!integerp(arg2))
            error(NOT_INT, "n_clause_with_arity", arg2);

        clauses = GET_CAR(arg1);
        l = GET_INT(arg2);
        res = NIL;

        while(!nullp(clauses)){
            clause = car(clauses);
            if(atomp(clause) && l == 0)
                res = cons(clause,res);
            else if(predicatep(clause) && length(clause) == l+1)
                res = cons(clause,res);
            else if(clausep(clause) && atomp(cadr(clause)) && l == 0)
                res = cons(clause,res);
            else if(clausep(clause) && length(cadr(clause)) == l+1)
                res = cons(clause,res);

            clauses = cdr(clauses);
        }
        res = listreverse(res);
        unify(res,arg3);
        return(YES);
    }
    return(NO);
}

int b_property(int arglist, int rest){
    int n,arg1,arg2;

    n = length(arglist);
    if(n == 2){
        arg1 = deref(car(arglist));
        arg2 = deref(cadr(arglist));

        if(builtinp(arg1))
            return(unify(arg2,makeconst("builtin")));
        else if(predicatep(arg1))
            return(unify(arg2,makeconst("predicate")));
        else if(compiledp(arg1))
            return(unify(arg2,makeconst("compiled")));
        else if(operationp(arg1))
            return(unify(arg2,makeconst("operation")));
        else if(operatorp(arg1))
            return(unify(arg2,makeconst("operation")));
        else if(user_operation_p(arg1))
            return(unify(arg2,makeconst("userop")));
        else
            return(NO);

    }
    return(NO);
}

int b_bignum(int arglist, int rest){
    int n,arg1;

    n = length(arglist);
    if(n == 1){
        arg1 = deref(car(arglist));

        if(bignump(arg1))
            return(YES);
        else
            return(NO);
    }
    return(NO);
}

int b_longnum(int arglist, int rest){
    int n,arg1;

    n = length(arglist);
    if(n == 1){
        arg1 = deref(car(arglist));

        if(longnump(arg1))
            return(YES);
        else
            return(NO);
    }
    return(NO);
}

int b_argument_list(int arglist, int rest){
    int n,arg1,arg2,res;

    n = length(arglist);
    if(n == 2){
        arg1 = car(arglist);
        arg2 = cadr(arglist);

        arg1 = listreverse(cdr(arg1));
        res = NIL;
        while(!nullp(arg1)){
           res = listcons(car(arg1),res);
           arg1 = cdr(arg1);
        }
        return(unify(arg2,res));
    }
    return(NO);
}

int b_findatom(int arglist, int rest){
    int n,arg1,arg2,arg3,res;

    n = length(arglist);
    if(n == 3){
        arg1 = deref(car(arglist)); //atom
        arg2 = deref(cadr(arglist));//property
        arg3 = deref(caddr(arglist));//address

        res = NIL;
        if(eqlp(arg2,makeconst("constant")))
            res = findatom(arg1,SIMP);
        else if(eqlp(arg2,makeconst("predicate")))
            res = findatom(arg1,PRED);
        else if(eqlp(arg2,makeconst("builtin")))
            res = findatom(arg1,SYS);
        else if(eqlp(arg2,makeconst("compiled")))
            res = findatom(arg1,COMP);
        else if(eqlp(arg2,makeconst("operator")))
            res = findatom(arg1,OPE);
        else if(eqlp(arg2,makeconst("userop")))
            res = findatom(arg1,USER);
        else
            error(ILLEGAL_ARGS,"findatom ", arg2);

        return(unify(arg3,makeint(res)));
    }
    return(NO);
}

int b_defined_predicate(int arglist, int rest){
	int n,arg1;

    n = length(arglist);
    if(n == 1){
    	arg1 = deref(car(arglist));

        if(predicatep(arg1)){
            if(atomp(arg1) && GET_CAR(arg1) != NIL)
                return(YES);
            else if(structurep(arg1) && GET_CAR(car(arg1)) != NIL)
        	    return(YES);
            else
                return(NO);
        }
        else
        	return(NO);
    }
    return(NO);
}

int b_defined_userop(int arglist, int rest){
	int n,arg1;

    n = length(arglist);
    if(n == 1){
    	arg1 = deref(car(arglist));

        if(user_operation_p(arg1)){
            if(atomp(arg1) && GET_CAR(arg1) != NIL)
                return(YES);
            else if(structurep(arg1) && GET_CAR(car(arg1)) != NIL)
        	    return(YES);
            else
                return(NO);
        }
        else
        	return(NO);
    }
    return(NO);
}

int b_get_execute(int arglist, int rest){
	int n,arg1,pos,res;

    n = length(arglist);
    if(n == 1){
    	arg1 = car(arglist);

        res = NIL;
        pos = execute_list;
        while(!nullp(pos)){
           res = listcons(car(pos),res);
           pos = cdr(pos);
        }
        return(unify(arg1,res));
    }
    return(NO);
}