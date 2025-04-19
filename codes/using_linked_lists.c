#include<stdio.h>
#include <stdlib.h>
#include<string.h>

typedef enum {FAILURE,SUCCESS} status_code;

typedef enum {False,True} Boolean;
typedef struct date_tag{
	int day;
	int month;
	int year;
}Date ;

typedef struct user{
	int id;
	char name[50];
	float income;
	struct user *next;
}User;
typedef struct expense {
	int expense_id;
	int user_id;
	float expense_amount;
	char category[10];
	Date date;
	struct expense *next;
}Expense;
typedef struct family{
	int family_id;
	char family_name[50];
	User* members[4];
    int No_members;
	float family_income;
	float family_expense;
	struct family *next;
	
}Family;

Boolean FindUser(User *userList,int user_id){

    Boolean ret_val=False;
    int flag=0;
    while(!(flag)){
        User*userptr=userList;
    while(!(flag) && userptr!=NULL){
        if(userptr->id==user_id){
            ret_val=True;
        }
        userptr=userptr->next;
    }
    }
    return ret_val;
}


Family* FindFamily(Family *familyList,int family_id){
    Family* ret_val=NULL;
    while(ret_val==NULL && familyList!=NULL){
        if(familyList->family_id==family_id){
            ret_val=familyList;
        }
        familyList=familyList->next;
    }
    return ret_val;
}
Boolean FindExpense(Expense *expenseList,int expense_id){
    Boolean ret_val=False;
    while(ret_val==False && expenseList!=NULL){
        if(expenseList->expense_id==expense_id){
            ret_val=True;
        }
        expenseList=expenseList->next;
    }
    return ret_val;
}


// Define structur

status_code AddExpense2(Expense** expenseList, Family* familyList, Expense* newExpense) {
    // Insert expense in sorted order
    if (*expenseList == NULL || (*expenseList)->expense_id > newExpense->expense_id) {
        newExpense->next = *expenseList;
        *expenseList = newExpense;
    } else {
        Expense *trail = *expenseList;
        while (trail->next != NULL && trail->next->expense_id < newExpense->expense_id) {
            trail = trail->next;
        }
        newExpense->next = trail->next;
        trail->next = newExpense;
    }
    return SUCCESS;
}

// Function prototypes
void Initialize_DB(User** userList, Family** familyList, Expense** expenseList);
status_code AddExpense2(Expense** expenseList, Family* familyList, Expense* newExpense);

// Main Initialize_DB function
void Initialize_DB(User** userList, Family** familyList, Expense** expenseList) {
    *userList = NULL;
    *familyList = NULL;
    *expenseList = NULL;

    FILE *userFile, *familyFile, *expenseFile;
    User *newUser, *userptr;
    Family *newFamily, *familyptr;
    Expense *newExpense;
    status_code sc;

    // Open files
    userFile = fopen("users.txt", "r");
    if (userFile == NULL) {
        perror("Failed to open users.txt");
        return;
    }

    familyFile = fopen("families.txt", "r");
    if (familyFile == NULL) {
        perror("Failed to open families.txt");
        fclose(userFile);
        return;
    }

    expenseFile = fopen("expenses.txt", "r");
    if (expenseFile == NULL) {
        perror("Failed to open expenses.txt");
        fclose(userFile);
        fclose(familyFile);
        return;
    }

    // Read 25 users from users.txt
    for (int i = 0; i < 25; i++) {
        newUser = (User *)malloc(sizeof(User));
        if (newUser == NULL) {
            perror("Failed to allocate memory for new user");
            fclose(userFile);
            fclose(familyFile);
            fclose(expenseFile);
            return;
        }

        if (fscanf(userFile, "%d %s %f", &newUser->id, newUser->name, &newUser->income) != 3) {
            perror("Error reading user data");
            free(newUser);
            fclose(userFile);
            fclose(familyFile);
            fclose(expenseFile);
            return;
        }

        newUser->next = NULL;

        // Insert user in sorted order
        if (*userList == NULL || (*userList)->id > newUser->id) {
            newUser->next = *userList;
            *userList = newUser;
        } else {
            User *trail = *userList;
            while (trail->next != NULL && trail->next->id < newUser->id) {
                trail = trail->next;
            }
            newUser->next = trail->next;
            trail->next = newUser;
        }

        printf("Added user having ID: %d\n", newUser->id);
    }

    // Read 10 families from families.txt
    for (int i = 0; i < 10; i++) {
        newFamily = (Family *)malloc(sizeof(Family));
        if (newFamily == NULL) {
            perror("Failed to allocate memory for new family");
            fclose(userFile);
            fclose(familyFile);
            fclose(expenseFile);
            return;
        }

        if (fscanf(familyFile, "%d %s", &newFamily->family_id, newFamily->family_name) != 2) {
            perror("Error reading family data");
            free(newFamily);
            fclose(userFile);
            fclose(familyFile);
            fclose(expenseFile);
            return;
        }

        newFamily->No_members = 0;
        newFamily->family_income = 0;
        newFamily->family_expense = 0;
        newFamily->next = NULL;

        // Read up to 4 member user IDs
        int userID;
        for (int j = 0; j < 4; j++) {
            if (fscanf(familyFile, "%d", &userID) == 1 && userID != 0) {
                userptr = *userList;
                while (userptr != NULL) {
                    if (userptr->id == userID) {
                        newFamily->members[newFamily->No_members++] = userptr;
                        newFamily->family_income += userptr->income;
                        break;
                    }
                    userptr = userptr->next;
                }
            }
        }

        // Insert family in sorted order
        if (*familyList == NULL || (*familyList)->family_id > newFamily->family_id) {
            newFamily->next = *familyList;
            *familyList = newFamily;
        } else {
            Family *trail = *familyList;
            while (trail->next != NULL && trail->next->family_id < newFamily->family_id) {
                trail = trail->next;
            }
            newFamily->next = trail->next;
            trail->next = newFamily;
        }

        printf("Added family having ID: %d\n", newFamily->family_id);
    }

    // Read 40 expenses from expenses.txt
    for (int i = 0; i < 40; i++) {
        newExpense = (Expense *)malloc(sizeof(Expense));
        if (newExpense == NULL) {
            perror("Failed to allocate memory for new expense");
            fclose(userFile);
            fclose(familyFile);
            fclose(expenseFile);
            return;
        }

        if (fscanf(expenseFile, "%d %d %f %s %d %d %d",
                   &newExpense->expense_id, &newExpense->user_id, &newExpense->expense_amount,
                   newExpense->category, &newExpense->date.day, &newExpense->date.month, &newExpense->date.year) != 7) {
            perror("Error reading expense data");
            free(newExpense);
            fclose(userFile);
            fclose(familyFile);
            fclose(expenseFile);
            return;
        }

        newExpense->next = NULL;

        // Add expense to the list
        sc = AddExpense2(expenseList, *familyList, newExpense);
        if (sc == FAILURE) {
            printf("Failed to add expense with ID: %d\n", newExpense->expense_id);
        } else {
            printf("Added expense having ID: %d\n", newExpense->expense_id);
        }
    }

    // Close files
    fclose(userFile);
    fclose(familyFile);
    fclose(expenseFile);

    printf("Database initialized successfully!\n");
}

