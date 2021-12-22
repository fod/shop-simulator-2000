package shop;

import java.util.ArrayList;
import java.util.List;
import java.util.Collections;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.charset.StandardCharsets;
import java.lang.StringBuilder;
import java.io.IOException;
import java.util.Random;

/*
*   Customer.java
*   Class representing a single shop customer
*   
*/


public class Customer {
    
    private String name;
    private String face;
    private double budget;
    private List<ProductStock> shoppingList;
    private List<ProductStock> receipt;
    
    // Constructor for nameless customers -- gets random name
    public Customer(double budget, List<ProductStock> shoppingList) {
        this.name = randName();
        this.face = randFace();
        this.budget = budget;
        this.shoppingList = shoppingList;
        this.receipt = new ArrayList<>();
    }

    // Constructor for named customers - i.e. from file
    public Customer(String name, double budget, List<ProductStock> shoppingList) {
        this.name = name;
        this.face = randFace();
        this.budget = budget;
        this.shoppingList = shoppingList;
        this.receipt = new ArrayList<>();
    }

    // Constructor for customer without shopping list - i.e. for live mode
    public Customer(String name, double budget) {
        this.name = name;
        this.face = randFace();
        this.budget = budget;
        this.shoppingList = new ArrayList<>();
        this.receipt = new ArrayList<>();
    }

    // Getters and setters
    public String getName() {
        return this.name;
    }

    public String getFace() {
        return this.face;
    }

    public double getBudget() {
        return this.budget;
    }

    public void setBudget(double budget) {
        this.budget = budget;
    }

    public List<ProductStock> getShoppingList() {
        return this.shoppingList;
    }

    public void addToShoppingList(ProductStock productStock) {
        this.shoppingList.add(productStock);
    }

    public List<ProductStock> getReceipt() {
        return this.receipt;
    }

    public void addToReceipt(ProductStock productStock) {
        this.receipt.add(productStock);
    }

    public double getTotal() {
        double total = 0;
        for (ProductStock productStock : this.shoppingList) {
            total += productStock.getCost();
        }
        return total;
    }

    // Randomly generate a name
    private String randName() {
        // Get relative file path
        String namePath = System.getProperty("user.dir") + Configuration.NAMES_PATH;
        List<String> lines = Collections.emptyList();
        List<String> names = new ArrayList<>();
        try {
            // Read entire file
            lines = Files.readAllLines(Paths.get(namePath), StandardCharsets.UTF_8);
            // extract names
            for (String line : lines) {
                names.add(line.trim());
            }
        } catch (IOException e) {
            System.out.println("Cannot locate Names file");
            e.printStackTrace();
            System.exit(-1);
        }
        // Seed random with current time
        Random random = new Random(System.currentTimeMillis());
        // Return random name
        return names.get(random.nextInt(names.size()));
    }

    // Randomly generate a face
    private String randFace() {
        // Get relative file path
        String facePath = System.getProperty("user.dir") + Configuration.FACES_PATH;
        List<String> lines = Collections.emptyList();
        List<String> faces = new ArrayList<>();
		try {
            // Read entire file
			lines = Files.readAllLines(Paths.get(facePath), StandardCharsets.UTF_8);
            
            // Build faces line by line
            StringBuilder sb = new StringBuilder();
            boolean append = false;
			for (String line : lines) {
                // If line is not empty and append is false, then it is a new face
                if ((line.trim().length() > 0) && !append) {
                    sb.append(line + System.lineSeparator());
                    append = true;
                }
                // If line is empty and append is true, then it is the end of a face
                else if ((line.trim().length() == 0) && append) {
                    faces.add(sb.toString());
                    sb = new StringBuilder();
                    append = false;
                }
                // If line is not empty and append is true, then it is part of a face
                else if ((line.trim().length() > 0) && append) {
                    sb.append(line + System.lineSeparator());
                }
			}
		}
		catch (IOException e) {
			System.out.println("Cannot locate Faces file");
			e.printStackTrace();
			System.exit(-1);
		}
        // Seed random with current time
        Random random = new Random(System.currentTimeMillis());
        // Return random face
        return faces.get(random.nextInt(faces.size()));
    }



    @Override
    public String toString() {
        // Build customer display table line by line
        StringBuilder sb = new StringBuilder();
        sb.append(this.getFace() + System.lineSeparator());
        sb.append("Name: " + this.name + System.lineSeparator());
        sb.append(String.format("Budget: %.2f", this.budget) + System.lineSeparator());
        // Call toStringWithTotal for shopping list display
        sb.append(this.toStringWithTotal(this.shoppingList));
        return sb.toString();
    }

    // Build shopping list display wihtout total
    public String toString(List<ProductStock> bill) {
        StringBuilder sb = new StringBuilder();
        int count = 1;
        for (ProductStock ps : bill) {
            sb.append(String.format("%d.  %s", count, ps.toStringWithTotal() + System.lineSeparator()));
            count++; 
        }
        return sb.toString();
    }

    // Build shopping list display with total
    public String toStringWithTotal(List<ProductStock> bill) {
        StringBuilder sb = new StringBuilder();
        sb.append(Configuration.LINE_LONG + System.lineSeparator());
        sb.append("    Item             Price     Quantity     Total" + System.lineSeparator());
        sb.append(Configuration.LINE_LONG + System.lineSeparator());
        sb.append(this.toString(bill));
        sb.append(Configuration.LINE_LONG + System.lineSeparator());
        sb.append(String.format("Total Cost: %37.2f", this.getTotal()) + System.lineSeparator());
        sb.append(Configuration.LINE_LONG + System.lineSeparator());
        return sb.toString();
    }

 

}
