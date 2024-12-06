#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "account.h"
#include "transaction.h"

#define NUM_WORKERS 10
pthread_t *thread_ids;

void accountInit(int numAccounts, account *accounts, FILE *file);
int getNumTransaction(char *filename);
void *processTransaction(void *arg);
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

    int AccountInfoLines;
    int numTransactions = getNumTransaction(argv[1]);
    int transactionsPerWorker = numTransactions / NUM_WORKERS;
    int extraTransactions = numTransactions % NUM_WORKERS;

    thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * NUM_WORKERS); // Number of worker threads to work concurrently
    
    accountInit(numAccounts, accounts, file);
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        AccountInfoLines = (numAccounts * 5);
        TransactionArgs *args = malloc(sizeof(TransactionArgs));
        if(!args) {
            perror("Failed to allocate memory for thread arguments!");
            return EXIT_FAILURE;
        }
        args->accounts = accounts;
        args->numAccounts = numAccounts;
        args->file = file; 
        
        int startLine;
        if(i == 0){
            startLine =  AccountInfoLines + 2;
        } else {
            startLine = i * transactionsPerWorker; // startLine for each worker thread
        }

        args->startLine = startLine; 
        args->numTransactions = numTransactions - AccountInfoLines; // TODO

        printf("Thread %d created\n", i);
        if(pthread_create(&thread_ids[i], NULL, (void *)processTransaction, args)) {
            perror("Failed to create threads!");
            free(args);
            return EXIT_FAILURE;
        };
    }
    

    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    applyRewards(accounts, numAccounts);
    printBalance(accounts, numAccounts);
    free(accounts);
    fclose(file);
    return 0;
}

void accountInit(int numAccounts, account *accounts, FILE *file){
    char index[1024];
    for(int i = 0; i < numAccounts; i++){
        fscanf(file, "%s %*d", index);
        fscanf(file, "%s", accounts[i].account_number);
        fscanf(file, "%s", accounts[i].password);
        fscanf(file, "%lf", &accounts[i].balance);
        fscanf(file, "%lf", &accounts[i].reward_rate);
        accounts[i].transaction_tracter = 0.0;
        pthread_mutex_init(&accounts[i].ac_lock, NULL);
    }
}
int getNumTransaction(char *filename){
    FILE *file = fopen(filename, "r");
    if(file == NULL){
        printf("Error Opening File : %s\n", filename);
        exit(1);
    }

    int numTransactions = 0;
    char line[1024];

    while (fgets(line, sizeof(line), file)) {
        numTransactions++;
    }

    return numTransactions;
}

int findAccount(int numAccounts, account *accounts, const char *accountNumber) {
    for (int i = 0; i < numAccounts; i++) {
        if (strcmp(accounts[i].account_number, accountNumber) == 0) {
            return i; // Return the index of the account
        }
    }
    return -1; // Return -1 if account not found
}

void *processTransaction(void *arg) {   
    TransactionArgs *args = (TransactionArgs *)arg;

    account *accounts = args->accounts;
    int numAccounts = args->numAccounts;
    FILE *file = args->file ; 
    int numTransactions = args-> numTransactions;
    int startLine = args->startLine;
    printf("Start line: %d\n", startLine);
    
    char transactionDetails[1024];
    for(int i = 0; i < startLine; i++) {
        fgets(transactionDetails, sizeof(transactionDetails), file);
    }

    char *type = strtok(transactionDetails, " ");
    if(type == NULL) return NULL;
    
    char *accountNumber = strtok(NULL, " ");
    char *password = strtok(NULL, " ");
    if(!password && !accountNumber) return NULL;

    int accountIndex = findAccount(numAccounts, accounts, accountNumber);
    if (accountIndex == -1) return NULL;

    if(strcmp(accounts[accountIndex].password, password) != 0) return NULL;

    for(int i = 0; i < numTransactions; i++){
        fgets(transactionDetails, sizeof(transactionDetails), file);
        printf("Transaction Details: %s\n", transactionDetails);
        pthread_mutex_lock(&accounts[accountIndex].ac_lock);

        if(strcmp("T", type) == 0){
            char *transferAccount = strtok(NULL, " ");
            double transferAmount = atof(strtok(NULL, " "));
            int transferIndex = findAccount(numAccounts, accounts, transferAccount);
            accounts[accountIndex].balance -= transferAmount;
            accounts[transferIndex].balance += transferAmount;
            accounts[accountIndex].transaction_tracter += transferAmount;
        } else if(strcmp("C", type) == 0) {
            return NULL;
        } else if(strcmp("D", type) == 0){
            double depositAmount = atof(strtok(NULL, " "));
            accounts[accountIndex].balance += depositAmount;
            accounts[accountIndex].transaction_tracter += depositAmount;
        
        } else if(strcmp("W", type) == 0) {
            double withdrawlAmount = atof(strtok(NULL, " "));
            accounts[accountIndex].balance -= withdrawlAmount;
            accounts[accountIndex].transaction_tracter += withdrawlAmount;
        }
        pthread_mutex_unlock(&accounts[accountIndex].ac_lock);
    }
    free(args);
    return NULL;
}

void applyRewards(account *accounts, int numAccounts){
    for(int i = 0; i < numAccounts; i++){
        double totalReward = accounts[i].transaction_tracter * accounts[i].reward_rate;
        accounts[i].balance += totalReward;
        printf("Account %s: balance after reward: %.2f\n", accounts[i].account_number, accounts[i].balance);
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

