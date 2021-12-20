# shop.py
# A simple shopping simulation in procedural Python
# Author: Fiachra O' Donoghue

from dataclasses import dataclass, field
from typing import List
import csv
import random
import time
from faces import faces
import re

# Constants
# Paths to stock and names files
STOCK_PATH = '../stock.csv'
NAMES_PATH = '../names.list'
CUSTOMERS_PATH = '../customers.csv'

# Value ranges for random customer generation
BUDGET_RANGE = (100, 400)
ITEMS_RANGE = (1, 5)
PIECES_RANGE = (1, 100)
REORDER_THRESHOLD = 5

shopkeeper = r"""
       _www_ 
      /-o-o-\  
    (|   -   |)
      \ -=- /  
      /`---'\
"""

# Clear console by printing 100 newlines
def clear_console():
    print("\n" * 100)

# Continue or quit
def cont_or_quit(shop=None):
    choice = input("Press Enter to continue or \n'Q' to return to main menu: ")
    if choice.lower() == 'q':
        main_menu(shop)

# Force user to chooses one of two options
def confirm(message, yes='y', no='n', auto=True):
    ans = input(message + " ")
    if auto and ans == '':
        ans = yes
    if ans.lower() == yes:
        return True
    elif ans.lower() == no:
        return False
    else:
        return confirm(message, yes, no)

# Get a float from user
def getFloat(msg):
    while (True):
        try:
            num = float(input(msg))
            if num < 0:
                raise ValueError
            break
        except ValueError:
            print("Please enter a positive number.")
    return num

# Each product has a name and a price
@dataclass
class Product:
    name: str
    price: float
    max_quantity: int

# A product's quantity in stock
@dataclass
class ProductStock:
    product: Product
    quantity: int

# The shop has an amount of cash and a list of products
@dataclass
class Shop:
    cash: float = 0.0
    stock: List[ProductStock] = field(default_factory=list)

# A customer has a name, a budget,
# a shopping list --> list of Products and quantity desired
# a receipt --> list of Products and quantity purchased
@dataclass
class Customer:
    name: str = ""
    face: str = ""
    budget: float = 0.0
    shopping_list: List[ProductStock] = field(default_factory=list)
    receipt: List[ProductStock] = field(default_factory=list)


def generate_shop():
    """Load stock from CSV file, populate Product and ProductStock data 
       structures, place in shop's stock list, and return populated shop.

    Returns:
        Shop: The fully initialised shop.
    """

    # Initialise a Shop dataclass
    shop = Shop()
    # Read csv using location stored in STOCK_PATH constant
    with open(STOCK_PATH, 'r') as f:
        csv_reader = csv.reader(f)
        # The first row holds the shop's cash
        shop.cash = float(next(csv_reader)[0])
        # Construct a list of lists.
        # Each list contains a product name, price, and quantity
        items = [row for row in csv_reader]

    # Create Product and ProductStock data structures and
    # add to the shop's stock field
    for item in items:
        # Create a Product dataclass (name, price, max_quantity)
        product = Product(item[0], float(item[1]), int(item[2]))
        quantity = int(item[2])
        stock = ProductStock(product, quantity)
        shop.stock.append(stock)

    return shop


def generate_customer(shop, budget_range, names_path, items_range, pieces_range):
    """Generate a random customer for the shop. The customer will have a random name, 
    a budget randomly chosen from within a specified range, and a shopping list with a 
    random number of items and a random quantity of each item. The number of items and 

    Args:
        shop (dataclass): The shop
        budget_range (tuple(int, int)): The range within which the randomly generated 
                                        customer budget will fall.
        items_range (tuple(int, int)): The range within which the randomly generated 
                                       number of items on the customer's shopping list
                                       will fall.
        pieces_range (tuple(int, int)): The range within which the randomly generated
                                        number of units of each shopping list item 
                                        will fall.

    Returns:
        customer dataclass: A dataclass reperesenting the generated customer.
    """
    customer = Customer()

    # Generate a random name from the names file
    with open(names_path, 'r') as f:
        names = f.readlines()
        names = [name.rstrip() for name in names]
        customer.name = random.choice(names)

    # Generate a face for the customer from the faces file
    customer.face = random.choice(faces)

    # Generate budget and shopping list length
    customer.budget = random.randint(budget_range[0], budget_range[1])
    num_items = random.randint(items_range[0], items_range[1])
    # Randomly select shopping list items
    items = random.sample(shop.stock, num_items)

    # Generate number of units for each item and construct shopping list
    for item in items:
        num_pieces = random.randint(pieces_range[0], pieces_range[1])
        customer.shopping_list.append(ProductStock(item.product, num_pieces))

    return customer


