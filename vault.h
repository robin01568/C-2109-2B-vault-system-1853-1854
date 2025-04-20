#ifndef VAULT_H
#define VAULT_H

extern char global_username[50]; // Global variable for logged-in user

int user_exists(const char *username);
int register_user(const char *name, const char *username, const char *password);
int authenticate_user(const char *username, const char *password);
void get_name(const char *username, char *name_out);
float get_balance();
int update_balance(float amount);
int deposit(float amount);
int withdraw(float amount);

#endif
