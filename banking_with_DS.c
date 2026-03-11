#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ================= Account Structure ================= */
struct Account {
    int accNo;
    char name[30];
    char phone[15];
    char address[50];
    float balance;
};

/* ================= Linked List Node ================= */
struct Node {
    struct Account data;
    struct Node *next;
};

struct Node *head = NULL;

/* ================= Files ================= */
#define ACCOUNT_FILE "accounts.dat"
#define TRANSACTION_FILE "transactions.txt"


/* ================= Time Utility ================= */
void getTime(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", tm_info);
}

int isWithinOneHour(char *timeStr) {
    struct tm tm_time = {0};
    time_t now;

    sscanf(timeStr, "%d-%d-%d %d:%d:%d",
           &tm_time.tm_year,
           &tm_time.tm_mon,
           &tm_time.tm_mday,
           &tm_time.tm_hour,
           &tm_time.tm_min,
           &tm_time.tm_sec);

    tm_time.tm_year -= 1900;
    tm_time.tm_mon -= 1;

    time_t transTime = mktime(&tm_time);
    time(&now);

    return difftime(now, transTime) <= 3600 && difftime(now, transTime) >= 0;
}

/* ================= Transaction ================= */
void saveTransaction(int accNo, char name[], char type[], float amount) {
    FILE *fp = fopen(TRANSACTION_FILE, "a");
    char timeStr[30];
    getTime(timeStr);

    fprintf(fp,
        "Time: %s | Account: %d | Name: %s | %s | Amount: %.2f\n",
        timeStr, accNo, name, type, amount);

    fclose(fp);
}

