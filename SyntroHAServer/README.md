# SyntroHAServer

### Build

#### Linux
Go to the Linux subdirectory and enter:

	qmake
	make
	sudo make install

#### Windows
Go to the Windows subdirectory and use the provided solution file with VS2010 or newer. VS2010 will need the Qt plugin.

### Operation
Start the app. The first thing that needs to be done is to select the correct serial port for the Insteon 2413U PowerLinc modem. Use Action -> Serial port to select the correct port.

The Devices tab contains all devices known to the server. If the connected modem is paired with devices that aren't known to the server, the server will update its database with the new entries automatically. Devices can be added manually by using the Add button and filling out the Insteon address and giving the device a name and type. The modem is then updated with the new device information automatically.

The timers tab allows automatic timers to be created and managed. To create a timer, press the Add button. Timers can be configured to set devices to a specific level either at a specific time of selected days of the week, a randomized time or sunset or sunrise. Any number of devices can be added to a timer. For example, a timer could be created to turn all known devices off at a specific time each day.

Sunset and sunrise are calculated from the lattitude and longitude. To set this, the .ini file has to be edited. By default this is SyntroHAServer.ini and located at ~/.config/Syntro (Linux) or c:/users/<username>/AppData/Roaming/Syntro on Windows 7. The .ini file contains a [PLM] group and the lattitude and longitude can be entered there. When the app starts up, it displays the currently calculated sunrise and sunset times for validation.

Device states can be manually changed via SyntroHAController.
