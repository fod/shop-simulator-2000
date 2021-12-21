package shop;

import java.io.IOException;
import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.charset.StandardCharsets;
import java.util.Random;


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

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(Configuration.LINE_MED + System.lineSeparator());
        sb.append("  Item             Price        Stock" + System.lineSeparator());
        sb.append(Configuration.LINE_MED + System.lineSeparator());
        for (ProductStock ps : stock) {
            sb.append("  " + ps.toString() + System.lineSeparator());
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
        // TODO Auto-generated method stub
    }

    public void doLiveMode() {
        // TODO Auto-generated method stub
    }

    public void generateCustomers() {
        // TODO Auto-generated method stub
    }

    public void goodbye() {
        // TODO Auto-generated method stub
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
            ShopSim.menuControl();
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
            System.out.println(String.format("%s%n", outOfStock.get(0).getName()));
            for (int i = 1; i < outOfStock.size(); i++) {
                System.out.println(String.format("%s, ", outOfStock.get(i).getName()));
            }
            System.out.println(String.format("and %s.", outOfStock.get(outOfStock.size() - 1).getName()));
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