/* ================= Linked List ================= */
struct Node* searchAccount(int accNo) {
    struct Node *temp = head;
    while (temp) {
        if (temp->data.accNo == accNo)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void insertAccount(struct Account acc) {
    struct Node *newNode = malloc(sizeof(struct Node));
    newNode->data = acc;
    newNode->next = head;
    head = newNode;
}

/* ================= File Sync ================= */
void loadAccountsFromFile() {
    FILE *fp = fopen(ACCOUNT_FILE, "rb");
    struct Account acc;

    if (!fp) return;

    while (fread(&acc, sizeof(struct Account), 1, fp))
        insertAccount(acc);

    fclose(fp);
}

void saveAccountsToFile() {
    FILE *fp = fopen(ACCOUNT_FILE, "wb");
    struct Node *temp = head;

    while (temp) {
        fwrite(&temp->data, sizeof(struct Account), 1, fp);
        temp = temp->next;
    }
    fclose(fp);
}

/* ================= Create Account ================= */
void createAccount() {
    struct Account acc;

    printf("Enter Account Number: ");
    scanf("%d", &acc.accNo);

    if (searchAccount(acc.accNo)) {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter Name: ");
    scanf("%s", acc.name);

    printf("Enter Phone: ");
    scanf("%s", acc.phone);

    printf("Enter Address: ");
    scanf(" %[^\n]", acc.address);

    acc.balance = 0;
    insertAccount(acc);
    saveAccountsToFile();

    printf("Account created successfully.\n");
}

/* ================= Login ================= */
int login(struct Node **current) {
    int accNo;
    char name[30];

    printf("Enter Account Number: ");
    scanf("%d", &accNo);

    struct Node *node = searchAccount(accNo);
    if (!node) {
        printf("Account not found.\n");
        return 0;
    }

    printf("Enter Account Name: ");
    scanf("%s", name);

    if (strcmp(node->data.name, name) != 0) {
        printf("Login failed.\n");
        return 0;
    }

    *current = node;
    return 1;
}

/* ================= Deposit ================= */
void deposit(struct Node *acc) {
    float amount;
    printf("Enter Deposit Amount: ");
    scanf("%f", &amount);

    acc->data.balance += amount;
    saveAccountsToFile();
    saveTransaction(acc->data.accNo, acc->data.name, "Deposit", amount);

    printf("Deposit successful.\nThank you for banking with us - BRM Bank\n");
}

/* ================= Withdraw ================= */
void withdrawMoney(struct Node *acc) {
    float amount;
    printf("Enter Withdraw Amount: ");
    scanf("%f", &amount);

    if (acc->data.balance < amount) {
        printf("Insufficient balance.\n");
        return;
    }

    acc->data.balance -= amount;
    saveAccountsToFile();
    saveTransaction(acc->data.accNo, acc->data.name, "Withdraw", amount);

    printf("Withdraw successful.\nThank you for banking with us - BRM Bank\n");
}


/* ================= Fund Transfer ================= */
void fundTransfer(struct Node *acc) {
    int choice;
    printf("1. Bank Fund Transfer\n2. Mobile Banking Transfer\nChoice: ");
    scanf("%d", &choice);

    if (choice == 1)
        bankFundTransfer(acc);
    else if (choice == 2)
        mobileBankingTransfer(acc);
}


/* ================= Bank Fund Transfer ================= */
void bankFundTransfer(struct Node *sender) {
    int choice;
    printf("1. This Bank\n2. Other Bank\nChoice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        int accNo;
        float amount;

        printf("Enter Receiver Account Number: ");
        scanf("%d", &accNo);

        struct Node *receiver = searchAccount(accNo);
        if (!receiver) {
            printf("Account not found.\n");
            return;
        }

        printf("Enter Amount: ");
        scanf("%f", &amount);

        if (sender->data.balance < amount) {
            printf("Insufficient balance.\n");
            return;
        }

        sender->data.balance -= amount;
        receiver->data.balance += amount;

        saveAccountsToFile();
        saveTransaction(sender->data.accNo, sender->data.name, "Transfer Out", amount);
        saveTransaction(receiver->data.accNo, receiver->data.name, "Transfer In", amount);

        printf("Transfer successful.\nThank you for banking with us - BRM Bank\n");
    }
    else {
        char bankName[30], ownerName[30], address[50];
        int accNo;
        float amount;

        printf("Enter Bank Name: ");
        scanf("%s", bankName);
        printf("Enter Account Number: ");
        scanf("%d", &accNo);
        printf("Enter Owner Name: ");
        scanf("%s", ownerName);
        printf("Enter Owner Address: ");
        scanf(" %[^\n]", address);
        printf("Enter Amount: ");
        scanf("%f", &amount);

        if (sender->data.balance < amount) {
            printf("Insufficient balance.\n");
            return;
        }

        sender->data.balance -= amount;
        saveAccountsToFile();
        saveTransaction(sender->data.accNo, sender->data.name,
                        "Other Bank Transfer", amount);

        printf("Transfer successful.\nThank you for banking with us - BRM Bank\n");
    }
}

/* ================= Mobile Banking ================= */
void mobileBankingTransfer(struct Node *sender) {
    int choice;
    char phone[15];
    float amount;

    printf("1. bKash\n2. Rocket\n3. Nagad\n4. Upay\nChoice: ");
    scanf("%d", &choice);

    if (choice == 2)
        printf("Enter 12 digit Phone Number: ");
    else
        printf("Enter 11 digit Phone Number: ");

    scanf("%s", phone);

    printf("Enter Amount: ");
    scanf("%f", &amount);

    if (sender->data.balance < amount) {
        printf("Insufficient balance.\n");
        return;
    }

    sender->data.balance -= amount;
    saveAccountsToFile();
    saveTransaction(sender->data.accNo, sender->data.name,
                    "Mobile Banking Transfer", amount);

    printf("Transfer successful.\nThank you for banking with us - BRM Bank\n");
}


/* ================= Transaction History (1 Hour) ================= */
void showTransactions() {
    FILE *fp = fopen(TRANSACTION_FILE, "r");
    char line[300], timeStr[30];
    int found = 0;

    if (!fp) {
        printf("No transaction history.\n");
        return;
    }

    printf("\nLast 1 Hour Transactions:\n");

    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "Time: %[^|]", timeStr);

        if (isWithinOneHour(timeStr)) {
            printf("%s", line);
            found = 1;
        }
    }

    if (!found)
        printf("No transactions in last 1 hour.\n");

    fclose(fp);
}

/* ================= Balance ================= */
void checkBalance(struct Node *acc) {
    printf("\nAccount: %d\nName: %s\nBalance: %.2f\n",
           acc->data.accNo, acc->data.name, acc->data.balance);
}

/* ================= Main ================= */
int main() {
    int choice;
    struct Node *current = NULL;

    loadAccountsFromFile();

    while (1) {
        printf("\n1. Create Account\n2. Login\n3. Exit\nChoice: ");
        scanf("%d", &choice);

        if (choice == 1)
            createAccount();
        else if (choice == 2) {
            if (login(&current)) {
                int ch;
                while (1) {
                    printf(
                        "\n1.Deposit\n2.Withdraw\n3.Fund Transfer\n4.Check Balance\n5.Transaction History\n6.Logout\nChoice: ");
                    scanf("%d", &ch);

                    if (ch == 1) deposit(current);
                    else if (ch == 2) withdrawMoney(current);
                    else if (ch == 3) fundTransfer(current);
                    else if (ch == 4) checkBalance(current);
                    else if (ch == 5) showTransactions();
                    else if (ch == 6) break;
                }
            }
        }
        else if (choice == 3)
            exit(0);
    }
}


