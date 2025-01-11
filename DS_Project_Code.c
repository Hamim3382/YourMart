#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <windows.h>

#define TABLE_SIZE 100

#define NAME_TABLE_SIZE 100

typedef struct Product {
    char ID[9];
    char proName[16];
    float proPrice;
    char per[4];
    int quantity;
    struct Product *next;
} product;

typedef struct User {
    char email[31];
    char username[31];
    char password[11];
} user;

typedef struct Merchant {
    char shopName[51];
    char username[31];
    char shortname[6];
    char password[11];
} merchant;

typedef struct HashNode {
    product item;
    struct HashNode *next;
} HashNode;

typedef struct NameHashNode {
    product *item;
    struct NameHashNode *next;
} NameHashNode;

typedef struct BSTNode {
    product data;
    struct BSTNode *left;
    struct BSTNode *right;
} BSTNode;

product *myCart = NULL;

HashNode *hashTable[TABLE_SIZE];

NameHashNode *nameHashTable[NAME_TABLE_SIZE];

BSTNode* root = NULL;

unsigned int hash (char *id) {
    unsigned int hashValue = 0;
    for (int i = 0; id[i] != '\0'; i++) {
        hashValue = (hashValue + id[i]) % TABLE_SIZE;
    }
    return hashValue;
}

void addToHashTable (product *item) {
    unsigned int index = hash(item->ID);
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    newNode->item = *item;
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}

product *findInHashTable (char *id) {
    unsigned int index = hash(id);
    HashNode *node = hashTable[index];
    while (node != NULL) {
        if (strcmp(node->item.ID, id) == 0) {
            return &node->item;
        }
        node = node->next;
    }
    return NULL;
}

void loadInventory () {
    FILE *file = fopen("inventory.txt", "r");
    if (!file) {
        printf("\n\n\n Error opening inventory file!\n\n\n");
        return;
    }
    char line[50];
    while (fgets(line, sizeof(line), file)) {
        product *newProduct = (product *)malloc(sizeof(product));
        sscanf(line, "%[^,],%[^,],%f,%[^,],%d", newProduct->ID, newProduct->proName, &newProduct->proPrice, newProduct->per, &newProduct->quantity);
        addToHashTable(newProduct);
    }
    fclose(file);
}

void initializeHashTable () {
    for (int i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }
}

void strToLower (char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

unsigned int nameHash (char *name) {
    unsigned int hashValue = 0;
    for (int i = 0; name[i] != '\0'; i++) {
        hashValue = (hashValue + tolower(name[i])) % NAME_TABLE_SIZE;
    }
    return hashValue;
}

void addToNameHashTable (product *item) {
    char lowerName[16];
    strcpy(lowerName, item->proName);
    strToLower(lowerName);
    unsigned int index = nameHash(lowerName);
    NameHashNode *newNode = (NameHashNode *)malloc(sizeof(NameHashNode));
    newNode->item = item;
    newNode->next = nameHashTable[index];
    nameHashTable[index] = newNode;
}

product *findByNameInHashTable (char *name) {
    char lowerName[16];
    strcpy(lowerName, name);
    strToLower(lowerName);
    unsigned int index = nameHash(lowerName);
    NameHashNode *node = nameHashTable[index];
    while (node != NULL) {
        char tempName[16];
        strcpy(tempName, node->item->proName);
        strToLower(tempName);
        if (strcmp(lowerName, tempName) == 0) {
            return node->item;
        }
        node = node->next;
    }
    return NULL;
}

void loadNameHashTable () {
    FILE *file = fopen("inventory.txt", "r");
    if (!file) {
        printf("\n\n\n Error opening inventory file!\n\n\n");
        return;
    }
    char line[50];
    while (fgets(line, sizeof(line), file)) {
        product *newProduct = (product *)malloc(sizeof(product));
        sscanf(line, "%[^,],%[^,],%f,%[^,],%d", newProduct->ID, newProduct->proName, &newProduct->proPrice, newProduct->per, &newProduct->quantity);
        addToNameHashTable(newProduct);
    }
    fclose(file);
}

void initializeNameHashTable () {
    for (int i = 0; i < NAME_TABLE_SIZE; i++) {
        nameHashTable[i] = NULL;
    }
}

product *AddToCart () {
    char id[9];
    printf("\n\n\t        Enter Product ID to add to cart : ");
    fgets(id, sizeof(id), stdin);
    id[strcspn(id, "\n")] = '\0';
    product *foundProduct = findInHashTable(id);
    if (foundProduct) {
        int cartQuantity;
        printf("\t        Enter quantity : ");
        scanf("%d", &cartQuantity);
        getchar();
        if (cartQuantity > foundProduct->quantity) {
            printf("\n\t        Not enough stock available! ");
            return myCart;
        }
        product *newItem = (product *)malloc(sizeof(product));
        strcpy(newItem->ID, foundProduct->ID);
        strcpy(newItem->proName, foundProduct->proName);
        newItem->proPrice = foundProduct->proPrice;
        strcpy(newItem->per, foundProduct->per);
        newItem->quantity = cartQuantity;
        newItem->next = myCart;
        myCart = newItem;
        printf("\n\n\t        Item added to cart! ");
        return myCart;
    } else {
        printf("\n\n\t        No product found with the given ID! ");
        return myCart;
    }
}

void DisplaySpecificProducts () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Find Product    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    char ProductName[16];
    printf("\t        Enter the Product Name : ");
    fgets(ProductName, 16, stdin);
    ProductName[strcspn(ProductName, "\n")] = '\0';
    product *foundProduct = findByNameInHashTable(ProductName);
    if (!foundProduct) {
        printf("\n\n\n\t        Oops! That product is currently unavailable! ");
        printf("\n\n\n\t        Press enter to go back to the menu ");
        getchar();
    } else {
        printf("\t        --------------------------------------------------------------------------------\n");
        printf("\t        |     ID     |         Name         |    Price    |     Per     |   Quantity   |\n");
        printf("\t        --------------------------------------------------------------------------------\n");
        printf("\t        |   %-6s   |   %-16s   |    %-7.2f  |     %-7s |     %-9d|\n", foundProduct->ID, foundProduct->proName, foundProduct->proPrice, foundProduct->per, foundProduct->quantity);
        printf("\t        --------------------------------------------------------------------------------\n");
        printf("\n\t        1. Add Item to Cart");
        printf("\n\t        0. Back to the menu");
        printf("\n\n\n\t        Enter your choice : ");
        int choice;
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1 :
                AddToCart();
                getchar();
                break;
            case 0 :
                return;
            default :
                printf("\n\t        Oops! That wasn't a valid option. Please try again ");
                getchar();
                break;
        }
    }
}

