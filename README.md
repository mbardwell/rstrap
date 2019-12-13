
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
│   │   ├── ble_peripheral 
|   |   |   ├──ble_app_hts <-- rStrap multi-service method (v1)
|   |   |   └──ble_nus <-- rStrap uni-service method (v2)
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


### Quick Start ###

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

NUS commands:

|                | NUS Channel | Sensor Byte | Command Byte | Data Byte 1* | Data Byte 2 | Data Byte 2 | ... | Explination                                                                                                                                          |
|----------------|-------------|:-----------:|:------------:|:------------:|:-----------:|:-----------:|:---:|------------------------------------------------------------------------------------------------------------------------------------------------------|
| Set tension    |  rx         | 05          | 01           | 95           |             |             |     | The client requests that the server take a new tension measurement, multiply it by 0.95 and use the output as the threshold. LED should change color |
| Tension Alert  |  rx         | 05          | 02           |              |             |             |     | The server has detected that the tension has fallen below the client-set threshold. LED should change color                                          |
| Update Battery | tx          | 03          | 00           |              |             |             |     | Client requests battery voltage update                                                                                                               |
| Update Battery | rx          | 03          | 00           | 33           | 31          | 36          | 32  | Client receives battery voltage update (3162)                                                                                                        |
| Update Temp    | tx          | 04          | 00           |              |             |             |     | Client requests temperature update                                                                                                                   |
| Update Temp    | rx          | 04          | 00           | 28 (+)       | 32          | 30          |     | Client receives temperature update (+20)                                                                                                             |
| Update Accel-X | rx          | 00          | 00           | 2D (-)       | 33          | 30          | 30  | Client receives accel-x update  (-300)                                                                                                               |
| Update Accel-Y | rx          | 01          | 00           | 2B           | 31          | 34          |     | Client receives accel-y update (+14)                                                                                                                 |
| Update Accel-Z | rx          | 02          | 00           | 2D           | 35          | 35          |     | Client receives accel-z update (-55)                                                                                                                 |
| Update Tension | rx          | 05          | 00           | 31           | 36          | 37          | 30  | Client receives tension update (1670)                                                                                                                |
| Shutdown       | tx          | 06          |              |              |             |             |     | Client requests server shutdown                                                                                                                      |
|                |             |             |              |              |             |             |     | * All data bytes are ASCII encoded                                                                                                                   |

on nRFConnect it looks like

![image](https://user-images.githubusercontent.com/11367325/67082812-1ea3a280-f157-11e9-835f-39bc6ea60ac4.png)

All the SENSOR and COMMAND bytes can be found in config.h

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

```e.g. sudo make -j8 flash DEBUG=1 DEBUG_PIN=26```

Suggestion: the fastest way to start viewing the data is to download a cellphone app that allows you to scan and connect with bluetooth low energy devices (e.g. nRF Connect)

### Resources ###

* [HX711 BLE UART sample code](https://devzone.nordicsemi.com/f/nordic-q-a/40271/timer-issue-with-hx711---stopped-by-vector-catch-error)
