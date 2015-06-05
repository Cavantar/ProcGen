#pragma once

#include "Includes.h"

/**
*	Profilowana jednostka
*/
class ProfilerItem {
public:
	ProfilerItem() : averageTime(0), lastTime(0), tempTime(0) {};

	/**
	*	Metoda zaczynaj�ca liczenie czasu
	*/
	void start(const int time);
	/**
	*	Metoda ko�cz�ca liczenie czasu
	*/
	void end(const int time);
	/**
	*	Zwraca �redni czas
	*/
	float getAverageTime(){ return averageTime; }
	/**
	*	Zwraca czas ostatniej klatki
	*/
	int getLastTime() { return lastTime; }

private:
	float averageTime;
	int lastTime;

	int tempTime;
};

/**
*	Klasa zawieraj�ca wszystkie profilowane jednostki i nimi zarz�dzaj�ca
*/
class Profiler {
public:
	/**
	*	Zaczyna liczenie czasu
	*/
	void start(const string name);
	/**
	*	Ko�czy liczenie czasu
	*/
	void end(const string name);
	/**
	*	Wy�wietla statystki profilowanych element�w na standardowe wyj�cie
	*/
	void log();
private:
	sf::Clock clock;
	map<string, ProfilerItem> items;
};