BSTNode* createNode (product data) {
    BSTNode* newNode = (BSTNode*)malloc(sizeof(BSTNode));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

BSTNode* insertByPriceHelper (BSTNode* node, product data) {
    if (node == NULL) {
        return createNode(data);
    }
    if (data.proPrice < node->data.proPrice) {
        node->left = insertByPriceHelper(node->left, data);
    } else {
        node->right = insertByPriceHelper(node->right, data);
    }
    return node;
}

void insertByPrice (product data) {
    root = insertByPriceHelper(root, data);
}

BSTNode* insertByPriceDescendingHelper (BSTNode* node, product data) {
    if (node == NULL) {
        return createNode(data);
    }
    if (data.proPrice > node->data.proPrice) {
        node->left = insertByPriceDescendingHelper(node->left, data);
    } else {
        node->right = insertByPriceDescendingHelper(node->right, data);
    }
    return node;
}

void insertByPriceDescending (product data) {
    root = insertByPriceDescendingHelper(root, data);
}

BSTNode* insertByIDHelper (BSTNode* node, product data) {
    if (node == NULL) {
        return createNode(data);
    }
    if (strcmp(data.ID, node->data.ID) < 0) {
        node->left = insertByIDHelper(node->left, data);
    } else {
        node->right = insertByIDHelper(node->right, data);
    }
    return node;
}

void insertByID (product data) {
    root = insertByIDHelper(root, data);
}

void inorderTraversalHelper (BSTNode* node) {
    if (node != NULL) {
        inorderTraversalHelper(node->left);
        printf("\t        |   %-6s   |   %-16s   |    %-7.2f  |     %-7s |     %-9d|\n", 
               node->data.ID, node->data.proName, node->data.proPrice, node->data.per, node->data.quantity);
        printf("\t        --------------------------------------------------------------------------------\n");
        inorderTraversalHelper(node->right);
    }
}

void inorderTraversal () {
    inorderTraversalHelper(root);
}

void SortByPrice () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////  Ascending  Order  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    FILE *file = fopen("inventory.txt", "r");
    if (!file) {
        printf(" No products found or unable to open file!");
        return;
    }
    root = NULL;
    product prod;
    char line[50];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%[^,],%f,%[^,],%d", prod.ID, prod.proName, &prod.proPrice, prod.per, &prod.quantity);
        insertByPrice(prod);
    }
    printf("\t        --------------------------------------------------------------------------------\n");
    printf("\t        |     ID     |         Name         |    Price    |     Per     |   Quantity   |\n");
    printf("\t        --------------------------------------------------------------------------------\n");
    inorderTraversal();
    printf("\n\t        1. Add Item to Cart");
    printf("\n\t        0. Return to Last Page");
    printf("\n\n\n\t        Enter your choice : ");
    int choice;
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1 :
            AddToCart();
            getchar();
            break;
        case 0 :
            fclose(file);
            return;
        default :
            printf("\n\t        Oops! That wasn't a valid option. Please try again ");
            getchar();
            break;
    }
    fclose(file);
}

void SortByPriceDescending () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////  Descending Order  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    FILE *file = fopen("inventory.txt", "r");
    if (!file) {
        printf(" No products found or unable to open file!");
        return;
    }
    root = NULL;
    product prod;
    char line[50];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%[^,],%f,%[^,],%d", prod.ID, prod.proName, &prod.proPrice, prod.per, &prod.quantity);
        insertByPriceDescending(prod);
    }
    printf("\t        --------------------------------------------------------------------------------\n");
    printf("\t        |     ID     |         Name         |    Price    |     Per     |   Quantity   |\n");
    printf("\t        --------------------------------------------------------------------------------\n");
    inorderTraversal();
    printf("\n\t        1. Add Item to Cart");
    printf("\n\t        0. Return to Last Page");
    printf("\n\n\n\t        Enter your choice : ");
    int choice;
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1:
            AddToCart();
            getchar();
            break;
        case 0:
            fclose(file);
            return;
        default:
            printf("\n\t        Oops! That wasn't a valid option. Please try again ");
            getchar();
            break;
    }
    fclose(file);
}

void SortByID () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Sorted By ID    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    FILE *file = fopen("inventory.txt", "r");
    if (!file) {
        printf(" No products found or unable to open file!");
        return;
    }
    root = NULL;
    product prod;
    char line[50];
    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%[^,],%[^,],%f,%[^,],%d", prod.ID, prod.proName, &prod.proPrice, prod.per, &prod.quantity);
        insertByID(prod);
    }
    printf("\t        --------------------------------------------------------------------------------\n");
    printf("\t        |     ID     |         Name         |    Price    |     Per     |   Quantity   |\n");
    printf("\t        --------------------------------------------------------------------------------\n");
    inorderTraversal();
    printf("\n\t        1. Add Item to Cart");
    printf("\n\t        0. Return to Last Page");
    printf("\n\n\n\t        Enter your choice : ");
    int choice;
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1 :
            AddToCart();
            getchar();
            break;
        case 0 :
            fclose(file);
            return;
        default :
            printf("\n\t        Oops! That wasn't a valid option. Please try again ");
            getchar();
            break;
    }
    fclose(file);
}

void DisplayProducts () {
    while (1) {
        system("cls");
        printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
        printf("\t\t\t\t/////////////// Available Products \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
        FILE *file = fopen("inventory.txt", "r");
        if (!file) {
            printf(" No products found or unable to open file!");
            return;
        }
        char line[30];
        printf("\t        --------------------------------------------------------------------------------\n");
        printf("\t        |     ID     |         Name         |    Price    |     Per     |   Quantity   |\n");
        printf("\t        --------------------------------------------------------------------------------\n");
        while (fgets(line, sizeof(line), file)) {
            char id[9], name[16], per[4];
            float price;
            int quantity;
            sscanf(line, "%[^,],%[^,],%f,%[^,],%d", id, name, &price, per, &quantity);
            printf("\t        |   %-6s   |   %-16s   |    %-7.2f  |     %-7s |     %-9d|\n", id, name, price, per, quantity);
            printf("\t        --------------------------------------------------------------------------------\n");
        }
        printf("\n\t        1. Add Item to Cart");
        printf("\n\t        2. Sort By Price (Ascending Order)");
        printf("\n\t        3. Sort By Price (Descending Order)");
        printf("\n\t        4. Sort By ID (Alphabetical Order)");
        printf("\n\t        0. Back to the menu");
        printf("\n\n\n\t        Enter your choice : ");
        int choice;
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1 :
                AddToCart();
                getchar();
                break;
            case 2 :
                SortByPrice();
                break;
            case 3 :
                SortByPriceDescending();
                break;
            case 4 :
                SortByID();
                break;
            case 0 :
                fclose(file);
                return;
            default :
                printf("\n\t        Oops! That wasn't a valid option. Please try again ");
                getchar();
                break;
        }
        fclose(file);
    }
}

