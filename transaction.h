#ifndef TRANSACTION_H
#define TRANSACTION_H

void send_money(const char *username, float amount);
void log_transaction(const char *type, float amount);
void show_transaction_history();

#endif // TRANSACTION_H
