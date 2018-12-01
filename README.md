
### What is this repository for? ###

* Firmware development for a BLE-enabled transportation safety device
* Development is NRF52832 specific

### Folders ###
Organization is based on the open source SDK. Folders of interest include:

* ./examples/peripheral
* ./examples/ble_peripheral
* Code that Mike is working on will be in folders with mike in the name; ex: ./examples/peripheral/twi_mike_newtemplate

### How do I get set up? ###

* Follow the SDK setup steps found [here](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk52.v0.9.1%2Findex.html)
* [Segger Embedded Studio](https://www.segger.com/products/development-tools/embedded-studio/) is a good, free, IDE
* Once SDK and Segger are setup. Place twi_mike folder from repo in ./examples/peripheral and open twi_mike.emProject (found in twi_mike\pca10040\blank\ses\). You now have access to all source files and can upload code to the NRF52 DK


### Contribution guidelines ###

* Make issues if you find bugs or want features

### Who do I talk to? ###

* Repo admin: please send emails with title 'rStrap Firmware - *Question/Comment*' to bardwell@ualberta.ca