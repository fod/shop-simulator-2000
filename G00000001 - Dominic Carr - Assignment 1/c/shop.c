#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Global Variables
// Adjust auto-generated customer parameters here
#define ITEMS_LOW   1	
#define ITEMS_HIGH  5
#define BUDGET_LOW 	100
#define BUDGET_HIGH 400
#define PIECES_LOW  1
#define PIECES_HIGH 100
size_t REORDER_THRESHOLD = 5;

// Paths to data files
char STOCK_PATH[] = "../stock.csv";
char NAMES_PATH[] = "../names.list";
char FACES_PATH[] = "../faces";
char CUSTOMERS_PATH[] = "../customer.csv";

char LINE[] = "------------------------------------\n";
// The shopkeeper's face
char shopkeeper[] = "       _www_ \n"
                    "      /-o-o-\\  \n"
					"    (|   -   |)\n"
					"      \\ -=- /  \n"
					"      /`---'\\";

// Structs representing the shop, products, stock and customers
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

// Clear the console by printing 100 newlines
void clear_console() 
{
	for (int i = 0; i < 100; i++)
	{
  		printf ("\n");
	}
}	

// Wait for user to press enter
void wait_for_input(char* message)
{
	printf("%s", message);
	char enter = 0;
	while (enter != '\r' && enter != '\n') { enter = getchar(); }
}

