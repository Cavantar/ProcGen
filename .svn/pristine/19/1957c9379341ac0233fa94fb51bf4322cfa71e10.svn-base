#include "Profiler.h"

void ProfilerItem::start(const int time) {
	tempTime = time;
}
void ProfilerItem::end(const int time) {
	lastTime = time - tempTime;
	if(averageTime != 0) averageTime = (averageTime + (float)lastTime) / 2.0f;
	else averageTime = (float)tempTime;
}

void Profiler::start(const string name) {
	if(items.find(name) == items.end()) items.insert(pair<string, ProfilerItem>(name, ProfilerItem()));
	items[name].start((int)clock.getElapsedTime().asMicroseconds());
}
void Profiler::end(const string name) {
	items[name].end((int)clock.getElapsedTime().asMicroseconds());
	if(name == "Game") clock.restart();
}
void Profiler::log() {
	ProfilerItem* game = &items["Game"];
	stringstream temp;
	gameLog("\nProfiler\n");

	for(auto i = items.begin(); i != items.end(); i++) {
		if(i->first == "Game") continue;
		temp << "Name : " << i->first << "\t lastTime: " << i->second.getLastTime() << "\t avgTime: " << i->second.getAverageTime();
		temp << "\t FrameUse: " << ((float)i->second.getLastTime() / (float)game->getLastTime()) * 100 << " %";
		gameLog(temp.str());
		temp.str("");
	}
	
	temp << "Name : Game\t lastTime: " << game->getLastTime() << "\t avgTime: " << game->getAverageTime();
	gameLog(temp.str());
}