int SaveOrderHistory (char *UserName, float GrandTotal) {
    FILE *orderFile = fopen("orderhistory.txt", "a");
    if (!orderFile) {
        printf("\n\t        Error opening order history file!");
        return 1;
    }
    time_t t;
    struct tm *tm_info;
    time(&t);
    tm_info = localtime(&t);
    char phoneNumber[15], address[50];
    printf("\n\t        Enter your phone number : ");
    fgets(phoneNumber, sizeof(phoneNumber), stdin);
    phoneNumber[strcspn(phoneNumber, "\n")] = '\0';
    if (strlen(phoneNumber) < 11) {
        fclose(orderFile);
        printf("\n\n\n\t        Invalid phone number ");
        return 1;
    }
    printf("\t        Enter your address : ");
    fgets(address, sizeof(address), stdin);
    address[strcspn(address, "\n")] = '\0';
    if (strcmp(address, "") == 0) {
        fclose(orderFile);
        printf("\n\n\n\t        Invalid address ");
        return 1;
    }
    fprintf(orderFile, "\n Order From : %s   Phone : %s\n Address : %s\n", UserName, phoneNumber, address);
    fprintf(orderFile, " Date & Time : %02d-%02d-%04d  %02d:%02d:%02d\n", tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900, tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    fprintf(orderFile, " --------------------------------------------------------------------------------\n");
    fprintf(orderFile, " |     ID     |         Name         |    Price    |     Per     |   Quantity   |\n");
    fprintf(orderFile, " --------------------------------------------------------------------------------\n");
    product *current = myCart;
    while (current) {
        fprintf(orderFile, " |   %-6s   |   %-16s   |    %-7.2f  |     %-7s |     %-9d|\n", current->ID, current->proName, current->proPrice, current->per, current->quantity);
        current = current->next;
    }
    fprintf(orderFile, " --------------------------------------------------------------------------------\n");
    fprintf(orderFile, "\n Grand Total : %.2f TK\n", GrandTotal);
    fprintf(orderFile, " ================================================================================\n");
    fclose(orderFile);
    return 0;
}

void BuyItems (char *UserName, float  GrandTotal) {
    if (SaveOrderHistory(UserName, GrandTotal)) {
        return;
    }
    FILE *file = fopen("inventory.txt", "r");
    if (!file) {
        printf("\n\t        Error opening inventory file!");
        return;
    }
    FILE *tempFile = fopen("temp.txt", "w");
    if (!tempFile) {
        printf("\n\t        Error opening temporary file!");
        fclose(file);
        return;
    }
    char line[30];
    while (fgets(line, sizeof(line), file)) {
        char invID[9], name[16], per[4];
        float price;
        int quantity;
        sscanf(line, "%[^,],%[^,],%f,%[^,],%d", invID, name, &price, per, &quantity);
        product *current = myCart;
        while (current) {
            if (strcmp(current->ID, invID) == 0) {
                if (quantity >= current->quantity) {
                    quantity -= current->quantity;
                } else {
                    printf("\n\t        Insufficient stock for item : %s\n", current->proName);
                }
            }
            current = current->next;
        }
        fprintf(tempFile, "%s,%s,%.2f,%s,%d\n", invID, name, price, per, quantity);
    }
    fclose(file);
    fclose(tempFile);
    remove("inventory.txt");
    rename("temp.txt", "inventory.txt");
    product *current = myCart;
    while (current) {
        product *temp = current;
        current = current->next;
        free(temp);
    }
    myCart = NULL;
    printf("\n\t        Order confirmed successfully! Thank you for choosing YourMart! ");
}

product* RemoveFromCart (product *myCart) {
    char id[9];
    printf("\n\t        Enter Product ID to remove : ");
    fgets(id, 9, stdin);
    id[strcspn(id, "\n")] = '\0';
    product *current = myCart, *prev = NULL;
    while (current != NULL && strcmp(current->ID, id) != 0) {
        prev = current;
        current = current->next;
    }
    if (!current) {
        printf("\n\t        No product found with the given ID! ");
        return myCart;
    }
    if (!prev) {
        myCart = current->next;
    } else {
        prev->next = current->next;
    }
    free(current);
    printf("\n\t        Product removed from cart! ");
    return myCart;
}

void DisplayCart (char *UserName) {
    while (1) {
        if (!myCart) {
            system("cls");
            printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
            printf("\t\t\t\t///////////////     Your  Cart     \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n");
            printf(" Your cart is currently empty, so check out our inventory for great finds");
            printf("\n\n\n Press Enter to Return Customer Portal ");
            getchar();
            return;
        }
        system("cls");
        printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
        printf("\t\t\t\t///////////////     Your  Cart     \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n");
        printf("\t        ------------------------------------------------------------------------------------\n");
        printf("\t        |     ID     |         Name         |     Per     |   Quantity   |   Total Price   |\n");
        printf("\t        ------------------------------------------------------------------------------------\n");
        product *current = myCart;
        float GrandTotal = 0.0;
        while (current) {
            float total = current->proPrice * current->quantity;
            GrandTotal += total;
            printf("\t        |   %-6s   |   %-16s   |     %-7s |     %-9d|     %-10.2f  |\n", current->ID, current->proName, current->per, current->quantity, total);
            printf("\t        ------------------------------------------------------------------------------------\n");
            current = current->next;
        }
        printf("\n\n\t        Grand Total : %.2f TK", GrandTotal);
        printf("\n\n\n\t        1. Confirm Your Purchase");
        printf("\n\t        2. Add Item To Cart");
        printf("\n\t        3. Remove From Cart");
        printf("\n\t        4. Explore Products");
        printf("\n\t        0. Back to the menu");
        printf("\n\n\n\t        Enter your choice : ");
        int choice;
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1 :
                BuyItems(UserName, GrandTotal);
                getchar();
                return;
                break;
            case 2 :
                AddToCart();
                getchar();
                break;
            case 3 :
                myCart = RemoveFromCart(myCart);
                getchar();
                break;
            case 4 :
                DisplayProducts();
                break;
            case 0 :
                return;
                break;
            default :
                printf("\n\t        Oops! That wasn't a valid option. Please try again ");
                getchar();
                DisplayCart(UserName);
                break;
        }
    }
}

void CheckProductReview () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////  User Experiences  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    FILE *file = fopen("productreview.txt", "r");
    if (file == NULL) {
        printf(" Error opening file for reading");
        return;
    }
    char line[151];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
    printf("\n\n\n Press Enter to go back to Menu ");
    getchar();
}

void ProductReview (char *UserName) {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Rate & Review   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    printf("\n 1. Give Review");
    printf("\n 0. Back To Menu");
    printf("\n\n\n Enter your choice : ");
    int choice;
    scanf("%d", &choice);
    getchar();
    if (choice == 1) {
        char ProductID[9];
        char *review = (char *)malloc(151 * sizeof(char));
        if (review == NULL) {
            printf("\n\n Memory allocation failed for review");
            return;
        }
        printf("\n\n Enter Product ID : ");
        fgets(ProductID, 9, stdin);
        ProductID[strcspn(ProductID, "\n")] = '\0';
        if (strlen(ProductID) == 0) {
            printf("\n\n Product ID cannot be empty. Press Enter Back to Menu ");
            free(review);
            getchar();
            return;
        }
        printf("\n Enter your review (150 Characters Max) :\n\n ");
        fgets(review, 151, stdin);
        int len = strlen(review);
        if (len > 0 && review[len - 1] == '\n') {
            review[len - 1] = '\0';
        }
        if (strlen(review) == 0) {
            printf("\n\n Review cannot be empty. Press Enter Back to Menu ");
            free(review);
            getchar();
            return;
        }
        FILE *file = fopen("productreview.txt", "a");
        if (file == NULL) {
            printf(" Error opening file for writing");
            free(review);
            return;
        }
        fprintf(file, " Username : %s\n Product ID : %s\n Feedback : %s\n\n ------------------------------\n\n", UserName, ProductID, review);
        fclose(file);
        free(review);
        printf("\n\n\n We value your input, %s. Thanks for your feedback!", UserName);
        getchar();
    } else if (choice == 0) {
        return;
    } else {
        printf("\n Oops! That wasn't a valid option. Please try again ");
        getchar();
    }
}

