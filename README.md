### What is Intel Edison ?
(From http://www.intel.pl/content/www/pl/pl/do-it-yourself/edison.html )
>The Intel® Edison development platform is the first in a series of low-cost, product-ready, general purpose compute platforms that help lower the barriers to entry for entrepreneurs of all sizes—from pro makers to consumer electronics and companies working in the Internet of Things (IoT). The Intel Edison development platform packs a robust set of features into its small size, delivering great performance, durability, and a broad spectrum of I/O and software support. Those versatile features help meet the needs of a wide range of customers.

(Image below is linked from **Spark Fun's Shop**: https://www.sparkfun.com/products/13024 )
<div align="center"><img src="https://cdn.sparkfun.com//assets/parts/1/0/0/1/0/13024-01.jpg" alt="alt text" width="200px"/></div>

### What is this repo ?

A set of snippets how to develop Edison Bluetooth Low Energy applications using C programming language ( with BlueZ stack ).

Snippets list:

1. [DONE] How to advertise itself with custom name ? ( see main.c file )
2. [DONE] How to scan near devices ( with RSSI value ) ?

### How to build & run ?

1. Download and install Intel Edison SDK ( https://communities.intel.com/docs/DOC-23242 ).
2. Checkout this repostiory.
3. Create *config.py* file and add *EDISON_SDK_ROOT* variable there. EDISON_SDK_ROOT should point to Intel Edison SDK path.
4. Call *python build.py* from command line.
5. Copy main executable to Intel Edison and run it.
6. Check if Intel Edison device is visible during scanning on other bluetooth device.


<div align="center"><img src="http://i.imgur.com/zAGqc03.png"></div>

