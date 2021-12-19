#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>	// for isspace() and tolower()
#include <unistd.h> // for sleep()
#include <stdbool.h> // for bool type
#include <math.h> // for fabs()

/* Global constants
   ---------------- */
// Adjust auto-generated customer parameters here
#define NUM_PRODUCTS 24		  // Number of products in the shop -- i.e. in the stock.csv file
#define ITEMS_LOW 1			  // Lowest number of items in a customer order
#define ITEMS_HIGH 5		  // Highest number of items in a customer order
#define BUDGET_LOW 100		  // Lowest budget in a customer order
#define BUDGET_HIGH 400		  // Highest budget in a customer order
#define PIECES_LOW 1		  // Lowest number of items of a particular item type in a customer order
#define PIECES_HIGH 100		  // Highest number of items of a particular item type in a customer order
#define EURO "€"			  // Euro symbol
#define REORDER_THRESHOLD 5   // Shopkeeper reorders item if stock is below this threshold

/* Paths to data files */
char STOCK_PATH[] = "../stock.csv";		   // Stock, quantities, prices, plus initial shop cash
char NAMES_PATH[] = "../names.list";	   // List of 200 names for customers
char FACES_PATH[] = "../faces";			   // 19 ASCII art faces for customers
char CUSTOMERS_PATH[] = "../customers.csv"; // Saved andomly generated customers go here

/* Display variables */
char LINE[] = "---------------------------------------\n";
char LINE_LONG[] = "-------------------------------------------------\n";

// The shopkeeper's face
char shopkeeper[] = "       _www_ \n"
					"      /-o-o-\\  \n"
					"    (|   -   |)\n"
					"      \\ -=- /  \n"
					"      /`---'\\";

/* Structs representing the shop,
products, stock and customers */
struct Product {
	char *name;
	double price;
	int max_quantity; // The shop's stock of this item cannot exceed max_quantity
};

struct ProductStock {
	struct Product product;
	int quantity; // The amount of this product in stock
};

struct Shop {
	double cash;
	struct ProductStock stock[NUM_PRODUCTS];
	size_t index; // Maximum index of stock[]
};

// The customer can have up to ITEMS_HIGH on shopping list
struct Customer {
	char *name;
	char *face;
	double budget;
	struct ProductStock shoppingList[ITEMS_HIGH];
	struct ProductStock receipt[ITEMS_HIGH];
	size_t sl_index; // Maximum index of shoppingList[]
	size_t r_index;	 // Maximum index of receipt[]
};

// Prototypes
void main_menu(struct Shop *shop);

/* Utility functions */
// Clear the console by printing 100 newlines
void clear_console()
{
	for (int i = 0; i < 100; i++) {
		printf("\n");
	}
}

// Wait for user to press enter
bool cont_or_quit()
{
	printf("%s\n", "Press Enter to continue or \n'Q' to return to main menu: ");
	char enter = 0;
	while (enter != '\r' && enter != '\n' && tolower(enter) != 'q'){
		enter = getchar();
	}
	if (tolower(enter) == 'q'){
		//main_menu(shop);
		return true;
	}
	else {
		return false;
	}
}


// Get single character input from user
char input()
{
	char c;
	scanf("%c", &c);
	return c;
}

// Detect character arrays containing only whitespace
int all_space(const char *str)
{
	while (*str) {
		if (!isspace(*str++)) {
			return 0;
		}
	}
	return 1;
}

/* Main application functions */
// Initialise a shop struct
struct Shop generate_shop()
{
	FILE *fp;		   // filehandle
	char *line = NULL; // line buffer
	size_t len = 0;	   // line length
	ssize_t read;	   // number of bytes read

	// Open stock csv file
	fp = fopen(STOCK_PATH, "r");
	if (fp == NULL){
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
	}

	// The first line of stock.csv holds the shop's cash amount
	read = getline(&line, &len, fp);
	float cash = atof(line);

	// Initialise shop struct and add cash
	struct Shop shop;
	shop.cash = cash;

