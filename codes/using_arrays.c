#include<stdio.h>
#include<string.h>

#define MAX_USERS 1000
#define MAX_EXPENSES 1000
#define MAX_NO_OF_FAMALIES 100
#define MAX_NO_OF_INDIVIDUALS 4
#define MAX_EXPENSE_CATEGORY_SIZE 15
#define SUCCESS 1
#define FAILURE 0

static int users_count = 0;
int expense_count = 0;
int family_id_count = 0;

struct individual{
    int user_ID;//FRom 1 TILL 1000
    char user_name[100]; //A name doesn't contain more than 100 letters
    float income; //Income in decimal
};

struct family{
    int family_ID;
    char family_name[100]; //A name family doesn't exceed more than 100 letters
    int total_members; //NO. of members in the family
    int family_members_user_id[4]; //Max members In a Family is 4, the user_ids in the form of an array
    float total_family_income;
    float total_family_expense;
};

struct dates{
    unsigned int date; //unsigned int meaning a date/month/year is always positive
    unsigned int month;
    unsigned int year;
};

struct expense{
    int expense_ID;
    int user_ID;
    char expense_category[MAX_EXPENSE_CATEGORY_SIZE]; // THe max. no. of Categories Are 5, i.e, Rent, Utility, GRocery, STationary & Leisure
    float expense_amount;
    struct dates date_of_expense;
};

struct daily_expense{
    struct dates date;
    float total_expense;
};

struct category_expense{
    char category_name[15];
    float expense;
};

struct family_expense_cat_name{
    int user_id;
    float expense;
};

int AddUser(struct individual users[], int users_count, int user_id, float income, char name[],int size) {
    int status_code;
    if (users_count >= size || users_count < 0) {
        printf("The User COUnt is INValid, it can't cross the size\n");
        status_code = FAILURE;
    } else {
        int found = 0;
        for (int i = 0; i < users_count; i++) {
            if (users[i].user_ID == user_id) {
                found = 1;
            }
        }

        if (found == 0) {
            users[users_count].user_ID = user_id; 
            users[users_count].income = income;
            strcpy(users[users_count].user_name, name); 
            users_count++; 
            status_code = SUCCESS;
        } else {
            printf("THere already EXists a User with USer ID %d", user_id);
            status_code = FAILURE;
        }
    }

    return status_code;
}

int AddExpense(struct expense expenses[], struct individual users[], int size, int expense_id, int user_id, float expense_amount, char expenses_category[], struct dates date){
    int status_code = SUCCESS;
    if(expense_count >= MAX_EXPENSES){
        status_code = FAILURE;
    }else{
        if(user_id < 1 || user_id >1000){
            printf("User ID is Invalid\n");
            status_code = FAILURE;
        }else{
            int found = 0;
            for (int i = 0; i < size; i++) {
                if (users[i].user_ID == user_id) {
                    found = 1;
                }
            }
            if(found == 0){
                printf(" User ID Is INValid, there is no such USer\n");
                status_code = FAILURE;
            }else{
                expenses[expense_count].expense_ID = expense_id;
                expenses[expense_count].expense_amount = expense_amount;
                expenses[expense_count].user_ID = user_id;
                strcpy(expenses[expense_count].expense_category, expenses_category);
                expenses[expense_count].date_of_expense = date;
                expense_count = expense_count + 1;
            }
        }
    }
    return status_code;
}

int CreateFamily(struct individual user[], int user_count, struct family families[], int size, char family_name[], int family_id, int family_user_id[], int total_members) {
    int status_code = SUCCESS;
    if (size > MAX_NO_OF_FAMALIES) {  
        printf("YOu Have ENtered WROng SIze, the size SHouldn't exceed 10 \n");
        status_code = FAILURE;
    } else {
        if (total_members > 4 || total_members < 1) {
            status_code = FAILURE;
            printf("A Family can ONLy conatin max. 4 members & min. 1 member\n");
        } else {
            int count = 0;
            for (int i = 0; i < total_members; i++) {
                for (int j = 0; j < user_count; j++) {
                    if (user[j].user_ID == family_user_id[i]) { 
                        count++;
                    }
                }
            }
            if (count == total_members) {
                int found = 0, i = 0;
                while (found == 0 && i < size) {
                    if (families[i].family_ID == family_id) {
                        found = 1;
                    } else {
                        i++;
                    }
                }
                if (found == 1) {
                    printf("Family ID already exists\n");
                    status_code = FAILURE;
                } else {
                    families[size].family_ID = family_id;
                    families[size].total_members = total_members;
                    strcpy(families[size].family_name, family_name);
                    for (int i = 0; i < total_members; i++) {
                        families[size].family_members_user_id[i] = family_user_id[i];
                    }
                }
            }
        }
    }

    return status_code;
}

int update_individual_details(struct individual user[], int *current_users, struct family families[], int current_family_count, int user_id, float income, char name[]){
    int found = -1;
    int status_code = SUCCESS;
    int i = 0;
    for(; i < *current_users && found == -1; i++){
        if(user[i].user_ID == user_id){
            found = i;
        }
    }
    if((found != -1) && (found < *current_users)){
        float temp_income = user[found].income;
        user[found].income = income;
        strcpy(user[found].user_name, name);
        for(int i = 0; i < current_family_count; i++){
            for(int j = 0; j < families[i].total_members; j++){
                if(families[i].family_members_user_id[j] == user_id){
                    families[i].total_family_income += (income - temp_income);
                }
            }
        }
    }else{
        int is_free_loc = -1;
        for(int i = 0; i < MAX_USERS && is_free_loc == -1; i++){
            if((user[i].income == -1) && (user[i].user_ID == -1) && (user[i].user_name[0] == '\0')){
                is_free_loc = i;
            }
        }
        if(is_free_loc < MAX_USERS){
            user[is_free_loc].user_ID = user_id;
            user[is_free_loc].income = income;
            strcpy(user[is_free_loc].user_name, name);
            (*current_users) += 1;
        }else {
            status_code = FAILURE;
        }
    }
    return status_code;
}

