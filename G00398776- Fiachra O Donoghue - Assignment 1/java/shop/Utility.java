package shop;

import java.util.Scanner;
import java.io.IOException;

/*
 *  Utility.java
 *
 *  Some helper methods for the shop simulator
 * 
 */
public class Utility {

    // Clear the console by printing a 100 newlines
    public static void clearConsole() {
        for (int i = 0; i < 100; i++) {
            System.out.println();
        }
    }

    // Wait for enter to be pressed
    public static void waitForInput() {
        try {
            System.in.read();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    // Wait for enter or quit
    public static boolean contOrQuit() {
        System.out.println("Press Enter to continue or 'Q' to quit.");
        Scanner scanner = new Scanner(System.in);
        String input = scanner.nextLine();
        if (input.equalsIgnoreCase("Q")) {
            System.out.println("Goodbye!");
            return true;
        }
        return false;
    }

}