	// idx of the last item in the stock array
	shop.index = 0;
	// Read stock.csv line by line
	while ((read = getline(&line, &len, fp)) != -1){
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
		struct Product product = {name, price, quantity};
		// Initialise ProductStock struct for quantity of that product
		struct ProductStock stockItem = {product, quantity};
		// Add product to stock list and increment shop index
		shop.stock[shop.index++] = stockItem;
	}

	// Close the filehandle
	fclose(fp);
	free(line);

	return shop;
}

// Return a random name from the NAMES_PATH file
char *get_name()
{
	FILE *fp;		   // filehandle
	char *line = NULL; // line buffer
	size_t len = 0;	   // line length
	ssize_t read;	   // number of bytes read

	// Open names.list file
	fp = fopen(NAMES_PATH, "r");
	if (fp == NULL)
	{
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
	}

	// Declare array to hold all names
	char *names[200];
	// Read names.list and add each name to names array
	for (int i = 0; i < 200; i++) {
		char *name = malloc(sizeof(char) * 12);
		fgets(name, 12, fp);
		// Remove newline character from name
		name[strcspn(name, "\n")] = 0;
		names[i] = name;
	}

	// close file handle
	fclose(fp);

	// Get a random name from names array
	char *randname = malloc(sizeof *randname * 12);
	strcpy(randname, names[rand() % 200]);

	// Free the names array
	for (int i = 0; i < 200; i++) {
		free(names[i]);
	}

	// Return a random name
	return randname;
}

// Return a random face from the FACES_PATH file
char *get_face()
{
	FILE *fp;		   // filehandle
	char *line = NULL; // line buffer
	size_t len = 0;	   // line length
	ssize_t read;	   // number of bytes read

	// Open faces file for reading
	fp = fopen(FACES_PATH, "r");
	if (fp == NULL)
	{
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
	}

	// Declare array to hold all faces
	char *faces[19];
	int i = 0; // index in faces array to add next face

	// append flag - if 1 then keep adding to current face
	int append = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
	
		/* If current line is not blank and append flag is false then
		   this is the first line of a new face. Set append flag to
		   true, and start filling a new faces array slot*/
		if (all_space(line) == 0 && append == 0) {
			faces[i] = malloc(sizeof(char) * 400);
			faces[i] = strcpy(faces[i], line);
			append = 1;
		}
		/* If current line is blank and append flag is true then the
		   current face is complete. Increment the faces array index
		   and toggle turn off the append flag*/
		else if (all_space(line) == 1 && append == 1) {
			append = 0;
			i++;
		}
		/* If current line is not blank and append flag is true then add
		   current line to current face. */
		else if (append == 1) {
			strcat(faces[i], line);
		}
	}

	// close file handle
	fclose(fp);
	free(line);

	// Get a random face from faces array
	char *randface = malloc(sizeof *randface * 400);
	strcpy(randface, faces[rand() % 19]);

	// Free the faces array
	for (int i = 0; i < 19; i++) {
		free(faces[i]);
	}

	// Return a random face
	return randface;
}

// Generate a customer struct
struct Customer generate_customer(struct Shop shop,
								  int b_low, int b_high,
								  int i_low, int i_high,
								  int p_low, int p_high)

{
	struct Customer customer;
	char *name = get_name();
	char *face = get_face();
	customer.name = name;
	customer.face = face;

	customer.budget = (double)(rand() % (b_high - b_low + 1) + b_low);
	int num_items = (rand() % (i_high + 1 - i_low) + i_low);

	customer.sl_index = num_items;
	for (int i = 0; i < num_items; i++){
		int index = rand() % shop.index;
		int num_pieces = (rand() % (p_high + 1 - p_low) + p_low);
		customer.shoppingList[i] = shop.stock[index];
		customer.shoppingList[i].quantity = num_pieces;
	}
	customer.r_index = 0;

	return customer;
}

struct Customer *load_customers(struct Shop shop, char *path, int *num_customers) 
{
	FILE *fp;		   // filehandle
	char *line = NULL; // line buffer
	size_t len = 0;	   // line length
	ssize_t read;	   // number of bytes read

