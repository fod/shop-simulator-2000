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

public class Customer {
    
    private String name;
    private String face;
    private double budget;
    private List<ProductStock> shoppingList;
    private List<ProductStock> receipt;
    
    public Customer(double budget, List<ProductStock> shoppingList) {
        this.name = randName();
        this.face = randFace();
        this.budget = budget;
        this.shoppingList = shoppingList;
        this.receipt = new ArrayList<>();
    }

    private String randName() {
        String namePath = System.getProperty("user.dir") + Configuration.NAMES_PATH;
        List<String> lines = Collections.emptyList();
        List<String> names = new ArrayList<>();
        try {
            lines = Files.readAllLines(Paths.get(namePath), StandardCharsets.UTF_8);
            for (String line : lines) {
                names.add(line.trim());
            }
        } catch (IOException e) {
            System.out.println("Cannot locate Names file");
            e.printStackTrace();
            System.exit(-1);
        }
        Random random = new Random(System.currentTimeMillis());
        return names.get(random.nextInt(names.size()));
    }

    private String randFace() {
        String facePath = System.getProperty("user.dir") + Configuration.FACES_PATH;
        List<String> lines = Collections.emptyList();
        List<String> faces = new ArrayList<>();
		try {
			lines = Files.readAllLines(Paths.get(facePath), StandardCharsets.UTF_8);
            
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
        Random random = new Random(System.currentTimeMillis());
        return faces.get(random.nextInt(faces.size()));
    }

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

    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append(this.getFace() + System.lineSeparator());
        sb.append("Name: " + this.name + System.lineSeparator());
        sb.append(String.format("Budget: %.2f", this.budget) + System.lineSeparator());
        sb.append(this.toStringWithTotal(this.shoppingList));
        return sb.toString();
    }

    public String toString(List<ProductStock> bill) {
        StringBuilder sb = new StringBuilder();
        int count = 1;
        for (ProductStock ps : bill) {
            sb.append(String.format("%d.  %s", count, ps.toStringWithTotal() + System.lineSeparator()));
            count++; 
        }
        return sb.toString();
    }

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
