package shop;

import java.util.Properties;

/*
*   Configuration.java
*   Auto generated config file from shop.properties
*
*/
public class Configuration {

    private static final Properties properties = new Properties();

    static {
        try {
            /* Load the project config file */
            properties.load(Configuration.class.getResourceAsStream("shop.properties"));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    /* Input and output support file paths are global constants */
    public static final String STOCK_PATH = properties.getProperty("stock.path");
    public static final String CUSTOMERS_PATH = properties.getProperty("customers.path");
    public static final String NAMES_PATH = properties.getProperty("names.path");
    public static final String FACES_PATH = properties.getProperty("faces.path");
    public static final String SHOPKEEPER = properties.getProperty("face.shopkeeper");
    public static final String LINE_SHORT = properties.getProperty("line.short");
    public static final String LINE_MED = properties.getProperty("line.med");
    public static final String LINE_LONG = properties.getProperty("line.long");

    /* Other global config variables can be adjusted */
    public static int BUDGET_MIN = Integer.parseInt(properties.getProperty("budget.range.min"));
    public static int BUDGET_MAX = Integer.parseInt(properties.getProperty("budget.range.max"));
    public static int ITEMS_MIN = Integer.parseInt(properties.getProperty("items.range.min"));
    public static int ITEMS_MAX = Integer.parseInt(properties.getProperty("items.range.max"));
    public static int PIECES_MIN = Integer.parseInt(properties.getProperty("pieces.range.min"));
    public static int PIECES_MAX = Integer.parseInt(properties.getProperty("pieces.range.max"));
    public static int REORDER_THRESHOLD = Integer.parseInt(properties.getProperty("reorder.threshold")); 
    
}
