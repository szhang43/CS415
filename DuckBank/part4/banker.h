#ifndef BANKER_H_
#define BANKER_H_
#include <stdio.h>  

#include "account.h"

typedef struct {
    account *accounts;        
    int numAccounts;          
} BankerArgs;

#endif