	// Open file for reading
	fp = fopen(path, "r");
	if (fp == NULL) {
		printf("Error opening file\n");
		exit(EXIT_FAILURE);
	}

	struct Customer *customers = malloc(sizeof(struct Customer));
	int cust_idx = 0; // index in customers array to add next customer
	bool id_flag = true;
	while ((read = getline(&line, &len, fp)) != -1) {
		// If current line is record seperator then add a new customer
		if (strncmp(line, "----", 4) == 0) {
			// Indicates the next line will be customers name and budget
			id_flag = true;
			cust_idx++;
			continue;
		}
		// If a blank line then the end of the file has been reached
		if (all_space(line) == 1) {
			printf("%s\n", "End of file reached\n");
			break;
		}
		// If current line is not record seperator and id_flag is true
		if (id_flag) {
			struct Customer customer; 
			// Set customer's name, budget and face
			char *n = strtok(line, ",");
			char *b = strtok(NULL, ",");
			double budget = atof(b);

			char *name = malloc(sizeof(char) * 20);
			name = strcpy(name, n);
			char *face = get_face();

			customer.name = name;
			customer.budget = budget;
			customer.face = face;

			// initialise array indices
			customer.sl_index = 0;
			customer.r_index = 0;

			// The next line will not be an id line
			id_flag = false;
			if (cust_idx > 0) {
				customers = realloc(customers, sizeof(struct Customer) * (cust_idx + 1));
			}
			customers[cust_idx] = customer;
		}
		// If the current line is not an id or record separator line
		// then it must be a shopping list line
		else {
			struct Customer *customer = &customers[cust_idx];

			// extract item name and quantity
			char *item = strtok(line, ",");
			char *q = strtok(NULL, ",");
			int quantity = atoi(q);
			// find item in shop stock
			bool found = false;

			for (int i = 0; i < shop.index; i++) {
				if (strcmp(shop.stock[i].product.name, item) == 0) {
					customer->shoppingList[customer->sl_index] = shop.stock[i];
					customer->shoppingList[customer->sl_index].quantity = quantity;
					customer->sl_index++;
					found = true;
					break;
				}
			}
			// if item not found in shop stock then print error message
			if (!found) {
				printf("Error: item %s not found in shop stock\n", item);
			}
		}
	}

	// close file handle
	fclose(fp);
	free(line);
	customers = realloc(customers, sizeof(struct Customer) * (cust_idx));
	// Return array of customers
	*num_customers = cust_idx;
	return customers;
}

char *stringify_product(struct Product product)
{
	char *string = malloc(sizeof(string) * strlen(LINE_LONG) + 1);
	snprintf(string, strlen(LINE_LONG), "%-15s%7.2f", product.name, product.price);
	return string;
}

// Generic function to print shop's stock list or customer's shopping list
// with optional row totals calculated
char *stringify_list(struct ProductStock *itemlist, int max_idx, bool total)
{
	// The separator lines and headers are dependent on whether row totals are required
	char *line = LINE;
	char *header = "    Item             Price     Quantity";
	if (total==true) {
		line = LINE_LONG;
		header = "    Item             Price     Quantity     Total";
	}
	
	// string will hold the output table
	char *string = malloc(sizeof(string) * strlen(line) * (5 + max_idx));
	// buffer to hold the current row
	char *buff = malloc(sizeof(buff) * strlen(line) + 1);
	// Generate the table header
	int offset = snprintf(string, strlen(line) * 3, "%s%s\n%s", line, header, line);
	
	// max_idx is the number of items in the list
	for (int i = 0; i < max_idx; i++) {
		// Get the stringified product representation
		char* product_string = stringify_product(itemlist[i].product);
		// print formatted row into buffer.
		int offset = snprintf(buff, strlen(LINE) + 1, "\n%2d. %s%13d", i + 1, 
						product_string, itemlist[i].quantity);
		if (total==true) {
			// Add row total after offset chars if required
			snprintf(buff + offset, strlen(line) + 1 - offset, "%10.2f\n", 
				itemlist[i].product.price * itemlist[i].quantity);
		}
		// Append the current row to the output string
		strcat(string, buff);
		// Free product string memory
		free(product_string);
	}
	// Free buffer memory
	free(buff);

	// Footer line
	strcat(string, "\n");
	strcat(string, line);

	// Return the output string - needs to be freed by caller
	return string;
}

