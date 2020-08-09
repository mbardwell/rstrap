from typing import List
import logging

import PySimpleGUI as sg

import rstrap.comms.collect_data as source

logger = logging.getLogger(__name__)

sg.theme('BluePurple')

sensors = {
    "Accel X": {"key": "ax", "bytes": {"tag": 0x0, "command": 0x0}, "value": None},
    "Accel Y": {"key": "ay", "bytes": {"tag": 0x1, "command": 0x0}, "value": None},
    "Accel Z": {"key": "az", "bytes": {"tag": 0x2, "command": 0x0}, "value": None},
    "Tension": {"key": "t",  "bytes": {"tag": 0x5, "command": 0x0}, "value": None},
}

sensor_bytes = {data["bytes"]["tag"]: sensor for sensor, data in sensors.items()}

layout = [[sg.Text(display_name), sg.Text(size=(30,1), key=key["key"])] for display_name, key in sensors.items()]
layout.append([sg.Button('Exit')])

window = sg.Window('Show Me The Data', layout)

class SensorData:
    """ Convenience methods for sensor data collection """
    def setup(self):
        dev = source.Peripheral
        port = source.get_device_dev_path(dev.SERIAL_NUMBER)
        self.connection = source.UARTComms(115200, port)

    def get(self):
        return self.connection.read()

    def set_callback(self, cb):
        self.callback = cb

    def update_callback(self):
        if hasattr(self, "callback"):
            self.callback(self.get())

def convert_bytes(data: bytes) -> List:
    """ Incoming bytes look like b'BSx00+188BSx01-180BSx02+3996', where 

        Convert to string to break between BSx and clean any ' characters,
        then convert back to bytes

        **BS is the backslash escape character 
    """
    bytes_ = [bytes(i.replace("'",""), "utf8") for i in str(data).split("\\x")][1:]  # drop "b'"
    return bytes_

def update_data(bytes_: bytearray):
    """
    :param bytes_: ex - b'\x00\x00\x00+228\x01-152\x02+4020'
    """

    byte_list = convert_bytes(bytes_)
    for byte in byte_list:
        try:
            sensor = sensor_bytes[int(byte[:2])]
            sensors[sensor]["value"] = int(byte[2:])
        except Exception as e:
            logger.info(f"Exception {repr(e)}")


sensor = SensorData()
sensor.setup()
sensor.set_callback(update_data)
while True:  # Event Loop
    event, values = window.read(timeout=1)
    if event == sg.WIN_CLOSED or event == 'Exit':
        break
    sensor.update_callback()
    [window[data["key"]].update(data["value"]) for sensor,data in sensors.items()]
window.close()