void UserPortal (char *UserName) {
    while (1) {
        system("cls");
        printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
        printf("\t\t\t\t///////////////  Customer  Portal  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/                                                \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/               1. Search                        \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/               2. Explore Products              \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/               3. Your Cart                     \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/               4. Product Reviews               \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/               5. Review a Product              \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/               0. Sign Out                      \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/                                                \\\n");
        printf("\t\t\t\t\\ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ /\n\n\n");
        printf("\t\t\t\t              Enter your choice : ");
        int choice;
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1 :
                DisplaySpecificProducts();
                break;
            case 2 :
                DisplayProducts();
                break;
            case 3 :
                DisplayCart(UserName);
                break;
            case 4 :
                CheckProductReview();
                break;
            case 5 :
                ProductReview(UserName);
                break;
            case 0 :
                return;
            default :
                printf("\n\t\t\t        Oops! That wasn't a valid option. Please try again ");
                getchar();
                break;
        }
    }
}

void RegisterUser () {
    system("cls");
    FILE *file;
    user newUser;
    char confirmPassword[11];
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Registration    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    file = fopen("users.dat", "ab");
    if (file == NULL) {
        printf("\n\n\n Error opening file for writing");
        return;
    }
    printf("\n\n\n Enter Email (Max 30 Characters) : ");
    fgets(newUser.email, 31, stdin);
    newUser.email[strcspn(newUser.email, "\n")] = '\0';
    printf("\n Enter Username (Max 30 Characters) : ");
    fgets(newUser.username, 31, stdin);
    newUser.username[strcspn(newUser.username, "\n")] = '\0';
    printf("\n Enter Password (Max 10 Characters) : ");
    fgets(newUser.password, 11, stdin);
    newUser.password[strcspn(newUser.password, "\n")] = '\0';
    printf("\n Confirm Password : ");
    fgets(confirmPassword, 11, stdin);
    confirmPassword[strcspn(confirmPassword, "\n")] = '\0';
    if (strcmp(newUser.password, confirmPassword) != 0) {
        printf("\n\n\n Oops! The passwords do not match. Registration failed ");
        fclose(file);
        return;
    }
    fwrite(&newUser, sizeof(user), 1, file);
    fclose(file);
    printf("\n\n\n Registration successful! ");
}

void LoginPortalUser () {
    system("cls");
    FILE *file;
    user tempUser;
    char *inputUsername = (char*) malloc(31 * sizeof(char));
    char *inputPassword = (char*) malloc(11 * sizeof(char));
    int found = 0;
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Login Portal    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/                                                \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/       1. Welcome Back! Sign In                 \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/       2. New to YourMart? Create Account       \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/       0. Exit                                  \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/                                                \\\n");
    printf("\t\t\t\t\\ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ /\n\n\n");
    printf("\t\t\t\t              Enter your choice : ");
    int action;
    scanf("%d", &action);
    getchar();
    if (action == 1) {
        system("cls");
        printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
        printf("\t\t\t\t///////////////    Login Portal    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
        file = fopen("users.dat", "rb");
        if (file == NULL) {
            printf("\n\n\n Error opening file for reading");
            free(inputUsername);
            free(inputPassword);
            getchar();
            return;
        }
        printf("\n\n\n Username : ");
        fgets(inputUsername, 31, stdin);
        inputUsername[strcspn(inputUsername, "\n")] = '\0';
        printf("\n Password : ");
        fgets(inputPassword, 11, stdin);
        inputPassword[strcspn(inputPassword, "\n")] = '\0';
        while (fread(&tempUser, sizeof(user), 1, file)) {
            if (strcmp(tempUser.username, inputUsername) == 0 && strcmp(tempUser.password, inputPassword) == 0) {
                found = 1;
                break;
            }
        }
        fclose(file);
        if (found) {
            char CurrentUsername[31];
            strcpy(CurrentUsername, tempUser.username);
            printf("\n\n\n Login successful! ");
            getchar();
            free(inputUsername);
            free(inputPassword);
            UserPortal(CurrentUsername);
            return;
        } else {
            printf("\n\n\n Invalid username or password ");
            getchar();
        }
    } else if (action == 2) {
        free(inputUsername);
        free(inputPassword);
        RegisterUser();
        getchar();
    }  else if (action == 0) {
        free(inputUsername);
        free(inputPassword);
        return;
    } else {
        system("cls");
        printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
        printf("\t\t\t\t///////////////    Login Portal    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
        printf("\n\n\n Invalid choice. Returning to main menu ");
        free(inputUsername);
        free(inputPassword);
        getchar();
    }
}

void AddProductMassage () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Add  Product    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n");
    printf("\n Product ID Guidelines :\n");
    printf("\n To add a new product, you'll need to create a Product ID. This ID is unique and\n helps link the product to your shop.\n");
    printf("\n Product ID Format :\n");
    printf("\n # The first part is your shop's Code Name (e.g., GRO for Grocery Store).\n # The second part is a number  (up to 3 digits)  that you choose  (e.g. , GRO1,\n GRO35, GRO719).\n");
    printf("\n Important :\n");
    printf("\n # The Product ID is case - sensitive and identifies the product as belonging to\n your shop.\n # Make sure to follow this format correctly!\n");
    printf("\n For example :");
    printf("\n If your shop is Grocery Store with the Code Name  GRO , your Product IDs can be\n GRO1, GRO35, or GRO719.");
    printf("\n\n\n Press Enter to Continue ");
}

void AddProduct () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Add  Product    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    FILE *file = fopen("inventory.txt", "a");
    if (!file) {
        printf("\n\n\n Error opening file!");
        return;
    }
    product newProduct;
    printf("\n\n\n Enter Product ID (Max 6 Characters) : ");
    fgets(newProduct.ID, 9, stdin);
    newProduct.ID[strcspn(newProduct.ID, "\n")] = 0;
    printf("\n Enter Product Name (Max 15 Characters) : ");
    fgets(newProduct.proName, 16, stdin);
    newProduct.proName[strcspn(newProduct.proName, "\n")] = 0;
    printf("\n Enter Product Price : ");
    scanf("%f", &newProduct.proPrice);
    getchar();
    printf("\n Enter Unit (kg/pcs) : ");
    fgets(newProduct.per, 4, stdin);
    newProduct.per[strcspn(newProduct.per, "\n")] = 0;
    printf("\n Enter Quantity : ");
    scanf("%d", &newProduct.quantity);
    getchar();
    fprintf(file, "%s,%s,%.2f,%s,%d\n", newProduct.ID, newProduct.proName, newProduct.proPrice, newProduct.per, newProduct.quantity);
    fclose(file);
    printf("\n\n\n Product added successfully! ");
    initializeHashTable();
    loadInventory();
    getchar();
}

