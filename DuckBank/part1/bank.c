#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "account.h"

void processTransaction(account *accounts, int numAccounts, char *transactions);
void applyRewards(account *accounts, int numAccounts);
void printBalance(account *accounts, int numAccounts);

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Wrong number of arguments!\n"); // TODO
        return EXIT_FAILURE;
    }
    FILE *file = fopen(argv[1], "r");
    if(file == NULL){
        printf("Error Opening File : %s\n", argv[1]);
        exit(1);
    }

    int numAccounts;
    fscanf(file, "%d", &numAccounts);

    account *accounts = malloc(numAccounts * sizeof(account));

    if(!accounts) {
        perror("Memory Allocation Failed");
        fclose(file);
        return EXIT_FAILURE;
    }

    char index[1024];

    for(int i = 0; i < numAccounts; i++){
        fscanf(file, "%s %*d", index);
        fscanf(file, "%s", accounts[i].account_number);
        fscanf(file, "%s", accounts[i].password);
        fscanf(file, "%lf", &accounts[i].balance);
        fscanf(file, "%lf", &accounts[i].reward_rate);
        accounts[i].transaction_tracter = 0.0;
    }

    char transactionDetails[1024];
    while(fgets(transactionDetails, sizeof(transactionDetails), file)) {
        processTransaction (accounts, numAccounts, transactionDetails);
    }

    applyRewards(accounts, numAccounts);
    printBalance(accounts, numAccounts);
    free(accounts);
    fclose(file);
    return 0;
}

int findAccount(int numAccounts, account *accounts, const char *accountNumber) {
    for (int i = 0; i < numAccounts; i++) {
        if (strcmp(accounts[i].account_number, accountNumber) == 0) {
            return i; // Return the index of the account
        }
    }
    return -1; // Return -1 if account not found
}

void processTransaction(account * accounts, int numAccounts, char *transactionDetails) {    
    char *saveptr; // Pointer to store the context

    // Tokenize using strtok_r
    char *type = strtok_r(transactionDetails, " ", &saveptr);
    if (type == NULL) return;

    char *accountNumber = strtok_r(NULL, " ", &saveptr);
    char *password = strtok_r(NULL, " ", &saveptr);
    if (!password && !accountNumber) return;

    int accountIndex = findAccount(numAccounts, accounts, accountNumber);
    if (accountIndex == -1) return ;
    if (strcmp(accounts[accountIndex].password, password) != 0) return ;

    if (strcmp("T", type) == 0) {
        char *transferAccount = strtok_r(NULL, " ", &saveptr);
        double transferAmount = atof(strtok_r(NULL, " ", &saveptr));
        int transferIndex = findAccount(numAccounts, accounts, transferAccount);
        accounts[accountIndex].balance -= transferAmount;
        accounts[transferIndex].balance += transferAmount;
        accounts[accountIndex].transaction_tracter += transferAmount;
    } else if (strcmp("C", type) == 0) {
        return;
    } else if (strcmp("D", type) == 0) {
        double depositAmount = atof(strtok_r(NULL, " ", &saveptr));
        accounts[accountIndex].balance += depositAmount;
        accounts[accountIndex].transaction_tracter += depositAmount;

    } else if (strcmp("W", type) == 0) {
        double withdrawlAmount = atof(strtok_r(NULL, " ", &saveptr));
        accounts[accountIndex].balance -= withdrawlAmount;
        accounts[accountIndex].transaction_tracter += withdrawlAmount;
    }

    return;
}

void applyRewards(account *accounts, int numAccounts){
    for(int i = 0; i < numAccounts; i++){
        double totalReward = accounts[i].transaction_tracter * accounts[i].reward_rate;
        accounts[i].balance += totalReward;
    }
    return;
}

void printBalance(account *accounts, int numAccounts){
    FILE *output = fopen("output.txt", "w");
    if(output == NULL) {
        perror("Error creating output file!");
        return;
    }

    for(int i = 0; i < numAccounts; i++){
        fprintf(output, "%d balance: %0.2f\n", i, accounts[i].balance);
    }
    fclose(output);
    return;
}