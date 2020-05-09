import logging
from abc import ABC
from enum import Enum
from typing import List

import pynrfjprog
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

class SerialComms:
	def __init__(self, hardware: NordicDK):
		self.hardware = hardware
		self.api = self.get_api()
		self.open_device()
		logger.debug("hi")

	def get_api(self) -> pynrfjprog.LowLevel.API:
		return API.API(self.hardware.DEV_FAMILY)

	def open_device(self):
		self.api.open()

	def connect_to_device(self, sn: int):
		self.api.connect_to_emu_with_snr(sn)

	def get_serial_numbers(self) -> List:
		sn = self.api.enum_emu_snr()
		logger.info(f"serial numbers: {sn}")
		return sn

	def get_rtt(self):
		if self.api.is_rtt_started():
			self.api.rtt_start()
			logger.info(f"read: {self.api.rtt_read(0, 100)}")
		else:
			logger.info(f"Cannot read device. Rtt not started")

	def erase_device(self):
		self.api.erase_all()

	def write_to_device(self, ADDRESS, DATA, IS_FLASH: bool):
		self.api.write_u32(ADDRESS, DATA, IS_FLASH)

	def __del__(self):
		if self.api.is_rtt_started():
			self.api.rtt_stop()
		self.api.disconnect_from_emu()
		self.api.close()
		logger.debug("Exited cleanly")
