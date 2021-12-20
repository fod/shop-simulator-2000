package shop;

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

    public double getCost(){
        return product.getPrice() * quantity;
    }

    @Override
    public String toString() {
        return "ProductStock [product=" + product + ", quantity=" + quantity + "]";
    }
    
}
