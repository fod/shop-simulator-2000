package shop;

public class Shop {

    private Product[] products;
    private int productCount;

    public Shop() {
        products = new Product[100];
        productCount = 0;
    }

    public void addProduct(Product product) {
        products[productCount] = product;
        productCount++;
    }

    public Product getProduct(int index) {
        return products[index];
    }

    public int getProductCount() {
        return productCount;
    }

    public void printProducts() {
        for (int i = 0; i < productCount; i++) {
            System.out.println(products[i]);
        }
    }
}