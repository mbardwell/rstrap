
### What is this repository for? ###

* Firmware development for a BLE-enabled transportation safety device
* Development is NRF52832 specific. Tests are run on a Fanstel BlueNor BT832F

### Tree ###
```bash
├── nrf52.svd
├── nRF5_SDK_15.2.0_9412b96
│   ├── components
│   ├── config
│   ├── documentation
│   ├── examples
│   │   ├── ble_central
│   │   ├── ble_peripheral <-- rStrap code here (ble_app_hts folder)
│   │   └── peripheral
│   ├── external
│   ├── external_tools
│   ├── integration
│   ├── LICENSE
│   ├── license.txt
│   ├── modules
│   ├── nRF5x_MDK_8_17_0_IAR_NordicLicense.msi
│   ├── nRF5x_MDK_8_17_0_Keil4_NordicLicense.msi
│   └── README.md
├── README.md
└── rstrap.code-workspace
```


### Setup ###

* Clone repository
```git clone https://github.com/mbardwell/rstrap/```


### Developer Setup ###

* Follow the SDK setup steps found [here](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk52.v0.9.1%2Findex.html)
* [Segger Embedded Studio](https://www.segger.com/products/development-tools/embedded-studio/) is a good, free, IDE
* Once SDK and Segger are setup. To compile any of the example code provided (each example is in its own folder), follow the instructions of the README in that folder.


### Resources ###

* [HX711 BLE UART sample code](https://devzone.nordicsemi.com/f/nordic-q-a/40271/timer-issue-with-hx711---stopped-by-vector-catch-error)
