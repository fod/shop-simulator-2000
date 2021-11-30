# shop.py
# A simple shopping simulation in procedural Python
# Author: Fiachra O' Donoghue

# 1. stock.csv holds stock and wholesale prices
# 2. Customer prices are MARKUP greater than wholesale
# 3. MARKUP doesn't change during a programme run
# 4. Initial cash shop holds is in stock.csv
# 5. Modes: Fully automatic: Customers generated on 
#               the fly with random amount of money, random shopping list, 
#               when shop runs out of something it spends 50% of its
#               remaining money on that thing. Simulation runs until shop
#               can't restock.
#           Fully Manual: There are ten pregenerated cusomers in the customer
#                         list. They all shop on by one.
#           The user is the customer. Tell the programme your name and how much 
#           you have and then start buying things.



from dataclasses import dataclass, field
from typing import List
import csv
import random
import time
from faces import faces

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

def clear_console():
    print("\n" * 100)

# Each product has a name and a price
@dataclass
class Product:
    name: str
    price: float
    max_quantity: int

# A product's quantity
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
        shop ([type]): [description]
        budget_range ([type]): [description]
        items_range ([type]): [description]
        pieces_range ([type]): [description]

    Returns:
        [type]: [description]
    """
    customer = Customer()
    with open(names_path, 'r') as f:
        names = f.readlines()
        names = [name.rstrip() for name in names]
        customer.name = random.choice(names)

    customer.face = random.choice(faces)

    customer.budget = random.randint(budget_range[0], budget_range[1])
    num_items = random.randint(items_range[0], items_range[1])
    items = random.sample(shop.stock, num_items)

    for item in items:
        num_pieces = random.randint(pieces_range[0], pieces_range[1])
        customer.shopping_list.append(ProductStock(item.product, num_pieces))

    return customer

#def load_customers():

def stringify_product(product):
    return f"{product.name:<15}{product.price:>7.2f}"

def stringify_shop(shop):
    stock = line = "-" * 36 + "\n"
    stock += ("    Item" + " " * 13 + "Price" + " " * 5 + "Stock\n" + line)
    for i, item in enumerate(shop.stock):
        stock += (f"{i + 1:>2}. " + stringify_product(item.product) + 
                  f"{item.quantity:>10}" + "\n")
    stock += line
    cash = f"Cash: {shop.cash:>7.2f}"
    return stock + "\n" + cash + "\n" + line

def total_bill(itemlist):
    total = 0
    for item in itemlist:
        total += item.product.price * item.quantity
    return total

def stringify_bill(itemlist):
    out = line = "-" * 49 + "\n"
    out += ("    Item" + " " * 13 + "Price" + " " * 5 + "Quantity" + " " * 5 + "Total\n" + line)
    for i, item in enumerate(itemlist):
        out += (f"{i + 1:>2}. {stringify_product(item.product)}" + 
                          f"{item.quantity:>13}" +
                          f"{item.product.price * item.quantity:>10.2f}\n")
    out += line
    total = f"Total Cost: {total_bill(itemlist):37.2f}"

    return out + total + "\n" + line

def stringify_customer(customer):
    budget = f"Budget: {customer.budget:>7.2f}"
    return f"{customer.face}\nName: {customer.name}\n{budget}\n{stringify_bill(customer.shopping_list)}"

def stringify_receipt(customer):
    return stringify_bill(customer.receipt)

def transact(shop, customer):
    for item in customer.shopping_list:
        for stock in shop.stock:
            if item.product == stock.product:
                # If customer wants more than shop has
                if item.quantity > stock.quantity:
                    print(f"{customer.name} wants {item.quantity} x {item.product.name} but only {stock.quantity} are available.")
                    item.quantity = stock.quantity
                if customer.budget < item.product.price * item.quantity:
                    item.quantity = int(customer.budget / item.product.price)
                    if item.quantity == 0:
                        print(f"{customer.name} can't afford any {item.product.name}.")
                    else:
                        print(f"{customer.name} can only afford {item.quantity} x {item.product.name}")
                print(f"{customer.name} buys {item.quantity} x {item.product.name} for {item.product.price * item.quantity:.2f}")
                stock.quantity -= item.quantity
                shop.cash += item.product.price * item.quantity
                customer.budget -= item.product.price * item.quantity
                customer.receipt.append(item)
                print(f"{customer.name} has \N{euro sign}{customer.budget:.2f} left.\n")

    shop.cash += total_bill(customer.shopping_list)
    customer.budget -= total_bill(customer.shopping_list)
    input("Press Enter to continue.")
    clear_console()
    print(shopkeeper)
    print(f"Thank you {customer.name}. Here's your receipt. \nPlease come again.\n")
    print(f"{customer.name}'s Receipt:")
    print(stringify_bill(customer.receipt))


def restock(shop, product):
    for item in shop.stock:
        if item.product == product:
            current_quantity = item.quantity
            item.quantity = item.product.max_quantity
            shop.cash -= item.product.price * (item.product.max_quantity - current_quantity)


def check_stock(shop, reorder_threshold):
    out_of_stock = []
    for item in shop.stock:
        if item.quantity <= reorder_threshold:
            out_of_stock.append(item.product.name)
            restock(shop, item.product)
    return out_of_stock


def auto_mode():
    shop = generate_shop()
    while True:
        clear_console()
        customer = generate_customer(shop, BUDGET_RANGE, NAMES_PATH, ITEMS_RANGE, PIECES_RANGE)
        print(f"{customer.name} has come into the shop with a shopping list!")
        print(stringify_customer(customer))
        input("Press enter to begin transaction.")
        clear_console()
        print(f"{customer.face}")
        print(f"{customer.name} is shopping.\n")
        transact(shop, customer)
        input("Press Enter to continue...")

        # Check if any items are out of stock
        clear_console()
        print(shopkeeper)
        print("Better take a look in the stockroom.")
        out_of_stock = check_stock(shop, REORDER_THRESHOLD)
        if len(out_of_stock) == 0:
            print("Everything seems ok.")
        else:
            if len(out_of_stock) > 1:
                out_of_stock[-1] = "and " + out_of_stock[-1]
            print(f"Yikes, I'm almost out of {out_of_stock[0]} {', '.join(out_of_stock[1:])}")
            print("Better reorder")
        print(stringify_shop(shop))
        input("Press Enter to continue...")

#def preset_mode(customers_path, shop):
    


def live_mode():
    pass

def load_state():
    pass

def save_state():
    pass

def generate_customers(num_customers, shop, budget_range, names_path, items_range, pieces_range, file_path):
    print("Generating customers...")
    with open(file_path, 'w') as f:
        writer = csv.writer(f)
        for i in range(num_customers):
            customer = generate_customer(shop, budget_range, names_path, items_range, pieces_range)
            writer.writerow([customer.name, f"{float(customer.budget):.2f}"])
            for item in customer.shopping_list:
                writer.writerow([item.product.name, item.quantity])
            writer.writerow(['----------'])
    print(f"Generated {num_customers} customers.")
    input("Press Enter to continue.")

def print_help():
    pass

# Generate menu
def displayMenu():
    clear_console()
    line = "-" * 19
    menuItems = ["Auto Mode",
                 "Preset Mode",
                 "Live Mode",
                 "Load State",
                 "Save State",
                 "Generate Customers"]
    
    print(line)
    print("Shop Simulator 2000")
    print(shopkeeper)
    print("Hi I'm the shopkeeper.\nHow can I help?")
    print(line)
    print("MENU")
    print("----")

    for index, item in enumerate(menuItems):
        print(index + 1, "-", item)

    print("x - Exit Application")


def main():

    shop = generate_shop()

    while True:

        displayMenu()

        selection = input("Choice: ")

        if selection == "1":
            auto_mode()
        elif selection == "2":
            preset_mode()
        elif selection == "3":
            live_mode()
        elif selection == "4":
            load_state()
        elif selection == "5":
            save_state()
        elif selection == "6":
            generate_customers(10, shop, BUDGET_RANGE, NAMES_PATH, ITEMS_RANGE, PIECES_RANGE, CUSTOMERS_PATH)
        elif selection == "7":
            print_help()
        elif selection == "x":
            break
        else:
            print("\nPlease enter a number between 1 and 6, or x to quit")
            continue

if __name__ == "__main__":
    main()
    # shop = generate_shop()
    # customer = generate_customer(shop, BUDGET_RANGE, NAMES_PATH, ITEMS_RANGE, PIECES_RANGE)
    # print(stringify_customer(customer))
    # transact(shop, customer)
 
