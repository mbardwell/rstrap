
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
│   │   ├── ble_peripheral <-- rStrap code here (ble_app_nus folder)
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
  * J1: strain gauge
  * J2: 3V power

Device will start advertising itself.

LED indicators:

* Flashing blue means device is advertising
* Solid blue means device has connected
* Solid green means tension threshold has been set
* Solid red means tension has fallen below the set threshold

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
* DEVKIT=1: sets accelerometer, tension communication and led pins to natural areas on devkit board

```e.g. sudo make -j8 flash DEBUG=1 DEBUG_PIN=22```

Suggestion: the fastest way to start viewing the data is to download a cellphone app that allows you to scan and connect with bluetooth low energy devices (e.g. nRF Connect)



How to communicate with the device using the Nordic UART Service (NUS):

* Turn on device. Connect via bluetooth. Send bytes using UART RX characteristic
* Bytes should be sent in this order in the same transmission: SENSOR TAG [1 byte], COMMAND [1 byte], DATA [n ascii bytes]

e.g. sending 05 [NUS_TENSION_TAG], 01 [SET], 39, 35 [2 DATA BYTES] gets translated to 05, 01, 95

on nRFConnect it looks like

![image](https://user-images.githubusercontent.com/11367325/67082812-1ea3a280-f157-11e9-835f-39bc6ea60ac4.png)

All the SENSOR TAG and COMMAND bytes can be found in config.h

### Resources ###

* [HX711 BLE UART sample code](https://devzone.nordicsemi.com/f/nordic-q-a/40271/timer-issue-with-hx711---stopped-by-vector-catch-error)
