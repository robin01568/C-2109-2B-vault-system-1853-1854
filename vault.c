#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transaction.h"

#define USER_FILE "users.csv"
#define BALANCE_FILE "balances.csv"

char global_username[50] = "";

int user_exists(const char *username) {
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) return 0;

    char line[100], name[50], user[50], pass[50];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%[^,],%s", name, user, pass);
        if (strcmp(user, username) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int register_user(const char *name, const char *username, const char *password) {
    if (user_exists(username)) {
        printf("User '%s' already exists.\n", username);
        return 0;
    }

    FILE *ufp = fopen(USER_FILE, "a");
    FILE *bfp = fopen(BALANCE_FILE, "a");

    if (!ufp || !bfp) {
        printf("Error opening file.\n");
        return 0;
    }

    fprintf(ufp, "%s,%s,%s\n", name, username, password);
    fprintf(bfp, "%s,0.0\n", username);

    fclose(ufp);
    fclose(bfp);

    printf("User '%s' registered successfully.\n", name);
    return 1;
}

int authenticate_user(const char *username, const char *password) {
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) return 0;

    char line[100], name[50], user[50], pass[50];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%[^,],%s",name, user, pass);
        if (strcmp(user, username) == 0 && strcmp(pass, password) == 0) {
            strcpy(global_username, username); // Save to global variable
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void get_name(const char *username, char *name_out) {
    FILE *fp = fopen(USER_FILE, "r");
    if (!fp) {
        strcpy(name_out, "Unknown");
        return;
    }

    char line[150], name[50], user[50], pass[50];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%[^,],%s", name, user, pass);
        if (strcmp(user, username) == 0) {
            strcpy(name_out, name);
            fclose(fp);
            return;
        }
    }

    strcpy(name_out, "Unknown");
    fclose(fp);
}

float get_balance() {
    FILE *fp = fopen(BALANCE_FILE, "r");
    if (!fp) return -1;

    char line[100], user[50];
    float bal;

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%f", user, &bal);
        if (strcmp(user, global_username) == 0) {
            fclose(fp);
            return bal;
        }
    }
    fclose(fp);
    return -1;
}

int update_balance(float new_balance) {
    FILE *fp = fopen(BALANCE_FILE, "r");
    FILE *temp = fopen("temp.csv", "w");

    if (!fp || !temp) return 0;

    char line[100], user[50];
    float bal;
    int updated = 0;

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%[^,],%f", user, &bal);
        if (strcmp(user, global_username) == 0) {
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

int deposit(float amount) {
    if (amount <= 0) {
        printf("Invalid deposit amount.\n");
        return 0;
    }

    float bal = get_balance();
    if (bal < 0) {
        printf("Could not retrieve balance.\n");
        return 0;
    }

    if (update_balance(bal + amount)) {
        printf("Deposit successful. New balance: %.2f\n", bal + amount);
        log_transaction("Deposit", amount);
        return 1;
    }

    printf("Deposit failed.\n");
    return 0;
}

int withdraw(float amount) {
    if (amount <= 0) {
        printf("Invalid withdrawal amount.\n");
        return 0;
    }

    float bal = get_balance();
    if (bal < 0) {
        printf("Could not retrieve balance.\n");
        return 0;
    }

    if (amount > bal) {
        printf("Insufficient funds. Balance: %.2f\n", bal);
        return 0;
    }

    if (update_balance(bal - amount)) {
        printf("Withdrawal successful. New balance: %.2f\n", bal - amount);
        log_transaction("Withdraw", amount);
        return 1;
    }

    printf("Withdrawal failed.\n");
    return 0;
}