int delete_individual_details(struct individual user[], int *current_users, struct family families[], int *current_family_count, struct expense expenses[], int *current_expenses, int user_id){
    int found = -1;
    int status_code = SUCCESS;
    int i = 0;
    for(; i < *current_users && found == -1; i++){
        if(user[i].user_ID == user_id){
            found = i;
        }   
    }
    if((found != -1) && (found < *current_users)){
        float temp_income = user[found].income;
        user[found].user_ID = -1;
        user[found].income = -1;
        user[found].user_name[0] = '\0';
        for(int i = 0; i < *current_family_count; i++){
            int is_done = 0;
            for(int j = 0; j < families[i].total_members && is_done == 0; j++){
                if(families[i].family_members_user_id[j] == user_id){
                    families[i].total_family_income -= temp_income;
                    families[i].family_members_user_id[j] = -1;
                }
                for (int k = j; k < families[i].total_members - 1; k++) {
                    families[i].family_members_user_id[k] = families[i].family_members_user_id[k + 1];
                }
                families[i].total_members -= 1;
                families[i].family_members_user_id[families[i].total_members] = -1;
                is_done = 1;
                if(families[i].total_members == 0){
                    families[i].family_ID = -1;
                    families[i].family_name[0] = '\0';
                    families[i].total_members = 0;
                    families[i].total_family_income = 0;
                    families[i].total_family_expense = 0;
                    families[i].family_members_user_id[families[i].total_members] = -1;
                    for(int l = i; l < *current_family_count - 1; l++){
                        families[l].family_ID = families[l+1].family_ID;
                        strcpy(families[l].family_name, families[l+1].family_name);
                        families[l].total_members = families[l+1].total_members;
                        families[l].total_family_income = families[l+1].total_family_income;
                        families[l].total_family_expense = families[l+1].total_family_expense;
                        for(int m = 0; m < MAX_NO_OF_INDIVIDUALS; m++){
                            families[l].family_members_user_id[m] = families[l+1].family_members_user_id[m];
                        }
                    }
                    families[*current_family_count - 1].family_ID = -1;
                    families[*current_family_count - 1].family_name[0] = '\0';
                    families[*current_family_count - 1].total_members = 0;
                    families[*current_family_count - 1].total_family_income = 0;
                    families[*current_family_count - 1].total_family_expense = 0;
                    for (int j = 0; j < MAX_NO_OF_INDIVIDUALS; j++) {
                        families[*current_family_count - 1].family_members_user_id[j] = -1;
                    }
                    *(current_family_count) -= 1;
                } 
            }
        }
        for(int i = 0; i < *current_expenses; i++){
            if(expenses[i].user_ID == user_id){
                expenses[i].user_ID = -1;
                expenses[i].expense_ID = -1;
                expenses[i].expense_category[0] = '\0';
                expenses[i].expense_amount = 0;
                expenses[i].date_of_expense.date = 0;
                expenses[i].date_of_expense.month = 0;
                expenses[i].date_of_expense.year = 0;
                for (int j = i; j < *current_expenses - 1; j++) {
                    expenses[j].user_ID = expenses[j + 1].user_ID;
                    expenses[j].expense_ID = expenses[j+1].expense_ID;
                    strcpy(expenses[j].expense_category, expenses[j+1].expense_category);
                    expenses[j].expense_amount = expenses[j+1].expense_amount;
                    expenses[j].date_of_expense.date = expenses[j+1].date_of_expense.date; 
                    expenses[j].date_of_expense.month = expenses[j+1].date_of_expense.month;
                    expenses[j].date_of_expense.year = expenses[j+1].date_of_expense.year;
                }
                (*current_expenses)--;
                expenses[*current_expenses].user_ID = -1;
                expenses[*current_expenses].expense_ID = -1;
                expenses[*current_expenses].expense_category[0] = '\0';
                expenses[*current_expenses].expense_amount = 0;
                expenses[*current_expenses].date_of_expense.date = 0;
                expenses[*current_expenses].date_of_expense.month = 0;
                expenses[*current_expenses].date_of_expense.year = 0;
                i--; 
            }
        }
        for (int i = found; i < *current_users - 1; i++) {
            user[i].income = user[i + 1].income;
            user[i].user_ID = user[i + 1].user_ID;
            strcpy(user[i].user_name, user[i+1].user_name); 
        }
        user[*current_users - 1].user_ID = -1;
        user[*current_users - 1].income = -1;
        user[*current_users - 1].user_name[0] = '\0';
        (*current_users) -= 1;
    }else{
        printf("user not found\n");
        status_code = FAILURE;
    } 
    return status_code;
}

int update_family_details(struct family families[], int *current_family_count, int family_id, char family_name[], int user_id_arr[], int total_users){
    int found = -1;
    int status_code = SUCCESS;
    for(int i = 0; i < *current_family_count && found == -1; i++){
        if(families[i].family_ID == family_id){
            found = i;
        }
    }
    if(found != -1){
        strcpy(families[found].family_name, family_name);
        families[found].total_members = total_users;
        for(int i = 0; i < total_users; i++){
            families[found].family_members_user_id[i] = user_id_arr[i];
        }
    }else{
        int is_free_loc = 0;
        for(int i = 0; i < MAX_NO_OF_FAMALIES && is_free_loc == 0; i++){
            if(families[i].family_ID == -1){
                families[i].family_ID = family_id;
                families[i].total_members = total_users;
                strcpy(families[i].family_name, family_name);
                for(int j = 0; j < total_users; j++){
                    families[i].family_members_user_id[j] = user_id_arr[j]; 
                }
                is_free_loc = 1;
            }
        }
        if(is_free_loc == 1){
            (*current_family_count) += 1;
        }else{
            status_code = FAILURE; 
        }
    }
    return status_code;
}