char *stringify_shop(struct Shop shop)
{
	char *string = malloc(sizeof(string) * strlen(LINE) * (shop.index + 5));
	char *list_string = stringify_list(shop.stock, shop.index, false);
	snprintf(string, sizeof(string) * strlen(LINE) * (shop.index + 5), 
		"%sShop's Cash: %26.2f\n%s", 
		list_string, shop.cash, LINE);

	free(list_string);
	return string;
}

double total_bill(struct Customer customer)
{
	double total = 0;
	for (int i = 0; i <= customer.sl_index - 1; i++) {
		total += (customer.shoppingList[i].product.price * customer.shoppingList[i].quantity);
	}
	return total;
}

char *stringify_bill(struct Customer customer)
{
	char *string = malloc(sizeof(string) * strlen(LINE_LONG) * (customer.sl_index + 5));
	char *list_string = stringify_list(customer.shoppingList, customer.sl_index, true);
	snprintf(string, sizeof(string) * strlen(LINE_LONG) * (customer.sl_index + 5), "%sTotal Cost: %37.2f\n%s", 
		list_string, total_bill(customer), LINE_LONG);
	
	free(list_string);
	return string;
}

char *stringify_customer(struct Customer customer)
{
	char *string = malloc(sizeof(string) * 400 + (strlen(LINE_LONG) * customer.sl_index) + (strlen(LINE_LONG) * 5));
	char *bill_string = stringify_bill(customer);
	snprintf(string, sizeof(string) * 400 + (strlen(LINE_LONG) * customer.sl_index) + (strlen(LINE_LONG) * 5), 
		"%s\nName: %s\nBudget:%7.2f\n%s", customer.face, customer.name, customer.budget, bill_string);

	free(bill_string);	
	return string;
}

/* The main transaction logic of the application */
void transact(struct Shop *shop, struct Customer customer)
{
	/* For each shopping list entry find the
		equivalent shop product stock entry */
	for (int i = 0; i < customer.sl_index; i++)
	{
		for (int j = 0; j < shop->index; j++)
		{

			/* Short local variable names to make
			   the transaction logic more readable */
			const char *prod_c = customer.shoppingList[i].product.name;
			const char *prod_s = shop->stock[j].product.name;
			int quant_c = customer.shoppingList[i].quantity;
			int quant_s = shop->stock[j].quantity;
			const double price = customer.shoppingList[i].product.price;

			/* When the shop product entry for the
			   customer shopping list product is found ... */
			if (strcmp(prod_c, prod_s) == 0)
			{

				/* If the customer wants more than the shop has, the customer
				   must adjust his expectations and so the quantity of that
				   product desired in the shopping list is changed to the amount
				   of that product remaining in the shop */
				if (quant_c > quant_s)
				{
					printf("%s wants %d x %s but only %d are available.\n",
						   customer.name, quant_c, prod_c, quant_s);

					customer.shoppingList[i].quantity = quant_s;
					quant_c = customer.shoppingList[i].quantity;
				}

				/* If the customer can't afford as many units of the product as the
				   shopping list specifies, the quantity she can afford is calculated */
				if (customer.budget < (price * quant_c))
				{

					customer.shoppingList[i].quantity = (int)(customer.budget / price);
					quant_c = customer.shoppingList[i].quantity;

					if (quant_c == 0)
					{
						printf("%s can't afford any %s.\n", customer.name, prod_c);
					}
					else
					{
						printf("%s can only afford %d x %s.\n",
							   customer.name, quant_c, prod_c);
					}
				}

				/* The transaction occurs here */
				printf("%s buys %d x %s for %.2f.\n",
					   customer.name, quant_c, prod_c, (price * quant_c));
				// Take the quantity purchased away from shop stock
				shop->stock[j].quantity -= quant_c;
				// Increment shop cash by appropriate amount
				shop->cash += (price * quant_c);
				// Deduct cost of items from customer
				customer.budget -= (price * quant_c);
				// Append purchase to customer's receipt and increment the receipt index
				customer.receipt[customer.r_index] = customer.shoppingList[i];
				customer.r_index++;
				printf("%s has \u20AC%.2f left.\n\n", customer.name, customer.budget);
			}
		}
	}

	if (cont_or_quit() == true) {
		free(customer.name);
		free(customer.face);
		main_menu(shop);
	}
	clear_console();
	printf("%s\n", shopkeeper);
	printf("Thank you %s. Here's your receipt. \nPlease come again.\n\n", customer.name);
	printf("%s's Receipt:\n", customer.name);
	char *bill_string = stringify_bill(customer);
	printf("%s\n", bill_string);
	free(bill_string);
	return;
}

