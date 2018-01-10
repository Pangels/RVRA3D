#include "vrpn_Tracker.h"
#include <iostream>
using namespace std;

/*
Skeleton code
0 = HEAD
1 = NECK/CENTER SHOULDER
2 = SPINE
3 = CENTER HIP
4 = LEFT SHOULDER
5 = LEFT ELBOW
6 = LEFT WRIST
7 = LEFT HAND
8 = RIGHT SHOULDER
9 = RIGHT ELBOW
10 = RIGHT WRIST
11 = RIGHT HAND
12 = LEFT HIP
13 = LEFT KNEE
14 = LEFT ANKLE
15 = LEFT FOOT
16 = RIGHT HIP
17 = RIGHT KNEE
18 = RIGHT ANKLE
19 = RIGHT FOOT
*/

STARTUPINFO si;
PROCESS_INFORMATION pi;

/* Threshold Clap */
double thresholdClose = 1.20;
/* Threshold Open */
double thresholdOpen = 0.10;
/* mspaint execution; true = execute, false = not execute */
bool execPaint = false;
/* Past tracker */
vrpn_TRACKERCB pastTracker[20];
/* mouse input button event handler */
INPUT input[] = {0};

void copyPastPosition(const vrpn_TRACKERCB t)
{
	/* copy and paste the actual position */
	pastTracker[t.sensor].pos[0] = t.pos[0]; // pos[0] = axe X horizontal left(-1) to right(+1)
	pastTracker[t.sensor].pos[1] = t.pos[1]; // pos[1] = axe Y vertical up(+1) to bottom (-1)
	pastTracker[t.sensor].pos[2] = t.pos[2]; // pos[2] = axe Z depth near(0) to far(+)
}

void initSensor(const vrpn_TRACKERCB t)
{
	cout << "Initalising sensor number " << t.sensor << " ..." << endl;
	copyPastPosition(t);
}

void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t)
{
	/* Initialise sensor */
	if (pastTracker[t.sensor].pos[0] == pastTracker[t.sensor].pos[1]
		&& pastTracker[t.sensor].pos[1] == pastTracker[t.sensor].pos[2]
		&& pastTracker[t.sensor].pos[2] == 0.0)
	{
		initSensor(t);
	}

	/* Search for the right hand */
	if (t.sensor == 11)
	{
		/* When the hands is closed */
		if (execPaint == false
			&& (((pastTracker[7].pos[0] > t.pos[0]) && (pastTracker[7].pos[0] - t.pos[0]) < thresholdOpen) /* X Axis */
			|| ((pastTracker[7].pos[0] < t.pos[0]) && (t.pos[0] - pastTracker[7].pos[0]) < thresholdOpen)) 
			&& (((pastTracker[7].pos[1] > t.pos[1]) && (pastTracker[7].pos[1] - t.pos[1]) < thresholdOpen) /* Y Axis */
			|| ((pastTracker[7].pos[1] < t.pos[1]) && (t.pos[1] - pastTracker[7].pos[1]) < thresholdOpen))
			)
		{
			cout << "Opening Paint ..." << endl;
			if (CreateProcess(NULL, "mspaint.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				cout << "Paint opened" << endl;
				execPaint = true;
			}
			else
			{
				cout << "ERROR CALLING PROCESS." << endl;
			}
		}
		/* When the hands is opened */
		else if (execPaint == true
			&& (((pastTracker[7].pos[0] > t.pos[0]) && (pastTracker[7].pos[0] - t.pos[0]) > thresholdClose)
				|| ((pastTracker[7].pos[0] < t.pos[0]) && (t.pos[0] - pastTracker[7].pos[0]) > thresholdClose)))
		{
			cout << "Closing Paint..." << endl;
			if (WinExec("taskkill /IM mspaint.exe", 0))
			{
				cout << "Paint closed" << endl;
				execPaint = false;
			}
			else
			{
				cout << "ERROR KILLING PROCESS." << endl;
			}
		}

		/* Cursor max screen position x = 1535, y = 863 */
		SetCursorPos((t.pos[0] * 2.5 + 0.25) * 1540.0, (t.pos[1] * (-2.25) + 1.0) * 863.0);
	}
	else if (t.sensor == 7)
	{
		/* mouse right click*/
		/* While left hand is on the left of the left shoulder, mouse hold click */
		if (t.pos[0] + 0.50 < pastTracker[4].pos[0] && input[0].mi.dwFlags == MOUSEEVENTF_LEFTUP)
		{
			input[0].type = INPUT_MOUSE;
			input[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			SendInput(1, input, sizeof(INPUT));
		}
		/* Free the mouse button status when on the right of the left shoulder */
		else if (t.pos[0] > pastTracker[4].pos[0] && input[0].mi.dwFlags == MOUSEEVENTF_RIGHTDOWN)
		{
			input[0].type = INPUT_MOUSE;
			input[0].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
			SendInput(1, input, sizeof(INPUT));
		}

		/* mouse left click */
		/* While left hand is higher than the spine, mouse hold click */
		if (t.pos[1] > pastTracker[1].pos[1] && input[0].mi.dwFlags != MOUSEEVENTF_LEFTDOWN)
		{
			input[0].type = INPUT_MOUSE;
			input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			SendInput(1, input, sizeof(INPUT));
		}
		/* Free the mouse button status when lower than the spine */
		else if (t.pos[1] < pastTracker[1].pos[1] && input[0].mi.dwFlags == MOUSEEVENTF_LEFTDOWN)
		{
			input[0].type = INPUT_MOUSE;
			input[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;
			SendInput(1, input, sizeof(INPUT));
		}
	}

	copyPastPosition(t);
}

int main(int argc, char * argv[]) {
	//The constructor takes in the name of the device and the network address of the server.
	vrpn_Tracker_Remote* vrpnTracker = new vrpn_Tracker_Remote("Tracker0@localhost");

	//Registering a callback function
	vrpnTracker->register_change_handler(0, handle_tracker);

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// To make sure the remote is properly updated
	while (1)
	{
		vrpnTracker->mainloop();
		if (GetAsyncKeyState(VK_ESCAPE))
		{
			return EXIT_SUCCESS;
		}
	}

	return EXIT_SUCCESS;
}