// Dummy implementation of AddExpense2 (for demonstration)



void displayUsers(User *userList){
    while(userList!=NULL){
        printf(" User Id :%d \n",userList->id);
        printf("User Name : %s \n",userList->name);
        printf("User income :%f \n",userList->income);
        userList=userList->next;

    }
}
void displayFamily(Family* familyList){
    while(familyList!=NULL){
        printf("Family id :%d \n",familyList->family_id);
        printf("Family Name : %s \n",familyList->family_name);
        printf("Family Number of Members : %d\n",familyList->No_members);
        printf("Family Income : %f \n",familyList->family_income);
        printf("Family Expense :%f \n",familyList->family_expense);
        
        familyList=familyList->next;

    }
}

void displayDate(Date date){
    printf(" Date :%d/%d/%d\n",date.day,date.month,date.year);
}
void displayExpenses(Expense*expenseList){
    while(expenseList!=NULL){
        printf("Expense Id :%d \n",expenseList->expense_id);
        printf("Expense User Id :%d \n",expenseList->user_id);
        printf("Expense Amount :%f \n",expenseList->expense_amount);
        printf("Expense ");displayDate(expenseList->date);
        printf("Expense Category :%s",expenseList->category);
        expenseList=expenseList->next;
    }
}

status_code addUser(User **userList,Family **familyList ) {
    status_code sc = SUCCESS;
   
    User *newuser = (User *)malloc(sizeof(User));

    if (newuser == NULL) {
        sc=FAILURE;
    }

    // Take input from the user
    printf("Enter Name of the user: ");
    scanf("%49s", newuser->name);  // Reads a string safely

    printf("Enter the income of the user: ");
    scanf("%f", &newuser->income);
     newuser->next = NULL;

   int flag3=0;
	while(!flag3){
		User *ptr = *userList;
		printf("Enter the ID of the user: ");
        scanf("%d", &newuser->id);
		int flag4=0;
		while(ptr!=NULL &&(!(flag4))){
			if(ptr->id==newuser->id){
				flag4=1;
			}
			ptr=ptr->next;
		}
		if(flag4==1){
			printf("this id is already taken :\n try another one");
		}
		else{
			flag3=1;
		}
		
	}

    // Insert user in sorted order
    if (*userList == NULL || (*userList)->id > newuser->id) {
        newuser->next = *userList;
        *userList = newuser;
    } else {
        User *trail = *userList;
        while (trail->next != NULL && trail->next->id < newuser->id) {
            trail = trail->next;
        }
        newuser->next = trail->next;
        trail->next = newuser;
    }
    int flag;
    printf("Do you want to enter a existing family (Enter 0) or Create a new Family(enter 1)\n");
    scanf("%d",&flag);
    Family *familyptr =*familyList;
   
    
    if(flag==1){//we will create a new family
    	 Family*newfamily;
    	int flag1=0;
    	while(flag1==0){
    		newfamily =(Family *)malloc(sizeof(Family));
    		if(newfamily!=NULL){
    			flag1=1;
			}
		}
		printf("Enter name of the Family :");
		scanf("%49s", newfamily->family_name);
		flag1=0;
		
		while(!flag1){
			int flag2=0;
			printf("enter the family id ");
			scanf("%d",&newfamily->family_id);
			Family*ptr=familyptr;
			while(ptr!=NULL  && !(flag2)){
				if(ptr->family_id==newfamily->family_id){
					flag2=1;
				}
				ptr=ptr->next;
			}
			if(flag2==1){
				printf("This Id is already taken..\n  try another one");
			}
			else{
				flag1=1;
			}
			
		}
		newfamily->members[0]=newuser;
		newfamily->family_income=newuser->income;
		newfamily->next=NULL;	
        newfamily->family_expense=0;
        newfamily->No_members=1;

    	if(familyptr==NULL || familyptr->family_id>newfamily->family_id){
            newfamily->next=familyptr;
            familyptr=newfamily;
            *familyList=familyptr;
        }
        else{
            while(familyptr->next!=NULL && familyptr->next->family_id<newfamily->family_id){
                familyptr=familyptr->next;
            }
            newfamily->next=familyptr->next;
            familyptr->next=newfamily;
        }
    	
    	
	}
    else{
    	
    	if(familyptr==NULL){
    	    printf("Cant join a Family pls Create a New One");
    		 Family*newfamily;
    	int flag1=0;
    	while(flag1==0){
    		newfamily =(Family *)malloc(sizeof(Family));
    		if(newfamily!=NULL){
    			flag1=1;
			}
		}
		printf("Enter name of the Family :");
		scanf("%49s", newfamily->family_name);
		flag1=0;
		
		while(!flag1){
			int flag2=0;
			printf("enter the family id ");
			scanf("%d",&newfamily->family_id);
			while(familyptr!=NULL  && !(flag2)){
				if(familyptr->family_id==newfamily->family_id){
					flag2=1;
				}
				familyptr=familyptr->next;
			}
			if(flag2==1){
				printf("This Id is already taken..\n  try another one");
			}
			else{
				flag1=1;
			}
			
		}
		newfamily->members[0]=newuser;
		newfamily->family_income=newuser->income;
		newfamily->next=NULL;	
        newfamily->family_expense=0;
        newfamily->No_members=1;
        *familyList=newfamily;
    		
		}
		else{//adding to a existing family
            int flag=0;
            int id;
            while(!(flag)){

                printf("Enter the Id of the Family You Want to Join");
                scanf("%d",&id);
                int flag1=0;

                familyptr=*familyList;
                while(familyptr!=NULL &&!(flag1)){
                    if(familyptr->family_id==id){
                        //family found
                        flag1=1;
                    }
                    else{

                        familyptr=familyptr->next;

                    }
                }
                if(!(flag1)){
                    printf("Invalid Id : Try Another One");
                }
                else{
                    flag=1;
                }
            }
            //now familyptr points on the selected family
            familyptr->family_income+=newuser->income;
            familyptr->members[(familyptr->No_members)++]=newuser;
	}
    
}
    return sc;
}	


