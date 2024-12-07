#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include "account.h"
#include "transaction.h"

#define NUM_WORKERS 10
pthread_t *thread_ids;

int pipe_fd[2];	 // The pipe
pthread_mutex_t pipe_lock;	
int checkBalance500 = 0;

void accountInit(int numAccounts, account *accounts, FILE *file);
int getNumTransaction(char *filename);
void *processTransaction(void *arg);
void *auditorProcess(void *arg);
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

    pthread_mutex_init(&pipe_lock, NULL);	 // Initialize the pipe
    if (pipe(pipe_fd) == -1) {			// init the pipe!
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t auditor_pid = fork();
    if (auditor_pid == -1) {
        perror("Error creating Auditor process");
        return EXIT_FAILURE;
    }

    if (auditor_pid == 0) {  // Auditor child process
        close(pipe_fd[1]);  // Close write end of the pipe in the child
        auditorProcess(NULL);  // Process the pipe in the Auditor
        exit(0);
    }

    close(pipe_fd[0]);

    int numAccounts;
    fscanf(file, "%d", &numAccounts);
    account *accounts = malloc(numAccounts * sizeof(account));
    if(!accounts) {
        perror("Memory Allocation Failed");
        fclose(file);
        return EXIT_FAILURE;
    }

    int AccountInfoLines;
    AccountInfoLines = (numAccounts * 5);
    int numTransactions = getNumTransaction(argv[1]) - AccountInfoLines - 1;
    int transactionsPerWorker = numTransactions / NUM_WORKERS;
    // int extraTransactions = numTransactions % NUM_WORKERS;
    thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * NUM_WORKERS); // Number of worker threads to work concurrently
    
    accountInit(numAccounts, accounts, file);
    fclose(file);
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        TransactionArgs *args = malloc(sizeof(TransactionArgs));
        if(!args) {
            free(thread_ids);
            free(args);
            perror("Failed to allocate memory for thread arguments!");
            return EXIT_FAILURE;
        }
        args->accounts = accounts;
        args->numAccounts = numAccounts;
        FILE *uniqueFile = fopen(argv[1], "r");
        args->file = uniqueFile; 

        args->startLine = AccountInfoLines + (i * transactionsPerWorker) + 2; 
        args->numTransactions = transactionsPerWorker; // TODO
    
        if(pthread_create(&thread_ids[i], NULL, processTransaction, (void*)args)) {
            perror("Failed to create threads!");
            free(thread_ids);
            free(args);
            fclose(file);
            return EXIT_FAILURE;
        }
    }
    
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_join(thread_ids[i], NULL);
    }


    applyRewards(accounts, numAccounts);
    close(pipe_fd[1]);
    waitpid(auditor_pid, NULL, 0);

    printBalance(accounts, numAccounts);
    free(thread_ids);
    free(accounts);
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
    int numTransactions = args->numTransactions;
    
    char dummy[1024];
    for (int j = 0; j < args->startLine - 1; j++) {
        fgets(dummy, sizeof(dummy), file); //  user args->uniqueFile or unqiueFile?
    }

    char transactionDetails[1024];
    for(int i = 0; i < numTransactions; i++){
        fgets(transactionDetails, sizeof(transactionDetails), file);
        char *saveptr; // Pointer to store the context
        // Tokenize using strtok_r
        char *type = strtok_r(transactionDetails, " ", &saveptr);
        if (type == NULL) continue;

        char *accountNumber = strtok_r(NULL, " ", &saveptr);
        char *password = strtok_r(NULL, " ", &saveptr);
        if (!password && !accountNumber) continue;

        if(password == NULL || accountNumber == NULL){
            continue;
        }

        int accountIndex = findAccount(numAccounts, accounts, accountNumber);

        if (accountIndex == -1) continue;
        if (strcmp(accounts[accountIndex].password, password) != 0) continue;

        if (strcmp("T", type) == 0) {
            // printf("Transfer Requested!\n"); 
            char *transferAccount = strtok_r(NULL, " ", &saveptr);
            double transferAmount = atof(strtok_r(NULL, " ", &saveptr));
            int transferIndex = findAccount(numAccounts, accounts, transferAccount);
            
            pthread_mutex_lock(&accounts[accountIndex].ac_lock);
            accounts[accountIndex].balance -= transferAmount;
            accounts[accountIndex].transaction_tracter += transferAmount;
            pthread_mutex_unlock(&accounts[accountIndex].ac_lock);
            
            pthread_mutex_lock(&accounts[transferIndex].ac_lock);
            accounts[transferIndex].balance += transferAmount;
            pthread_mutex_unlock(&accounts[transferIndex].ac_lock);

        } else if (strcmp("C", type) == 0) {
            // printf("Account Balance : %f\n", accounts[accountIndex].balance);
            pthread_mutex_lock(&pipe_lock);
            checkBalance500++;	
            if (checkBalance500 % 500 == 0) {
                // Write to the pipe
                time_t now = time(NULL);
                struct tm *tm_info = localtime(&now);
                char message[256];
                snprintf(message, sizeof(message), "Account %s balance: $%.2f. Check occurred at %s", accountNumber, accounts[accountIndex].balance, asctime(tm_info));
                // printf("Account %s balance: $%.2f at %s", accountNumber, accounts[accountIndex].balance, asctime(tm_info));                
                write(pipe_fd[1], message, strlen(message));
            }
            pthread_mutex_unlock(&pipe_lock);
        } else if (strcmp("D", type) == 0) {
            pthread_mutex_lock(&accounts[accountIndex].ac_lock);
            double depositAmount = atof(strtok_r(NULL, " ", &saveptr));
            accounts[accountIndex].balance += depositAmount;
            accounts[accountIndex].transaction_tracter += depositAmount;
            pthread_mutex_unlock(&accounts[accountIndex].ac_lock);
        } else if (strcmp("W", type) == 0) {
            pthread_mutex_lock(&accounts[accountIndex].ac_lock);
            double withdrawlAmount = atof(strtok_r(NULL, " ", &saveptr));
            accounts[accountIndex].balance -= withdrawlAmount;
            accounts[accountIndex].transaction_tracter += withdrawlAmount;
            pthread_mutex_unlock(&accounts[accountIndex].ac_lock);
        }
    }
    free(args);
    fclose(file);
    pthread_exit(NULL);

}

