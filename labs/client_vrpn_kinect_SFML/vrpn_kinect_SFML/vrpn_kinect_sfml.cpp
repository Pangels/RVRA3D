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

/* SFML variables */
sf::RenderWindow windowR(sf::VideoMode(300, 300), "Right Hand");
sf::CircleShape shapeR(150.f);
sf::RenderWindow windowL(sf::VideoMode(300, 300), "Left Hand");
sf::CircleShape shapeL(150.f);
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

	/* Search for the left hand */
	if (t.sensor == 7)
	{
		/* When the hand moves from top to bottom zone */
		if (t.pos[1] < pastTracker[5].pos[1])
		{
			shapeL.setFillColor(sf::Color::Red);
		}
		/* When the hand moves from bottom to top zone */
		else if (t.pos[1] > pastTracker[5].pos[1])
		{
			shapeL.setFillColor(sf::Color::Blue);
		}
	}
	/* Search for the right hand */
	else if (t.sensor == 11)
	{
		/* When the hand moves from top to bottom zone */
		if (t.pos[1] < pastTracker[9].pos[1])
		{
			shapeR.setFillColor(sf::Color::Yellow);
		}
		/* When the hand moves from bottom to top zone */
		else if (t.pos[1] > pastTracker[9].pos[1])
		{
			shapeR.setFillColor(sf::Color::Magenta);
		}
		/* Close the SFML windows and terminate the program */
		if (((pastTracker[7].pos[0] > t.pos[0]) && (pastTracker[7].pos[0] - t.pos[0]) > 1.20)
			|| ((pastTracker[7].pos[0] < t.pos[0]) && (t.pos[0] - pastTracker[7].pos[0]) > 1.20))
		{
			windowL.close();
		}
	}
	copyPastPosition(t);
}

int main(int argc, char * argv[]) {
	//The constructor takes in the name of the device and the network address of the server.
	vrpn_Tracker_Remote* vrpnTracker = new vrpn_Tracker_Remote("Tracker0@localhost");

	//Registering a callback function
	vrpnTracker->register_change_handler(0, handle_tracker);
	
	shapeR.setFillColor(sf::Color::Green);
	shapeL.setFillColor(sf::Color::Green);

	windowL.setPosition(sf::Vector2i(200, 300));
	windowR.setPosition(sf::Vector2i(1300, 300));

	// To make sure the remote is properly updated
	while (windowR.isOpen() && windowL.isOpen())
	{
		vrpnTracker->mainloop();
		
		sf::Event event;
		while (windowR.pollEvent(event) || windowL.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || GetAsyncKeyState(VK_ESCAPE))
			{
				windowR.close();
				windowL.close();
			}
		}

		windowR.clear();
		windowL.clear();
		windowR.draw(shapeR);
		windowL.draw(shapeL);
		windowR.display();
		windowL.display();
	}

	return 0;
}
