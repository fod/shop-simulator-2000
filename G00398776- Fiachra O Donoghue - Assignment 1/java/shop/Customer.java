package shop;

import java.util.ArrayList;

public class Customer {
    
    private String name;
    private String face;
    private Float budget;
    private ArrayList<Product> shoppingList;
    
    public Customer(String name, String face, Float budget) {
        this.name = name;
        this.face = face;
        this.budget = budget;
        shoppingList = new ArrayList<Product>();
    }

    public String getName() {
        return this.name;
    }

    public String getFace() {
        return this.face;
    }

    public Float getBudget() {
        return this.budget;
    }

    public ArrayList<Product> getShoppingList() {
        return this.shoppingList;
    }
}