def load_customers(shop, path):
    """Loads customers from a csv file. Customers are separated by a line of hyphens (-).
    Each customer record conatins the customer's name and budget on it's first line, and 
    each subsequent line contains a shopping list item and quantity required

    Args:
        shop (dataclass): The shop
        path (str): The path to the csv file containing the customers

    Returns:
        list[customers]: A list in which each item is a customer dataclass
    """
    customers = []
    with open(path, 'r') as f:
        shopping_list = False
        customer = Customer()
        for line in f:
            # A line of hyphens indicates the end of a customer record
            if re.match('^-+$', line):
                # Generate a face for the customer from the faces file
                customer.face = random.choice(faces)
                customers.append(customer)
                customer = Customer()
                shopping_list = False
            else:
                if not shopping_list:
                    (customer.name, customer.budget) = line.split(',')
                    customer.budget = float(customer.budget)
                    shopping_list = True
                else:
                    (product_name, quantity) = line.split(',')
                    product = next(
                        item.product for item in shop.stock if item.product.name == product_name.strip())
                    customer.shopping_list.append(
                        ProductStock(product, int(quantity)))

    return customers


def stringify_product(product):
    """Return a string representation of a product"""
    return f"{product.name:<15}{product.price:>7.2f}"


def stringify_shop(shop):
    """Return a string representation of a shop"""
    stock = line = "-" * 36 + "\n"
    stock += ("    Item" + " " * 13 + "Price" + " " * 5 + "Stock\n" + line)
    for i, item in enumerate(shop.stock):
        stock += (f"{i + 1:>2}. " + stringify_product(item.product) +
                  f"{item.quantity:>10}" + "\n")
    stock += line
    cash = f"Shop's Cash: {shop.cash:>7.2f}"
    return stock + "\n" + cash + "\n" + line


def total_bill(itemlist):
    """Return the total cost of a list of items and quantities"""
    total = 0
    for item in itemlist:
        total += item.product.price * item.quantity
    return total


def stringify_bill(itemlist):
    """Calculate subtotals and totals for list of items and quantities and return as string"""
    out = line = "-" * 49 + "\n"
    out += ("    Item" + " " * 13 + "Price" + " " *
            5 + "Quantity" + " " * 5 + "Total\n" + line)
    for i, item in enumerate(itemlist):
        out += (f"{i + 1:>2}. {stringify_product(item.product)}" +
                f"{item.quantity:>13}" +
                f"{item.product.price * item.quantity:>10.2f}\n")
    out += line
    total = f"Total Cost: {total_bill(itemlist):37.2f}"
    return out + total + "\n" + line


def stringify_customer(customer):
    """Return a string representation of a customer"""
    budget = f"Budget: {customer.budget:>7.2f}"
    return f"{customer.face}\nName: {customer.name}\n{budget}\n{stringify_bill(customer.shopping_list)}"


def stringify_receipt(customer):
    """Pretty print a customer's receipt"""
    return stringify_bill(customer.receipt)