int userExists(User *userList, int userId) {
    while (userList != NULL) {
        if (userList->id == userId) {
            return 1; // User exists
        }
        userList = userList->next;
    }
    return 0; // User does not exist
}

// Helper function to check if an expense ID exists
int expenseIdExists(Expense *expenseList, int expenseId) {
    while (expenseList != NULL) {
        if (expenseList->expense_id == expenseId) {
            return 1; // Expense ID exists
        }
        expenseList = expenseList->next;
    }
    return 0; // Expense ID does not exist
}

// Main addExpense function
status_code addExpense(User *userList, Family *familyList, Expense **expenseList) {
    if (userList == NULL) {
        printf("No users exist. Cannot add expense.\n");
        return FAILURE;
    }

    Expense *newExpense = (Expense *)malloc(sizeof(Expense));
    if (newExpense == NULL) {
        printf("Memory allocation failed. Cannot add expense.\n");
        return FAILURE;
    }

    // Input expense details
    printf("Enter the expense amount: ");
    scanf("%f", &(newExpense->expense_amount));

    // Input expense category
    printf("Choose the Expense Category:\n");
    printf("(1) Travel\n(2) Leisure\n(3) Rent\n(4) Food\n(5) Other Expense\n");
    int categoryChoice;
    scanf("%d", &categoryChoice);

    switch (categoryChoice) {
        case 1:
            strcpy(newExpense->category, "Travel");
            break;
        case 2:
            strcpy(newExpense->category, "Leisure");
            break;
        case 3:
            strcpy(newExpense->category, "Rent");
            break;
        case 4:
            strcpy(newExpense->category, "Food");
            break;
        default:
            strcpy(newExpense->category, "Other");
            break;
    }

    // Input expense date
    printf("Enter the date of the Expense in dd mm yyyy format: ");
    scanf("%d %d %d", &(newExpense->date.day), &(newExpense->date.month), &(newExpense->date.year));

    // Input user ID and validate
    int userId;
    do {
        printf("Enter the ID of the User: ");
        scanf("%d", &userId);
        if (!userExists(userList, userId)) {
            printf("Invalid User ID. Try another one.\n");
        }
    } while (!userExists(userList, userId));
    newExpense->user_id = userId;

    // Input expense ID and validate
    int expenseId;
    do {
        printf("Enter the ID of the Expense: ");
        scanf("%d", &expenseId);
        if (expenseIdExists(*expenseList, expenseId)) {
            printf("Expense ID already exists. Try another one.\n");
        }
    } while (expenseIdExists(*expenseList, expenseId));
    newExpense->expense_id = expenseId;

    // Insert the new expense in sorted order
    newExpense->next = NULL;
    if (*expenseList == NULL || (*expenseList)->expense_id > newExpense->expense_id) {
        newExpense->next = *expenseList;
        *expenseList = newExpense;
    } else {
        Expense *current = *expenseList;
        while (current->next != NULL && current->next->expense_id < newExpense->expense_id) {
            current = current->next;
        }
        newExpense->next = current->next;
        current->next = newExpense;
    }

    // Update family expenses if the user belongs to a family
    Family *familyptr = familyList;
    while (familyptr != NULL) {
        for (int i = 0; i < familyptr->No_members; i++) {
            if (familyptr->members[i]->id == newExpense->user_id) {
                familyptr->family_expense += newExpense->expense_amount;
                break; // Exit the loop once the user is found in a family
            }
        }
        familyptr = familyptr->next;
    }

    printf("Expense added successfully!\n");
    return SUCCESS;
}


	
status_code update_individual(User** userList,Family **familyList){
    status_code sc=SUCCESS;
    User *userptr=*userList;
    Family*familyptr=*familyList;
   
    if(userptr==NULL){
        sc=FAILURE;
    }
    else{
            int flag=0;
            int id;
             User*ptr=userptr;
            while(!(flag)){
                printf("enter the user id of the User whose details are to be changed :");
                scanf("%d",&id);
                int flag1=0;
                ptr=userptr;
                while(!(flag1) && ptr!=NULL){
                    if(id==ptr->id){
                            flag1=1;
                    }
                    else{
                             ptr=ptr->next;
                    }
                     }
                if(flag1==0){
                     printf("Invalid Id :\n Try another one");
                     }
                else{
                         flag=1;
                      }

            }
            int previncome=ptr->income;
            printf("enter the New income of The User");
            scanf("%f",&ptr->income);
            printf("Enter the New Name of the User");
            scanf("%49s",ptr->name);
            flag=0;
            Family *family=familyptr;

            while(!(flag) ){
                family=familyptr;
                int flag1=0;
                while(!(flag1) && family!=NULL){
                    for(int i=0;i<family->No_members ;i++){
                        if(family->members[i]->id==id){
                            flag1=1;
                        }
                    }
                      if(flag1==0){
                           family=family->next;
                           }
                      else{
                         flag=1;
                      }

                }
               

            }
            family->family_expense=family->family_expense+ptr->income-previncome;


    }


        return sc;
}

