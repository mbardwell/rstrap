
### How do I use the code in ble_app_uart_w_hx711? ###

* Upload code according to the setup intructions below
* When example code starts, device is in wait mode. You should see LED 2 on. Attached the scale and place an item on top; this represents the strap being 'tightened'. Next, press button 2. The device samples the strain gauge, averages over x-number of readings and sets 90% of that number as the tension. When the sampling is done, LED 2 turns off and LED 3 turns on. If the tension falls below the threshold (remove the object from the scale), LED 3 turns off and LED 4 turns on.

### How do I get set up? ###

* Place ble_app_uart_w_hx711 folder from repo in ./examples/ble_peripheral and open ble_app_uart_pca10040_s132.emProject (found in ble_app_uart_w_hx711\pca10040\s132\ses\). The project should compile.


### Contribution guidelines ###

* Make issues if you find bugs or want features

### Who do I talk to? ###

* Repo admin: please send emails with title 'rStrap Firmware - *Question/Comment*' to bardwell@ualberta.ca