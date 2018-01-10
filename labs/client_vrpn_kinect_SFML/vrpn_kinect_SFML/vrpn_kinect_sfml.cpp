#include <vrpn_Tracker.h>
#include <iostream>
#include <stdio.h>
#include <SFML/Graphics.hpp>
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

/* Past tracker */
vrpn_TRACKERCB pastTracker[20];

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
}

void VRPN_CALLBACK handle_tracker(void* userData, const vrpn_TRACKERCB t)
{
	/* Initialise sensor */
	if (pastTracker[t.sensor].pos[0] == pastTracker[t.sensor].pos[1] && pastTracker[t.sensor].pos[1] == pastTracker[t.sensor].pos[2] && pastTracker[t.sensor].pos[2] == 0.0)
		initSensor(t);

	/* Search for the hand (left or right) */
	if (t.sensor == 7 || t.sensor == 11)
	{
		///* When the hand moves from top to bottom zone */
		//if (t.pos[1] < pastTracker[t.sensor == 7 ? 6 : 10].pos[1] && handStatus[t.sensor == 7 ? 0 : 1] == true)
		//{
		//	cout << (t.sensor == 7 ? "Left" : "Right") << " Hand Down." << endl;
		//	handStatus[t.sensor == 7 ? 0 : 1] = false;
		//}

		///* When the hand moves from bottom to top zone */
		//if (t.pos[1] > pastTracker[t.sensor == 7 ? 6 : 10].pos[1] && handStatus[t.sensor == 7 ? 0 : 1] == false)
		//{
		//	cout << (t.sensor == 7 ? "Left" : "Right") << " Hand Up." << endl;
		//	handStatus[t.sensor == 7 ? 0 : 1] = true;
		//}
	}
	copyPastPosition(t);
}

int main(int argc, char * argv[]) {
	////The constructor takes in the name of the device and the network address of the server.
	//vrpn_Tracker_Remote* vrpnTracker = new vrpn_Tracker_Remote("Tracker0@localhost");

	////Registering a callback function
	//vrpnTracker->register_change_handler(0, handle_tracker);

	///* Create SFML window and configuration */
	//sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

	//// To make sure the remote is properly updated
	//while (1)
	//{
	//	vrpnTracker->mainloop();
	//}

	//return 0;

	sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
		window.display();
	}

	return 0;
}
