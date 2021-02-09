#include <stdio.h>
#include <string.h>

#define N 10 //максимальный размер справочника

struct subscriber { //абонент
    char firstname[20]; //имя
    char lastname[20]; //фамилия
    char phone[20]; //телефон
};

///функции:
void Phonebook_init(struct subscriber p_book[]);
void Phonebook_print(struct subscriber p_book[]);
void Phonebook_add(struct subscriber p_book[]);
int Phonebook_delete(struct subscriber p_book[]);
int Phonebook_search(struct subscriber p_book[]);

int main() {
    struct subscriber p_book[N]; //справочник
    //Phonebook_print(p_book); //здесь в справочнике хранится мусор
    Phonebook_init(p_book);
    ///меню с выбором команд:
    int ans = 0;
    while (ans != 5) {
        printf("\nwhat to do? \n");
        printf("1. display phone book \n");
        printf("2. add new record \n");
        printf("3. delete record by index \n");
        printf("4. search record \n");
        printf("5. exit \n");

        //fgets(ans, 10, stdin);
        scanf("%d", &ans);//fgets(ans, 1, stdin);
        while(ans < 1 || ans > 5) {
            printf("wrong number of command \n");
            scanf("%d", &ans);
        }

        switch (ans) {
        case 1:
            Phonebook_print(p_book);
            break;
        case 2:
            Phonebook_add(p_book);
            break;
        case 3:
            Phonebook_delete(p_book);
            break;
        case 4:
            Phonebook_search(p_book);
            break;
        default:
            break;
        }
    }

    return 0;
}

///функции:
///задать начальные записи для справочника:
void Phonebook_init(struct subscriber p_book[]) {
    strcpy(p_book[0].firstname, "Anton");
    strcpy(p_book[0].lastname, "Antonov");
    strcpy(p_book[0].phone, "8-999-888-77-66");
    strcpy(p_book[1].firstname, "Vladimir");
    strcpy(p_book[1].lastname, "Antonov");
    strcpy(p_book[1].phone, "8-123-456-78-90");
    ///остальные записи инициализровать как пустые:
    for (int i = 2; i < N; i++) {
        strcpy(p_book[i].firstname, "0");
        strcpy(p_book[i].lastname, "0");
        strcpy(p_book[i].phone, "0");
    }
}

///1. вывести все "непустые" записи справочника:
void Phonebook_print(struct subscriber p_book[]) {
    for (int i = 0; i < N; i++) {
        if (p_book[i].firstname[0] == '0') ///если записи нет
            continue;
        printf("subscriber %d: \n", i);
        printf("\t firstname = %s \n", p_book[i].firstname);
        printf("\t lastname = %s \n", p_book[i].lastname);
        printf("\t phone = %s \n", p_book[i].phone);
    }
}

///2. добавить запись через stdin:
void Phonebook_add(struct subscriber p_book[]) {
    for (int i = 0; i < N; i++) {
        if (p_book[i].firstname[0] == '0') {
            printf("enter firstname: ");
            scanf("%s", p_book[i].firstname);//fgets считывает \n, который вводится при выборе функции
            printf("enter lastname: ");
            scanf("%s", p_book[i].lastname);
            printf("enter phone: ");
            scanf("%s", p_book[i].phone);
            break;
        }
    }
}

///3. удалить запись по индексу:
int Phonebook_delete(struct subscriber p_book[]) {
    printf("enter index of deleting record: ");
    int ind;
    scanf("%d", &ind);
    if (ind < 0 || ind >= N) {
        printf("wrong index, indexes can be from 0 to %d \n", N);
        return 1;
    }
    strcpy(p_book[ind].firstname, "0");
    strcpy(p_book[ind].lastname, "0");
    strcpy(p_book[ind].phone, "0");
    return 0;
}

///4. поиск по фамилии или телефону:
int Phonebook_search(struct subscriber p_book[]) {
    printf("search by lastname or phone? l/p ");
    char type;
    scanf("\n%c", &type);
    if (type != 'l' && type != 'p') {
        printf("wrong value \n");
        return 1;
    }

    printf("enter string for searching: ");
    char str[20];
    scanf("%s", str);

    int records_found = 0; ///сколько записей найдено
    if (type == 'l') { ///если поиск по фамилии
        for (int i = 0; i < N; i++) {
            if (strcmp(p_book[i].lastname, str) == 0) {
                printf("subscriber %d: \n", i);
                printf("\t firstname = %s \n", p_book[i].firstname);
                printf("\t lastname = %s \n", p_book[i].lastname);
                printf("\t phone = %s \n", p_book[i].phone);
                records_found++;
            }
        }
    }
    else { ///иначе поиск по телефону
        for (int i = 0; i < N; i++) {
            if (strcmp(p_book[i].phone, str) == 0) {
                printf("subscriber %d: \n", i);
                printf("\t firstname = %s \n", p_book[i].firstname);
                printf("\t lastname = %s \n", p_book[i].lastname);
                printf("\t phone = %s \n", p_book[i].phone);
                records_found++;
            }
        }
    }

    if (records_found == 0) {
        printf("records not found \n");
    }
    return 0;
}