def transact(shop, customer):
    """Simulate a transaction between a customer and a shop. The customer will attempt to
    buy all the items on their shopping list in order. If the shop cannot supply as many
    of an item as the customer requires then they will be supplied with as many as the shop
    has in stock, assuming they can afford them. If the customer cannot afford the full 
    quantity of an item then they will be supplied with as many as they can afford. When the
    transaction is complete the customer's receipt is displayed.

    Args:
        shop (dataclass): The shop
        customer (dataclass): The customer
    """
    # Find each item on the customer's list in the shop's stock
    for item in customer.shopping_list:
        for stock in shop.stock:
            if item.product == stock.product:
                # If customer wants more than shop has
                if item.quantity > stock.quantity:
                    print(
                        f"{customer.name} wants {item.quantity} x {item.product.name} but only {stock.quantity} are available.")
                    item.quantity = stock.quantity
                # If the customer can't afford as many of an item as they want
                if customer.budget < item.product.price * item.quantity:
                    item.quantity = int(customer.budget / item.product.price)
                    if item.quantity == 0:
                        print(
                            f"{customer.name} can't afford any {item.product.name}.")
                    else:
                        print(
                            f"{customer.name} can only afford {item.quantity} x {item.product.name}")
                # The purchase is made
                print(
                    f"{customer.name} buys {item.quantity} x {item.product.name} for {item.product.price * item.quantity:.2f}")
                # Thed shop's stock and cash are updated
                stock.quantity -= item.quantity
                shop.cash += item.product.price * item.quantity
                # The customers budget is update and the purchase is added to their receipt
                customer.budget -= item.product.price * item.quantity
                customer.receipt.append(item)
                print(
                    f"{customer.name} has \N{euro sign}{customer.budget:.2f} left.\n")

    # The transaction is completed and the customer's receipt is displayed
    cont_or_quit(shop)
    clear_console()
    print(shopkeeper)
    print(
        f"Thank you {customer.name}. Here's your receipt. \nPlease come again.\n")
    print(f"{customer.name}'s Receipt:")
    print(stringify_bill(customer.receipt))


def restock(shop, product):
    """Restock a product. The shop can only restock each product up to an arbirtary maximum.
    The maximum for each product is the quantity given for that product in the original shop
    stock csv that is used to initialise the shop.

    Args:
        shop (dataclass): The shop
        product (dataclass): The product to restock
    """
    for item in shop.stock:
        if item.product == product:
            current_quantity = item.quantity
            item.quantity = item.product.max_quantity
            shop.cash -= item.product.price * \
                (item.product.max_quantity - current_quantity)


def check_stock(shop, reorder_threshold):
    """Check the stock levels of all products in the shop. If a product's stock level is 
    below the reorder threshold it will be added to an 'out of stock' list and restocked.

    Args:
        shop (dataclass): The shop
        reorder_threshold (int): The quantity below which a product is considered to be 
                                 out of stock

    Returns:
        list[str]: A lsit of the names of products that have been restocked
    """
    out_of_stock = []
    for item in shop.stock:
        if item.quantity <= reorder_threshold:
            out_of_stock.append(item.product.name)
            restock(shop, item.product)
    return out_of_stock


def shop_visit(shop, customer):
    """Simulates a shop visit by a customer with a shopping list. The customer and the shopkeeper
    will engage in a transaction. Afterwards the shopkeeper will check stock levels and restock
    if necessary.

    Args:
        shop (dataclass): The shop
        customer (dataclass): The customer

    Returns:
        float: The amount of money the customer spent
    """
    cash = shop.cash
    clear_console()
    print(f"{customer.name} has come into the shop with a shopping list!")
    print(stringify_customer(customer))
    cont_or_quit(shop)
    clear_console()
    print(f"{customer.face}")
    print(f"{customer.name} is shopping.\n")
    transact(shop, customer)
    cont_or_quit(shop)
    # Check if any items are out of stock
    clear_console()
    print(shopkeeper)
    print("Better take a look in the stockroom.\n")
    cont_or_quit(shop)
    clear_console()
    print(stringify_shop(shop))
    print(shopkeeper)
    # Check stock and reorder if necessary
    out_of_stock = check_stock(shop, REORDER_THRESHOLD)
    # This is just a charade, check_stock has already done any necessary restocking
    if len(out_of_stock) == 0:
        print("Everything seems ok.")
    else:
        if len(out_of_stock) > 1:
            out_of_stock[-1] = "and " + out_of_stock[-1]
        print(
            f"Yikes, I'm almost out of {out_of_stock[0]} {', '.join(out_of_stock[1:])}")
        print("Better reorder")
        print("Reordering... ", end="")
        for item in out_of_stock:
            print(item, end=" ")
    print()
    cont_or_quit(shop)
    take = shop.cash - cash
    return take