int delete_family(struct family families[], int *current_family_count, struct individual user[], int *current_users, struct expense expenses[], int *current_expense, int fam_id){
    int found = -1;
    int status_code = SUCCESS;
    for(int i = 0; i < *current_family_count; i++){
        if(families[i].family_ID == fam_id){
            found = i;
        }
    }
    if(found != -1){
        int user_id_arr[MAX_NO_OF_INDIVIDUALS];
        int total_family_members = families[found].total_members;
        for(int i = 0; i < total_family_members; i++) {
            user_id_arr[i] = families[found].family_members_user_id[i];
        }
        for(int i = 0; i < total_family_members; i++) {
            int user_id = user_id_arr[i];
            int is_done = 0;
            for(int j = 0; j < *current_users && is_done == 0; j++) {
                if(user[j].user_ID == user_id) {
                    for(int k = j; k < *current_users - 1; k++) {
                        user[k].income = user[k + 1].income;
                        user[k].user_ID = user[k + 1].user_ID;
                        strcpy(user[k].user_name, user[k + 1].user_name); 
                    }
                    (*current_users)--;
                    is_done = 1;
                }
            }
        }
        for(int i = 0; i < total_family_members; i++) {
            int user_id = user_id_arr[i];
            for(int j = 0; j < *current_expense; j++) {
                if(expenses[j].user_ID == user_id) {
                    for(int k = j; k < *current_expense - 1; k++) {
                        expenses[k].user_ID = expenses[k+1].user_ID;
                        expenses[k].expense_ID = expenses[k+1].expense_ID;
                        strcpy(expenses[k].expense_category, expenses[k+1].expense_category);
                        expenses[k].expense_amount = expenses[k+1].expense_amount;
                        expenses[k].date_of_expense.date = expenses[k+1].date_of_expense.date; 
                        expenses[k].date_of_expense.month = expenses[k+1].date_of_expense.month;
                        expenses[k].date_of_expense.year = expenses[k+1].date_of_expense.year;
                    }
                    (*current_expense)--;
                    j--; 
                }
            }
        }
        float family_income = families[found].total_family_income;
        float family_expenses = families[found].total_family_expense;
        for(int i = found; i < *current_family_count - 1; i++) {
            families[i].family_ID = families[i+1].family_ID;
            strcpy(families[i].family_name, families[i+1].family_name);
            families[i].total_members = families[i+1].total_members;
            families[i].total_family_income = families[i+1].total_family_income;
            families[i].total_family_expense = families[i+1].total_family_expense;
            for(int m = 0; m < MAX_NO_OF_INDIVIDUALS; m++){
                families[i].family_members_user_id[m] = families[i+1].family_members_user_id[m];
            }
        }
        (*current_family_count)--;
    }else{
        status_code = FAILURE;
    }   
    return status_code;
}

int update_expense_details(struct expense expenses[], int *current_expenses, int user_id, int expense_id, float expense_amount, char name[], struct dates date){
    int status_code = SUCCESS;
    int found = -1;
    for(int i = 0; i < *current_expenses && found == -1; i++){
        if(expenses[i].user_ID == user_id && expenses[i].expense_ID == expense_id){
            found = i;
        }
    }
    if(found != -1){
        expenses[found].expense_amount = expense_amount;  
        strcpy(expenses[found].expense_category, name);  
        expenses[found].date_of_expense = date;           
    }else{
        int is_free_loc = 0;
        for(int i = 0; i < MAX_EXPENSES && is_free_loc == 0; i++) {
            if(expenses[i].expense_ID == -1) {  
                expenses[i].expense_ID = expense_id;
                expenses[i].user_ID = user_id;                   
                expenses[i].expense_amount = expense_amount;     
                strcpy(expenses[i].expense_category, name);      
                expenses[i].date_of_expense = date;               
                is_free_loc = 1;
            }
        }
        if(is_free_loc == 1) {
            (*current_expenses) += 1;
        }
        else{
            status_code = FAILURE;
        }
    }
    return status_code;
}

int delete_expense(struct expense expenses[], int *current_expenses, struct individual user[], int *current_users, struct family families[], int *current_family_count, int expense_id){
    int found = -1;
    int status_code = SUCCESS;
    for(int i = 0; i < *current_expenses && found == -1; i++) {
        if(expenses[i].expense_ID == expense_id) {
            found = i;
        }
    }
    if(found != -1) {
        int user_id = expenses[found].user_ID;
        float expense_amount = expenses[found].expense_amount;
        for(int i = 0; i < *current_family_count; i++) {
            int is_done = 0;
            for(int j = 0; j < families[i].total_members && is_done == 0; j++) {
                if(families[i].family_members_user_id[j] == user_id) {
                    families[i].total_family_expense -= expense_amount; 
                    is_done = 1;
                }
            }
        }
        for(int i = found; i < *current_expenses - 1; i++) {
            expenses[i].user_ID = expenses[i+1].user_ID;
            expenses[i].expense_ID = expenses[i+1].expense_ID;
            strcpy(expenses[i].expense_category, expenses[i+1].expense_category);
            expenses[i].expense_amount = expenses[i+1].expense_amount;
            expenses[i].date_of_expense.date = expenses[i+1].date_of_expense.date; 
            expenses[i].date_of_expense.month = expenses[i+1].date_of_expense.month;
            expenses[i].date_of_expense.year = expenses[i+1].date_of_expense.year;
        }
        (*current_expenses)--;
        expenses[*current_expenses].expense_ID = -1;
        expenses[*current_expenses].user_ID = -1;
        expenses[*current_expenses].expense_category[0] = '\0';
        expenses[*current_expenses].expense_amount = 0;
        expenses[*current_expenses].date_of_expense.date = 0;
        expenses[*current_expenses].date_of_expense.month = 0;
        expenses[*current_expenses].date_of_expense.year = 0;
    } else {
        status_code = FAILURE;
    }
    return status_code;
}