struct Shop generate_shop()
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(STOCK_PATH, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

	read = getline(&line, &len, fp);
	float cash = atof(line);
	
	struct Shop shop = { cash };

	shop.index = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
		char *n = strtok(line, ",");
		char *p = strtok(NULL, ",");
		char *q = strtok(NULL, ",");
		int quantity = atoi(q);
		double price = atof(p);
		char *name = malloc(sizeof(char) * 50);
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
		// Remove newline character from name
		name[strcspn(name, "\n")] = 0;
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
		char *face = malloc(sizeof *face * 50);
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

struct Customer generate_customer(struct Shop shop, 
								  int b_low, int b_high, 
								  int i_low, int i_high, 
								  int p_low, int p_high)

{
	struct Customer customer;
	customer.name = get_name();
	customer.face = get_face();

	customer.budget = (double) (rand() % (b_high - b_low + 1) + b_low);
	int num_items = (rand() % (i_high + 1 - i_low) + i_low);
	
	customer.sl_index = num_items;
	for (int i = 0; i < num_items; i++) {
		int index = rand() % shop.index;
		int num_pieces = (rand() % (p_high  + 1 - p_low) + p_low);
		customer.shoppingList[i] = shop.stock[index];
		customer.shoppingList[i].quantity = num_pieces;
	}
	customer.r_index = 0;

	return customer;
}

char* stringify_product(struct Product product)
{
	char *string = malloc(sizeof *string * 40);
	int _ = sprintf(string, "%-14s%7.2f", product.name, product.price);

	return string;
}

char* stringify_list(struct ProductStock *itemlist, int max_idx)
{
	char *string = malloc(sizeof *string * 20 * (max_idx + 5));

	strcpy(string, LINE);
	strcat(string, "    Item            Price   Quantity\n");
	strcat(string, LINE);

	for (int i = 0; i < max_idx; i++) {
		char *row = malloc(sizeof *row * 40);
		sprintf(row, "%2d. %s%11d", i + 1, stringify_product(itemlist[i].product), itemlist[i].quantity);
		strcat(string, row);
		strcat(string, "\n");
	}
	strcat(string, LINE);

	return string;
}

char* stringify_shop(struct Shop shop)
{
	char *string = stringify_list(shop.stock, shop.index);
	char *cash = malloc(sizeof *cash * 40);
	sprintf(cash, "Cash: %30.2f\n", shop.cash);
	strcat(string, cash);
	strcat(string, LINE); 

	return string;
}

double total_bill(struct Customer customer)
{
	double total = 0;
	for (int i = 0; i <= customer.sl_index-1; i++) {
		total += (customer.shoppingList[i].product.price * customer.shoppingList[i].quantity);
	}
	return total;
}

char* stringify_bill(struct Customer customer)
{
	char *string = stringify_list(customer.shoppingList, customer.sl_index);
	double total = total_bill(customer);
	char *cash = malloc(sizeof *cash * 40);
	sprintf(cash, "Total Cost: %24.2f\n", total);
	strcat(string, cash);
	strcat(string, LINE); 

	return string;

}

char* stringify_customer(struct Customer customer)
{
	char *string = customer.face;
	char *name = malloc(sizeof *name * 30);
	sprintf(name, "\nName: %s", customer.name);
	strcat(string, name);
	char *budget = malloc(sizeof *budget * 10);
	sprintf(budget, "\nBudget: %7.2f\n", customer.budget);
	strcat(string, budget);
	strcat(string, stringify_bill(customer));

	return string;
}

void transact(struct Shop *shop, struct Customer *customer)
{
	for (int i = 0; i <= customer->sl_index-1; i++) {
		for (int j = 0; j < shop->index; j++) {
			if (strcmp(customer->shoppingList[i].product.name, shop->stock[j].product.name) == 0) {
				// shop.stock[j].quantity -= customer.shoppingList[i].quantity;
				if (customer->shoppingList[i].quantity > shop->stock[j].quantity) {
					printf("%s wants %d x %s but only %d are available.\n", customer->name, 
					customer->shoppingList[i].quantity, customer->shoppingList[i].product.name, 
					shop->stock[j].quantity);
					customer->shoppingList[i].quantity = shop->stock[j].quantity;
				} 
				if (customer->budget < (customer->shoppingList[i].product.price * customer->shoppingList[i].quantity)) {
					customer->shoppingList[i].quantity = (int)(customer->budget / customer->shoppingList[i].product.price);

					if (customer->shoppingList->quantity == 0) {
						printf("%s can't afford any %s.\n", customer->name, customer->shoppingList[i].product.name);
					}
					else {
						printf("%s can only afford %d x %s.\n", customer->name, 
						customer->shoppingList[i].quantity, customer->shoppingList[i].product.name);
					}
				}
				printf("%s buys %d x %s for %.2f.\n", customer->name, customer->shoppingList[i].quantity, 
				customer->shoppingList[i].product.name, 
				(customer->shoppingList[i].product.price * customer->shoppingList[i].quantity));
				shop->stock->quantity -= customer->shoppingList[i].quantity;
				shop->cash += (customer->shoppingList[i].product.price * customer->shoppingList[i].quantity);
				customer->budget -= (customer->shoppingList[i].product.price * customer->shoppingList[i].quantity);
				customer->receipt[customer->r_index] = customer->shoppingList[i];
				customer->r_index++;
				printf("%s has \u20AC%.2f left.\n\n", customer->name, customer->budget);
			}
		}
	}
	shop->cash += total_bill(*customer);
	customer->budget -= total_bill(*customer);
	wait_for_input("Press enter to continue.");
	clear_console();
	printf("%s\n", shopkeeper);
	printf("Thank you %s. Here's your receipt. \nPlease come again.\n\n", customer->name);
	printf("%s's Receipt:\n", customer->name);
	printf("%s\n", stringify_bill(*customer));
}

void restock(struct Shop *shop, struct Product product)
{
	for (int i = 0; i < shop->index; i++) {
		if (strcmp(shop->stock[i].product.name, product.name) == 0) {
			int current_quantity = shop->stock[i].quantity;
			shop->stock[i].quantity = shop->stock[i].product.max_quantity;
			shop->cash -= product.price * (product.max_quantity - current_quantity);
		}
	}
}

char* check_stock(struct Shop shop, int reorder_threshold)
{
	char *out_of_stock[shop.index]; //= malloc(sizeof *out_of_stock * 100);
	for (int i=0; i < shop.index; i++) {
		if (shop.stock[i].quantity < reorder_threshold) {
			out_of_stock[i] = shop.stock[i].product.name;
			restock(&shop, shop.stock[i].product);
		}
	}
	return out_of_stock;
}

void auto_mode()
{
	struct Shop shop = generate_shop();
	while(1) {
		clear_console();
		struct Customer customer = generate_customer(shop, BUDGET_LOW, BUDGET_HIGH,
													 ITEMS_LOW, ITEMS_HIGH,
													 PIECES_LOW, PIECES_HIGH);
		printf("%s has come into the shop with a shopping list!\n", customer.name);
		printf("%s", stringify_customer(customer));
		wait_for_input("Press Enter to begin transaction.");
		clear_console();
		printf("%s\n", customer.face);
		printf("%s is shopping.\n", customer.name);
		transact(&shop, &customer);
		wait_for_input("Press Enter to continue.");

		// Check if any items are out of stock
        clear_console();
        printf("%s\n", shopkeeper);
        printf("Better take a look in the stockroom.");
		char* out_of_stock = check_stock(shop, REORDER_THRESHOLD);
		if (strlen(out_of_stock) == 0) {
			printf("Everything seems ok.\n");
		}
		else {
			printf("%s", out_of_stock);
		}
		printf("%s", stringify_shop(shop));
		wait_for_input("Press Enter to continue.");
	}

}

int main(void) 
{
	// Set random seed
	srand (time(NULL));
	// printf("%s\n", shopkeeper);
	// struct Shop shop = generate_shop();
	// printf("Cash: %f\n", shop.cash);
	// printf("%s", stringify_shop(shop));
	// struct Customer customer = generate_customer(shop, BUDGET_LOW, BUDGET_HIGH, 
	// 												   ITEMS_LOW, ITEMS_HIGH,
	// 												   PIECES_LOW, PIECES_HIGH);



	// printf(stringify_bill(customer));

	// printf(stringify_customer(customer));


	// transact(&shop, &customer);
	// for (int i = 0; i < customer.sl_index; i++) {
	// 	printf(stringify_product(customer.shoppingList[i].product));
	// 	printf("\n");
	// }

	// double total = total_bill(customer);
	// printf("%.2f", total);

	auto_mode();

	return 0;
	
}