status_code updateFamily(Family* familyList, Family* family){
    status_code sc = 0;
    if(familyList == NULL){
        sc = 3;
    }
    else if(family == NULL){
        sc = 6;
    }
    else{
        printf("Enter the new name of the family: ");
        scanf("%49s", family -> family_name);
        printf("Updated Details for family having ID : %d \n", family -> family_id);
    }
    return sc;
}


status_code delete_Individual(User** userList, Family** familyList, Expense** expenseList, int id) {
    status_code sc = SUCCESS;
    User* userptr = *userList;
    Family* familyptr = *familyList;
    Expense* expenseptr = *expenseList;

    // Find the user with the given ID
    User* uptr = userptr;
    while (uptr != NULL && uptr->id != id) {
        uptr = uptr->next;
    }
    if (uptr == NULL) {
        sc=FAILURE; // User not found
    }

    // Delete all expenses associated with the user
    float totalexpense = 0;
    Expense* eptr = expenseptr;
    Expense* trail = NULL;

    while (eptr != NULL) {
        if (eptr->user_id == id) {
            Expense* temp = eptr;
            if (trail == NULL) {
                // First expense matches
                expenseptr = eptr->next;
            } else {
                // Expense in the middle or end
                trail->next = eptr->next;
            }
            eptr = eptr->next;
            totalexpense += temp->expense_amount;
            free(temp);
        } else {
            trail = eptr;
            eptr = eptr->next;
        }
    }
    *expenseList = expenseptr;

    // Find the family containing the user
    Family* family = familyptr;
    int flag = 0;
    while (family != NULL && !flag) {
        for (int i = 0; i < family->No_members; i++) {
            if (family->members[i]->id == id) {
                flag = 1;
                break;
            }
        }
        if (!flag) {
            family = family->next;
        }
    }

    if (family != NULL) {
        if (family->No_members == 1) {
            // Delete the entire family
            if (familyptr == family) {
                // Family is at the first position
                Family*temp=familyptr;
                
                familyptr = familyptr->next;
             
                *familyList=familyptr;
            } else {
                // Family is in the middle or end
                Family* trail = familyptr;
                while (trail->next != family) {
                    trail = trail->next;
                }
                trail->next = family->next;
            }
            free(family);
        } else {
            // Remove the user from the family
            for (int i = 0; i < family->No_members; i++) {
                if (family->members[i]->id == id) {
                    // Shift members to the left
                    for (int j = i; j < family->No_members - 1; j++) {
                        family->members[j] = family->members[j + 1];
                    }
                    family->No_members--;
                    break;
                }
            }
            family->family_expense -= totalexpense;
            family->family_income -= uptr->income;
        }
    }

    // Delete the user
    if (uptr == userptr) {
        // User is at the first position
        User*temp=userptr;
        userptr = userptr->next;
        free(temp);
        *userList=userptr;
    } else {
        // User is in the middle or end
        User* trail = userptr;
        while (trail->next != uptr) {
            trail = trail->next;
        }
        trail->next = uptr->next;
    }
    free(uptr);
    *userList = userptr;

    return sc;
}

