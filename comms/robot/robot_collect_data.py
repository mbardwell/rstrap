from robot.api import logger as robot_logger

import rstrap.comms.collect_data as comms

class RobotUart(comms.UARTComms):
	def __init__(self, sn: int, baudrate: int=115200):
		port = comms.get_device_dev_path(sn)
		super(RobotUart, self).__init__(baudrate, port)