void restock(struct Shop *shop, char *product)
{
	for (int i = 0; i < shop->index; i++)
	{
		if (strcmp(shop->stock[i].product.name, product) == 0)
		{
			int current_quantity = shop->stock[i].quantity;

			shop->stock[i].quantity = shop->stock[i].product.max_quantity;
			shop->cash -= shop->stock[i].product.price * (shop->stock[i].product.max_quantity - current_quantity);
		}
	}
}

// Test to see if any item stocks are below reorder threshold and return array of those items
struct Product *check_stock(struct Shop *shop, int reorder_threshold, int *size)
{
	int count = 0;
	for (int i = 0; i < shop->index; i++) {
		if (shop->stock[i].quantity < reorder_threshold){
			count++;
		}
	}
	struct Product *out_of_stock = malloc(sizeof(struct Product) * count);
	int index = 0;
	for (int i = 0; i < shop->index; i++) {
		if (shop->stock[i].quantity < reorder_threshold){
			out_of_stock[index] = shop->stock[i].product;
			index++;
		}
	}
	*size = count;
	return out_of_stock;
}

double shop_visit(struct Shop *shop, struct Customer customer)
{
	double cash = shop->cash;
	clear_console();
	printf("%s has come into the shop with a shopping list!\n", customer.name);
	char *customer_string = stringify_customer(customer);
	printf("%s", customer_string);
	free(customer_string);

	char c = getchar(); // Consume extra \n from stdin
	if (cont_or_quit() == true) {
		free(customer.name);
		free(customer.face);
		main_menu(shop);
	}

	clear_console();
	printf("%s\n", customer.face);
	printf("%s is shopping.\n", customer.name);
	transact(shop, customer);
	free(customer.name);
	free(customer.face);

	if (cont_or_quit() == true) {
		main_menu(shop);
	}

	// Check if any items are out of stock
	clear_console();
	printf("%s\n", shopkeeper);
	printf("Better take a look in the stockroom.\n\n");

	// int *size = malloc(sizeof(int));
	// struct Product *o = check_stock(&shop, 100, size);
	// for (int i = 0; i < *size; i++) {
	// 	printf("%s\n", o[i].name);
	// }

	// printf("%d\n", *size);

	int *num_out_of_stock = malloc(sizeof(int));
	struct Product *out_of_stock = check_stock(shop, REORDER_THRESHOLD, num_out_of_stock);
	if (cont_or_quit() == true) {
		main_menu(shop);
	}
	clear_console();
	printf("%s\n", shopkeeper);

	if (*num_out_of_stock > 0) {
		printf("The following items are out of stock:\n");
		for (int i = 0; i < *num_out_of_stock; i++) {
			printf("%s\n", out_of_stock[i].name);
		}
	
		char *shop_string = stringify_shop(*shop);
		printf("%s", shop_string);
		free(shop_string);

		if (cont_or_quit() == true) {
			main_menu(shop);
		}
		clear_console();
		printf("%s\n", shopkeeper);

		for (int i = 0; i < *num_out_of_stock; i++) {
			printf("Restocking %s...\n", out_of_stock[i].name);
			restock(shop, out_of_stock[i].name);
		}
	}
	else {
		printf("Everything seems to be ok.\n");
	}
	free(out_of_stock);
	free(num_out_of_stock);

	char *shop_string = stringify_shop(*shop);
	printf("%s", shop_string);
	free(shop_string);

	if (cont_or_quit() == true) {
		main_menu(shop);
	}
	clear_console();
	double take = shop->cash - cash;
	return take;
}

