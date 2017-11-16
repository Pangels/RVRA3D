#include "vrpn_Analog.h"
#include "vrpn_Button.h"
#include "vrpn_Tracker.h"

#include <iostream>
using namespace std;

/*
* Anytime there's a new update coming from the vrpn server.
* The callback is called
*/
void VRPN_CALLBACK handle_analog(void * userData, const vrpn_ANALOGCB a) {
	int nbChannels = a.num_channel;

	cout << "Analog : ";

	for (int i = 0; i < a.num_channel; i++)
	{
		cout << a.channel[i] << " ";
	}

	cout << endl;
}

void VRPN_CALLBACK handle_button(void * userData, const vrpn_BUTTONCB b) {
	cout << "Button '" << b.button << "': " << b.state << endl;
}

void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t)
{
	cout << "Tracker '" << t.sensor << "' : " << t.pos[0] << "," << t.pos[1] << "," << t.pos[2] << endl;
}

int main(int argc, char * argv[]) {
	/* vrpn_Analog_Remote is a class that allows you to connect to a vrpn server
	* that has a defined a driver with an Analog output.
	* vrpn_Button_Remote is a class that allows you to connect to a vrpn server
	* that has a defined a driver with a Button output.
	* The constructor takes in the name of the device and the network address
	* of the server.
	*/
	vrpn_Analog_Remote* vrpnAnalog = new vrpn_Analog_Remote("Mouse0@localhost");
	vrpn_Button_Remote* vrpnButton = new vrpn_Button_Remote("Mouse0@localhost");
	vrpn_Tracker_Remote* vrpnTracker = new vrpn_Tracker_Remote("Tracker0@localhost");

	//Registering a callback function
	vrpnAnalog->register_change_handler(0, handle_analog);
	vrpnButton->register_change_handler(0, handle_button);
	vrpnTracker->register_change_handler(0, handle_tracker);

	// To make sure the remote is properly updated
	while (1)
	{
		vrpnAnalog->mainloop();
		vrpnButton->mainloop();
		vrpnTracker->mainloop();
	}

	return 0;
}