status_code delete_Individual_abstracted(User** userList, Family** familyList, Expense** expenseList) {
    status_code sc = SUCCESS;
    User* userptr = *userList;

    if (userptr == NULL) {
        sc = FAILURE;
    } else {
        int id;
        int flag = 0;
        do {
            printf("Enter the user ID of the user whose details are to be changed: ");
            scanf("%d", &id);

            User* uptr = userptr;
            while (uptr != NULL && uptr->id != id) {
                uptr = uptr->next;
            }
            if (uptr == NULL) {
                printf("Invalid ID. Try another one.\n");
            } else {
                flag = 1;
            }
        } while (!flag);

        sc = delete_Individual(userList, familyList, expenseList, id);
    }

    return sc;
}
 
status_code deleteFamily(User **userList, Family **familyList, Expense **expenseList, Family *family){
    status_code sc = 0;
    if(family == NULL){
        sc = 6;
    }
    else if(*familyList == NULL){
        sc = 2;
    }
    else{
        int no_members = family -> No_members;
        for(int i=0 ; i<no_members && family ->members[0] != NULL; i++){
            sc = delete_Individual(userList, familyList, expenseList, family -> members[0]->id);
        }
    }
    return sc;
}


status_code delete_Expense(Expense**expenseList,Family*familyList){
    status_code sc=SUCCESS;
    Family*familyptr=familyList;
    Expense*expenseptr=*expenseList,*temp;
    if(expenseptr==NULL){
        sc=FAILURE;
    }
    else{
        int flag=0;
        int id;
        while(!(flag)){
            printf("Enter the Expense Id");
            scanf("%d",&id);
            Boolean expenseExists=FindExpense(expenseptr,id);
            if(expenseExists){
                flag=1;
            }
            else{
                printf("Invalid Id: Try Another One ");
            }
        }
        float expenseAmount;
        int user_id;
        if(expenseptr->expense_id==id){
            //1st position
            temp=expenseptr;
            expenseptr=expenseptr->next;
            expenseAmount=temp->expense_amount;
            user_id=temp->user_id;
            free(temp);
            *expenseList=expenseptr;

        }
        else{
            //anywhere else
            while(expenseptr!=NULL && expenseptr->expense_id!=id){
                temp=expenseptr;
                expenseptr=expenseptr->next;
            }
            temp->next=expenseptr->next;
            expenseAmount=expenseptr->expense_amount;
            user_id=expenseptr->user_id;
            free(expenseptr);
        }
        flag=0;
        
         while(familyptr!=NULL && !(flag)){
                for(int i=0;i<familyptr->No_members;i++){
                    if(familyptr->members[i]->id==user_id){
                        flag=1;
                    }
                }
                if(flag==1){
                    familyptr->family_expense-=expenseAmount;
                }
                else{
                    familyptr=familyptr->next;
                }

            }
    }
    return sc;
}
status_code update_Expense(Expense*expenseList,Family*familyList){
       status_code sc=SUCCESS;
    Family*familyptr=familyList;
    Expense*expenseptr=expenseList,*temp;
    if(expenseptr==NULL){
        sc=FAILURE;
    }
    else{
        int flag=0;
        int id;
        while(!(flag)){
            printf("Enter the Expense Id");
            scanf("%d",&id);
            Boolean expenseExists=FindExpense(expenseptr,id);
            if(expenseExists){
                flag=1;
            }
            else{
                printf("Invalid Id: Try Another One ");
            }
        }
        temp=expenseptr;
        while(temp->expense_id!=id){
            temp=temp->next;
        }
        int user_id=temp->user_id;
        float prevExpense=temp->expense_amount;
        printf("Enter the New Expense Amount");
        scanf("%f",&temp->expense_amount);
        int category;
        printf("Choose the New Expense Category : \n(1) for Travel \n (2) for Leisure \n (3) for Rent \n (4) for Food \n (5) for Others \n  ");
        scanf("%d",&category);
        if(category==1){
            strcpy(temp->category,"Travel");

        }
        else if(category==2){
            strcpy(temp->category,"Leisure");
        }
        else if(category==3){
            strcpy(temp->category,"Rent");
        }
        else if(category==4){
            strcpy(temp->category,"Food");
        }
        else{
            strcpy(temp->category,"Others");
        }
        printf("Enter the New date of The Expense In dd mm yyyy Format");
        scanf("%d%d%d",&temp->date.day,&temp->date.month,&temp->date.year);
        flag=0;
//        printf("89\n");
        while(!(flag) && familyptr!=NULL){
//            for(int i=0;i<familyptr->No_members;i++){

                if(familyptr->members[0]->id==user_id){
                    flag=1;
                }
                if(flag==1){
                    familyptr->family_expense=familyptr->family_expense+temp->expense_amount-prevExpense;
                }
                else{
                    familyptr=familyptr->next;
                }
//            }
        }
     }
     return sc;
 }