void auto_mode(struct Shop *shop)
{
	while(1) {
		struct Customer customer = 
			generate_customer(*shop, BUDGET_LOW, BUDGET_HIGH,
									ITEMS_LOW, ITEMS_HIGH,
									PIECES_LOW, PIECES_HIGH);
		shop_visit(shop, customer);
	}
}

void preset_mode(struct Shop *shop)
{
	double total_take = 0;
	int *num_customers = malloc(sizeof(int));
	struct Customer *customers = load_customers(*shop, CUSTOMERS_PATH, num_customers);

	for (int i = 0; i < *num_customers; i++) {
		total_take += shop_visit(shop, customers[i]);
	}
	clear_console();
	printf("%s\n", shopkeeper);
	printf("%s\n", "Whew, that was a long day!");
    if (total_take == 0) {
        printf("%s\n\n", "And I only broke even!!!");
	}
    else if (total_take > 0) {
        printf("%s %s %.2f %s\n\n", "At least I made ", EURO, total_take, "profit.");
	}
	else {
        printf("%s %s%.2f\n\n", "And after all that I lost", EURO, fabs(total_take));
	}
	
	free(customers);
	free(num_customers);
	cont_or_quit();
}

bool live_mode(struct Shop *shop, bool seen)
{
	clear_console();
	printf("%s\n", shopkeeper);
	if (seen) {
		printf("%s\n", "Oh hello. It's you again.");
		printf("%s\n", "How much money have you got today?");
	}
	else {
		printf("%s\n", "Hello, I'm the shopkeeper. Welcome to my shop.");
		printf("%s\n", "I hope you don't mind me asking but how much money have you got?");
		seen = true;
	}
}

void display_menu()
{
	clear_console();
	char line[] = "-------------------";
	char *menu_items[] = {"Auto Mode",
						  "Preset Mode",
						  "Live Mode",
						  "Generate Customers",
						  "Reset shop"};

	int num_menu_items = 5;

	printf("%s\n\n", shopkeeper);
	printf("%s\n\n", "How can I help?");
	printf("%s\n", line);
	printf("%s\n", "MENU");
	printf("%s\n", "----");

	for (int i = 0; i < num_menu_items; i++)
	{
		printf("%d - %s\n", (i + 1), menu_items[i]);
	}

	printf("%s\n", "x - Exit application");
	printf("%s\n", line);
	printf("%s", "Choice: ");
}

void main_menu(struct Shop *shop)
{
	while (1) {
		display_menu();
		char choice = input();
		switch (choice) {
			case '1': {
				auto_mode(shop);
				break;
			}
			case '2': {
				preset_mode(shop);
				break;
			}
			// case '3': {
			// 	live_mode();
			// 	break;
			// }
			// case '4': {
			// 	generate_customers();
			// 	break;
			// }
			case 'x': {
				clear_console();
				printf("%s\n", shopkeeper);
				printf("%s\n", "Thank you. Come again!");
				sleep(1);
				for (int i = 0; i < shop->index; i++) {
					free(shop->stock[i].product.name);
				}
				exit(0);
			}
		}
	} 
}

int main(void)
{
	// Set random seed
	srand(time(NULL));
	// The shop
	struct Shop shop = generate_shop();
	
	clear_console();
    printf("%s\n", "-------------------");
    printf("%s\n\n", "Shop Simulator 2000");
    printf("%s\n\n", shopkeeper);
    printf("%s\n\n", "Hi I'm the shopkeeper.\nWelcome to my shop.");
    printf("%s\n\n", "A tall terminal window will\nenhance your experience.");
    printf("%s\n", "Press Enter to continue...");
	char enter = 0;
	while (enter != '\r' && enter != '\n'){
		enter = getchar();
	}
    main_menu(&shop);
}