#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Change these to change the number of items auto-generated
// customers will have on their shopping list
// defined as macros so they can be used to size arrays
#define ITEMS_LOW   1	
#define ITEMS_HIGH  5
#define BUDGET_LOW 	100
#define BUDGET_HIGH 400
#define PIECES_LOW  1
#define PIECES_HIGH 100

char STOCK_PATH[] = "../stock.csv";
char NAMES_PATH[] = "../names.list";
char FACES_PATH[] = "../faces";
char CUSTOMERS_PATH[] = "../customer.csv";
size_t BUDGET_RANGE[] = {100, 400};



size_t PIECES_RANGE[] = {1, 100};
size_t REORDER_THRESHOLD = 5;

char shopkeeper[] = "       _www_ \n"
                    "      /-o-o-\\  \n"
					"    (|   -   |)\n"
					"      \\ -=- /  \n"
					"      /`---'\\";

void clear_console() 
{
	for (int i = 0; i < 100; i++)
	{
  		printf ("\n");
	}
}	

struct Product {
	char* name;
	double price;
	size_t max_quantity;
};

struct ProductStock {
	struct Product product;
	size_t quantity;
};

struct Shop {
	double cash;
	struct ProductStock stock[24];
	size_t index;
};

struct Customer {
	char* name;
	char* face;
	double budget;
	struct ProductStock shoppingList[ITEMS_HIGH];
	size_t sl_index;
	struct ProductStock receipt[ITEMS_HIGH];
	size_t r_index;
};

struct Shop generate_shop()
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("../stock.csv", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

	read = getline(&line, &len, fp);
	float cash = atof(line);
	
	struct Shop shop = { cash };

    while ((read = getline(&line, &len, fp)) != -1) {
		char *n = strtok(line, ",");
		char *p = strtok(NULL, ",");
		char *q = strtok(NULL, ",");
		int quantity = atoi(q);
		double price = atof(p);
		char * name = malloc(sizeof(char) * 50);
		strcpy(name, n);
		struct Product product = { name, price };
		struct ProductStock stockItem = { product, quantity };
		shop.stock[shop.index++] = stockItem;
    }
	
	return shop;
}

// Detect lines containing only whitespace
int all_space(const char *str) {
    while (*str) {
        if (!isspace(*str++)) {
            return 0;
        }
    }
    return 1;
}

// Return a random name from the NAMES_PATH file
char* get_name()
{
	// Select a random name from the names file
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(NAMES_PATH, "r");
	if (fp == NULL) {
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
	}

	char * names[200][12];
	for (int i = 0; i < 200; i++) {
		char *name = malloc(sizeof(char) * 12);
		fgets(name, 12, fp);
		names[i][0] = name;
	}

	// close file handle
	fclose(fp);

	return names[rand() % 200][0];
}

// Return a random face from the FACES_PATH file
char* get_face()
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(FACES_PATH, "r");
	if (fp == NULL) {
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
	}

	char *faces[19];
	int i = 0;
	int append = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
		char *face = malloc(sizeof(char) * 50);
		strcpy(face, line);

		if (all_space(face) == 0 && append == 0) {
			append = 1; 
			faces[i] = face;
		} else if (all_space(face) == 1 && append == 1) {
			append = 0; 
			i++;
		} else if (append == 1) {
			strcat(faces[i], face);
		}
	}

	// close file handle
	fclose(fp);

	return faces[rand() % 19];
}

struct Customer generate_customer(struct Shop shop, int b_low, int b_high, 
													int i_low, int i_high, 
													int p_low, int p_high)

{
	struct Customer customer;
	customer.name = get_name();
	customer.face = get_face();

	customer.budget = (double) (rand() % (b_high - b_low + 1) + b_low);
	int num_items = (rand() % (i_high - i_low + 1) + i_low);
	int num_pieces = (rand() % (p_high - p_low + 1) + p_low);
	
	for (int i = 0; i < num_items; i++) {
		int index = rand() % shop.index;
		customer.shoppingList[i] = shop.stock[index];
		customer.shoppingList[i].quantity = num_pieces;
	}

	return customer;
}

char* stringify_product(struct Product product)
{
	char *string = malloc(sizeof(char) * 20);
	int _ = sprintf(string, "%-15s%7.2f", product.name, product.price);

	return string;
}

char* stringify_shop(struct Shop shop)
{
	char *string = malloc(sizeof(char) * 200);
}

// void printProduct(struct Product p)
// {
// 	printf("PRODUCT NAME: %s \nPRODUCT PRICE: %.2f\n", p.name, p.price);
// 	printf("-------------\n");
// }

// void printCustomer(struct Customer c)
// {
// 	printf("CUSTOMER NAME: %s \nCUSTOMER BUDGET: %.2f\n", c.name, c.budget);
// 	printf("-------------\n");
// 	for(int i = 0; i < c.index; i++)
// 	{
// 		printProduct(c.shoppingList[i].product);
// 		printf("%s ORDERS %d OF ABOVE PRODUCT\n", c.name, c.shoppingList[i].quantity);
// 		double cost = c.shoppingList[i].quantity * c.shoppingList[i].product.price; 
// 		printf("The cost to %s will be €%.2f\n", c.name, cost);
// 	}
// }

// struct Shop createAndStockShop()
// {
//     FILE * fp;
//     char * line = NULL;
//     size_t len = 0;
//     ssize_t read;

//     fp = fopen("../stock.csv", "r");
//     if (fp == NULL)
//         exit(EXIT_FAILURE);

// 	read = getline(&line, &len, fp);
// 	float cash = atof(line);
// 	// printf("cash in shop is %.2f\n", cash);
	
// 	struct Shop shop = { cash };

//     while ((read = getline(&line, &len, fp)) != -1) {
//         // printf("Retrieved line of length %zu:\n", read);
//         // printf("%s IS A LINE", line);
// 		char *n = strtok(line, ",");
// 		char *p = strtok(NULL, ",");
// 		char *q = strtok(NULL, ",");
// 		int quantity = atoi(q);
// 		double price = atof(p);
// 		char *name = malloc(sizeof(char) * 50);
// 		strcpy(name, n);
// 		struct Product product = { name, price };
// 		struct ProductStock stockItem = { product, quantity };
// 		shop.stock[shop.index++] = stockItem;
// 		// printf("NAME OF PRODUCT %s PRICE %.2f QUANTITY %d\n", name, price, quantity);
//     }
	
// 	return shop;
// }

// void printShop(struct Shop s)
// {
// 	printf("Shop has %.2f in cash\n", s.cash);
// 	for (int i = 0; i < s.index; i++)
// 	{
// 		printProduct(s.stock[i].product);
// 		printf("The shop has %d of the above\n", s.stock[i].quantity);
// 	}
// }

int main(void) 
{
	// Set random seed
	srand (time(NULL));
	printf("%s\n", shopkeeper);
	struct Shop shop = generate_shop();
	printf("Cashola: %f\n", shop.cash);
	// generate_customer();
	// struct Shop shop = createAndStockShop();
	// printShop(shop);
	struct Customer customer = generate_customer(shop, BUDGET_LOW, BUDGET_HIGH, 
													   ITEMS_LOW, ITEMS_HIGH,
													   PIECES_LOW, PIECES_HIGH);
	printf("%s\n", customer.name);
	printf("%s\n", customer.face);

	printf(stringify_product(customer.shoppingList[0].product));
    return 0;
	
}