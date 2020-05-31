*** Settings ***
Documentation		An application programming interface test suite
Library				collect_data.RobotUart	${SN_PERIPHERAL}	WITH NAME	Peripheral
Library				collect_data.RobotUart	${SN_CENTRAL}		WITH NAME	Central

*** Variables ***
${SN_PERIPHERAL}	682428779
${SN_CENTRAL}		681261902

*** Test Cases ***
PToC
	[Template]	Peripheral To Central
	hi

CToP
	[Template]	Central To Peripheral
	bye

*** Keywords ***
Peripheral To Central
	[Arguments]	${message}
	Peripheral.write	${message}
	${res} =	Central.read
	Log	${res}

Central To Peripheral
	[Arguments]	${message}
	Central.write	${message}
	${res} =	Peripheral.read
	Log	${res}