void DisplayMerchantProducts (char *merchantID) {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////   Your Inventory   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    int found = 0;
    printf("\t        --------------------------------------------------------------------------------\n");
    printf("\t        |     ID     |         Name         |    Price    |     Per     |   Quantity   |\n");
    printf("\t        --------------------------------------------------------------------------------\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode *node = hashTable[i];
        while (node != NULL) {
            if (strncmp(node->item.ID, merchantID, 3) == 0) {
                printf("\t        |   %-6s   |   %-16s   |    %-7.2f  |     %-7s |     %-9d|\n",
                       node->item.ID, node->item.proName, node->item.proPrice, node->item.per, node->item.quantity);
                printf("\t        --------------------------------------------------------------------------------\n");
                found = 1;
            }
            node = node->next;
        }
    }
    if (!found) {
        printf("\n\n\n\t        It seems like you haven't added any products yet ");
    }
    printf("\n\n\n\t        Press enter to go back to the menu ");
    getchar();
}

void EditProductMassage () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////   Update Product   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/   You can only update the price and quantity   \\\n");
    printf("\t\t\t\t\\  of your existing products. All other product  /\n");
    printf("\t\t\t\t/    details are fixed and cannot be changed.    \\\n");
    printf("\t\t\t\t\\ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ /\n\n\n");
    printf("\t\t\t\t             Press Enter to Continue ");
}

void UpdateProduct (char *merchantID) {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////   Update Product   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    char productID[9];
    printf(" Enter the Product ID to update : ");
    fgets(productID, sizeof(productID), stdin);
    productID[strcspn(productID, "\n")] = 0;
    product *currentProduct = findInHashTable(productID);
    if (currentProduct) {
        printf("\n\n\n Product found. Enter new details :\n");
        printf("\n Enter New Product Price : ");
        scanf("%f", &currentProduct->proPrice);
        printf("\n Enter New Quantity : ");
        scanf("%d", &currentProduct->quantity);
        getchar();
        FILE *file = fopen("inventory.txt", "r");
        if (!file) {
            printf(" Error opening inventory file!");
            return;
        }
        FILE *tempFile = fopen("temp.txt", "w");
        if (!tempFile) {
            printf(" Error opening temporary file!");
            fclose(file);
            return;
        }
        char line[100];
        while (fgets(line, sizeof(line), file)) {
            product tempProduct;
            sscanf(line, "%[^,],%[^,],%f,%[^,],%d", tempProduct.ID, tempProduct.proName, 
                   &tempProduct.proPrice, tempProduct.per, &tempProduct.quantity);
            if (strcmp(tempProduct.ID, productID) == 0) {
                fprintf(tempFile, "%s,%s,%.2f,%s,%d\n", tempProduct.ID, tempProduct.proName,
                        currentProduct->proPrice, currentProduct->per, currentProduct->quantity);
            } else {
                fprintf(tempFile, "%s,%s,%.2f,%s,%d\n", tempProduct.ID, tempProduct.proName,
                        tempProduct.proPrice, tempProduct.per, tempProduct.quantity);
            }
        }
        fclose(file);
        fclose(tempFile);
        remove("inventory.txt");
        rename("temp.txt", "inventory.txt");
        initializeHashTable();
        loadInventory();
        printf("\n\n\n Product updated successfully! ");
        getchar();
    } else {
        printf("\n\n\n Product ID not found! ");
        getchar();
    }
}

void RemoveProduct () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////   Remove Product   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    char productID[9];
    printf(" Enter the Product ID to remove : ");
    fgets(productID, sizeof(productID), stdin);
    productID[strcspn(productID, "\n")] = '\0';
    product *currentProduct = findInHashTable(productID);
    if (currentProduct) {
        FILE *file = fopen("inventory.txt", "r");
        if (!file) {
            printf(" Error opening inventory file!");
            return;
        }
        FILE *tempFile = fopen("temp.txt", "w");
        if (!tempFile) {
            printf(" Error opening temporary file!");
            fclose(file);
            return;
        }
        char line[100];
        while (fgets(line, sizeof(line), file)) {
            char id[9], name[16], per[4];
            float price;
            int quantity;
            sscanf(line, "%[^,],%[^,],%f,%[^,],%d", id, name, &price, per, &quantity);
            if (strcmp(id, productID) != 0) {
                fprintf(tempFile, "%s,%s,%.2f,%s,%d\n", id, name, price, per, quantity);
            }
        }
        fclose(file);
        fclose(tempFile);
        remove("inventory.txt");
        rename("temp.txt", "inventory.txt");
        initializeHashTable();
        loadInventory();
        printf("\n\n\n Product %s removed successfully! ", currentProduct->proName);
        getchar();
    } else {
        printf("\n\n\n Product ID not found! ");
        getchar();
    }
}

void DisplayParcelsToPack (char *merchantID) {
    FILE *orderFile = fopen("orderhistory.txt", "r");
    if (!orderFile) {
        printf("\n\t        Error opening order history file! ");
        return;
    }
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////   Packing  Queue   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    int flag = 1;
    char line[256];
    char currentMerchantID[4];
    int productFound = 0;
    while (fgets(line, sizeof(line), orderFile)) {
        if (strstr(line, "|")) {
            char productID[7];
            sscanf(line, " |   %6s", productID);
            strncpy(currentMerchantID, productID, 3);
            currentMerchantID[3] = '\0';
            if (strcmp(currentMerchantID, merchantID) == 0) {
                if (flag) {
                    printf("\t        --------------------------------------------------------------------------------\n");
                    printf("\t        |     ID     |         Name         |    Price    |     Per     |   Quantity   |\n");
                    printf("\t        --------------------------------------------------------------------------------\n");
                    flag = 0;
                }
                printf("\t       %s", line);
                printf("\t        --------------------------------------------------------------------------------\n");
                productFound = 1;
            }
        }
        if (strstr(line, "Grand Total")) {
            continue;
        }
    }
    if (!productFound) {
        printf("\n\t        Nothing to Pack Right Now ");
    }
    printf("\n\n\n\t        Press enter to go back to the menu ");
    fclose(orderFile);
}

void CheckMerchantProductReview (char *merchantID) {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////  User Experiences  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    FILE *file = fopen("productreview.txt", "r");
    if (file == NULL) {
        printf(" Error opening file for reading ");
        return;
    }
    char line[151];
    char productID[9];
    char username[31];
    int reviewFound = 0;
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "Username :") != NULL) {
            sscanf(line, " Username : %30s", username);
        }
        if (strstr(line, "Product ID :") != NULL) {
            sscanf(line, " Product ID : %s", productID);
            if (strncmp(productID, merchantID, strlen(merchantID)) == 0) {
                reviewFound = 1;
                printf("%s", line);
                printf(" Username : %s\n", username);
                do {
                    fgets(line, sizeof(line), file);
                    printf("%s", line);
                } while (strcmp(line, " ------------------------------\n") != 0);
                printf("\n");
            }
        }
    }
    if (!reviewFound) {
        printf(" No reviews found for your products ");
    }
    fclose(file);
    printf("\n\n\n Press Enter to go back to Menu ");
    getchar();
}

