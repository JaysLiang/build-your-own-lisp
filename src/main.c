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


typedef struct{
    int type;
    long num;
    int err
} lval;

enum {
    LVAL_NUM,
    LVAL_ERR
};

enum {
    LERR_DIV_ZERO,
    LERR_BAD_OP,
    LERR_BAD_NUM
};

lval lval_num(long x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(long x) {
    lval v;
    v.type = LVAL_ERR;
    v.num = x;
    return v;
}

void lval_print(lval v) {
    switch (v.type) {
        case LVAL_NUM:
            printf("%li", v.num);
            break;
        case LVAL_ERR:
            if (v.err == LERR_DIV_ZERO) {
                printf("Error: Division by zero!");
            } else if (v.err == LERR_BAD_OP) {
                printf("Error: Invalid Operator!");
            } else if (v.err == LERR_BAD_NUM) {
                printf("Error: Invalid Number!");
            }
            break;
    }
}

void lval_println(lval v) {
    lval_print(v);
    printf("\n");
}


lval eval_op(lval x, char* operator, lval y) {
    if (x.type == LVAL_ERR) {
        return x;
    }
    if (y.type == LVAL_ERR) {
        return y;
    }

    if (strstr(operator, "+")) {
        return lval_num(x.num + y.num);
    } else if (strstr(operator, "-")) {
        return lval_num(x.num - y.num);
    } else if (strstr(operator, "*")) {
        return lval_num(x.num * y.num);
    } else if (strstr(operator, "/")) {
        return y.num == 0 ? lval_err(LERR_DIV_ZERO): lval_num(x.num / y.num);
    }
}

lval eval(mpc_ast_t* a) {
    if (strstr(a->tag, "number")) {
        errno = 0;
        long x = strtol(a->contents, NULL, 10);
        return errno != ERANGE? lval_num(x):lval_err(LERR_BAD_NUM);
    }

    char* op=a->children[1]->contents;

    lval x = eval(a->children[2]);

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
            lval ret = eval(r.output);
            lval_println(ret);
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