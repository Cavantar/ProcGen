#pragma once

#include "Includes.h"

/**
*	Profilowana jednostka
*/
class ProfilerItem {
public:
	ProfilerItem() : averageTime(0), lastTime(0), tempTime(0) {};

	/**
	*	Metoda zaczynaj¹ca liczenie czasu
	*/
	void start(const int time);
	/**
	*	Metoda koñcz¹ca liczenie czasu
	*/
	void end(const int time);
	/**
	*	Zwraca œredni czas
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
*	Klasa zawieraj¹ca wszystkie profilowane jednostki i nimi zarz¹dzaj¹ca
*/
class Profiler {
public:
	/**
	*	Zaczyna liczenie czasu
	*/
	void start(const string name);
	/**
	*	Koñczy liczenie czasu
	*/
	void end(const string name);
	/**
	*	Wyœwietla statystki profilowanych elementów na standardowe wyjœcie
	*/
	void log();
private:
	sf::Clock clock;
	map<string, ProfilerItem> items;
};

