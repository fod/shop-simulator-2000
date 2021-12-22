 ### Shop Simulator 2000 ###
           _www_ 
          /-o-o-\  
        (|   -   |)
          \ -=- /  
          /`---'\
      How can I help?

 ##### Execution and use #####

# CONTENTS #
1. Introduction

2. Project structure

3. Compilation and Execution

    3.1 Python
    3.2 C
    3.3 Java

4. Description of program execution from a user perspective

------------------------------------------------------------

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
    -- a csv file containing the name, price, and intitial quantity (also the maximum allowable quantity) of each item in the shop's stock.

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

    4. Auto Mode
       Auto mode continuously generates random customers with random shopping lists and budgets, and conducts a transaction for each item on the customer's shopping list. A shopping list consists of the name of an item and the quantity desired. The transactions are conducted in the order in which the items appear on the shopping list. After each transaction, the customer's budget, the shop's stock, and the shop's cash are all updated. 






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

customers_poor BUDGET_RANGE = (10, 100)
custommers_greedy PIECES_RANGE = (100, 200)
customers_average BUDGET_RANGE = (100, 400)
ITEMS_RANGE = (1, 5)
REORDER_THRESHOLD = 5
