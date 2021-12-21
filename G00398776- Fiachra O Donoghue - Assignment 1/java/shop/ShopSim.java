package shop;

import java.util.Scanner;
import java.io.IOException;

public class ShopSim {

    private static boolean splash = true;

    public static String shopkeeper = "       _www_ " + System.lineSeparator()
                          + "      /-o-o-\\  " + System.lineSeparator()
                          + "    (|   -   |) " + System.lineSeparator()
                          + "      \\ -=- /  " + System.lineSeparator()
                          + "      /`---'\\  " + System.lineSeparator();

    private static void splash() {

        Utility.clearConsole();
        System.out.println(Configuration.LINE_SHORT);
        System.out.println();
        System.out.println("Shop Simulator 2000");
        System.out.println(shopkeeper);
        System.out.println("Hi I'm the shopkeeper.");
        System.out.println("Welcome to my shop.");
        System.out.println();
        System.out.println(String.format("A tall terminal window will %nenhance your experience."));
        System.out.println();
        System.out.println("Press Enter to continue.");
        try {
            System.in.read();
        } catch (IOException e) {
            e.printStackTrace();
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

        Utility.clearConsole();
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
        //input.close();
        return selection;
    }

    public static void menuControl() {
        // Shop csv file location is outside the project
        String stockPath = System.getProperty("user.dir") + Configuration.STOCK_PATH;
        Shop shop = new Shop(stockPath);
        int selection = menu();
        switch (selection) {
            case 1:
                    shop.doAutoMode();
                break;
            // case 2:
            //     shop.doPresetMode();
            //     break;
            // case 3:
            //     shop.doLiveMode();
            //     break;
            // case 4:
            //     shop.generateCustomers();
            //     break;
            // case 5:
            //     shop = new Shop(Configuration.STOCK_PATH);
            //     break;
            // case 6:
            //     shop.goodbye();
            //     break;
            default:
                System.out.println("Invalid selection.");
                break;

        }
    }
    public static void main(String[] args) {

        menuControl();

    }
}