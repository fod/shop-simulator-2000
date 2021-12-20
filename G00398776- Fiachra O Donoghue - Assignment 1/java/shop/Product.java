package shop;

public class Product {
    private String name;
    private double price;
    private int maxQuantity;

    public Product(String name, double price, int maxQuantity) {
        this.name = name;
        this.price = price;
        this.maxQuantity = maxQuantity;
    }

    public String getName() {
        return name;
    }

    public double getPrice() {
        return price;
    }

    public int getMaxQuantity() {
        return maxQuantity;
    }

    public void setPrice(double price) {
        this.price = price;
    }

    public void setMaxQuantity(int maxQuantity) {
        this.maxQuantity = maxQuantity;
    }

    @Override
    public String toString() {
        return name + ": " + price + ": " + maxQuantity;
    }

}