float get_total_expense(struct family fam){
    float monthly_expense = fam.total_family_expense;
    if(monthly_expense > fam.total_family_income){
        printf("THe EXpense LImit has been Exceeded BY %f\n", monthly_expense - fam.total_family_income);
    }
    return monthly_expense;
}

struct daily_expense max_expense_day_of_family(struct family fam, int total_members, struct expense expenses[], int current_expense){//family maxexpense day
    struct daily_expense expense_dates[MAX_EXPENSES];
    for(int i = 0; i < MAX_EXPENSES; i++){
        expense_dates[i].date.date = 0;
        expense_dates[i].date.month = 0;
        expense_dates[i].date.year = 0;
        expense_dates[i].total_expense = 0;
    }
    int daily_count = 0;
    for(int i = 0; i < total_members; i++){
        for(int j = 0; j < current_expense; j++){
            if(fam.family_members_user_id[i] == expenses[j].user_ID){
                int found = 0;
                for(int k = 0; (k < daily_count) && (found == 0); k++){
                    if((expense_dates[k].date.date == expenses[j].date_of_expense.date) && (expense_dates[k].date.month == expenses[j].date_of_expense.month) && (expense_dates[k].date.year == expenses[j].date_of_expense.year)){
                        expense_dates[k].total_expense += expenses[j].expense_amount;
                        found = 1;
                    }
                }
                if(found == 0){
                    if(daily_count < MAX_EXPENSES){
                        expense_dates[daily_count].date.date = expenses[j].date_of_expense.date;
                        expense_dates[daily_count].date.month = expenses[j].date_of_expense.month;
                        expense_dates[daily_count].date.year = expenses[j].date_of_expense.year;
                        expense_dates[daily_count].total_expense = expenses[j].expense_amount;
                        daily_count += 1;
                    }
                }
            }
        }
    }
    int max_expense = 0;
    struct daily_expense max_expense_date;
    for(int i = 0; i < daily_count; i++){
        if(max_expense < expense_dates[i].total_expense){
            max_expense = expense_dates[i].total_expense;
            max_expense_date.total_expense = expense_dates[i].total_expense;
            max_expense_date.date.date = expense_dates[i].date.date;
            max_expense_date.date.month = expense_dates[i].date.month;
            max_expense_date.date.year = expense_dates[i].date.year;
        }
    }
    return max_expense_date;
}

float get_individual_expense(struct individual person, struct expense expenses[], int current_expense, struct category_expense category_wise[]){
    float sum = 0;
    for(int i = 0; i < current_expense; i++){
        if(person.user_ID == expenses[i].user_ID){
            sum += expenses[i].expense_amount;
            if(strcmp(expenses[i].expense_category , "rent") == 0){
                category_wise[0].expense += expenses[i].expense_amount;
            }else if(strcmp(expenses[i].expense_category , "utility") == 0){
                category_wise[1].expense += expenses[i].expense_amount;
            }else if(strcmp(expenses[i].expense_category , "grocery") == 0){
                category_wise[2].expense += expenses[i].expense_amount;
            }else if(strcmp(expenses[i].expense_category , "stationary") == 0){
                category_wise[3].expense += expenses[i].expense_amount;
            }else if(strcmp(expenses[i].expense_category , "leisure") == 0){
                category_wise[4].expense += expenses[i].expense_amount;
            }
        }
    }
    return sum;
}

void sort_category_wise(struct category_expense category_wise[], int start, int end){
    for(int i = start+1; i <= end; i++){
        float comp = category_wise[i].expense;
        int j = i - 1;
        char temp_name[15];
        strcpy(temp_name, category_wise[i].category_name);
        while(j >= start && category_wise[j].expense < comp){
            category_wise[j+1].expense = category_wise[j].expense;
            strcpy(category_wise[j+1].category_name, category_wise[j].category_name);
            j = j - 1;
        }
        category_wise[j+1].expense = comp;
        strcpy(category_wise[j+1].category_name, temp_name);
    }
}

float get_categorical_expense(char category_name[], struct family fam, int current_family_count, struct expense expenses[], int current_expense, struct family_expense_cat_name fecn[]){
    float total_expense = 0;
    int size = fam.total_members;
    int fecn_index = 0;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < current_expense; j++){
            if(fam.family_members_user_id[i] == expenses[j].user_ID){
                if(strcmp(expenses[j].expense_category, category_name) == 0){
                    total_expense += expenses[j].expense_amount;
                    int exists = 0;
                    for(int k = 0; k < fecn_index && exists == 0; k++){
                        if(fecn[k].user_id == expenses[j].user_ID){
                            fecn[k].expense += expenses[j].expense_amount;
                            exists = 1;
                        }
                    }
                    if(!exists && fecn_index < size){
                        fecn[fecn_index].expense = expenses[j].expense_amount;
                        fecn[fecn_index].user_id = expenses[j].user_ID;
                        fecn_index++;
                    }
                }
            }
        }
    }
    return total_expense;
}

void sort_fecn(struct family_expense_cat_name fecn[], int start, int end){
    for(int i = start+1; i <= end; i++){
        float x = fecn[i].expense;
        int temp = fecn[i].user_id;
        int j = i - 1;
        while(j >= start && fecn[j].expense < x){
            fecn[j+1].expense = fecn[j].expense;
            fecn[j+1].user_id = fecn[j].user_id;
            j = j - 1;
        }
        fecn[j+1].expense = x;
        fecn[j+1].user_id = temp;
    }
}   

