import logging
import time
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

class RTT(Enum):
	BYTES_TO_READ = 1024

class SerialComms:
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

	def connect_to_device(self, sn: int):
		self.api.connect_to_emu_with_snr(sn)

	def get_rtt(self):
		if not self.api.is_rtt_started():
			self.api.rtt_start()
			time.sleep(1)

		logs = self.api.rtt_read(0, RTT.BYTES_TO_READ.value)
		logger.info(f"read: {logs}")
		return logs

	def erase_device(self):
		self.api.erase_all()

	def write_to_device(self, ADDRESS, DATA, IS_FLASH: bool):
		self.api.write_u32(ADDRESS, DATA, IS_FLASH)

	def __del__(self):
		if self.api.is_rtt_started():
			self.api.rtt_stop()
		self.api.reset_connected_emu()
		self.api.disconnect_from_emu()
		self.api.close()
		logger.debug("Exited cleanly")
