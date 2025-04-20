#include <stdio.h>
#include <string.h>
#include "vault.h"

int main() {
    int choice;
    char name[50], username[50], password[50];
    float amount;

    while (1) {
        printf("\n=== Vault System ===\n");
        printf("1. Register\n2. Login\n3. Exit\nChoose: ");
        scanf("%d", &choice);
        getchar(); // Clear newline

        if (choice == 1) {
            printf("Name: ");
            scanf("%s", name);
            printf("Username: ");
            scanf("%s", username);
            printf("Password: ");
            scanf("%s", password);
            register_user(name, username, password);

        } else if (choice == 2) {
            printf("Username: ");
            scanf("%s", username);
            printf("Password: ");
            scanf("%s", password);

            if (authenticate_user(username, password)) {
                printf("Login successful. Welcome, %s!\n", global_username);

                while (1) {
                    printf("\n1. Check Balance\n2. Deposit\n3. Withdraw\n4. Logout\nChoose: ");
                    scanf("%d", &choice);

                    if (choice == 1) {
                        printf("Your balance: %.2f\n", get_balance());
                    } else if (choice == 2) {
                        printf("Enter amount to deposit: ");
                        scanf("%f", &amount);
                        deposit(amount);
                    } else if (choice == 3) {
                        printf("Enter amount to withdraw: ");
                        scanf("%f", &amount);
                        withdraw(amount);
                    } else {
                        printf("🔒 Logged out.\n");
                        strcpy(global_username, ""); // Clear session
                        break;
                    }
                }
            } else {
                printf("❌ Invalid username or password.\n");
            }

        } else {
            printf("Exiting Vault. Bye!\n");
            break;
        }
    }

    return 0;
}
