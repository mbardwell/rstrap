import logging
import re
from pathlib import Path

import click

logging.basicConfig(level=logging.DEBUG)


class Patterns:
	"""
	Class attributes are structured using JSON syntax:
		LOGGER->EMBEDDED_VERSION->DATA->[DATA STRUCTURE WITH N LEVELS]
	
	Supported loggers:
	- nrfconnect
		- 2
			- tension
				- sensor byte
					- command byte
						- data bytes (between 0-N)
	"""
	nrfconnect = {"2": {"tension": {"05": "00"}}}

class Parser:
	def __init__(self, logger: "Patterns.class_attribute" = Patterns.nrfconnect, version: str = "2"):
		"""
		:param logger: application used to take logs
		:param version: code version on embedded device when logs were taken
		"""
		self.pattern = logger[version]
		self.logger = logging.getLogger(self.__class__.__name__)

	def read_log(self, file: Path) -> str:
		with open(file, 'r') as f:
			self.logger.debug(f"file <{file}>:\n{f.read()}"); f.seek(0)
			read_file = f.readlines()
			return read_file

	def parse_nrfconnect_log(self, log: str):
		TIMESTAMP_PATTERN = "\d\d:\d\d:\d\d.\d\d\d"
		for line in log:
			for sensor_type in self.pattern:
				for sensor_byte in self.pattern[sensor_type]:
					command_byte = self.pattern[sensor_type][sensor_byte]
					match = re.compile(rf"({TIMESTAMP_PATTERN}).*\(0x\) {sensor_byte}-{command_byte}-([\d-]+)").search(line)
					if match:
						self.logger.info(
							f"{sensor_type} ({sensor_byte}{command_byte}) match: {match.group(1)} {self.dashed_ascii_to_decimal(match.group(2))}"
							)

	def dashed_ascii_to_decimal(self, dashed_ascii: str) -> str:
		"""
		ex. 35-32-33-35-31-34 -> 523514
		"""
		return bytearray.fromhex(dashed_ascii.replace("-", "")).decode()

@click.command(help=f"Parses tension from given log into a readable file. Valid log types: {[attr for attr in dir(Patterns) if not attr.startswith('__')]}")
@click.argument("log_file")
def main(log_file):
	parser = Parser(Patterns.nrfconnect)
	log = parser.read_log(log_file)
	parser.parse_nrfconnect_log(log)

if __name__ == "__main__":
	main()