void MerchantPortal (char *merchantID) {
    while (1) {
        system("cls");
        printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
        printf("\t\t\t\t///////////////  Merchant  Portal  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/                                                \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            1. Add Product                      \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            2. Display Products                 \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            3. Update Existing Product          \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            4. Remove Product                   \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            5. Parcels To Pack                  \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            6. Check Reviews                    \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            0. Sign Out                         \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/                                                \\\n");
        printf("\t\t\t\t\\ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ /\n\n\n");
        printf("\t\t\t\t              Enter your choice : ");
        int choice;
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1 :
                AddProductMassage();
                getchar();
                AddProduct();
                break;
            case 2 :
                DisplayMerchantProducts(merchantID);
                break;
            case 3 :
                EditProductMassage();
                getchar();
                UpdateProduct(merchantID);
                break;
            case 4 :
                RemoveProduct();
                break;
            case 5:
                DisplayParcelsToPack(merchantID);
                getchar();
                break;
            case 6 :
                CheckMerchantProductReview(merchantID);
                break;
            case 0 :
                return;
            default :
                printf("\n\t\t\t        Oops! That wasn't a valid option. Please try again ");
                getchar();
                break;
        }
    }
}

void ShopNameMassage () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Registration    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n");
    printf("\n In this section , you need to set a Shop Code Name for your shop . This will be\n used in creating unique product IDs, so it is important to choose carefully.\n");
    printf("\n Requirements :\n");
    printf("\n # The Shop Code Name must be exactly three uppercase letters (A-Z).\n # It should represent your shop  name . For example , if your shop  is  Grocery\n Store, a good Code Name could be GRO.\n");
    printf("\n Important :\n");
    printf("\n # The Code Name is case-sensitive and will be part of your product IDs.\n # Once set, it cannot be changed.");
    printf("\n\n\n Press Enter to Continue ");
}

void RegisterMerchant () {
    system("cls");
    FILE *file;
    merchant newMerchant;
    char confirmPassword[11];
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Registration    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    file = fopen("merchant.dat", "ab");
    if (file == NULL) {
        printf("\n\n\n Error opening file for writing");
        return;
    }
    printf("\n\n\n Enter Shop Name (Max 50 Characters) : ");
    fgets(newMerchant.shopName, 51, stdin);
    newMerchant.shopName[strcspn(newMerchant.shopName, "\n")] = '\0';
    printf("\n Enter Shop Code Name (3 Characters) : ");
    fgets(newMerchant.shortname, 6, stdin);
    newMerchant.shortname[strcspn(newMerchant.shortname, "\n")] = '\0';
    printf("\n Enter Username (Max 30 Characters) : ");
    fgets(newMerchant.username, 31, stdin);
    newMerchant.username[strcspn(newMerchant.username, "\n")] = '\0';
    printf("\n Enter Password (Max 10 Characters) : ");
    fgets(newMerchant.password, 11, stdin);
    newMerchant.password[strcspn(newMerchant.password, "\n")] = '\0';
    printf("\n Confirm Password : ");
    fgets(confirmPassword, 11, stdin);
    confirmPassword[strcspn(confirmPassword, "\n")] = '\0';
    if (strcmp(newMerchant.password, confirmPassword) != 0) {
        printf("\n\n\n Passwords do not match. Registration failed ");
        fclose(file);
        return;
    }
    fwrite(&newMerchant, sizeof(merchant), 1, file);
    fclose(file);
    printf("\n\n\n Registration successful! ");
}

void LoginPortalMerchant () {
    system("cls");
    FILE *file;
    merchant tempMerchant;
    char *inputUsername = (char*) malloc(31 * sizeof(char));
    char *inputPassword = (char*) malloc(11 * sizeof(char));
    int found = 0;
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Login Portal    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/                                                \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/          1. Welcome Back! Sign In              \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/          2. New Merchant? Register Now         \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/          0. Exit                               \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/                                                \\\n");
    printf("\t\t\t\t\\ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ /\n\n\n");
    printf("\t\t\t\t              Enter your choice : ");
    int action;
    scanf("%d", &action);
    getchar();
    if (action == 1) {
        system("cls");
        printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
        printf("\t\t\t\t///////////////    Login Portal    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
        file = fopen("merchant.dat", "rb");
        if (file == NULL) {
            printf("\n\n\n Error opening file for reading");
            free(inputUsername);
            free(inputPassword);
            getchar();
            return;
        }
        printf("\n\n\n Username : ");
        fgets(inputUsername, 31, stdin);
        inputUsername[strcspn(inputUsername, "\n")] = '\0';
        printf("\n Password : ");
        fgets(inputPassword, 11, stdin);
        inputPassword[strcspn(inputPassword, "\n")] = '\0';
        while (fread(&tempMerchant, sizeof(merchant), 1, file)) {
            if (strcmp(tempMerchant.username, inputUsername) == 0 && strcmp(tempMerchant.password, inputPassword) == 0) {
                found = 1;
                break;
            }
        }
        fclose(file);
        if (found) {
            char CurrentMerchantShortName[4];
            strcpy(CurrentMerchantShortName, tempMerchant.shortname);
            printf("\n\n\n Login successful! ");
            getchar();
            free(inputUsername);
            free(inputPassword);
            MerchantPortal(CurrentMerchantShortName);
            free(inputUsername);
            free(inputPassword);
            return;
        } else {
            printf("\n\n\n Invalid username or password ");
            getchar();
        }
    } else if (action == 2) {
        free(inputUsername);
        free(inputPassword);
        ShopNameMassage();
        getchar();
        RegisterMerchant();
        getchar();
    }  else if (action == 0) {
        free(inputUsername);
        free(inputPassword);
        return;
    } else {
        system("cls");
        printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
        printf("\t\t\t\t///////////////    Login Portal    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
        printf("\n\n\n Invalid choice. Returning to main menu");
        free(inputUsername);
        free(inputPassword);
        getchar();
    }
}

void UserList () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////   Customer  List   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    FILE *file = fopen("users.dat", "rb");
    if (file == NULL) {
        printf(" Error opening file for reading");
        return;
    }
    int temp = 1;
    if (temp == 1) {
        printf(" ----------------------------\n");
        temp = 0;
    }
    user tempUser;
    while (fread(&tempUser, sizeof(user), 1, file)) {
        printf(" Email    : %s\n", tempUser.email);
        printf(" Username : %s\n", tempUser.username);
        //printf(" Password : %s\n", tempUser.password);
        printf(" ----------------------------\n");
    }
    fclose(file);
    printf("\n\n\n Press Enter Back to Admin Menu ");
    getchar();
}

void MerchantList () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////   Merchant  List   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    FILE *file = fopen("merchant.dat", "rb");
    if (file == NULL) {
        printf(" Error opening file for reading");
        return;
    }
    int temp = 1;
    if (temp == 1) {
        printf(" -------------------------------\n");
        temp = 0;
    }
    merchant tempMerchant;
    while (fread(&tempMerchant, sizeof(merchant), 1, file)) {
        printf(" Shop Name : %s\n", tempMerchant.shopName);
        printf(" Username  : %s\n", tempMerchant.username);
        printf(" Code Name : %s\n", tempMerchant.shortname);
        //printf(" Password : %s\n", tempMerchant.password);
        printf(" -------------------------------\n");
    }
    fclose(file);
    printf("\n\n\n Press Enter Back to Admin Menu ");
    getchar();
}

