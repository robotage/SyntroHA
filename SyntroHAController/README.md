# SyntroHAController

### Build

#### Linux
Go to the Linux subdirectory and enter:

	qmake
	make
	sudo make install

#### Windows
Go to the Windows subdirectory and use the provided solution file with VS2010 or newer. VS2010 will need the Qt plugin.

#### OSX
Using a terminal window, navigate to the OSX subdirectory and enter:

	qmake
	make
	sudo make install

#### Android
Use QtCreator to build and deploy to an Android device. Information on setting up Qt for Android can be found here - http://qt-project.org/doc/qt-5/androidgs.html.

#### IOS
Use QtCreator or qmake and Xcode to build and deploy to an IOS device. Information on setting up Qt for IOS can be found here - http://qt-project.org/doc/qt-5/ios-support.html.

### Operation
Start the app. After a few seconds, the status line at the bottom of the window should show that it has found a SyntroControl. At the top, the "Choose server" button will become available (when the first SyntroNet directory has been received). Press this and the dialog will show the available SyntroHAServers. Choose the correct one and press "Ok". The main window should then populate with all the Insteon devices known to the server. As the server polls, it will update the device states.

To control a device, select the device's row. On desktops, just click the mouse to select. On Android or IOS, long press (i.e. press and hold for about two seconds) to do the same thing. The device control dialog will then appear. 
