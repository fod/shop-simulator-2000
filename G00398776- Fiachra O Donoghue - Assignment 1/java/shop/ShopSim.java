package shop;

import java.util.Scanner;
import java.io.IOException;

public class ShopSim {

    private static boolean splash = true;

    private static String shopkeeper = "       _www_ \n"
                          + "      /-o-o-\\  \n"
                          + "    (|   -   |) \n"
                          + "      \\ -=- /  \n"
                          + "      /`---'\\  \n";

    private static void splash() {

        clearConsole();
        System.out.println(Configuration.LINE_SHORT);
        System.out.println();
        System.out.println("Shop Simulator 2000");
        System.out.println(shopkeeper);
        System.out.println("Hi I'm the shopkeeper.");
        System.out.println("Welcome to my shop.");
        System.out.println();
        System.out.println("A tall terminal window will \nenhance your experience.");
        System.out.println();
        System.out.println("Press Enter to continue.");
        try {
            System.in.read();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static void clearConsole() {
        for (int i = 0; i < 100; i++) {
            System.out.println();
        }
    }

    public static int menu() {


        // Show splash screen once only
        if (splash) {
            splash();
            splash = false;
        }

        String[] menuItems = {
            "Auto Mode",
            "Preset Mode",
            "Live Mode",
            "Generate Customers",
            "Reset Shop"
        };

        int selection;
        Scanner input = new Scanner(System.in);

        clearConsole();
        System.out.println(shopkeeper);
        System.out.println();
        System.out.println("How can I help?");
        System.out.println();
        System.out.println(Configuration.LINE_SHORT);
        System.out.println("MENU");
        System.out.println("----");
        for (int i = 0; i < menuItems.length; i++) {
            System.out.println(i + 1 + " - " + menuItems[i]);
        }
        System.out.println("x - Exit application");
        selection = input.nextInt();
        input.close();
        return selection;
    }
    public static void main(String[] args) {

        Shop shop = new Shop(Configuration.STOCK_PATH);
        int selection = menu();
        switch (selection) {
            case 1:
                //shop.doAutoMode();
                System.out.println(shop);
                break;
            case 2:
                shop.doPresetMode();
                break;
            case 3:
                shop.doLiveMode();
                break;
            case 4:
                shop.generateCustomers();
                break;
            case 5:
                shop = new Shop(Configuration.STOCK_PATH);
                break;
            case 6:
                shop.goodbye();
                break;
            default:
                System.out.println("Invalid selection.");
                break;

        }


    }
}