package shop;


/*
*   Product.java
*   Class representing a the stock of a product in the shop
* 
*/

public class ProductStock {
    
    private Product product;
    private int quantity;

    public ProductStock(Product product, int quantity) {
        this.product = product;
        this.quantity = quantity;
    }

    public Product getProduct() {
        return product;
    }

    public double getUnitPrice() {
        return product.getPrice();
    }

    public String getName(){
        return product.getName();
    }

    public int getQuantity() {
        return quantity;
    }

    public void setQuantity(int quantity) {
        this.quantity = quantity;
    }

    public double getCost(){
        return product.getPrice() * quantity;
    }

    @Override
    public String toString() {
        return String.format("%s%13d", this.product.toString(), this.quantity);
    }
    
    public String toStringWithTotal() {
        return String.format("%s%10.2f", this.toString(), this.getCost());
    }
}
