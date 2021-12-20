package shop;

import java.io.IOException;
import java.util.List;
import java.util.ArrayList;
import java.util.Collections;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.charset.StandardCharsets;

public class Shop {

    private double cash;
    private ArrayList<ProductStock> stock;

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
			System.out.println("The program could not find the CSV file with the shop information in it");
			e.printStackTrace();
			System.exit(-1);
		}
    }

    @Override
    public String toString() {
        String s = "";
        for (ProductStock ps : stock) {
            s += ps.toString() + "\n";
        }
        return s;
    }


    public void doAutoMode() {
        // TODO Auto-generated method stub
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



}