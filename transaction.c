#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "vault.h" // For global_username and user_exists()

#define BALANCE_FILE "balances.csv"

// This is for transaction history start
void log_transaction(const char *type, float amount) {
    char file_name[100];
    snprintf(file_name, sizeof(file_name), "%s.csv", global_username);  // format the filename

    FILE *fp = fopen(file_name, "a");
    if (!fp) {
        printf("Could not log transaction.\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    fprintf(fp, "%s,%s,%.2f,%s\n", global_username, type, amount, timestamp);
    fclose(fp);
}

void show_transaction_history() {
    char file_name[100];
    snprintf(file_name, sizeof(file_name), "%s.csv", global_username);  // user-specific file

    FILE *fp = fopen(file_name, "r");
    if (!fp) {
        printf("No transaction history found for user '%s'.\n", global_username);
        return;
    }

    char line[150], user[50], type[20], timestamp[30];
    float amount;

    printf("Transaction History for %s:\n", global_username);
    printf("----------------------------------------\n");
    printf("Date & Time           | Type     | Amount\n");
    printf("----------------------------------------\n");

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%[^,],%f,%[^\n]", user, type, &amount, timestamp);
        printf("%-20s | %-8s | %8.2f\n", timestamp, type, amount);
    }

    fclose(fp);
}

// This is for transaction history end


int update_send_money_balance(float new_balance, const char *username) {
    FILE *fp = fopen(BALANCE_FILE, "r");
    FILE *temp = fopen("temp.csv", "w");

    if (!fp || !temp) return 0;

    char line[100], user[50];
    float bal;
    int updated = 0;

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%f", user, &bal);
        if (strcmp(user, username) == 0) {
            fprintf(temp, "%s,%.2f\n", user, new_balance);
            updated = 1;
        } else {
            fprintf(temp, "%s,%.2f\n", user, bal);
        }
    }

    fclose(fp);
    fclose(temp);
    remove(BALANCE_FILE);
    rename("temp.csv", BALANCE_FILE);

    return updated;
}

float get_user_balance(const char *username) {
    FILE *fp = fopen(BALANCE_FILE, "r");
    if (!fp) return -1;

    char line[100], user[50];
    float bal;

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%f", user, &bal);
        if (strcmp(user, username) == 0) {
            fclose(fp);
            return bal;
        }
    }

    fclose(fp);
    return -1; // Not found
}

void send_money(const char *receiver_username, float amount) {
    if (!user_exists(receiver_username)) {
        printf("Username doesn't exist.\n");
        return;
    }

    if (amount <= 0) {
        printf("Invalid amount.\n");
        return;
    }

    float sender_balance = get_balance(); // uses global_username internally
    if (sender_balance < amount) {
        printf("Insufficient balance.\n");
        return;
    }

    // Deduct from sender
    float new_sender_balance = sender_balance - amount;
    if (!update_send_money_balance(new_sender_balance, global_username)) {
        printf("Failed to update sender's balance.\n");
        return;
    }

    // Add to receiver
    float receiver_balance = get_user_balance(receiver_username);
    if (receiver_balance < 0) receiver_balance = 0;

    float new_receiver_balance = receiver_balance + amount;
    if (!update_send_money_balance(new_receiver_balance, receiver_username)) {
        printf("Failed to update receiver's balance.\n");
        return;
    }

    printf("Money sent successfully to %s.\n", receiver_username);
    printf("Your new balance is: %.2f\n", new_sender_balance);
    log_transaction("Send Money", amount);
}