status_code Highest_Expense_Day(Expense*expenseList,Family*familyList){
    status_code sc=SUCCESS;
    Expense*expenseptr=expenseList;
    Family*familyptr=familyList;
    if(familyptr==NULL){
        sc=FAILURE;
    }
    else{
        int flag=0,id;
        while(!(flag)){
            printf(" Enter the Family Id");
            scanf("%d",&id);
            Family* FamilyExists=FindFamily(familyptr,id);
            if(FamilyExists){
                flag=1;
            }
            else{
                printf("Invalid Id : Try Another One");
            }
        }
        int month,year;
        printf("Enter the month and year :");
        scanf("%d%d",&month,&year);
        float expenseArray[31]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        flag=0;
        Family*family=familyptr;
        while(family!=NULL && !(flag)){
            if(family->family_id==id){
                flag=1;
            }
            else{
                family=family->next;
            }
            
           
        }
        while(expenseptr!=NULL){
            for(int i=0;i<family->No_members;i++){
                if(expenseptr->user_id==family->members[i]->id){
                    if(expenseptr->date.month==month && expenseptr->date.year==year){
                        expenseArray[(expenseptr->date.day-1)] += expenseptr->expense_amount;

                    }
                }
            }
            expenseptr=expenseptr->next;
        }
        int maxExpense =0,maxDay=1;
        for(int i=0;i<30;i++){
            if(maxExpense<expenseArray[i]){
                maxExpense=expenseArray[i];
                maxDay=i+1;
            }
        }

        printf("The Family With Id %d spended Maximum Amount of %d on %d day in month %d and year %d",id,maxExpense,maxDay,month,year);

    }

 return sc;
}


  status_code Get_individual_expense(Expense* expenseList,User *userList, int month, int year){
    status_code sc = SUCCESS;
    User* userptr = userList;

    if (userptr == NULL) {
        sc = FAILURE;
    } else {
        int id;
        int flag = 0;
        do {
            printf("Enter the user ID of the user whose details are to be changed: ");
            scanf("%d", &id);

            User* uptr = userptr;
            while (uptr != NULL && uptr->id != id) {
                uptr = uptr->next;
            }
            if (uptr == NULL) {
                printf("Invalid ID. Try another one.\n");
            } else {
                flag = 1;
            }
        } while (!flag);
        User* uptr=userList;
        while(uptr->id!=id){
            uptr=uptr->next;
        }

    
        char *expenseNames[5] = {"Travel", "Leisure", "Rent", "Food", "Other"};
        float CategoricalExpenses[5]={0,0,0,0,0};
        Expense* expenseptr = expenseList;
        int found;
        while(expenseptr!=NULL && expenseptr->user_id==uptr->id){ 
            if(expenseptr->date.month == month && expenseptr->date.year == year){
                if(!(strcmp("Travel",expenseptr->category)) ){
                    CategoricalExpenses[0] += expenseptr->expense_amount;
                }
                else if(!(strcmp("Leisure",expenseptr->category))){
                    CategoricalExpenses[1] += expenseptr->expense_amount;
                }
                 else if(!(strcmp("Rent",expenseptr->category))){
                    CategoricalExpenses[2] += expenseptr->expense_amount;
                }
                  else if(!(strcmp("Food",expenseptr->category))){
                    CategoricalExpenses[3] += expenseptr->expense_amount;
                }
                else{
                    CategoricalExpenses[4] += expenseptr->expense_amount;
                }
            }
            expenseptr = expenseptr -> next;
        }
        
        // Sorting based on Categories
         for(int i=0;i<5-1;i++){
            for(int j=0;j<5-i-1;j++){
                if(CategoricalExpenses[j]<CategoricalExpenses[j+1]){
                    float temp=CategoricalExpenses[j];
                    CategoricalExpenses[j]=CategoricalExpenses[j+1];
                    CategoricalExpenses[j+1]=temp;
                    char* tempName=expenseNames[j];
                    expenseNames[j]=expenseNames[j+1];
                    expenseNames[j+1]=tempName;
                  
                
            }
        }

        printf("The Category Wise Expenses of the User with Id %d are \n",id);
        for(int i=0;i<5;i++){
            printf("Category : %s || Amount : %f \n",expenseNames[i],CategoricalExpenses[i]);
        }
    }
}
    return sc;
}
status_code Get_categorical_expense(Expense* expenseList, Family* familyList, int category){
    status_code sc=SUCCESS;
    
    Family*familyptr=familyList;
    Expense*expenseptr=expenseList;

if(familyptr==NULL){
    sc=FAILURE;
}
else{
        int flag=0;
        int id;
         Family*family=familyptr;
         //loop to get valid user id
        while(!(flag)){
            printf("enter the user id of the Family whose details are to be changed :");
            scanf("%d",&id);
            int flag1=0;
            family=familyptr;
            while(!(flag1) && family!=NULL ){
                if(id==family->family_id){
                        flag1=1;
                }
                else{
                         family=family->next;
                }
            }
            if(flag1==0){
                 printf("Invalid Id :\n Try another one");
                 }
            else{
                     flag=1;
                  }
                }
        char category_string[15];
        float total_cat_expense = 0.0;
        // Convert Category to string
        switch(category){
            case 1:
                strcpy(category_string, "Travel");
                break;
            case 2:
                strcpy(category_string, "Leisure");
                break;
            case 3:
                strcpy(category_string, "Rent");
                break;
            case 4:
                strcpy(category_string, "Food");
                break;
            default:
                strcpy(category_string, "Other");
                break;
        }

        // Defining a structure to store user_ids with expense 
        struct member_expense{
            int id;
            float amount;
        };
        struct member_expense member_Expenses[4];
        // Initializing the members' Expenses array 
        for (int i=0 ; i<4 ; i++){
            member_Expenses[i].id = 0;
            member_Expenses[i].amount = 0.0;
        }
        for(int i=0 ; i<family -> No_members ; i++){
            member_Expenses[i].id = family ->members[i] ->id;
        }

        Expense* expenseptr = expenseList;
        int found;
        while(expenseptr != NULL){
            found = 0;
            for(int i=0; i< family->No_members && !found; i++){
                if(strcmp(category_string, expenseptr -> category) == 0){
                    total_cat_expense += expenseptr -> expense_amount;
                    member_Expenses[i].amount += expenseptr -> expense_amount;
                    found = 1;
                }
            }
            expenseptr = expenseptr -> next;
        }

        // Sorting in descending order
        struct member_expense temp;
        for (int i=0 ; i < 3 ; i++){
            for (int j=0 ; j < 3-i ; j++){
                if (member_Expenses[j].amount < member_Expenses[j+1].amount){
                    temp = member_Expenses[j];
                    member_Expenses[j] = member_Expenses[j+1];
                    member_Expenses[j+1] = temp;
                }
            }
        }
        printf("The total Category expense for Family (ID : %d) is : %f \n", family->family_id, total_cat_expense);
        printf("Member-wise contribution to category Expenses:- \n");
        for (int i=0 ; i<4 ; i++){
            if (member_Expenses[i].id != 0){
                printf("User ID: %d ", member_Expenses[i].id);
                printf("Expense contributed %f \n", member_Expenses[i].amount);
            }
        }
    }
    return sc;
}
  
