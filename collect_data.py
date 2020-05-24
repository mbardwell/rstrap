import logging
import time
from abc import ABC
from enum import Enum
from typing import List

import click
import pynrfjprog
import serial
from pynrfjprog import API

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

class NordicDK(ABC):
	SERIAL_NUMBER = None
	DEV_FAMILY = None

class Client(NordicDK):
	SERIAL_NUMBER = 682428779
	DEV_FAMILY = 'NRF52'

class Server(NordicDK):
	SERIAL_NUMBER = 681261902
	DEV_FAMILY = 'NRF51'

class CommParameters(Enum):
	BYTES_TO_READ = 1024

class RTTComms:
	def __init__(self, hardware: NordicDK):
		self.hardware = hardware
		self.api = self.get_api()
		self.open_device()
		self.connect_to_device(hardware.SERIAL_NUMBER)

	def get_api(self) -> pynrfjprog.LowLevel.API:
		return API.API(self.hardware.DEV_FAMILY)

	def open_device(self):
		if not self.api.is_open():
			self.api.open()
			logger.debug("Opened JLinkARM.dll")
		else:
			logger.debug("JLinkARM.dll is already open")

	def connect_to_device(self, sn: int):
		try:
			self.api.connect_to_emu_with_snr(sn)
		except Exception as e:
			logger.debug(f"Exception: {e}")

	def read(self, nbytes: int=CommParameters.BYTES_TO_READ.value):
		if not self.api.is_rtt_started():
			self.api.rtt_start()
			time.sleep(1)
		logs = self.api.rtt_read(0, nbytes)
		logger.info(f"read: {logs}")
		return logs

	def erase_device(self):
		self.api.erase_all()

	def write_to_device(self, ADDRESS, DATA, IS_FLASH: bool):
		self.api.write_u32(ADDRESS, DATA, IS_FLASH)

	def read_continuously(self, freq: int=2):
		hold = time.time()
		while True:
			if (time.time() - hold) > (1/freq):
				self.read()
				hold = time.time()

	def __del__(self):
		if self.api.is_rtt_started():
			self.api.rtt_stop()
		self.api.reset_connected_emu()
		self.api.sys_reset()
		self.api.disconnect_from_emu()
		self.api.close()
		logger.debug("Exited RTT cleanly")

class UARTComms:
	def __init__(self, baudrate, port):
		self.baudrate = baudrate
		self.port = port
		self.ser = serial.Serial(port, baudrate, timeout=1)

	def read(self, nbytes: int=CommParameters.BYTES_TO_READ.value):
		return self.ser.read(nbytes)

	def readline(self):
		return self.ser.readline()

	def read_continuously(self, freq: int=2):
		hold = time.time()
		while True:
			if (time.time() - hold) > (1/freq):
				self.read()
				hold = time.time()

	def __del__(self):
		self.ser.close()
		logger.debug("Exited UART cleanly")


@click.command()
@click.option("-c", '--channel',required=True,	type=click.Choice(['uart', 'rtt'], case_sensitive=False),
help="UART [requires --port] or RTT channel [requires --device]")
@click.option("-d", '--device',	required=True,	type=click.Choice(['client', 'server']),
help="client (peripheral) or server (central) device")
def main(device, channel, port):
	if channel == "uart":
		if port is None:
			raise ValueError("uart requires --port to be specified")
		uart = UARTComms(115200, port)
		uart.read_continuously()
	else:
		if device is None:
			raise ValueError("rtt requires --device to be specified")
		rtt = RTTComms(Client) if channel == "client" else RTTComms(Server)
		rtt.read_continuously()

if __name__ == "__main__":
	main()
