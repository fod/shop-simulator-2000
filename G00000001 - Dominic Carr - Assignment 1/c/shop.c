#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>	// for isspace() and tolower()
#include <unistd.h> // for sleep()

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
char CUSTOMERS_PATH[] = "../customer.csv"; // Saved andomly generated customers go here

/* Display variables */
char LINE[] = "------------------------------------\n";

// The shopkeeper's face
char shopkeeper[] = "       _www_ \n"
					"      /-o-o-\\  \n"
					"    (|   -   |)\n"
					"      \\ -=- /  \n"
					"      /`---'\\";

/* Structs representing the shop,
products, stock and customers */
struct Product
{
	char *name;
	double price;
	int max_quantity; // The shop's stock of this item cannot exceed max_quantity
};

struct ProductStock
{
	struct Product product;
	int quantity; // The amount of this product in stock
};

struct Shop
{
	double cash;
	struct ProductStock stock[NUM_PRODUCTS];
	size_t index; // Maximum index of stock[]
};

// The customer can have up to ITEMS_HIGH on shopping list
struct Customer
{
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
	for (int i = 0; i < 100; i++)
	{
		printf("\n");
	}
}

// Wait for user to press enter
void cont_or_quit(struct Shop *shop)
{
	printf("%s\n", "Press Enter to continue or \n'Q' to return to main menu: ");
	char enter = 0;
	while (enter != '\r' && enter != '\n' && tolower(enter) != 'q')
	{
		enter = getchar();
	}

	if (tolower(enter) == 'q')
	{
		main_menu(shop);
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
	while (*str)
	{
		if (!isspace(*str++))
		{
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
	if (fp == NULL)
	{
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
	while ((read = getline(&line, &len, fp)) != -1)
	{
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
		// Initialise ProductStock struct for quanitity of that product
		struct ProductStock stockItem = {product, quantity};
		// Add product to stock list and increment shop index
		shop.stock[shop.index++] = stockItem;
	}

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
	char *names[200][12];
	// Read names.list and add each name to names array
	for (int i = 0; i < 200; i++)
	{
		char *name = malloc(sizeof(char) * 12);
		fgets(name, 12, fp);
		// Remove newline character from name
		name[strcspn(name, "\n")] = 0;
		names[i][0] = name;
	}

	// close file handle
	fclose(fp);

	// Return a random name from names array
	return names[rand() % 200][0];
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

	// append flag - if 1 then add to last face, if 0 the
	// face is complete and can be added to faces array
	int append = 0;
	while ((read = getline(&line, &len, fp)) != -1)
	{
		// Start building face
		char *face = malloc(sizeof *face * 50);
		strcpy(face, line);

		/* If current line is not blank and append flag is false then
		   this is the first line of a new face. Set append flag to
		   true, add current face to faces array and restart loop*/
		if (all_space(face) == 0 && append == 0)
		{
			append = 1;
			faces[i] = face;
		}
		/* If current line is blank and append flag is true then the
		   current face is complete. Increment the faces array index.
		   The face will be added on the next loop iteration.*/
		else if (all_space(face) == 1 && append == 1)
		{
			append = 0;
			i++;
		}
		/* If current line is not blank and append flag is true then add
		   current line to current face. */
		else if (append == 1)
		{
			strcat(faces[i], face);
		}
		/* Otherwise just read the next line */
	}

	// close file handle
	fclose(fp);

	// Return a random face from faces array
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

	customer.budget = (double)(rand() % (b_high - b_low + 1) + b_low);
	int num_items = (rand() % (i_high + 1 - i_low) + i_low);

	customer.sl_index = num_items;
	for (int i = 0; i < num_items; i++)
	{
		int index = rand() % shop.index;
		int num_pieces = (rand() % (p_high + 1 - p_low) + p_low);
		customer.shoppingList[i] = shop.stock[index];
		customer.shoppingList[i].quantity = num_pieces;
	}
	customer.r_index = 0;

	return customer;
}

char *stringify_product(struct Product product)
{
	char *string = malloc(sizeof *string * 40);
	int _ = sprintf(string, "%-14s%7.2f", product.name, product.price);

	return string;
}

char *stringify_list(struct ProductStock *itemlist, int max_idx)
{
	char *string = malloc(sizeof *string * 20 * (max_idx + 5));

	strcpy(string, LINE);
	strcat(string, "    Item            Price   Quantity\n");
	strcat(string, LINE);

	for (int i = 0; i < max_idx; i++)
	{
		char *row = malloc(sizeof *row * 40);
		sprintf(row, "%2d. %s%11d", i + 1, stringify_product(itemlist[i].product), itemlist[i].quantity);
		strcat(string, row);
		strcat(string, "\n");
	}
	strcat(string, LINE);

	return string;
}

char *stringify_shop(struct Shop shop)
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
	for (int i = 0; i <= customer.sl_index - 1; i++)
	{
		total += (customer.shoppingList[i].product.price * customer.shoppingList[i].quantity);
	}
	return total;
}

char *stringify_bill(struct Customer customer)
{
	char *string = stringify_list(customer.shoppingList, customer.sl_index);
	double total = total_bill(customer);
	char *cash = malloc(sizeof *cash * 40);
	sprintf(cash, "Total Cost: %24.2f\n", total);
	strcat(string, cash);
	strcat(string, LINE);

	return string;
}

char *stringify_customer(struct Customer customer)
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

	cont_or_quit(shop);
	clear_console();
	printf("%s\n", shopkeeper);
	printf("Thank you %s. Here's your receipt. \nPlease come again.\n\n", customer.name);
	printf("%s's Receipt:\n", customer.name);
	printf("%s\n", stringify_bill(customer));
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

char **check_stock(struct Shop *shop, int reorder_threshold, int *num_out_of_stock)
{
	char **out_of_stock = NULL;
	for (int i = 0; i < shop->index; i++)
	{
		if (shop->stock[i].quantity <= reorder_threshold)
		{
			(*num_out_of_stock)++;
			out_of_stock = realloc(out_of_stock, sizeof num_out_of_stock * sizeof(char *));
			size_t len = strlen(shop->stock[i].product.name);
			out_of_stock[*num_out_of_stock - 1] = malloc(sizeof(char) * len);
			strcpy(out_of_stock[*num_out_of_stock - 1], shop->stock[i].product.name);
		}
	}
	return out_of_stock;
}

void shop_visit(struct Shop *shop, struct Customer customer)
{
	while (1)
	{
		clear_console();

		printf("%s has come into the shop with a shopping list!\n", customer.name);
		printf("%s", stringify_customer(customer));
		char c = getchar();
		cont_or_quit(shop);
		clear_console();
		printf("%s\n", customer.face);
		printf("%s is shopping.\n", customer.name);
		transact(shop, customer);
		cont_or_quit(shop);

		// Check if any items are out of stock
		clear_console();
		printf("%s\n", shopkeeper);
		printf("Better take a look in the stockroom.\n\n");

		int num_out_of_stock = 0;
		char **out_of_stock = check_stock(shop, REORDER_THRESHOLD, &num_out_of_stock);
		cont_or_quit(shop);
		clear_console();
		printf("%s\n", shopkeeper);
		if (out_of_stock)
		{
			printf("The following items are out of stock:\n");
			for (int i = 0; i < num_out_of_stock; i++)
			{
				printf("%s\n", out_of_stock[i]);
			}
			printf("%s", stringify_shop(*shop));
			cont_or_quit(shop);
			clear_console();
			printf("%s\n", shopkeeper);
			for (int i = 0; i < num_out_of_stock; i++)
			{
				printf("Restocking %s...\n", out_of_stock[i]);
				restock(shop, out_of_stock[i]);
				free(out_of_stock[i]);
			}
		}
		else
		{
			printf("Everything seems to be ok.\n");
		}

		printf("%s", stringify_shop(*shop));
		cont_or_quit(shop);
		clear_console();
	}
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
	// Set random seed
	srand(time(NULL));

	while (1) {
		display_menu();
		char choice = input();
		switch (choice) {
			case '1': {
				auto_mode(shop);
				break;
			}
			// case '2': {
			// 	preset_mode();
			// 	break;
			// }
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
				sleep(2);
				exit(0);
			}
		}
	}
}

int main(void)
{
	struct Shop shop = generate_shop();

	clear_console();
    printf("%s\n", "-------------------");
    printf("%s\n\n", "Shop Simulator 2000");
    printf("%s\n\n", shopkeeper);
    printf("%s\n\n", "Hi I'm the shopkeeper.\nWelcome to my shop.");
    printf("%s\n\n", "A tall terminal window will\nenhance your experience.");
    cont_or_quit(&shop);
    main_menu(&shop);
}