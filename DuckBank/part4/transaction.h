#ifndef TRANSACTION_H_
#define TRANSACTION_H_
#include <stdio.h>  

#include "account.h"

typedef struct {
    account *accounts;        
    int numAccounts;          
    FILE *file;
    int startLine; 
    int numTransactions;
} TransactionArgs;

#endif