void RemoveUser () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Remove  User    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    printf("\n 1. Enter Customer Username");
    printf("\n 0. Back to the Admin Portal");
    printf("\n\n\n Enter your choice : ");
    int choice;
    scanf("%d", &choice);
    getchar();
    if (choice == 1) {
        FILE *file = fopen("users.dat", "rb");
        FILE *tempFile = fopen("temp.dat", "wb");
        if (file == NULL || tempFile == NULL) {
            printf(" Error opening file.");
            return;
        }
        user tempUser;
        char username[31];
        int found = 0;
        printf("\n\n Username : ");
        fgets(username, 31, stdin);
        username[strcspn(username, "\n")] = '\0';
        while (fread(&tempUser, sizeof(user), 1, file)) {
            if (strcmp(tempUser.username, username) != 0) {
                fwrite(&tempUser, sizeof(user), 1, tempFile);
            } else {
                found = 1;
            }
        }
        fclose(file);
        fclose(tempFile);
        remove("users.dat");
        rename("temp.dat", "users.dat");
        if (found) {
            printf("\n\n User removed successfully ");
            getchar();
        } else {
            printf("\n\n User not found ");
            getchar();
        }
    } else if (choice == 0) {
        return;
    } else {
        printf("\n Oops! That wasn't a valid option. Please try again ");
        getchar();
    }
}

void RemoveMerchant () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////  Remove  Merchant  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    printf("\n 1. Enter Merchant Username");
    printf("\n 0. Back to the Admin Portal");
    printf("\n\n\n Enter your choice : ");
    int choice;
    scanf("%d", &choice);
    getchar();
    if (choice == 1) {
        FILE *file = fopen("merchant.dat", "rb");
        FILE *tempFile = fopen("temp.dat", "wb");
        if (file == NULL || tempFile == NULL) {
            printf(" Error opening file");
            return;
        }
        merchant tempMerchant;
        char username[31];
        int found = 0;
        printf("\n\n Username : ");
        fgets(username, 31, stdin);
        username[strcspn(username, "\n")] = '\0';
        while (fread(&tempMerchant, sizeof(merchant), 1, file)) {
            if (strcmp(tempMerchant.username, username) != 0) {
                fwrite(&tempMerchant, sizeof(merchant), 1, tempFile);
            } else {
                found = 1;
            }
        }
        fclose(file);
        fclose(tempFile);
        remove("merchant.dat");
        rename("temp.dat", "merchant.dat");
        if (found) {
            printf("\n\n Merchant removed successfully ");
            getchar();
        } else {
            printf("\n\n Merchant not found ");
            getchar();
        }
    } else if (choice == 0) {
        return;
    } else {
        printf("\n Oops! That wasn't a valid option. Please try again ");
        getchar();
    }
}

void RemoveOrder (char* phoneNumber) {
    FILE *orderHistory, *tempFile;
    char line[256];
    int skip = 0;
    orderHistory = fopen("orderhistory.txt", "r");
    tempFile = fopen("temp.txt", "w");
    if (orderHistory == NULL || tempFile == NULL) {
        printf("\n Error opening file!");
        return;
    }
    while (fgets(line, sizeof(line), orderHistory)) {
        if (strstr(line, "Phone") && strstr(line, phoneNumber)) {
            skip = 1;
            do {
                fgets(line, sizeof(line), orderHistory);
            } while (strstr(line, "===") == NULL);
            continue;
        }
        if (!skip) {
            fputs(line, tempFile);
        } else {
            skip = 0;
        }
    }
    fclose(orderHistory);
    fclose(tempFile);
    remove("orderhistory.txt");
    rename("temp.txt", "orderhistory.txt");
}

void ConfirmOrder (char* phoneNumber) {
    FILE *orderHistory, *confirmedOrders;
    char line[256];
    int found = 0;
    orderHistory = fopen("orderhistory.txt", "r");
    confirmedOrders = fopen("confirmedorders.txt", "r+");
    if (orderHistory == NULL || confirmedOrders == NULL) {
        printf("\n Error opening file!");
        return;
    }
    char orderDetails[5000] = "\n # Order Confirmed #\n\n";
    while (fgets(line, sizeof(line), orderHistory)) {
        if (strstr(line, "Phone") && strstr(line, phoneNumber)) {
            found = 1;
            do {
                strcat(orderDetails, line);
                fgets(line, sizeof(line), orderHistory);
            } while (strstr(line, "===") == NULL);
            strcat(orderDetails, line);
            break;
        }
    }
    if (found) {
        fseek(confirmedOrders, 0, SEEK_SET);
        char allOrders[5000] = ""; 
        while (fgets(line, sizeof(line), confirmedOrders)) {
            strcat(allOrders, line);
        }
        freopen("confirmedorders.txt", "w", confirmedOrders);
        fputs(orderDetails, confirmedOrders);
        fputs(allOrders, confirmedOrders);
        printf("\n Order Confirmed! ");
    } else {
        printf("\n Order not found ");
    }
    fclose(orderHistory);
    fclose(confirmedOrders);
}

void PendingOrdersMenu () {
    int choice;
    char phoneNumber[13];
    FILE *orderHistory;
    char line[256];
    int hasOrders = 0;
    orderHistory = fopen("orderhistory.txt", "r");
    if (orderHistory == NULL) {
        printf("\n Error opening file!");
        return;
    }
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////  Record & Details  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    while (fgets(line, sizeof(line), orderHistory)) {
        if (strcmp(line, "\n") != 0) {
            printf("%s", line);
            hasOrders = 1;
        }
    }
    fclose(orderHistory);
    if (!hasOrders) {
        printf("\n No pending orders");
        printf("\n\n\n Press Enter Back to Admin Menu ");
        getchar();
        return;
    }
    printf("\n 1. Confirm an order");
    printf("\n 0. Back to the menu");
    printf("\n\n Enter your choice : ");
    int action;
    scanf("%d", &action);
    getchar();
    switch (action) {
        case 1:
            printf("\n Enter customer phone number : ");
            scanf("%s", phoneNumber);
            getchar();
            if (strlen(phoneNumber) < 11) {
                printf("\n Invalid phone number");
                getchar();
                break;
            } 
            ConfirmOrder(phoneNumber);
            RemoveOrder(phoneNumber);
            getchar();
            break;
        case 0:
            return;
        default:
            printf("\n Oops! That wasn't a valid option. Please try again ");
            getchar();
    }
}

void ConfirmedOrders () {
    FILE *confirmedFile = fopen("ConfirmedOrders.txt", "r");
    if (!confirmedFile) {
        printf(" Error opening Confirmed Orders file!");
        return;
    }
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////  Confirmed Orders  \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n");
    char line[200];
    int isOrderPresent = 0;
    while (fgets(line, sizeof(line), confirmedFile)) {
        printf("%s", line);
        isOrderPresent = 1;
    }
    if (!isOrderPresent) {
        printf(" No confirmed orders found!");
    }
    fclose(confirmedFile);
    printf("\n\n\n Press Enter Back to Admin Menu ");
    getchar();
}

