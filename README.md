
### What is this repository for? ###

* Firmware development for a BLE-enabled transportation safety device
* Development is NRF52832 specific. Tests are run on a Fanstel BlueNor BT832F

<p align="center">
<img src="https://user-images.githubusercontent.com/11367325/65797133-b0595a80-e12b-11e9-8613-f1bada2212fb.png" alt="drawing" width="200"/>
</p>

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

* Use mount holes to connect printed circuit board to hardware
* Plug in:
  * J1: hx711 tension sensor
  * J2: 3V power

Device will start advertising itself

### Developer Setup ###

Linux:

1. Clone repository
```git clone https://github.com/mbardwell/rstrap/```
2. Run ```./setup```


Make optional arguments:

* -jX: use X cores ([speeds up compiling](https://stackoverflow.com/questions/414714/compiling-with-g-using-multiple-cores))
* DEBUG=1: turns on logging
* DEBUG_PIN=X: sets debug pin to X [default pin found in ble_app_hts/pca10040/s132/config/sdk_config.h]
* BAUDRATE=X: sets debug pin baudrate to X [default BR found in ble_app_hts/pca10040/s132/config/sdk_config.h]
* DEVKIT=1: sets accelerometer, tension communication pins to natural areas on devkit board

```e.g. sudo make -j8 flash DEBUG=1 DEBUG_PIN=22```

Suggestion: the fastest way to start viewing the data is to download a cellphone app that allows you to scan and connect with bluetooth low energy devices (e.g. nRF Connect)


### Resources ###

* [HX711 BLE UART sample code](https://devzone.nordicsemi.com/f/nordic-q-a/40271/timer-issue-with-hx711---stopped-by-vector-catch-error)
