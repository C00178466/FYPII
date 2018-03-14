#pragma once
#include "vec3f.h"
#include "imageloader.h"

//Represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {

public:
	Terrain() {};
	Terrain(int w2, int l2);
	~Terrain();

	//Load terrain from a heightmap
	static Terrain* loadTerrain(const char* filename, float height);

	//Computes the normals, if they haven't been computed yet
	void computeNormals();

	/////////////////////////////////////////
	//
	// Get / Set Functions
	//
	/////////////////////////////////////////
	int width() { return w; }
	int length() { return l; }

	//Sets the height at (x, z) to y
	void setHeight(int x, int z, float y)
	{
		hs[z][x] = y;
		computedNormals = false;
	}

	//Returns the height at (x, z)
	float getHeight(int x, int z) { return hs[z][x]; }

	//Returns the normal at (x, z)
	Vec3f getNormal(int x, int z)
	{
		if (!computedNormals)
		{
			computeNormals();
		}
		return normals[z][x];
	}

private:
	int w; //Width
	int l; //Length
	float** hs; //Heights
	Vec3f** normals;
	bool computedNormals; //Check if normals have already been computed
};