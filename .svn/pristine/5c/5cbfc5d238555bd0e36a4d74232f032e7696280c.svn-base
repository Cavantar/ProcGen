#include "TerrainGenerator.h"

float TerrainGenerator::sharpness = 16.0f;

vector<glm::vec4> TerrainGenerator::generateTerrain(int dimensionLog2, glm::vec4 seed, float unitLength) {
	int gridSize = (int)pow(2, dimensionLog2) + 1;

	cout << "Generating Terrain Vertices: " << gridSize << " x " << gridSize << endl;

	vector<float> grid;
	grid.resize(gridSize * gridSize, 0);
	//cout << grid.size() << endl;
	
	grid[0] = seed.x;
	grid[gridSize - 1] = seed.y;
	grid[gridSize * (gridSize - 1)] = seed.z;
	grid[gridSize * gridSize - 1] = seed.w;

	int requiredPasses = (int)log2(gridSize);
	cout << "Required Passes: " << requiredPasses << endl;

	//cout << requiredPasses << endl;
	static int time = glutGet(GLUT_ELAPSED_TIME);

	for(int i = 0; i < requiredPasses; i++) {
		cout << "  Pass: " << i + 1 << endl;
		doSquarePass(grid, gridSize, (int)pow(2, dimensionLog2 - i) + 1, (int)pow(2, i));
		doDiamondPass(grid, gridSize, (int)pow(2, dimensionLog2 - i) + 1, (int)pow(2, i));
		
		//showMap(grid, gridSize);
		//float tempFloat = pow(0.5f, log2(pow(2,i)));
	}
	cout << "Finished in " << float(glutGet(GLUT_ELAPSED_TIME) - time) / 1000.0f << " seconds \n\n";
	return createVectorMap(grid, gridSize, unitLength);
}
void TerrainGenerator::doSquare(vector<float>& map, int mapWidth, int startIndex, int length, int passNumber) {
	if(length < 3) return;
	passNumber++;

	float sum = 0;
	sum += map[startIndex];
	sum += map[startIndex + length - 1];
	sum += map[startIndex + mapWidth * (length - 1) ];
	sum += map[startIndex + mapWidth * (length  - 1) + length - 1];

	map[startIndex + mapWidth *(length / 2) + length / 2] = sum / 4.0f + getRandomOffset(passNumber);
}
void TerrainGenerator::doDiamond(vector<float>& map, int mapWidth, int startIndex, int length, int passNumber) {
	// U¿ywane wartoœci
	
	float lu = map[startIndex];
	float ru = map[startIndex + length - 1];;

	float m = map[startIndex + mapWidth*(length / 2) + length / 2];

	float ld = map[startIndex + mapWidth * (length - 1)];
	float rd = map[startIndex + mapWidth * (length - 1) + (length - 1)];

	//cout << "lu: " << lu << " ru: " << ru << " m: " << m << " ld: " << ld << " rd: " << rd << endl;

	float sum = 0;
	int index = 0;
	// Góra	
	sum = lu + m + ru;
	index = startIndex + length / 2;

	if(index >= mapWidth && map[index - (length / 2) * mapWidth] != 0) {
		sum += map[index - (length/2) * mapWidth];
		sum /= 4.0f;
	}
	else sum /= 3.0f;
	if(map[index] != 0) {
		sum += map[index];
		sum /= 2.0f;
	}
	map[startIndex + length / 2] = sum + getRandomOffset(passNumber);

	// Lewo
	sum = lu + m + ld;
	index = startIndex + mapWidth * (length / 2);

	if(index % mapWidth && map[index - (length/2)] != 0) {
		sum += map[index - (length / 2)];
		sum /= 4.0f;
	}
	else sum /= 3.0f;

	if(map[index] != 0) {
		sum += map[index];
		sum /= 2.0f;
	}
	map[index] = sum + getRandomOffset(passNumber);
	
	// Prawo
	sum = ru + m + rd;
	index = startIndex + mapWidth * (length / 2) + length - 1;

	if(index % mapWidth != mapWidth - 1 && map[index + (length/2)] != 0){
		sum += map[index + (length / 2)];
		sum /= 4.0f;
	}
	else sum /= 3.0f;
	if(map[index] != 0) {
		sum += map[index];
		sum /= 2.0f;
	}
	map[index] = sum + getRandomOffset(passNumber);

	// Dó³
	sum = m + ld + rd;
	index = startIndex + mapWidth * (length - 1) + (length / 2);
	if(index < (mapWidth * (mapWidth - 1)) && map[index + mapWidth * (length/2)] != 0){
		sum += map[index + mapWidth * (length / 2)];
		sum /= 4.0f;
	}
	else sum /= 3.0f;
	if(map[index] != 0) {
		sum += map[index];
		sum /= 2.0f;
	}
	map[index] = sum + getRandomOffset(passNumber) ;
}
void TerrainGenerator::doSquarePass(vector<float>& map, int mapWidth, int length, int divisions) {
	for(int y = 0; y < divisions; y++) {
		for(int x = 0; x < divisions; x++) {
			doSquare(map, mapWidth, (y * (length - 1))* mapWidth + x * (length - 1), length, divisions);
		}
	}
}
void TerrainGenerator::doDiamondPass(vector<float>& map, int mapWidth, int length, int divisions) {
	for(int y = 0; y < divisions; y++) {
		for(int x = 0; x < divisions; x++) {
			doDiamond(map, mapWidth, (y * (length - 1)) * mapWidth + x * (length - 1), length, divisions);
		}
	}
}
float TerrainGenerator::getRandomOffset(int passNumber) {
	float tempFloat = (float)pow(0.5f, log2(passNumber)) *sharpness;
	return tempFloat*(1.0f - 0.0002f * float(rand() % 10000));
}
vector<glm::vec4> TerrainGenerator::createVectorMap(vector<float>& map, int mapWidth, float unitLength) {
	vector<glm::vec4> vertices;
	vertices.resize(mapWidth * mapWidth);
	for(int y = 0; y < mapWidth; y++) {
		for(int x = 0; x < mapWidth; x++) {																			// Something important
			vertices[x + y * mapWidth] = glm::vec4(x * unitLength - unitLength *  mapWidth / 2.0f, map[x + y * mapWidth] , y * unitLength - unitLength * mapWidth / 2.0f, 1.0f);
		}
	}
	return vertices;
}
void TerrainGenerator::showMap(vector<float>& map, int length) {
	cout << "Map: \n\n";
	for(auto i = map.begin(); i != map.end(); i++) {
		int xmod = distance(map.begin(), i) % length;
		if(xmod == 0) cout << "[";
		//if(*i < 10) cout << " ";
		//if(*i < 100) cout << " ";
		printf("%8.3f ", *i);
		//cout << *i << " " ;
		if(xmod == length - 1) cout << " ]\n";
	}
}