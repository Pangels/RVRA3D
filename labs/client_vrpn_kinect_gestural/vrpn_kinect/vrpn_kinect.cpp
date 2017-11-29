#include "vrpn_Tracker.h"

#include <iostream>
using namespace std;

int zone = 0;
int sensorNumber = 0; // 0 => sensor 7, 1 => sensor 11
bool handStatus[2] = { true, true }; // true == down, false == up
double pastSensor[2][3] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // index 0 == left, index 1 == right

void copyPos(const vrpn_TRACKERCB t)
{
	/* copy and paste the actual position */
	pastSensor[sensorNumber][0] = t.pos[0];
	pastSensor[sensorNumber][1] = t.pos[1];
	pastSensor[sensorNumber][2] = t.pos[2];
}

void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t)
{
	/* Search for the hand (left or right) */
	if (t.sensor == 7 || t.sensor == 11)
	{
		if (t.sensor == 7)
			sensorNumber = 0; // Left hand
		else
			sensorNumber = 1; // Right hand

		if (pastSensor[sensorNumber][0] == pastSensor[sensorNumber][1] && pastSensor[sensorNumber][1] == pastSensor[sensorNumber][2] && pastSensor[sensorNumber][2] == 0.0)
		{
			cout << "Initalising " << (sensorNumber == 0 ? "Left" : "Right") << " hand..." << endl;
			copyPos(t);
			if (pastSensor[sensorNumber][1] > 0.0)
				handStatus[sensorNumber] = false; // hands up
		}

		if (t.pos[1] > 0.0 && pastSensor[sensorNumber][1] < 0.0 && handStatus[sensorNumber] == true)
		{
			cout << (sensorNumber == 0 ? "Left" : "Right") << " Hand " << "up." << endl;
			handStatus[sensorNumber] = false;
		}
		if (t.pos[1] < 0.0 && pastSensor[sensorNumber][1] > 0.0 && handStatus[sensorNumber] == false)
		{
			cout << (sensorNumber == 0 ? "Left" : "Right") << " Hand " << "down." << endl;
			handStatus[sensorNumber] = true;
		}
		
		// Zone A or B
		if ((t.pos[1] > 0.0 && pastSensor[sensorNumber][1] < 0.0)
			|| (((t.pos[0] > 0.0 && pastSensor[sensorNumber][0] < 0.0)
				|| (t.pos[0] < 0.0 && pastSensor[sensorNumber][0] > 0.0))
				&& (t.pos[1] > 0.0 && pastSensor[sensorNumber][1] > 0.0)
				))
		{
			// Zone A / x E [-1,0] && y E [0,1]
			if (t.pos[0] < 0.0 && zone != 0)
			{
				zone = 0;
				Beep(250, 500);
				cout << "Zone A" << endl;
			}
			// Zone B / x E [0,1] && y E [0,1]
			else if (t.pos[0] > 0.0 && zone != 1)
			{
				zone = 1;
				Beep(300, 500);
				cout << "Zone B" << endl;
			}
		}
		// Zone C or D
		else if ((t.pos[1] < 0.0 && pastSensor[sensorNumber][1] > 0.0)
			|| (((t.pos[0] > 0.0 && pastSensor[sensorNumber][0] < 0.0)
				|| (t.pos[0] < 0.0 && pastSensor[sensorNumber][0] > 0.0))
				&& (t.pos[1] < 0.0 && pastSensor[sensorNumber][1] < 0.0)
				))
		{
			// Zone C / x E [-1,0] && y E [-1,0]
			if (t.pos[0] < 0.0 && zone != 2)
			{
				zone = 2;
				Beep(320, 500);
				cout << "Zone C" << endl;
			}
			// Zone D / x E [0,1] && y E [-1,0]
			else if (t.pos[0] > 0.0 && zone != 3)
			{
				zone = 3;
				Beep(340, 500);
				cout << "Zone D" << endl;
			}
		}
		copyPos(t);
	}
}

int main(int argc, char * argv[]) {
	/*
	* The constructor takes in the name of the device and the network address
	* of the server.
	*/
	vrpn_Tracker_Remote* vrpnTracker = new vrpn_Tracker_Remote("Tracker0@localhost");
	vrpn_Tracker_Remote* vrpnTrackerBis = new vrpn_Tracker_Remote("Tracker1@localhost");

	//Registering a callback function
	vrpnTracker->register_change_handler(0, handle_tracker);
	vrpnTrackerBis->register_change_handler(0, handle_tracker);

	// To make sure the remote is properly updated
	while (1)
	{
		vrpnTracker->mainloop();
		vrpnTrackerBis->mainloop();
	}

	return 0;
}
