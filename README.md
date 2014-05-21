# SyntroHA

SyntroHA contains the apps for SyntroNet Home Automation. This includes SyntroHAServer and SyntroHAController.

SyntroHAServer interfaces to an Insteon 2413U PowerLinc modem in order to control various Insteon devices. Timers can be configured on SyntroHAServer for automatic operation. SyntroHAServer has been tested on Ubuntu and Windows 7.

SyntroHAController is a simple app to control Insteon devices via SyntroNet that includes an instance of SyntroHAServer. SyntroHAController has been tested on Linux (Ubuntu), Windows 7, Mac OSX, Android and IOS. All platforms use the Qt environment for building and deployment.

Check out www.richards-tech.com for more details.

### Dependencies

1. Qt5.2.1 development libraries and headers (with Android and IOS versions if desired)
2. SyntroCore libraries and headers 

### Fetch and Build

####Linux

If SyntroCore has not been installed yet:

	mkdir ~/SyntroNet
	cd ~/SyntroNet
	git clone git://github.com/richards-tech/SyntroCore.git
	cd SyntroCore
	qmake
	make
	sudo make install

Then:

	cd ~/SyntroNet
	git clone git://github.com/richards-tech/SyntroHA.git
	cd SyntroHA
	qmake
	make
	sudo make install

This will make and install SyntroHAServer and SyntroHAController.

Alternatively, each app can be individually compiled. For example, to compile SyntroHAServer:

	cd ~/SyntroNet/SyntroHA/SyntroHAServer
	qmake
	make
	sudo make install

### Run

See the readme in the appropriate subdirectory for each app.


