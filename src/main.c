//
// Created by liangweiran on 2020/4/14.
//
#include <stdio.h>
#include <stdlib.h>
#include "mpc.h"

#ifdef _WIN32
#include <string.h>
static char buffer[2048];

char *readline(char* prompt) {
    fputs(prompt,stdout);
    fgets(buffer,2048, stdin);
    char* cpy=malloc(strlen(buffer)+1);
    strcpy(cpy,buffer);
    return cpy;
}

void add_history(char* unused) {
    //fake function
}
#else
    #include <editline/history.h>
    #include <editline/readline.h>
#endif



void printchild(mpc_ast_t* a) {
    printf("Tag: %s\n", a->tag);
    printf("Contents: %s\n", a->contents);
    printf("Number of children: %i\n", a->children_num);

    for (int i = 0 ; i < a->children_num; i++) {
        mpc_ast_t* c0 = a->children[i];
        printf("First Child Tag: %s\n", c0->tag);
        printf("First Child Contents: %s\n", c0->contents);
        printf("First Child Number of children: %i\n",
               c0->children_num);

    }

}

long eval_op(long x, char* operator, long y) {
    if (strstr(operator, "+")) {
        return x + y;
    } else if (strstr(operator, "-")) {
        return x - y;
    } else if (strstr(operator, "*")) {
        return x * y;
    } else if (strstr(operator, "/")) {
        return x / y;
    }

}

long eval(mpc_ast_t* a) {
    if (strstr(a->tag, "number")) {
        return atoi(a->contents);
    }

    char* op=a->children[1]->contents;

    long x = eval(a->children[2]);

    int i = 3;
    while(strstr(a->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(a->children[i]));
        i++;
    }
    return x;
}

int main(int argc, char** argv) {
    puts("Lispy Version 0.0.0.0.1");
    puts("Press Ctrl+c to Exit\n");

    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Lispy    = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
              "                                                     \
    number   : /-?[0-9]+/ ;                             \
    operator : '+' | '-' | '*' | '/' ;                  \
    expr     : <number> | '(' <operator> <expr>+ ')' ;  \
    lispy    : /^/ <operator> <expr>+ /$/ ;             \
    ",
              Number, Operator, Expr,Lispy);

    while(1) {
        char* input = readline("lispy> ");

        add_history(input);

//        printf("No you're a %s\n", input);
        /* Attempt to Parse the user Input */
        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {
            /* On Success Print the AST */
//            printchild(r.output);
            mpc_ast_print(r.output);
            long ret = eval(r.output);
            printf("%ld\n",ret);
            mpc_ast_delete(r.output);
        } else {
            /* Otherwise Print the Error */
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input);
    }

    mpc_cleanup(4, Number, Operator, Expr, Lispy);
    return 0;
}