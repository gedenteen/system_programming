#include <stdio.h>
#include <string.h>
#include <malloc.h> //для правильного malloc
#include <stdlib.h> //для exit()

struct subscriber { //абонент
    char firstname[20]; //имя
    char lastname[20]; //фамилия
    char phone[20]; //телефон
};

///функции:
void Phonebook_init(struct subscriber **p_book, int *cnt_records);
void Phonebook_print(struct subscriber *p_book, int cnt_records);
void Phonebook_add(struct subscriber **p_book, int *cnt_records);
int Phonebook_delete(struct subscriber **p_book, int *cnt_records);
int Phonebook_search(struct subscriber *p_book, int cnt_records);

int main() {
    struct subscriber* p_book = NULL; ///указатель, по которому хранятся записи
    int cnt_records = 0; ///количество записей в книге
    Phonebook_init(&p_book, &cnt_records);
    printf("\t __test__ cnt_records = %d \n", cnt_records);
    printf("\t __test__ sizeof(p_book) = %ld \n", sizeof(p_book));
    printf("\t __test__ firstname = %s \n", p_book[0].firstname);
    ///меню с выбором команд:
    int ans = 0;
    while (ans != 5) {
        printf("\nwhat to do? \n");
        printf("1. display phone book \n");
        printf("2. add new record \n");
        printf("3. delete record by index \n");
        printf("4. search record \n");
        printf("5. exit \n");

        scanf("%d", &ans);//fgets(ans, 1, stdin);
        while(ans < 1 || ans > 5) {
            printf("wrong number of command \n");
            scanf("%d", &ans);
        }

        switch (ans) {
        case 1:
            Phonebook_print(p_book, cnt_records);
            break;
        case 2:
            Phonebook_add(&p_book, &cnt_records);
            break;
        case 3:
            Phonebook_delete(&p_book, &cnt_records);
            break;
        case 4:
            Phonebook_search(p_book, cnt_records);
            break;
        default:
            break;
        }
    }

    free(p_book);
    return 0;
}

///функции:
///задать начальные записи для справочника:
void Phonebook_init(struct subscriber **p_book, int *cnt_records) {
    *p_book = (struct subscriber*) malloc(2 * sizeof(struct subscriber));
    if (*p_book == NULL) {
        perror("Error in Pronebook_init() with malloc \n");
        exit(1);
    }
    strcpy((*p_book)[0].firstname, "Anton");
    strcpy((*p_book)[0].lastname, "Antonov");
    strcpy((*p_book)[0].phone, "8-999-888-77-66");
    strcpy((*p_book)[1].firstname, "Vladimir");
    strcpy((*p_book)[1].lastname, "Antonov");
    strcpy((*p_book)[1].phone, "8-123-456-78-90");
    *cnt_records = 2;

    printf("\t __test__ firstname = %s \n", (*p_book)[0].firstname);
    printf("\t __test__ firstname = %s \n", (*p_book)[1].firstname);
}

///1. вывести все "непустые" записи справочника:
void Phonebook_print(struct subscriber *p_book, int cnt_records) {
    for (int i = 0; i < cnt_records; i++) {
        printf("subscriber %d: \n", i);
        printf("\t firstname = %s \n", p_book[i].firstname);
        printf("\t lastname = %s \n", p_book[i].lastname);
        printf("\t phone = %s \n", p_book[i].phone);
    }
}

///2. добавить запись через stdin:
void Phonebook_add(struct subscriber **p_book, int *cnt_records) {
    (*cnt_records)++;
    *p_book = (struct subscriber*)
        realloc(*p_book, *cnt_records * sizeof(struct subscriber));
    if (*p_book == NULL) {
        perror("Error in Phonebook_add() with realloc \n");
        exit(1);
    }

    printf("enter firstname: ");
    scanf("%s", (*p_book)[*cnt_records -1].firstname);
    printf("enter lastname: ");
    scanf("%s", (*p_book)[*cnt_records -1].lastname);
    printf("enter phone number: ");
    scanf("%s", (*p_book)[*cnt_records -1].phone);
}

///3. удалить запись по индексу:
int Phonebook_delete(struct subscriber **p_book, int *cnt_records) {
    printf("enter index of deleting record: ");
    int ind;
    scanf("%d", &ind);
    if (ind < 0 || ind >= *cnt_records) {
        printf("Error in Phonebook_delete(): wrong index");
        exit(1);
    }
    ///передвинуть удаляемую запись в конец:
    struct subscriber str_temp = (*p_book)[*cnt_records -1];
    (*p_book)[*cnt_records - 1] = (*p_book)[ind];
    (*p_book)[ind] = str_temp;
    ///освободить память под последнюю запись:
    (*cnt_records)--;
    *p_book = (struct subscriber*)
        realloc(*p_book, *cnt_records * sizeof(struct subscriber));
    if (*p_book == NULL) {
        perror("Error in Phonebook_delete() with realloc \n");
        exit(1);
    }
    return 0;
}

///4. поиск по фамилии или телефону:
int Phonebook_search(struct subscriber *p_book, int cnt_records) {
    printf("search by lastname or phone? l/p ");
    char type;
    scanf("\n%c", &type);
    if (type != 'l' && type != 'p') {
        printf("Error in Phonebook_search(): wrong value \n");
        return 1;
    }

    printf("enter string for searching: ");
    char str[20];
    scanf("%s", str);

    int records_found = 0; ///сколько записей найдено
    if (type == 'l') { ///если поиск по фамилии
        for (int i = 0; i < cnt_records; i++) {
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
        for (int i = 0; i < cnt_records; i++) {
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

