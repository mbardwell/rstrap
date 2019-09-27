
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

Suggestion: the fastest way to start viewing the data is to download a cellphone app that allows you to scan and connect with bluetooth low energy devices (e.g. nRF Connect)


### Resources ###

* [HX711 BLE UART sample code](https://devzone.nordicsemi.com/f/nordic-q-a/40271/timer-issue-with-hx711---stopped-by-vector-catch-error)
