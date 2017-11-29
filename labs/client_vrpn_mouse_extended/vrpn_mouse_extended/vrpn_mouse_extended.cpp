#include "vrpn_Analog.h"
#include "vrpn_Button.h"

#include <iostream>
using namespace std;

int zone = 0;
bool mouseStatus = true;

/*
* Anytime there's a new update coming from the vrpn server.
* The callback is called
*/
void VRPN_CALLBACK handle_analog(void * userData, const vrpn_ANALOGCB a) {
	// While the mouse is activated
	if (mouseStatus)
	{
		// Zone A or B
		if (a.channel[0] < 0.5)
		{
			// Zone A / x E [0,0.5] && y E [0,0.5]
			if (a.channel[1] < 0.5 && zone != 0)
			{
				zone = 0;
				Beep(250, 500);
				cout << "Zone A" << endl;
			}
			// Zone B / x E [0,0.5] && y E [0.5,1]
			else if (a.channel[1] > 0.5 && zone != 1)
			{
				zone = 1;
				Beep(300, 500);
				cout << "Zone B" << endl;
			}
		}
		// Zone C or D
		else
		{
			// Zone C / x E [0.5,1] && y E [0,0.5]
			if (a.channel[1] < 0.5 && zone != 2)
			{
				zone = 2;
				Beep(320, 500);
				cout << "Zone C" << endl;
			}
			// Zone D / x E [0.5,1] && y E [0.5,1]
			else if (a.channel[1] > 0.5 && zone != 3)
			{
				zone = 3;
				Beep(340, 500);
				cout << "Zone D" << endl;
			}
		}
	}
}

/*
* Anytime there's a new update coming from the vrpn server.
* The callback is called
*/
void VRPN_CALLBACK handle_button(void * userData, const vrpn_BUTTONCB b) {	
	if (b.button == 0 && b.state == 1 && mouseStatus == true)
	{
		mouseStatus = false;
		cout << "The mouse is disabled." << endl;
	}

	if (b.button == 2 && b.state == 1 && mouseStatus == false)
	{
		mouseStatus = true;
		Beep(360, 500);
		cout << "The mouse is enabled." << endl;
	}
}

int main(int argc, char * argv[]) {
	/*
	* vrpn_Analog_Remote is a class that allows you to connect to a vrpn server
	* that has a defined a driver with an Analog output.
	* vrpn_Button_Remote is a class that allows you to connect to a vrpn server
	* that has a defined a driver with a Button output.
	* The constructor takes in the name of the device and the network address
	* of the server.
	*/
	vrpn_Analog_Remote* vrpnAnalog = new vrpn_Analog_Remote("Mouse0@localhost");
	vrpn_Button_Remote* vrpnButton = new vrpn_Button_Remote("Mouse0@localhost");

	//Registering a callback function
	vrpnAnalog->register_change_handler(0, handle_analog);
	vrpnButton->register_change_handler(0, handle_button);

	// To make sure the remote is properly updated
	while (1)
	{
		vrpnAnalog->mainloop();
		vrpnButton->mainloop();
	}

	return 0;
}