void initialize_user(struct individual user[], int size){
    for(int i = 0; i < size; i++){
        user[i].income = -1;
        user[i].user_ID = -1;
        user[i].user_name[0] = '\0';
    }
}

void initialize_family(struct family families[], int size) {
    for (int i = 0; i < size; i++) {
        families[i].family_ID = -1;
        families[i].family_name[0] = '\0';
        families[i].total_members = 0;
        families[i].total_family_income = 0;
        families[i].total_family_expense = 0;
        for (int j = 0; j < MAX_NO_OF_INDIVIDUALS; j++) {
            families[i].family_members_user_id[j] = -1;
        }
    }
}

void initialize_expenses(struct expense expenses[], int size){
    for(int i = 0; i < size; i++){
        expenses[i].user_ID = -1;
        expenses[i].expense_ID = -1;
        expenses[i].expense_category[0] = '\0';
        expenses[i].expense_amount = 0;
        expenses[i].date_of_expense.date = 0;
        expenses[i].date_of_expense.month = 0;
        expenses[i].date_of_expense.year = 0;
    }
}

void swap_user(struct individual user[], int start, int pivot){
    if(start != pivot){
        struct individual temp = user[start];
        user[start] = user[pivot];
        user[pivot] = temp;
    }
}

int partition_array_of_user(struct individual user[], int start, int end){
    int x = user[start].user_ID;
    int i = start + 1;
    int j = end;
    while(i <= j){
        while(i <= end && user[i].user_ID <= x){
            i++;
        }
        while(j >= start && user[j].user_ID > x){
            j--;
        }
        if(i < j){
            swap_user(user, i, j);
        }
    }
    swap_user(user, start, j);
    return j;
}

void sort_users_using_user_id(struct individual user[], int start, int end){
    if(start < end){
        int pivot = partition_array_of_user(user, start, end);
        sort_users_using_user_id(user, start, pivot-1);
        sort_users_using_user_id(user, pivot+1, end);
    }
}

void swap_expenses(struct expense expenses[], int i, int j){
    struct expense temp_expense = expenses[i];
    expenses[i] = expenses[j];
    expenses[j] = temp_expense;
}

int partition_array_of_expenses(struct expense expenses[], int start, int end){
    int x = expenses[start].user_ID;
    int i = start + 1;
    int j = end;
    while(i <= j){
        while(i <= end && expenses[i].user_ID <= x){
            i++;
        }
        while(j >= start && expenses[j].user_ID > x){
            j--;
        }
        if(i < j){
            swap_expenses(expenses, i, j);
        }
    }
    swap_expenses(expenses, start, j);
    return j;
}

void sort_expenses_by_user_id(struct expense expenses[], int start, int end){
    if(start < end){
        int pivot = partition_array_of_expenses(expenses, start, end);
        sort_expenses_by_user_id(expenses, start, pivot-1);
        sort_expenses_by_user_id(expenses, pivot+1, end);
    }
}
void sort_expenses_id_of_expenses(struct expense expenses[], int start, int end){
    for(int i = start+1; i <= end; i++){
        struct expense x = expenses[i];
        int j = i-1;
        while(j >= start && expenses[j].expense_ID > x.expense_ID){
            expenses[j+1] = expenses[j];
            j= j -1;
        }
        expenses[j+1] = x; 
    }
}

void sort_expenses(struct expense expenses[], int size){
    sort_expenses_by_user_id(expenses, 0, size-1);
    int i = 0;
    while(i < size){
        int x = expenses[i].user_ID;
        int j = i;
        while(j < size && expenses[j].user_ID == x){
            j++;
        }
        sort_expenses_id_of_expenses(expenses, i, j-1);
        i = j;
    }
}

float calculate_monthly_expense_of_family(struct family fam, int total_members, struct expense expenses[], int expense_count){
    float sum = 0;
    for(int i = 0; i < total_members; i++){
        for(int j = 0; j < expense_count; j++){
            if(expenses[j].user_ID == fam.family_members_user_id[i]){
                sum += expenses[j].expense_amount;
            }
        }
    }
    return sum;
}

float calculate_monthly_income_of_family(struct family fam, int total_members, struct individual user[], int current_user){
    float sum = 0;
    for(int i = 0; i < total_members; i++){
        for(int j = 0; j < current_user; j++){
            if(user[j].user_ID == fam.family_members_user_id[i]){
                sum += user[j].income;
            }
        }
    }
    return sum;
}

