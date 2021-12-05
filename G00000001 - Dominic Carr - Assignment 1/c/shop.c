#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Global constants
// Adjust auto-generated customer parameters here
#define ITEMS_LOW   1		  // Lowest number of items in a customer order
#define ITEMS_HIGH  5	      // Highest number of items in a customer order
#define BUDGET_LOW 	100		  // Lowest budget in a customer order
#define BUDGET_HIGH 400		  // Highest budget in a customer order
#define PIECES_LOW  1		  // Lowest number of items of a particular item type in a customer order
#define PIECES_HIGH 100	      // Highest number of items of a particular item type in a customer order
size_t REORDER_THRESHOLD = 5; // Shopkeeper reorders item if stock is below this threshold

// Paths to data files
char STOCK_PATH[] = "../stock.csv";				// Stock, quantities, prices, plus initial shop cash
char NAMES_PATH[] = "../names.list";			// List of 200 names for customers
char FACES_PATH[] = "../faces";					// 19 ASCII art faces for customers
char CUSTOMERS_PATH[] = "../customer.csv";		// Saved andomly generated customers go here

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
	int max_quantity; 
};

struct ProductStock {
	struct Product product;
	int quantity;
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
	for (int i = 0; i < 100; i++) {
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

// Initialise a shop struct
struct Shop generate_shop()
{

    FILE *fp;				// filehandle
    char *line = NULL;		// line buffer
    size_t len = 0;         // line length
    ssize_t read;           // number of bytes read

	// Open stock csv file
    fp = fopen(STOCK_PATH, "r");
    if (fp == NULL) {
		printf("Error opening file\n");
        exit(EXIT_FAILURE);
	}

	// The first line of stock.csv holds the shop's cash amount
	read = getline(&line, &len, fp);
	float cash = atof(line);
	
	// Initialise shop struct and add cash
	struct Shop shop;
	shop.cash = cash;

	// Shop index is the index of the last item in the stock array
	shop.index = 0;
	// Read stock.csv line by line
    while ((read = getline(&line, &len, fp)) != -1) {
		// Split line into name, price, max_quantity
		char *n = strtok(line, ",");
		char *p = strtok(NULL, ",");
		char *q = strtok(NULL, ",");
		// convert quantity and price strings to int & double
		int quantity = atoi(q);
		double price = atof(p);
		// 50 characters for name
		char *name = malloc(sizeof(char) * 50);
		strcpy(name, n);
		// Initialise Product struct for new product
		struct Product product = { name, price, quantity };
		// Initialise ProductStock struct for quanitity of that product
		struct ProductStock stockItem = { product, quantity };
		// Add product to stock list and increment shop index
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

	char *names[200][12];
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

/* The main transaction logic of the application */
void transact(struct Shop *shop, struct Customer *customer)
{
	/* For each shopping list entry find the 
        equivalent shop product stock entry */
	for (int i = 0; i < customer->sl_index; i++) {
		for (int j = 0; j < shop->index; j++) {

			/* Short local variable names to make 
			   the transaction logic more readable */
			const char *prod_c = customer->shoppingList[i].product.name;
			const char *prod_s = shop->stock[j].product.name;
			int quant_c  = customer->shoppingList[i].quantity;
			int quant_s  = shop->stock[j].quantity;
			const double price = customer->shoppingList[i].product.price;
			
			/* When the shop product entry for the 
			   customer shopping list product is found ... */
			if (strcmp(prod_c, prod_s) == 0) {

				/* If the customer wants more than the shop has, the customer 
				   must adjust his expectations and so the quantity of that 
				   product desired in the shopping list is changed to the amount 
				   of that product remaining in the shop */
				if (quant_c > quant_s) {
					printf("%s wants %d x %s but only %d are available.\n", 
							customer->name, quant_c, prod_c, quant_s);

					customer->shoppingList[i].quantity = quant_s;
					quant_c = customer->shoppingList[i].quantity;
				} 

				/* If the customer can't afford as many units of the product as the 
				   shopping list specifies, the quantity she can afford is calculated */
				if (customer->budget < (price * quant_c)) {

					customer->shoppingList[i].quantity = (int)(customer->budget / price);
					quant_c = customer->shoppingList[i].quantity;
					
					if (quant_c == 0) {
						printf("%s can't afford any %s.\n", customer->name, prod_c);
					}
					else {
						printf("%s can only afford %d x %s.\n", 
						customer->name, quant_c, prod_c);
					}
				}
				
				/* The transaction occurs here */
				printf("%s buys %d x %s for %.2f.\n", 
					    customer->name, quant_c, prod_c, (price * quant_c));
				// Take the quantity purchased away from shop stock
				shop->stock[j].quantity -= quant_c;
				// Increment shop cash by appropriate amount
				shop->cash += (price * quant_c);
				// Deduct cost of items from customer
				customer->budget -= (price * quant_c);
				// Append purchase to customer's receipt and increment the receipt index
				customer->receipt[customer->r_index] = customer->shoppingList[i];
				customer->r_index++;
				printf("%s has \u20AC%.2f left.\n\n", customer->name, customer->budget);
			}
		}
	}

	wait_for_input("Press enter to continue.");
	clear_console();
	printf("%s\n", shopkeeper);
	printf("Thank you %s. Here's your receipt. \nPlease come again.\n\n", customer->name);
	printf("%s's Receipt:\n", customer->name);
	printf("%s\n", stringify_bill(*customer));
}

void restock(struct Shop *shop, char* product)
{
	for (int i = 0; i < shop->index; i++) {
		if (strcmp(shop->stock[i].product.name, product) == 0) {
			int current_quantity = shop->stock[i].quantity;
			
			shop->stock[i].quantity = shop->stock[i].product.max_quantity;
			shop->cash -= shop->stock[i].product.price * (shop->stock[i].product.max_quantity - current_quantity);
		}
	}
}

char** check_stock(struct Shop *shop, int reorder_threshold, int *num_out_of_stock)
{
	char** out_of_stock = NULL;
	for (int i = 0; i < shop->index; i++) {
		if (shop->stock[i].quantity <= reorder_threshold) {
			(*num_out_of_stock)++;
			out_of_stock = realloc(out_of_stock, sizeof num_out_of_stock * sizeof(char*));
			size_t len = strlen(shop->stock[i].product.name);
			out_of_stock[*num_out_of_stock - 1] = malloc(sizeof(char)*len);
			strcpy(out_of_stock[*num_out_of_stock - 1], shop->stock[i].product.name);
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
        printf("Better take a look in the stockroom.\n\n");

		int num_out_of_stock = 0;
		char **out_of_stock = check_stock(&shop, REORDER_THRESHOLD, &num_out_of_stock);
		wait_for_input("Press Enter to continue.");
		clear_console();
		printf("%s\n", shopkeeper);
		if (out_of_stock) {
			printf("The following items are out of stock:\n");
			for (int i = 0; i < num_out_of_stock; i++) {
				printf("%s\n", out_of_stock[i]);
			}
			printf("%s", stringify_shop(shop));
			wait_for_input("Press Enter to continue.");
			clear_console();
			printf("%s\n", shopkeeper);
			for (int i = 0; i < num_out_of_stock; i++) {
				printf("Restocking %s...\n", out_of_stock[i]);
				restock(&shop, out_of_stock[i]);
				free(out_of_stock[i]);
			}
		}
		else {
			printf("Everything seems to be ok.\n");
		}

		printf("%s", stringify_shop(shop));
		wait_for_input("Press Enter to continue.");
		clear_console();
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