void CheckFeedback () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t/////////////// Feedback  Overview \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    FILE *file = fopen("feedback.txt", "r");
    if (file == NULL) {
        printf(" Error opening file for reading");
        return;
    }
    char line[301];
    while (fgets(line, 301, file)) {
        printf("%s", line);
    }
    fclose(file);
    printf("\n\n\n Press Enter Back to Admin Menu ");
    getchar();
}

void AdminPortal () {
    while (1) {
        system("cls");
        printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
        printf("\t\t\t\t///////////////    Admin  Portal   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/                                                \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            1. View All Customers               \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            2. View All Merchants               \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            3. Remove a Customer                \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            4. Remove a Merchant                \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            5. Pending Orders                   \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            6. Confirmed Orders                 \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            7. Review Product Feedback          \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            8. Check System Feedback            \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/            0. Sign Out                         \\\n");
        printf("\t\t\t\t\\                                                /\n");
        printf("\t\t\t\t/                                                \\\n");
        printf("\t\t\t\t\\ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ /\n\n\n");
        printf("\t\t\t\t              Enter your choice : ");
        int choice;
        scanf("%d", &choice);
        getchar();
        switch (choice) {
            case 1 :
                UserList();
                break;
            case 2 :
                MerchantList();
                break;
            case 3 :
                RemoveUser();
                break;
            case 4 :
                RemoveMerchant();
                break;
            case 5 :
                PendingOrdersMenu();
                break;
            case 6 :
                ConfirmedOrders();
                break;
            case 7 :
                CheckProductReview();
                break;
            case 8 :
                CheckFeedback();
                break;
            case 0 :
                printf("\n\t\t\t                  Returning to the Main Menu...");
                return;
            default :
                printf("\n\t\t\t        Oops! That wasn't a valid option. Please try again ");
                getchar();
                break;
        }    
    }
}

void LoginPortalAdmin () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Login Portal    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n\n\n");
    char *username, *password;
    username = (char *)malloc(11 * sizeof(char));
    password = (char *)malloc(11 * sizeof(char));
    if (username == NULL || password == NULL) {
        printf(" Memory allocation failed\n");
        free(username);
        free(password);
        getchar();
        return;
    }
    printf("\t\t\t\t          Username : ");
    fgets(username, 11, stdin);
    int len = strlen(username);
    if (len > 0 && username[len - 1] == '\n') {
        username[len - 1] = '\0';
    }
    printf("\n\n\t\t\t\t          Password : ");
    fgets(password, 11, stdin);
    len = strlen(password);
    if (len > 0 && password[len - 1] == '\n') {
        password[len - 1] = '\0';
    }
    if (strcmp(username, "Shadul") == 0 && strcmp(password, "24115271") == 0) {
        AdminPortal();
        return;
    }
    else {
        printf("\n\n\n Admin access denied! Make sure your username and password are correct.");
        getchar();
    }
    free(username);
    free(password);
}

void savefeedback () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Share & Shape   \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    printf("\n 1. Give Review");
    printf("\n 0. Back To Menu");
    printf("\n\n\n Enter your choice : ");
    int choice;
    scanf("%d", &choice);
    getchar();
    if (choice == 1) {
        FILE *file;
        char *username, *feedback;
        username = (char *)malloc(31 * sizeof(char));
        feedback = (char *)malloc(301 * sizeof(char));
        if (username == NULL || feedback == NULL) {
            printf("\n\n Memory allocation failed");
            free(username);
            free(feedback);
            return;
        }
        file = fopen("feedback.txt", "a");
        if (file == NULL) {
            printf("\n\n Error opening file for writing");
            free(username);
            free(feedback);
            return;
        }
        printf("\n\n What Should We Call You (Max 30 Characters) : ");
        fgets(username, 31, stdin);
        int len = strlen(username);
        username[strcspn(username, "\n")] = '\0';
        if (strlen(username) == 0) {
            printf("\n\n\n Username cannot be empty. Press Enter Back to Main Menu ");
            fclose(file);
            free(username);
            free(feedback);
            getchar();
            return;
        }
        printf("\n Share Your Thoughts & Ideas with Us (300 Characters Max) :\n\n ");
        fgets(feedback, 301, stdin);
        len = strlen(feedback);
        if (len > 0 && feedback[len - 1] == '\n') {
            feedback[len - 1] = '\0';
        }
        if (strlen(feedback) == 0) {
            printf("\n\n Feedback cannot be empty. Press Enter Back to Main Menu ");
            fclose(file);
            free(username);
            free(feedback);
            getchar();
            return;
        }
        fprintf(file, " Username : %s\n Feedback : %s\n\n", username, feedback);
        fprintf(file, " ------------------------------\n\n");
        printf("\n\n\n We value your input, %s. Thanks for your feedback!", username);
        getchar();
        fclose(file);
        free(username);
        free(feedback);
    } else if (choice == 0) {
        return;
    } else {
        printf("\n Oops! That wasn't a valid option. Please try again ");
        getchar();
    }
}

void exitmenu () {
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////    Exit  Portal    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    char quit;
    printf("\n\n\n Are you sure? Your cart won't shop itself! (y or n) : ");
    scanf("%c",&quit);
    if (quit == 'Y' || quit == 'y') {
        printf("\n Thanks for visiting! See you next time!");
        Sleep(3000);
        printf("\n\n\n\n");
        exit(0);
    } else {
        return;
    }
}

void mainmenu () {
    system("color F0");
    system("cls");
    printf("\n\n\t\t\t\t\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      YourMart      ///////////////\n");
    printf("\t\t\t\t///////////////     Main  Menu     \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/                                                \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/            1. Shop & Discover                  \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/            2. Manage Your Store                \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/            3. Admin Panel                      \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/            4. Feedback & Suggestions           \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/            0. Exit & Close                     \\\n");
    printf("\t\t\t\t\\                                                /\n");
    printf("\t\t\t\t/                                                \\\n");
    printf("\t\t\t\t\\ _ _ _ _ _ _ _ _ _ _ _ __ _ _ _ _ _ _ _ _ _ _ _ /\n\n\n");
    printf("\t\t\t\t       Your shopping adventure starts here!\n\n");
    printf("\t\t\t\t              Enter your choice : ");
}

int main () {
    initializeHashTable();
    loadInventory();
    initializeNameHashTable();
    loadNameHashTable();
    while (1) {
        mainmenu();
        int action;
        scanf("%d", &action);
        getchar();
        switch (action) {
            case 1 :
                LoginPortalUser();
                break;
            case 2 :
                LoginPortalMerchant();
                break;
            case 3 :
                LoginPortalAdmin();
                break;
            case 4 :
                savefeedback();
                break;
            case 0 :
                exitmenu();
                break;
            default :
                printf("\n\t\t\t        Oops! That wasn't a valid option. Please try again ");
                getchar();
                break;
        }
    }
    return 0;  
}