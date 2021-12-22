package shop;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.charset.StandardCharsets;
import java.util.Random;
import java.util.Scanner;
import java.io.PrintWriter;

public class Shop {

    private double cash;
    private List<ProductStock> stock;

    public Shop(String fileName) {

        stock = new ArrayList<>();
        List<String> lines = Collections.emptyList();
		try {
			lines = Files.readAllLines(Paths.get(fileName), StandardCharsets.UTF_8);
			cash = Double.parseDouble(lines.get(0));
			// i am removing at index 0 as it is the only one treated differently
			lines.remove(0);
			for (String line : lines) {
				String[] arr = line.split(",");
				String name = arr[0];
				double price = Double.parseDouble(arr[1]);
				int quantity = Integer.parseInt(arr[2].trim());
				Product p = new Product(name, price, quantity);
				ProductStock s = new ProductStock(p, quantity);
				stock.add(s);
			}
		}

		catch (IOException e) {
			System.out.println("Cannot locate Stock file");
			e.printStackTrace();
			System.exit(-1);
		}
    }

    private List<Customer> loadCustomers(String fileName) {
        String fullPath = System.getProperty("user.dir") + fileName;
        List<Customer> customers = new ArrayList<>();
        List<String> lines = Collections.emptyList();
        try {
            boolean id = true;
            lines = Files.readAllLines(Paths.get(fullPath), StandardCharsets.UTF_8);
            for (String line : lines) {
                // End of a record
                if (line.matches("-+")) {
                    id = true;
                    continue;
                }
                // End of the file
                if (line.matches("\s*")) {
                    break;
                }
                // Start of a record
                if (id) {
                    String[] idRec = line.split(",", 2);
                    customers.add(new Customer(idRec[0], Double.parseDouble(idRec[1])));
                    id = false;
                }
                // Shopping list line
                else {
                    String[] arr = line.split(",");
                    String name = arr[0];
                    int quantity = Integer.parseInt(arr[1].trim());
                    // Find the product in the stock
                    for (ProductStock ps : stock) {
                        if (ps.getProduct().getName().equalsIgnoreCase(name)) {
                             customers.get(customers.size() - 1).addToShoppingList(new ProductStock(ps.getProduct(), quantity));
                        }
                    }
                }
            }
        }
        catch (IOException e) {
            System.out.println("Cannot locate Customers file");
            e.printStackTrace();
            System.exit(-1);
        }
        return customers;
    }      

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(Configuration.LINE_MED + System.lineSeparator());
        sb.append("  Item             Price        Stock" + System.lineSeparator());
        sb.append(Configuration.LINE_MED + System.lineSeparator());
        int count = 1;
        for (ProductStock ps : stock) {
            sb.append(count + " " + ps.toString() + System.lineSeparator());
            count++;
        }
        sb.append(Configuration.LINE_MED + System.lineSeparator());
        sb.append(System.lineSeparator());
        sb.append(String.format("Shop's Cash: %17s%.2f%n", "€", cash));
        sb.append(Configuration.LINE_MED + System.lineSeparator());
        return sb.toString();
    }

    public double getCash() {
        return this.cash;
    }

    public List<ProductStock> getStock() {
        return stock;
    }

    private List<Product> checkStock(int reorder_threshold) {
        List<Product> outOfStock = new ArrayList<>();
        for (ProductStock ps : stock) {
            if (ps.getQuantity() < reorder_threshold) {
                outOfStock.add(ps.getProduct());
            }
        }
        return outOfStock;
    }

    private void restock(Product p) {
        for (ProductStock ps : stock) {
            if (ps.getProduct().equals(p)) {
                int currentQuantity = ps.getQuantity();
                ps.setQuantity(p.getMaxQuantity());
                // Shop pays for restocking
                this.cash -= p.getPrice() * (p.getMaxQuantity() - currentQuantity);
            }
        }
    }

    // Shop is responsible for parts of customer that are dependent on it
    // Customer provides its own name and face
    private Customer randomCustomer() {
        Random random = new Random(System.currentTimeMillis());
        double budget = random.nextDouble() * (Configuration.BUDGET_MAX - Configuration.BUDGET_MIN) + Configuration.BUDGET_MIN;
        int numItems = random.nextInt(Configuration.ITEMS_MIN, Configuration.ITEMS_MAX + 1);
        List<ProductStock> shoppingList = new ArrayList<>();
        for (int i = 0; i < numItems; i++) {
            int numPieces = random.nextInt(Configuration.PIECES_MIN, Configuration.PIECES_MAX + 1);
            List<ProductStock> stock = this.getStock();
            Product item = stock.get(random.nextInt(stock.size())).getProduct();
            shoppingList.add(new ProductStock(item, numPieces));
        }
        Customer customer = new Customer(budget, shoppingList);
        return customer;
    }

    public void doAutoMode() {
        while (true) {
            Customer customer = randomCustomer();
            this.shopVisit(customer);
        }
    }

    public void doPresetMode() {
        double totalTake = 0;
        List<Customer> customers = loadCustomers(Configuration.CUSTOMERS_PATH);
        for (Customer customer : customers) {
            totalTake += this.shopVisit(customer);
        }
        Utility.clearConsole();
        System.out.println(ShopSim.shopkeeper);
        System.out.println("Whew, that was a long day!");
        if (totalTake == 0) {
            System.out.println("And I only broke even!!!");
        }
        else if (totalTake > 0) {
            System.out.println(String.format("At least I made %s %.2f %s%n", "€", totalTake, "profit."));
        }
        else {
            System.out.println(String.format("%s %s%.2f\n\n", "And after all that I lost", "€", Math.abs(totalTake)));
        }
        if (Utility.contOrQuit()) {
            ShopSim.menuControl();
        }
        ShopSim.menuControl();
    }

    /* Live Mode */
    public void doLiveMode(boolean seen) {

        // Get price of cheapest item
        double minPrice = 0;
        for (ProductStock ps : stock) {
            if (minPrice == 0 || ps.getProduct().getPrice() < minPrice) {
                minPrice = ps.getProduct().getPrice();
            }
        }

        // Get array of lower-case product names
        List<String> names = new ArrayList<>();
        for (ProductStock ps : stock) {
            names.add(ps.getProduct().getName().toLowerCase());
        }

        // Clear console and show shopkeeper
        Utility.clearConsole();
        System.out.println(ShopSim.shopkeeper);

        // Greet Customer and elicit budget
        if (seen) {
            System.out.println("Oh hello. It's you again.");
		    System.out.println("How much money have you got today?");
        }
        else {
            System.out.println("Hello, I'm the shopkeeper. Welcome to my shop.");
            System.out.println("I hope you don't mind me asking but how much money have you got?");
            System.out.print("€ ");
		    seen = true;
        }
        double budget;
        Scanner scanner = new Scanner(System.in);
        while (true) {
            try {
                budget = Double.parseDouble(scanner.nextLine());
                if (budget < 0) {
                    throw new NumberFormatException();
                }
                System.out.println("Well it's not much but hopefully you'll be able to find something you can afford.");
                break;
            }
            catch (NumberFormatException e) {
                System.out.println("Please enter a positive number.");
            }
        }

        // The Live Mode loop
        while (true) {
            // Make sure the customer has enough money to buy something
            if (budget < minPrice) {
                if (budget == 0) {
                    System.out.println("You don't have any money left.");
                }
                else {
                    System.out.println(String.format("You've only got €%.2f left", budget));
                    System.out.println(String.format("The cheapest item I have costs €$,2f", minPrice));
                    System.out.println("Come back when you have more money.");
                    System.out.println("Press Enter to continue.");
                    Utility.waitForInput();
                    ShopSim.menuControl();
                }
            }

            // Customer is presented with a list of products
            System.out.println("Would you like to see my stock (y/n)?");
            scanner = new Scanner(System.in);

            String input = scanner.nextLine();
            if (input.equalsIgnoreCase("Y")) {
                Utility.clearConsole();
                System.out.println(this);
                System.out.println(ShopSim.shopkeeper);
                System.out.println("You can select a product to buy by number or name.");
            }
            else {
                System.out.println("OK, Looks like you know what you want.");
                System.out.println("So what'll it be?");
            }

            // Customer selects product
            Product selectedProduct = null;
            System.out.print("Enter product number or name (or 'Q' to end transaction): ");
            scanner = new Scanner(System.in);

            // If a number has been entered...
            if (scanner.hasNextInt()){
                int productNum = scanner.nextInt();
                if (productNum < 0 || productNum >= this.getStock().size() + 1 ) {
                    System.out.println("That's not a valid selection.");
                    continue;
                }
                else {
                    // The products are numbered from 1 but indexed from 0
                    productNum--;
                    selectedProduct = this.stock.get(productNum).getProduct();                    
                }
            }

            // If a String has been entered...
            else if (scanner.hasNextLine()) {
                input = scanner.nextLine();
                // If its a 'Q then quit'
                if (input.equalsIgnoreCase("Q")) {
                    Utility.clearConsole();
                    System.out.println(ShopSim.shopkeeper);
                    System.out.println("Thanks for shopping! Come back soon!");
                    System.out.println("Press enter to continue");
                    Utility.waitForInput();
                    ShopSim.menuControl();
                }

                // If the string is a valid product name...
                else if (names.contains(input.toLowerCase())) {
                    int index = names.indexOf(input.toLowerCase());
                    selectedProduct = this.stock.get(index).getProduct();
                }
                else {
                    System.out.println("We're all out of that.");
                    Random random = new Random(System.currentTimeMillis());
                    int randomProduct = random.nextInt(this.getStock().size());
                    System.out.println(String.format("How about some nice %s instead?", 
                        this.getStock().get(randomProduct).getProduct().getName()));
                    continue;
                }
            }
            else {
                System.out.println("That's not a valid selection.");
                continue;
            }
            // Get desired quantity
            System.out.println(String.format("%s costs €%.2f", selectedProduct.getName(), selectedProduct.getPrice()));
            System.out.println("How many would you like?");
            int quantity;
            while (true) {
                try {
                    quantity = Integer.parseInt(scanner.nextLine());
                    if (quantity < 0) {
                        throw new NumberFormatException();
                    }
                    break;
                }
                catch (NumberFormatException e) {
                    System.out.println("Please enter a positive number.");
                }
            }

            // Get available quantity of requested product
            int quantityAvailable = 0;
            ProductStock selectedProductStock = null;
            for (ProductStock ps : this.stock) {
                if (ps.getProduct().equals(selectedProduct)) {
                    quantityAvailable = ps.getQuantity();
                    selectedProductStock = ps;
                }
            }

            // if customer wants more than is in stock
            if (quantity > quantityAvailable) {
                System.out.println(String.format("Sorry, I don't have that many %ss.\n", selectedProduct.getName()));
                // the customer's quantity is adjusted to match remaining stock
                quantity = quantityAvailable;
            }
            // if the customer wants more than they can afford
            if (quantity * selectedProduct.getPrice() > budget) {
                // the customer's quantity is adjusted to match remaining budget
                quantity = (int) (budget / selectedProduct.getPrice());
                if (quantity == 0) {
                    System.out.println(String.format("You can't afford any %s.", selectedProduct.getName()));
                    continue;
                }
                else {
                    System.out.println("You can't afford that many.");
				    System.out.println(String.format("You can only get %d with €%.2f", quantity, budget));
                }
            }
            // Make an offer and elicit confirmation
            System.out.println(String.format("I can give you %d for €%.2f", quantity, quantity * selectedProduct.getPrice()));
            System.out.println("Is that ok (y/n)?");

            input = scanner.nextLine();
            if (input.equalsIgnoreCase("Y")) {
                // Customer pays
                budget -= quantity * selectedProduct.getPrice();
                // Stock is adjusted
                selectedProductStock.setQuantity(quantityAvailable - quantity);
                // Shop gets paid
                this.cash += quantity * selectedProduct.getPrice();
            }
            else {
                System.out.println("OK, Never mind.");
            }
            // Customer can leave or continue shopping
            System.out.println(String.format("You have €%.2f left.", budget));
            System.out.println("Would you like to buy something else (y/n)?");
            input = scanner.nextLine();
            if (input.equalsIgnoreCase("Y")) {
                continue;
            }
            else {
                Utility.clearConsole();
                System.out.println(ShopSim.shopkeeper);
                System.out.println("OK. Thanks for shopping! Come back soon!");
                System.out.println("Press Enter to continue");
                Utility.waitForInput();
                ShopSim.menuControl();
            }
        }
    } 

    public void generateCustomers(int numCustomers) {
        String customersPath = System.getProperty("user.dir") + Configuration.CUSTOMERS_PATH;
        try {
            PrintWriter writer = new PrintWriter(customersPath, "UTF-8");
            for (int i = 0; i < numCustomers; i++) {
                Customer customer = randomCustomer();
                writer.println(String.format("%s, %.2f", customer.getName(), customer.getBudget()));
                for (ProductStock ps : customer.getShoppingList()) {
                    writer.println(String.format("%s, %d", ps.getProduct().getName(), ps.getQuantity()));
                }
                writer.println("----------");
            }
        }
        catch (IOException e) {
            System.out.println("Error writing to file");
        }

    }
        
    

    public void transact(Customer customer) {
        // Search for the item the customer is looking for in the shop's stock
        for (ProductStock ps : stock) {
            for (ProductStock itemstock : customer.getShoppingList()) {
                // If it's found...
                if (ps.getProduct().equals(itemstock.getProduct())) {
                    // If the customer wants more than the shop can supply...
                    if (itemstock.getQuantity() > ps.getQuantity()) {
                        System.out.println(String.format("%s wants %d x %s but only %d are available.", 
                            customer.getName(), itemstock.getQuantity(), itemstock.getProduct().getName(), ps.getQuantity()));
                            // The customer reduces the quantity they want to buy to what they can get
                            itemstock.setQuantity(ps.getQuantity());
                    }
                    // If the customer can' afford all they want, the amount they can afford is calculated
                    if (customer.getBudget() < itemstock.getCost()) {
                        itemstock.setQuantity((int)(customer.getBudget() / itemstock.getUnitPrice()));

                        if (itemstock.getQuantity() == 0) {
                            System.out.println(String.format("%s can't afford any %s.", customer.getName(), itemstock.getProduct().getName()));
                        }
                        else {
                            System.out.println(String.format("%s can only afford %d x %s.", customer.getName(), itemstock.getQuantity(), itemstock.getProduct().getName()));
                        }

                    }
                    // The transaction...
                    System.out.println(String.format("%s buys %d x %s for %.2f.", 
                        customer.getName(), itemstock.getQuantity(), itemstock.getProduct().getName(), itemstock.getCost()));
                    // Deduct stock from shop
                    ps.setQuantity(ps.getQuantity() - itemstock.getQuantity());
                    // Deduct money from customer
                    customer.setBudget(customer.getBudget() - itemstock.getCost());
                    // Add money to shop
                    cash += itemstock.getCost();
                    // Add purchase to customer's receipt
                    customer.addToReceipt(itemstock);
                    System.out.println(String.format("%s has  €%.2f left.%n", customer.getName(), customer.getBudget()));
                }
            }
        }
        if (Utility.contOrQuit()) {
            ShopSim.menuControl();
        }
        Utility.clearConsole();
        System.out.println(ShopSim.shopkeeper);
        System.out.println(String.format("Thank you %s. Here's your receipt. %nPlease come again.", customer.getName()));
        System.out.println(customer.toStringWithTotal(customer.getReceipt()));
    }

    public double shopVisit(Customer customer) {
        double cacheCash = this.getCash();
        Utility.clearConsole();
        System.out.println(String.format("%s has come into the shop with a shopping list!%n", customer.getName()));
        System.out.println(customer);
        if (Utility.contOrQuit()) {
            ShopSim.menuControl();
        }

        Utility.clearConsole();
        System.out.println(customer.getFace());
        System.out.println(String.format("%s is shopping", customer.getName()));

        transact(customer);

        if (Utility.contOrQuit()) {
            return 0;
        }

        // Check stock levels
        Utility.clearConsole();
        System.out.println(ShopSim.shopkeeper);
        System.out.println(String.format("Better take a look in the stockroom.%n%n"));
        List<Product> outOfStock = this.checkStock(Configuration.REORDER_THRESHOLD);
        if (Utility.contOrQuit()) {
            ShopSim.menuControl();
        }
        Utility.clearConsole();
        System.out.println(ShopSim.shopkeeper);
        if (outOfStock.size() > 0) {
            System.out.println("The following items are out of stock:");
            for (int i = 0; i < outOfStock.size(); i++) {
                System.out.println(String.format("%s%n", outOfStock.get(i).getName()));
            }
            System.out.println(this);
            if (Utility.contOrQuit()) {
                ShopSim.menuControl();
            }
            Utility.clearConsole();
            System.out.println(ShopSim.shopkeeper);
            for (Product p : outOfStock) {
                System.out.println(String.format("Restocking %s...%n", p.getName()));
                restock(p);
            }
        }
        else {
            System.out.println(String.format("Everything seems to be ok.%n"));
        }

        System.out.println(this);
        if (Utility.contOrQuit()) {
            ShopSim.menuControl();
        }
        Utility.clearConsole();

        double take = this.getCash() - cacheCash;

        return take;

    }
    
}