status_code Get_total_expense(Family* familyList,  Expense* expenseList, int month, int year){
    status_code sc=SUCCESS;
    
    Family*familyptr=familyList;
    Expense*expenseptr=expenseList;

   if(familyptr==NULL){
    sc=FAILURE;
    }
   else{
        int flag=0;
        int id;
         Family*family=familyptr;
         //loop to get valid user id
        while(!(flag)){
            printf("enter the user id of the Family whose details are to be changed :");
            scanf("%d",&id);
            int flag1=0;
            family=familyptr;
            while(!(flag1) && family!=NULL ){
                if(id==family->family_id){
                        flag1=1;
                }
                else{
                         family=family->next;
                }
            }
            if(flag1==0){
                 printf("Invalid Id :\n Try another one");
                 }
            else{
                     flag=1;
                  }
                }
   
        Expense* expenseptr = expenseList;
        int total_expense = 0;
        int found = 0;
        while(expenseptr != NULL){
            for(int i = 0; i < family -> No_members && !found ; i++){
                if(family -> members[i] -> id == expenseptr -> user_id){
                    if(expenseptr -> date.month == month && expenseptr -> date.year == year){
                        total_expense += expenseptr -> expense_amount;
                    }
                    found = 1;
                }
            }
            expenseptr = expenseptr -> next;
        }
        printf("Total Monthly expense for the family in Month : %d, Year : %d is %d \n", month,year,total_expense);
        if(total_expense > family -> family_income){
            printf("The Family (ID : %d) is in debt \n", family -> family_id);
            printf("The family is in debt of Rs: %f \n", total_expense - (family -> family_income));
        }
        else{
            printf("The Family (ID : %d) is well-off \n", family -> family_id);
            printf("The family is saving Rs: %f \n", (family -> family_income) - total_expense);
        }
    }
    return sc;
}