void applyRewards(account *accounts, int numAccounts){
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    for(int i = 0; i < numAccounts; i++){
        pthread_mutex_lock(&accounts[i].ac_lock);
        double totalReward = accounts[i].transaction_tracter * accounts[i].reward_rate;
        accounts[i].balance += totalReward;
        pthread_mutex_unlock(&accounts[i].ac_lock);
        // printf("Account %s:  balance after reward: %.2f\n", accounts[i].account_number, accounts[i].balance);
        
        char message[256];
        snprintf(message, sizeof(message), "Account %d balance after rewards: $%.2f. Time of Update %s", i, accounts[i].balance, asctime(tm_info));

        // Write to the pipe (auditor will read this)
        pthread_mutex_lock(&pipe_lock);
        write(pipe_fd[1], message, strlen(message));  // Send reward balance info to the auditor process
        pthread_mutex_unlock(&pipe_lock);
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
        fprintf(output, "%d balance:\t%0.2f\n\n", i, accounts[i].balance);
        char message[256];
    }
    fclose(output);
    return;
}

void *auditorProcess(void *arg) {
    char buffer[256];
    ssize_t bytes_read;
    FILE *ledger = fopen("ledger.txt", "w");
    
    if (ledger == NULL) {
        perror("Error opening ledger.txt");
        return NULL;
    }

    // Read from the pipe and write to ledger.txt
    while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
        buffer[bytes_read] = '\0'; 
        fwrite(buffer, 1, bytes_read, ledger);
    }

    fclose(ledger);
    return NULL;
}
