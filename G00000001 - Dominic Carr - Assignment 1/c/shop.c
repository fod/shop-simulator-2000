#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Change these to change the number of items auto-generated
// customers will hav on their shopping list
// defined as macros so they can be used to size arrays
#define ITEMS_RANGE_LOW  1	
#define ITEMS_RANGE_HIGH 5

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
	struct ProductStock shoppingList[ITEMS_RANGE_HIGH];
	size_t sl_index;
	struct ProductStock receipt[ITEMS_RANGE_HIGH];
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

int all_space(const char *str) {
    while (*str) {
        if (!isspace(*str++)) {
            return 0;
        }
    }
    return 1;
}

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

	srand ( time(NULL) );
	return names[rand() % 200][0];
}

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
	srand ( time(NULL) );
	return faces[rand() % 19];
}

struct Customer generate_customer()
	// struct Shop shop, int[] BUDGET_RANGE, 
	// 							  char* NAMES_PATH, int items_range_low, 
	// 							  int items_range_high, int[] pieces_range)	
{
	struct Customer customer;





	//printf("%s", faces[5]);

	


	// while ((read = getline(&line, &len, fp)) != -1) {
		
	// 	printf("%s", line);

	// 	char *name = malloc(sizeof(char) * 12);
	// 	strcpy(name, line);
	// 	printf("%s", name);
	// 	return customer;
	// }

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

	printf("%s\n", shopkeeper);
	struct Shop shop = generate_shop();
	printf("Cashola: %f\n", shop.cash);
	// generate_customer();
	// struct Shop shop = createAndStockShop();
	// printShop(shop);
	printf("%s\n", get_name());
	printf("%s\n", get_face());
    return 0;
	
}