void status_code_checker(status_code sc){
    if(sc == 1){
        printf("The Operation was a success \n");
    }
    else if(sc == 0){
        printf("error");
    }
    
}

int main() {
    int flag = 0;
    int choice;
    status_code status_code;
    User*userList;
    Family*familyList;
    Expense*expenseList;
    Initialize_DB(&userList,&familyList,&expenseList);
    
    while (!flag){
        printf("Enter choices from 1 to 16 \n");
        printf("1. Add a User \n");
        printf("2. Add an expense \n");
        printf("3. Update individual details \n");
        printf("4. Delete individual details \n");
        printf("5. Update a family \n");
        printf("6. Delete a family \n");
        printf("7. Update an expense \n");
        printf("8. Delete an expense \n");
        printf("9. Get highest expense day for a family \n");
        printf("10. Get total family expense \n");
        printf("11. Get categorical expense for a particular category and family \n");
        printf("12. Get individual expense \n");
        printf("13. Display User Database \n");
        printf("14. Display Family Database \n");
        printf("15. Display Expense Database \n");
        printf("16. Exit \n");
        printf("Enter a choice (1/2/3/4/5/6/7/8/9/10/11/12/13/14/15/16): ");
        scanf("%d", &choice);
        printf("--------------------------------------------------- \n");
        if (choice == 1){
            status_code=addUser(&userList,&familyList);
            
            status_code_checker(status_code);
        }
        else if (choice == 2){
            status_code=addExpense(userList,familyList,&expenseList);
            status_code_checker(status_code);
        }
        else if (choice == 3){
            status_code=update_individual(&userList,&familyList);
           status_code_checker(status_code);
        }
        else if (choice == 4){
            status_code=delete_Individual_abstracted(&userList,&familyList,&expenseList);
           status_code_checker(status_code);
        }
         else if (choice == 5){
         	int f_id;
			printf("Enter the family id :");
        	scanf("%d",&f_id);
            Family*existingFamily=FindFamily(familyList,f_id);
            status_code=updateFamily(familyList,existingFamily);
            status_code_checker(!status_code);
        }
        else if (choice == 6){
        	int f_id;
        	printf("Enter the family id :");
        	scanf("%d",&f_id);
            Family*existingFamily=FindFamily(familyList,f_id);
            status_code=deleteFamily(&userList,&familyList,&expenseList,existingFamily);
            status_code_checker(status_code);
        }
        else if (choice == 7){
            status_code=update_Expense(expenseList,familyList);
            status_code_checker(status_code);
        }
        else if (choice == 8){
            status_code=delete_Expense(&expenseList,familyList);    
            status_code_checker(status_code);
        }
        else if (choice == 9){
            status_code=Highest_Expense_Day(expenseList,familyList);
            status_code_checker(status_code);
        }
        else if (choice == 10){
            int month,year;
            printf("enter the month and year :");
            scanf("%d%d",&month,&year);
            status_code=Get_total_expense(familyList,expenseList,month,year);
            status_code_checker(status_code);
        }
        else if (choice == 11){
            int category;
            printf("enter the category(1 to 5) :");
            scanf("%d",&category);
            status_code=Get_categorical_expense(expenseList,familyList,category);
            status_code_checker(status_code);
        } 
        else if (choice == 12){
            
            status_code=Highest_Expense_Day(expenseList,familyList);
           status_code_checker(status_code);
        }
        
        else if (choice == 13){
            displayUsers(userList);
        }
        else if (choice == 14){
            displayFamily(familyList);
        }
        else if (choice == 15){
           displayExpenses(expenseList);
        }
        else if(choice ==16){
            flag=1;
        }
        else {
            printf("Choice is INVALID. Please re-enter choice \n");
        }
    }
    return 0;
}
