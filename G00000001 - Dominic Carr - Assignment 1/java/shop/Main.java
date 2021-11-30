package shop;

public class Main {
    public static void main(String[] args) {

        Shop shop = new Shop();
        shop.addProduct(new Product("Bread", 1.99, 10));
        shop.addProduct(new Product("Milk", 2.99, 30));
        shop.addProduct(new Product("Butter", 3.99, 75));
        shop.addProduct(new Product("Cheese", 4.99, 62));
        shop.addProduct(new Product("Eggs", 5.99, 49));
        shop.addProduct(new Product("Bacon", 6.99, 101));
        shop.addProduct(new Product("Coffee", 7.99, 22));

        shop.printProducts();

    }
}