def auto_mode(shop):
    """Randomly generate customers with randomised shopping lists and budgets and 
    simulate their visits to the shop. Runs until the program is interrupted"""
    while True:
        customer = generate_customer(
            shop, BUDGET_RANGE, NAMES_PATH, ITEMS_RANGE, PIECES_RANGE)
        shop_visit(shop, customer)


def preset_mode(shop):
    """Loads a customers from a csv file and simulates their visits to the shop.
    The csv file has been randomly generated using the generate_customers function below."""
    total_take = 0
    customers = load_customers(shop, CUSTOMERS_PATH)
    for customer in customers:
        total_take += shop_visit(shop, customer)
    clear_console()
    print(shopkeeper)
    print("Whew, that was a long day!")
    if total_take == 0:
        print("And I only broke even!!!")
    elif total_take > 0:
        print(f"At least I made \N{euro sign}{total_take:.2f} profit.")
    else:
        print(f"{total_take:.2f}")
        print(f"And after all that I lost \N{euro sign}{abs(total_take):.2f}.")
    cont_or_quit(shop)


def live_mode(shop, seen):
    """Runs the shop in live mode. Asks the user to enter their budget then choose an item and quantity.
    If the customer can afford the requested quantity of that item it is sold to the customer and the shop's 
    cash and stock, and the customer's budget, are updated. If stock levels cannot meet the customer's request
    or the customer cannot afford the requested quantity, the closest affordable quantity og the item is 
    offered to the customer. The transaction ends when the customer quits or the customer's budget is less
    than the price of the cheapest item in the shop."""
    # List of item names for easy selection
    item_names = [name for name in [item.product.name.lower()
                                    for item in shop.stock]]
    min_price = min([item.product.price for item in shop.stock])
    clear_console()
    print(shopkeeper)
    if seen:
        print("Oh hello. It's you again.")
        print("How much money have you got today?")
    else:
        print("Hello, I'm the shopkeeper. Welcome to my shop.")
        print("I hope you don't mind me asking but how much money have you got?")
        seen = True
    budget = getFloat("\N{euro sign} ")
    print("Well it's not much but hopefully you'll be able to find something you can afford.")
    while(True):
        # End transaction if customer hasn't got enough money to buy anything
        if budget < min_price:
            if budget == 0:
                print("You don't have any money left.")
            else: print(f"You've only got \N{euro sign}{budget:.2f} left.", 
                        f"The cheapest item I have costs \N{euro sign}{min_price:.2f}.")
            print("Come back when you have more money.")
            input("Press Enter to continue")    
            break

        # Get item request from customer
        if confirm("Would you like to see my stock (y/n)?"):
            print(stringify_shop(shop))
            print(shopkeeper)
            print("You can select a product to buy by number or name.")           
        else:
            print("OK, Looks like you know what you want.")
            print("So what'll it be?")
        selection = input("Enter product number or name (or 'Q' to end transaction): ")
        item = None
        if selection.isdigit():
            try:
                item = shop.stock[int(selection) - 1]
            except IndexError:
                print("That's not a valid selection.")
                continue
        elif selection.lower() in item_names:
            item = shop.stock[item_names.index(selection)]
        elif selection.lower() == "q":
                clear_console()
                print(shopkeeper)
                print("Thanks for shopping! Come back soon!")
                input("Press Enter to continue")
                break

        #  Restart transaction if a non-existent or out of stock item is selected
        if item is None or item.quantity == 0:
            print(f"I'm sorry, I'm all out of that. How about some nice",
                  f"{random.choice(item_names)} instead?")
            continue

        # Get quantity request from customer
        print(f"{item.product.name} costs \N{euro sign}{item.product.price:.2f}")
        print(f"How many would you like?")
        quantity = int(getFloat(f"Number of {item.product.name}: "))

        # If customer wants more than is in stock
        if quantity > item.quantity:
            print(f"Sorry, I don't have that many {item.product.name}s.")
            quantity = item.quantity
        # If customer wants more than they can afford
        if (item.quantity * item.product.price) > budget:
            quantity = int(budget / item.product.price)
            if quantity == 0:
                print(f"You can't afford any {item.product.name}.")
                continue
            else:
                print(f"You can't afford that many. You can only get {quantity} with \N{euro sign}{budget:.2f}")
        # Make offer and elicit confirmation of purchase from customer
        print(f"I can give you {quantity} for \N{euro sign}{quantity * item.product.price:.2f}.")
        if confirm("Do you want them (y/n)?"):
            # Do some stock management and accounting      
            item.quantity -= quantity
            shop.cash += quantity * item.product.price
            budget -= quantity * item.product.price
        else:
            print("OK, Never mind.")

        # Give customer opportunity to leave shop or continue
        print(f"You have \N{euro sign}{budget:.2f} left.")
        if confirm("Would you like to buy something else (y/n)?"):
            continue
        else:
            clear_console()
            print(shopkeeper)
            print("OK. Thanks for shopping! Come back soon!")
            input("Press Enter to continue")
            break

    return seen