int main(){
    int input_users, total_expenses, total_families;
    do{
        printf("ENter THE Tot. No. OF Users : ");
        scanf("%d", &input_users);
    }while(input_users < 25);
    do{
        printf("Enter THe TOt. No. of EXpenses : ");
        scanf("%d", &total_expenses);
    }while(total_expenses < 40);
    do{
        printf("Enter THe TOt. No. Of Families : ");
        scanf("%d", &total_families);
    }while(total_families < 10);
    struct individual user[MAX_USERS];
    struct expense expenses[MAX_EXPENSES];
    struct family families[MAX_NO_OF_FAMALIES];
    initialize_user(user, MAX_USERS);
    initialize_expenses(expenses, MAX_EXPENSES);
    initialize_family(families, MAX_NO_OF_FAMALIES);
    int current_users = 0;
    int current_expenses = 0;
    int current_family_count = 0;
    int n = 1;
    while(n < 11 && n != 10 && n > 0){
        printf("Press 1  : To Add the User\n");
        printf("Press 2  : To Add the Expense\n");
        printf("Press 3  : TO Create The Family\n");
        printf("Press 4  : TO Update or Delete an Individual Family\n");
        printf("Press 5  : To Update Or Delete an Expense\n");
        printf("Press 6  : TO Get The Total Expense\n");
        printf("Press 7  : TO Get the EXpense of the Category\n");
        printf("Press 8  : TO Get THe Day OF HIghest EXpense\n");
        printf("Press 9  : TO Get THe Individual Expense\n");
        printf("Press 10 : For Exit\n");
        printf("Enter number out of 1 to 10 : ");
        scanf("%d", &n);
        switch(n){
            case 1 :{
                printf("Enter The FOllowing DEtails:-\n");
                int user_id;
                float income;
                char name[100];
                printf("Enter The User ID : ");
                scanf("%d",&user_id);
                printf("Enter THe INcome Of THe User : ");
                scanf("%f", &income);
                printf("Enter the Name of the Individual USer : ");
                scanf(" %s", name);
                while(user_id > 1000 || user_id < 1){
                    printf("You Have entered WRong input, it should be less than 100 and greater than 1. Enter AGain\n");
                    printf("Enter The User ID : ");
                    scanf("%d",&user_id);
                }
                while(income < 0){
                    printf("You Have ENtered WRong INput, it can't be negative.Enter again\n");
                    printf("Enter The INcome of the User : ");
                    scanf("%f", &income);
                }
                while(name[0] == '\0'){
                    printf("You Have Entered WROng INput, a name can't be empty. Enter AGain\n");
                    printf("Enter THe Name of THe INDIvidual User : ");
                    fgets(name, 100, stdin);
                }
                int status = AddUser(user, current_users, user_id, income, name,input_users);
                if(status == SUCCESS){
                    printf("THe User has been Added Successfully\n");
                    current_users = current_users + 1;
                    sort_users_using_user_id(user, 0, current_users-1);
                    for(int i=0;i<current_users;i++){
                        printf("%d %f %s \n",user[i].user_ID,user[i].income,user[i].user_name);
                    }
                }else{
                    printf("Error in adding the User\n");
                }

                break;
            }
            case 2 :{
                int user_id, expense_id;
                float expense_amount;
                char expense_category[MAX_EXPENSE_CATEGORY_SIZE];
                struct dates date;
                printf("Enter the User ID for the EXpense: ");
                scanf("%d", &user_id);
                printf("Enter the EXpense ID for the EXpense: ");
                scanf("%d", &expense_id);
                printf("Enter the AMount of the Expense: ");
                scanf("%f", &expense_amount);
                printf("Enter THe Category Of the EXpense: ");
                scanf("%s", expense_category);
                printf("Enter THe Date Of the EXpense in this Form (DD MM YYYY): ");
                scanf("%u %u %u", &date.date, &date.month, &date.year);

                int status = AddExpense(expenses, user, current_users, expense_id, user_id, expense_amount, expense_category, date);
                if (status == SUCCESS) {
                    printf("THe EXpense Has BEen added Sucessfully\n");
                    current_expenses += 1;
                    sort_expenses(expenses, current_expenses);
                    for(int i = 0 ; i < current_expenses; i++){
                        printf("%d %d %f %s\n", expenses[i].user_ID, expenses[i].expense_ID, expenses[i].expense_amount, expenses[i].expense_category);
                    }
                } else {
                    printf("ERROr in adding THe Expense\n");
                }
                break;
             }
            case 3 :{
                int family_id;
                int total_members; 
                printf("Enter THe Family ID: ");
                scanf("%d", &family_id);
                printf("Enter THe TOT. NO. of MEmbers IN the Family: ");
                scanf("%d", &total_members);
                int family_user_id[total_members];  
                for (int i = 0; i < total_members; i++) {
                    printf("Enter the User ID of the Family Member %d: ", i + 1);
                    scanf("%d", &family_user_id[i]);
                }
                char family_name[25]; 
                printf("Enter the Name of Family: ");
                scanf("%s", family_name);
                int status = CreateFamily(user, current_users, families, current_family_count, family_name, family_id, family_user_id, total_members);
                if (status == SUCCESS) {
                    printf("THe Family has been successfully CReated\n");
                    families[current_family_count].total_family_expense = calculate_monthly_expense_of_family(families[current_family_count], total_members, expenses, current_expenses);
                    families[current_family_count].total_family_income = calculate_monthly_income_of_family(families[current_family_count], total_members, user, current_users);
                    current_family_count++;
                    for (int i = 0; i < current_family_count; i++) {
                        printf("%s %d %d %f %f\n", families[i].family_name, families[i].family_ID, families[i].total_members, families[i].total_family_expense, families[i].total_family_income);
                    }
                } else {
                    printf("ERROR in creating the Family\n");
                }
                break;
            }
            case 4:{
                int j;
                printf("Press 1 : TO UPdate THe User\n");
                printf("Press 2 : To Update the Family\n");
                printf("Press 3 : To Delete the User\n");
                printf("Press 4 : To Delete the Family\n");
                printf("Enter number out of 1/2/3/4 : ");
                scanf("%d", &j);
                switch(j){
                    case 1:
                    {
                        int user_id;
                        float income;
                        char name[100];
                        printf("Enter The User ID : ");
                        scanf("%d",&user_id);
                        printf("Enter the current Income : ");
                        scanf("%f", &income);
                        printf("Enter the Name: ");
                        scanf("%s", name);
                        int found = 0;
                        for(int i = 0; i < current_users && found == 0; i++){
                            if(user[i].user_ID == user_id){
                                found = 1;
                            }
                        }
                        if(found == 1){
                            int status = update_individual_details(user, &current_users, families, current_family_count, user_id, income, name);
                            if(status == SUCCESS){
                                for(int i=0;i<current_users;i++){
                                    printf("%d %f %s \n",user[i].user_ID,user[i].income,user[i].user_name);
                                }
                                for(int i = 0; i < current_family_count; i++){
                                    families[i].total_family_income = calculate_monthly_income_of_family(families[i],families[i].total_members,user, current_users);
                                    families[i].total_family_expense = calculate_monthly_expense_of_family(families[i], families[i].total_members, expenses, current_expenses);
                                }
                            }else{
                                printf("THe process has FAILED\n");
                            }
                        }else{
                            printf("THe User Is NOT FOUND\n");
                        }
                        break;
                    }
                    case 2:{
                        int fam_id;
                        printf("Enter THe Family ID : ");
                        scanf("%d", &fam_id);
                        int total_users;
                        printf("Enter the Total No. Of Members : ");
                        scanf("%d", &total_users);
                        int user_id_arr[4];
                        for(int i = 0; i < total_users; i++){
                            printf("Enter THe User ID of the User %d : ",i+1);
                            scanf("%d", &user_id_arr[i]);
                        }
                        char family_name[100];
                        printf("Enter The Name OF Family : ");
                        scanf("%s", family_name);
                        int found = 0;
                        for(int i = 0; i < current_family_count && found == 0; i++){
                            if(families[i].family_ID == fam_id){
                                found = 1;
                            }
                        }
                        if(found != 0){
                            int status = update_family_details(families, &current_family_count, fam_id, family_name, user_id_arr, total_users);
                            if(status == SUCCESS){
                                for(int i = 0; i < current_family_count; i++){
                                    families[i].total_family_income = calculate_monthly_income_of_family(families[i],families[i].total_members,user, current_users);
                                    families[i].total_family_expense = calculate_monthly_expense_of_family(families[i], families[i].total_members, expenses, current_expenses);
                                }
                            }else{
                                printf("THe Process Has FAILED\n");
                            }
                        }else{
                            printf("THe Family ID is NOT FOUND\n");
                        }
                        break;
                    }
                    case 3:{
                        int user_id;
                        printf("Enter the User ID which you wanna Remove : ");
                        scanf("%d",&user_id);
                        int found = 0;
                        for(int i = 0; i < current_users && found == 0; i++){
                            if(user[i].user_ID == user_id){
                                found = 1;
                            }
                        }
                        if(found == 1){
                            int status = delete_individual_details(user, &current_users, families, &current_family_count, expenses, &current_expenses, user_id);
                            if(status == SUCCESS){
                                for(int i=0;i<current_users;i++){
                                    printf("%d %f %s \n",user[i].user_ID,user[i].income,user[i].user_name);
                                }
                                printf("\n");
                                for(int i = 0; i < current_family_count; i++){
                                    families[i].total_family_income = calculate_monthly_income_of_family(families[i],families[i].total_members,user, current_users);
                                    families[i].total_family_expense = calculate_monthly_expense_of_family(families[i], families[i].total_members, expenses, current_expenses);
                                }
                                for (int i = 0; i < current_family_count; i++) {
                                    printf("%s %d %d %f %f\n", families[i].family_name, families[i].family_ID, families[i].total_members, families[i].total_family_expense, families[i].total_family_income);
                                }
                            }else{
                                printf("THe Process has FAILED\n");
                            }
                        }else{
                            printf("The User ID DOESN'T EXIST\n");
                        }
                        break;
                    }
                    case 4:{
                        int fam_id;
                        printf("Enter the FAmily ID which you wanna DElete : ");
                        scanf("%d", &fam_id);
                        int found = 0;
                        for(int i = 0; i < current_family_count; i++){
                            if(families[i].family_ID == fam_id){
                                found = 1;
                            }
                        }
                        if(found == 1){
                            int status = delete_family(families, &current_family_count, user, &current_users, expenses, &current_expenses, fam_id);
                            if(status == SUCCESS){
                                 for(int i=0;i<current_users;i++){
                                    printf("%d %f %s \n",user[i].user_ID,user[i].income,user[i].user_name);
                                }
                                printf("\n");
                                for(int i = 0; i < current_family_count; i++){
                                    families[i].total_family_income = calculate_monthly_income_of_family(families[i],families[i].total_members,user, current_users);
                                    families[i].total_family_expense = calculate_monthly_expense_of_family(families[i], families[i].total_members, expenses, current_expenses);
                                }
                                for (int i = 0; i < current_family_count; i++) {
                                    printf("%s %d %d %f %f\n", families[i].family_name, families[i].family_ID, families[i].total_members, families[i].total_family_expense, families[i].total_family_income);
                                }
                            }else{
                                printf("The Process has FAILED\n");
                            }
                        }else{
                            printf("THe Family ID is NOT FOUND\n");
                        }
                    }
                    break;
                }
                break;
            }
            case 5:{
                int p;
                printf("Press 1 : To Update the Expenses\n");
                printf("Press 2 : To Delete the Expenses\n");
                printf("Enter number 1 or 2 : ");
                scanf("%d", &p);
                switch(p){
                    case 1:
                    {
                        int user_id;
                        int expense_id;
                        printf("Enter The User ID : ");
                        scanf("%d", &user_id);
                        printf("ENter THe Expense ID : ");
                        scanf("%d", &expense_id);
                        char name[100];
                        printf("ENter the Name of the CAtegory : ");
                        scanf("%s", name);
                        struct dates date;
                        printf("Enter The Date : ");
                        scanf("%u %u %u", &date.date, &date.month, &date.year);
                        float expense_amount;
                        printf("Enter the Expense AMount : ");
                        scanf("%f", &expense_amount);
                        int found = 0;
                        for(int i = 0; i < current_expenses && found == 0; i++){
                            if(expenses[i].user_ID == user_id && expenses[i].expense_ID == expense_id){
                                found = 1;
                            }
                        }
                        if(found == 1){
                            int status = update_expense_details(expenses, &current_expenses, user_id, expense_id, expense_amount, name, date);
                            if(status == SUCCESS){
                                for(int i = 0; i < current_family_count; i++){
                                    families[i].total_family_income = calculate_monthly_income_of_family(families[i],families[i].total_members,user, current_users);
                                    families[i].total_family_expense = calculate_monthly_expense_of_family(families[i], families[i].total_members, expenses, current_expenses);
                                }
                                for(int i = 0 ; i < current_expenses; i++){
                                    printf("%d %d %f %s\n", expenses[i].user_ID, expenses[i].expense_ID, expenses[i].expense_amount, expenses[i].expense_category);
                                }
                            }else{
                                printf("The Process has Failed\n");
                            }
                        }else{
                            printf("EXpense is NOT FOUND\n");
                        }
                        break;
                    }
                
                    case 2:{
                        int expense_id;
                        printf("Enter THe expense ID which you wanna Remove : ");
                        scanf("%d", &expense_id);
                        int found = 0;
                        for(int i = 0; i < current_expenses && found == 0; i++){
                            if(expenses[i].expense_ID == expense_id){
                                found = 1;
                            }
                        }
                        if(found == 1){
                            int status = delete_expense(expenses, &current_expenses, user, &current_expenses, families, &current_family_count, expense_id);
                            if(status == SUCCESS){
                                for(int i = 0; i < current_family_count; i++){
                                    families[i].total_family_income = calculate_monthly_income_of_family(families[i],families[i].total_members,user, current_users);
                                    families[i].total_family_expense = calculate_monthly_expense_of_family(families[i], families[i].total_members, expenses, current_expenses);
                                }
                                for(int i = 0 ; i < current_expenses; i++){
                                    printf("%d %d %f %s\n", expenses[i].user_ID, expenses[i].expense_ID, expenses[i].expense_amount, expenses[i].expense_category);
                                }
                            }else{
                                printf("The process has FAILED\n");
                            }
                        }else{
                            printf("EXpense is NOT FOUND\n");
                        }
                        break;
                    }
                }
                break;
            }
            case 6:
            {
                struct family fam;
                int fam_id;
                printf("Enter The Family ID : ");
                scanf("%d", &fam_id);
                int found = 0;
                for(int i = 0; i < current_family_count && found == 0; i++){
                    if(fam_id == families[i].family_ID){
                        found = 1;
                        fam = families[i];
                    }
                }
                if(found == 1){
                    float ans = get_total_expense(fam);
                    printf("THe TOtal EXpenses for the MOnth : %f\n", ans);
                }else{
                    printf("Family NOT FOUND\n");
                }
                break;
            }
            case 7:
            {
                char cat_name[15];
                printf("Enter THE Category Name : ");
                scanf("%s", cat_name);
                struct family fam;
                int fam_id;
                printf("Enter The Family ID : ");
                scanf("%d", &fam_id);
                int found = 0;
                for(int i = 0; i < current_family_count && found == 0; i++){
                    if(fam_id == families[i].family_ID){
                        found = 1;
                        fam = families[i];
                    }
                }
                if(found == 1){
                    int s = fam.total_members;
                    struct family_expense_cat_name fecn[s];
                    for(int i = 0; i < s; i++){
                        fecn[i].expense = 0;
                        fecn[i].user_id = 0;
                    }
                    float total_expense = get_categorical_expense(cat_name, fam, current_family_count, expenses, current_expenses, fecn);
                    printf("The Total Expense of THe Family on %s is %f\n", cat_name, total_expense);
                    sort_fecn(fecn, 0, s-1);
                    for(int i = 0; i < s; i++){
                        printf("User %d : %f\n",fecn[i].user_id, fecn[i].expense);
                    }
                }else{
                    printf("NO SUCH Family FOund\n");
                }   
                break;
            }
            case 8 : 
            {
                int family_id;
                printf("Enter the Family Id of family whose Maximum EXpense you wanna check : ");
                scanf("%d", &family_id);
                struct family fam;
                int found = 0;
                for(int i = 0; i < current_family_count && found == 0; i++){
                    if(families[i].family_ID == family_id){
                        fam = families[i];
                        found = 1;
                    }
                }
                if(found == 1){
                    struct daily_expense max_date = max_expense_day_of_family(fam, fam.total_members, expenses, current_expenses);
                    printf("THe Maximum EXpense Day : ");
                    printf("%d %d %d\n", max_date.date.date, max_date.date.month, max_date.date.year);
                    printf("The Maximum Expense : %f\n", max_date.total_expense);
                }else{
                    printf("The Family Is NOT FOUND\n");
                }
                break;
            }
            case 9 :
            {
                struct category_expense category_wise[5];
                strcpy(category_wise[0].category_name , "rent");
                strcpy(category_wise[1].category_name , "utility");
                strcpy(category_wise[2].category_name , "grocery");
                strcpy(category_wise[3].category_name , "stationary");
                strcpy(category_wise[4].category_name , "leisure");
                for(int i = 0; i < 5; i++){
                    category_wise[i].expense = 0;
                }
                int user_id;
                printf("Enter the User ID of user whose Monthly expense YOu want to KNow :  ");
                scanf("%d", &user_id);
                struct individual person;
                int found = 0;
                for(int i = 0; i < current_users && found == 0; i++){
                    if(user[i].user_ID == user_id){
                        person = user[i];
                        found = 1;
                    }
                }
                if(found == 1){
                    float total_expense = get_individual_expense(person, expenses, current_expenses, category_wise);
                    sort_category_wise(category_wise, 0, 4);
                    printf("The Total Monthly Expense : %f\n",total_expense);
                    for(int i = 0; i < 5; i++){
                        printf("%s : %f\n",category_wise[i].category_name, category_wise[i].expense);
                    }
                }else{
                    printf("The User is NOT FOUND\n");    
                }
                break;
            }
        }
    }
   return 0;
}
