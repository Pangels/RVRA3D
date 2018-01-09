#include "vrpn_Tracker.h"
#include <iostream>
#include <stdio.h>
#include <Windows.h>
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

/* left hand zone */
int zoneLH = 0;
/* right hand zone */
int zoneRH = 0;
/* Threshold Clap */
double thresholdClap = 0.05;
/* Threshold Open */
double thresholdOpen = 1.2;
/* mspaint execution; true = execute, false = not execute */
bool execPaint = false;
/* true == up, false == down */
/* 0 = left, 1 = right */
bool handStatus[2] = { false, false };
/* Past tracker */
vrpn_TRACKERCB pastTracker[20];

bool compare_joints = false;
int index_first_joint = 0;
int index_last_joint = 23;

void copyPastPosition(const vrpn_TRACKERCB t)
{
	/* copy and paste the actual position */
	pastTracker[t.sensor].pos[0] = t.pos[0]; // pos[0] = axe X horizontal left(-) to right(+)
	pastTracker[t.sensor].pos[1] = t.pos[1]; // pos[1] = axe Y vertical up(+) to bottom (-)
	pastTracker[t.sensor].pos[2] = t.pos[2]; // pos[2] = axe Z depth near(0) to far(+)
}

void initSensor(const vrpn_TRACKERCB t)
{
	cout << "Initalising sensor number " << t.sensor << " ..." << endl;
	copyPastPosition(t);
	/* If a hand is up, updated the hand */
	if ((t.sensor == 7 || t.sensor == 11) && pastTracker[t.sensor].pos[1] > pastTracker[t.sensor == 7 ? 6 : 10].pos[1])
		handStatus[t.sensor == 7 ? 0 : 1] = true;
}

void mouvementZone(const vrpn_TRACKERCB t, int zone)
{
	// Zone A or B
	if ((t.pos[1] > 0.0 && pastTracker[t.sensor].pos[1] < 0.0)
		|| (((t.pos[0] > 0.0 && pastTracker[t.sensor].pos[0] < 0.0)
			|| (t.pos[0] < 0.0 && pastTracker[t.sensor].pos[0] > 0.0))
			&& (t.pos[1] > 0.0 && pastTracker[t.sensor].pos[1] > 0.0)
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
	else if ((t.pos[1] < 0.0 && pastTracker[t.sensor].pos[1] > 0.0)
		|| (((t.pos[0] > 0.0 && pastTracker[t.sensor].pos[0] < 0.0)
			|| (t.pos[0] < 0.0 && pastTracker[t.sensor].pos[0] > 0.0))
			&& (t.pos[1] < 0.0 && pastTracker[t.sensor].pos[1] < 0.0)
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
}

void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t)
{
	/* Initialise sensor */
	if (pastTracker[t.sensor].pos[0] == pastTracker[t.sensor].pos[1] && pastTracker[t.sensor].pos[1] == pastTracker[t.sensor].pos[2] && pastTracker[t.sensor].pos[2] == 0.0)
		initSensor(t);

	/* Search for the hand (left or right) */
	if (t.sensor == 7 || t.sensor == 11)
	{
		/* When the hand moves from top to bottom zone */
		if (t.pos[1] < pastTracker[t.sensor == 7 ? 6 : 10].pos[1] && handStatus[t.sensor == 7 ? 0 : 1] == true)
		{
			cout << (t.sensor == 7 ? "Left" : "Right") << " Hand Down." << endl;
			handStatus[t.sensor == 7 ? 0 : 1] = false;
		}

		/* When the hand moves from bottom to top zone */
		if (t.pos[1] > pastTracker[t.sensor == 7 ? 6 : 10].pos[1] && handStatus[t.sensor == 7 ? 0 : 1] == false)
		{
			cout << (t.sensor == 7 ? "Left" : "Right") << " Hand Up." << endl;
			handStatus[t.sensor == 7 ? 0 : 1] = true;
		}
		//cout << "Claping ? " << pastTracker[7].pos[0] - t.pos[0] << endl;
		/* When the hands claps */
		if (t.sensor == 11 && execPaint == true && (((pastTracker[7].pos[0] > t.pos[0]) && (pastTracker[7].pos[0] - t.pos[0]) < thresholdClap) || ((pastTracker[7].pos[0] < t.pos[0]) && (t.pos[0] - pastTracker[7].pos[0]) < thresholdClap)))
		{
			cout << "Closing Paint ..." << endl;
			if (WinExec("taskkill /IM mspaint.exe /F",0))
			{
				cout << "Paint closed" << endl;
				execPaint = false;
			}
			else
				cout << "ERROR : Issue on killing process." << endl;
		}
		else if (t.sensor == 11 && execPaint == false && (((pastTracker[7].pos[0] > t.pos[0]) && (pastTracker[7].pos[0] - t.pos[0]) > thresholdOpen) || ((pastTracker[7].pos[0] < t.pos[0]) && (t.pos[0] - pastTracker[7].pos[0]) > thresholdOpen)))
		{
			cout << "Opening Paint ..." << endl;
			if (CreateProcess(NULL, "mspaint.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
			{
				cout << "Paint opened" << endl;
				execPaint = true;
			}
			else
				cout << "ERROR : Issue on calling process." << endl;
		}
		//mouvementZone(t, t.sensor);
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
	}

	// Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(&si);
	CloseHandle(&pi);
	system("pause");
	return 0;
}