def generate_customers(num_customers, shop, budget_range, names_path,
                       items_range, pieces_range, file_path):
    """Generate a list of random customers with randomised shopping lists and budgets.
    The ranges within which the customers' randomised atttributes are restricted are 
    paramaterised. The customers are saved to a csv file.

    Args:
        num_customers (int): The number of customers to generate
        shop (dataclass): The shop. Needed to get the stock
        budget_range (tuple(int)): The minimum and maximum values for each customer's budget
        names_path (str): The path to the names csv file
        items_range (tuple(int)): The minimim and maximum possible number of items on each 
                                  customer's shopping list
        pieces_range (tuple(int)): The minimum and maximum possible number units of each item
                                   on each customer's shopping list
        file_path (str): The path to the output csv file
    """
    print("Generating customers...")
    with open(file_path, 'w') as f:
        writer = csv.writer(f)
        for i in range(num_customers):
            customer = generate_customer(
                shop, budget_range, names_path, items_range, pieces_range)
            writer.writerow([customer.name, f"{float(customer.budget):.2f}"])
            for item in customer.shopping_list:
                writer.writerow([item.product.name, item.quantity])
            writer.writerow(['----------'])
    print(f"Generated {num_customers} customers.")
    cont_or_quit(shop)

# Initial screen with instructions
def first_run():
    clear_console()
    print("-" * 19)
    print("Shop Simulator 2000")
    print(shopkeeper)
    print("Hi I'm the shopkeeper.\nWelcome to my shop.\n")
    print('A tall terminal window will\nenhance your experience.\n')
    input("Press Enter to continue")
    main_menu()

# Generate menu
def displayMenu():
    clear_console()
    line = "-" * 19
    menuItems = ["Auto Mode",
                 "Preset Mode",
                 "Live Mode",
                 "Generate Customers"]

    clear_console()
    print(shopkeeper)
    print("How can I help?\n")
    print(line)
    print("MENU")
    print("----")

    for index, item in enumerate(menuItems):
        print(index + 1, "-", item)

    print("x - Exit Application")
    print(line)
    
# Display main menu
def main_menu(shop=None):

    if shop is None:
        shop = generate_shop()

    # Flag for live mode - if True the shopkeeper will recognise you
    seen = False

    while True:

        displayMenu()
        selection = input("Choice: ")

        if selection == "1":
            auto_mode(shop)
        elif selection == "2":
            preset_mode(shop)
        elif selection == "3":
            seen = live_mode(shop, seen)
        elif selection == "4":
            generate_customers(5, shop, BUDGET_RANGE, NAMES_PATH,
                               ITEMS_RANGE, PIECES_RANGE, CUSTOMERS_PATH)
        elif selection == "x":
            print(shopkeeper)
            print("Thank you. Come again!")
            time.sleep(2)
            quit()
        else:
            print("\nPlease enter a number between 1 and 6, or x to quit")
            continue


if __name__ == "__main__":
    first_run()
