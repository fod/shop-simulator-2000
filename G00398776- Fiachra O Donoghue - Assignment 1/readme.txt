 ### Shop Simulator 2000 ###
           _www_ 
          /-o-o-\  
        (|   -   |)
          \ -=- /  
          /`---'\
      How can I help?

 ### Execution and use ###

# CONTENTS #
1. Introduction

2. Project structure

3. Compilation and Execution

    3.1 Python
    3.2 C
    3.3 Java

4. Description of program execution from a user perspective

    4.1 Auto Mode
    4.2 Preset Mode
    4.3 Live Mode
    4.4 Generate Customers
    4.5 Exit Application

5. Configuration variables

Appendix. 
    
    Output of gcc -v


---------------------------------------------------------------------
---------------------------------------------------------------------

# 1. Introduction

This project attempts to simulate some aspects of the purchase and sale of items in a small grocery shop. It is implemented separately in three programming languages, Python, C, and Java, and is intended to demonstrate the differences and similarities of software development within the (loosely defined) paradigms of high-level procedural, low-level procedural, and object orientation. This document contains instructions concerning the compilation (where relevant), execution, and testing of the project.

2. Project structure

The project contains the following directories and files:

    c/
    -- A directory containing the C source code. Details of the contents of this directory can be found in the C-specific section below (section 3.2).

    java/ 
    -- A directory containing the Java source code. Details of the contents of this directory can be found in the Java-specific section below (section 3.3).

    python/ 
    -- A directory containing the Python source code. Details of the contents of this directory can be found in the Python-specific section below (section 3.1).

    customers_average.csv 
    -- A csv file containing the name, budget, and shopping list for five randomly generated customers of middling means and demands.

    customers_greedy.csv 
    -- A csv file containing the name, budget and shopping list for five randomly generated customers of with requirements for high numbers of each item on their shopping list.

    customers_poor.csv 
    -- A csv file containing the name, budget and shopping list for five randomly generated customers low budgets.

    customers.csv
    -- A csv file containing the name, budget, and shopping list for each of sex randomly generated customers; two from each of the preceding customers files.

    faces 
    -- a plaintext file containing ascii art faces for the shop customers.

    names.list 
    -- a plaintext file containing a list of names for the shop customers.

    readme.txt 
    -- this file.

    report.pdf 
    -- a PDF file containing an description of the differences and similarities between the different implementations of the project.

    report.tex 
    -- the LaTeX source code for the report.

    stock.csv
    -- a csv file containing the name, price, and intitial quantity (also the maximum allowable quantity) of each item in the shop's stock. The first line of the file contains the shop's initial cash.

3. Compilation and Execution

    This software has been developed and compiled on a Linux x86_64 system. The specific compiler / interpreter versions used are listed below.

    3.1 Python
        shop.py has been tested using Python v3.10.1. Only standard library imports are used. A local file, found in teh same folder, containing ascii art customer faces, faces.py, is also imported. The software is executed with:

            python shop.py

        The software expects to find the resource (csv, etc) files in their current locations relative to the executable. The expected locations of these files can be altered by changing the values of the relevant global variables defined at the beginning of the shop.py file.

    3.2 C
        shop.c has been tested using GCC v11.1.0. The full output of GCC -v is included in the appendix at the end of this document. The software has been tested as follows:

        The C source code is compiled using the following command:

            gcc shop.c

        The software is executed with:

            ./a.out

        The software expects to find the resource (csv, etc) files in their current locations relative to the executable. The expected locations of these files can be altered by changing the values of the relevant variables defined at the beginning of the shop.c file.

    3.3 Java
        shop.java has been tested using OpenJDK v17.0.1. The project was compiled using the following command from outside the shop directory:
        
            javac shop/ShopSim.java

        For convenience the .class bytecode files are included in the project zip file. The software should be executed from OUTSIDE THE SHOP DIRECTORY with:

            java shop/ShopSim

4. Description of program execution from a user perspective

    The shop simulator software is designed to offer an almost identical experience regardless of the implementation language. The following section describes some execution paths the user may experience. While features are described, their implementation is outside the scope of this document. More details on the implementation of the software can be found in the report.pdf document.

    Execution of the shop simulator software begins with a splash screen advising the user to use a tall terminal window. Following this advice will lead to a better experience as at certain points the number simulator output lines may exceed terminal height.

    The splash screen is followed by a menu containing the following items:

        1 - Auto Mode
        2 - Preset Mode
        3 - Live Mode
        4 - Generate Customers
        5 - Exit Application 

    The desired option is selected by typing the appropriate key followed by Enter.

    4.1 Auto Mode
        Auto mode continuously generates random customers with random shopping lists and budgets, and conducts a transaction for each item on the customer's shopping list. A shopping list consists of the name of an item and the quantity desired. The transactions are conducted in the order in which the items appear on the shopping list. After each transaction, the customer's budget, the shop's stock, and the shop's cash are all updated. A transaction is conductede using the following rules:

            1. If an item is in stock, the shop's stock meets or exceeds the customer's desired quantity, and the customer's budget is sufficient, the item is purchased and the customer's budget, the shops cash, and the shop's stock are all updated.

            2. If an item is in stock, the shop's stock meets or exceeds the customer's desired quantity, but the customer's budget is insufficient, The number of units the customer can afford is calculated and the item is purchased. The customer's budget, the shops cash, and the shop's stock are all updated.

            3. If an item is in stock but the shop's stock is insufficient to provide the full quantity the customer requires then the customer purchases the entire remaining stock and the customer's budget and the shops cash and stock are updated appropriately.

            4. If an item is not in stock (i.e. the stock is zero), then no purchase is made.

        After each shop visit, the cutomer's receipt is displayed showing what items and quantities were purchased with subtotals and total.

        Also after each shop visit the shop's stock list is displayed and any items the quantity of which falls below a reorder threshold - defined in the source file - are restocked. The restock consists of bringing the stock quantity back up to its initial level - defined in the stock.csv file.

        Note that the same shop persists through the lifetime of the application run. Also note that, unlike a real shop, no markup is added to the prices of items - the shop sells everything at the same price it purchased it. This leads to the shop's cash balancing out at, using the default initial values, 3000-4000 euro over time.

        Auto Mode continues until the user returns to the main menu which they can do at any time by pressing the 'Q' key followed by Enter.

    4.2 Preset Mode
        Preset mode operates identically to auto mode except that instead of generating random cutomers, cutomers are loaded from the customers.csv file (however the customers in the customers.csv file by default have, in fact, been randomly generated). After all customers have visited the shop's take for the day is displayed and the user is returned to the main menu.

    4.3 Live Mode
        In live mode the user plays the part of the customer. The user is first asked to enter their budget, after which they are guided through a series of transactions. Each transaction consists of selecting an item and quantity and then applying thwe transaction rules listed above in the normal way. When the user's budget has been depleted to the point where they can't afford anything in the shop, they are ejected by the shopkeeper.

    4.4 Generate Customers
        This option is used to generate lists of random customers to be used in preset mode. The number of customers to be generated, as well as all of the customer's attributes are specified in configuration variables in the source code. These variables are near the top of the source file in the Python and C versions of the code, and in a separate shop.properties file in the Java version (this is used to generate a Configuration class).

        The attributes that can be adjusted are as follows. Where an atribute is a range then, for each customer, the specific random value is chosen from that range.

            BUDGET_RANGE: The range from which the customer's budget is randomly selected. Default = 100-400.

            ITEMS_RANGE: The number of items on the customers shopping list. Default = 1-5.

            PIECES_RANGE: The quantity of each item on the customers shopping list. Default = 1-100.

        The pregenerated lists held in the customers_greedy.csv, and customers_poor.csv files are the same as the defaults but adjusted as follows:

            customers_poor BUDGET_RANGE = 10-100
            customers_greedy PIECES_RANGE = 100-200

        The customers_average.csv file was generated using the default attribute values.

        The number of customers to be generated defaults to 5 and is passed as the first argument to the customer genertaion function.

    4.5 Exit Application
        The user can exit the application at any time by pressing the 'Q' key followed by Enter or the 'x' key on the main menu screen (or the '9' key in the Java version).

5. Configuration variables
    The application is configured by adjusting the values of configuration variables near the top of the source file in the Python and C versions, and in the shop.properties file in the Java version. The random customer generation variables are discussed in section 4.4 above and will not be listed again here. The remaining variables are:

        STOCK_PATH: Path to the stock.csv file. Default = ./stock.csv.
        NAMES_PATH: Path to a file containing a list of names from which to choose names for customers.
        FACES_PATH (C and Java versions only): Path to a file containing ASCII art faces to be used for customers.
        CUSTOMERS_PATH: Path to random customers file. Used for reading in customers for Preset Mode and wroting customers in Generate Customers Mode.
        REORDER_THRESHOLD: The quantity below which an item is restocked by the shop. Default = 5.

Appendix

Output of gcc -v

    The output of gcc -v on the system used to compile the project is:
    ❯ gcc -v
    Using built-in specs.
    COLLECT_GCC=gcc
    COLLECT_LTO_WRAPPER=/usr/lib/gcc/x86_64-pc-linux-gnu/11.1.0/lto-wrapper
    Target: x86_64-pc-linux-gnu
    Configured with: /build/gcc/src/gcc/configure --prefix=/usr --libdir=/usr/lib --libexecdir=/usr/lib --mandir=/usr/share/man --infodir=/usr/share/info --with-bugurl=https://bugs.archlinux.org/ --enable-languages=c,c++,ada,fortran,go,lto,objc,obj-c++,d --with-isl --with-linker-hash-style=gnu --with-system-zlib --enable-__cxa_atexit --enable-cet=auto --enable-checking=release --enable-clocale=gnu --enable-default-pie --enable-default-ssp --enable-gnu-indirect-function --enable-gnu-unique-object --enable-install-libiberty --enable-linker-build-id --enable-lto --enable-multilib --enable-plugin --enable-shared --enable-threads=posix --disable-libssp --disable-libstdcxx-pch --disable-libunwind-exceptions --disable-werror gdc_include_dir=/usr/include/dlang/gdc
    Thread model: posix
    Supported LTO compression algorithms: zlib zstd
    gcc version 11.1.0 (GCC) 


