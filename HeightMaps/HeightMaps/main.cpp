#include <iostream>
#include <noise/noise.h>
#include "noiseutils.h"

using namespace noise;

/*
libnoise is an external open-source c++ library.
http://libnoise.sourceforge.net/index.html
*/

int main(int argc, char** argv)
{
	module::Perlin myModule; //create instance of perlin noise module

	utils::NoiseMap heightMap;
	utils::NoiseMapBuilderPlane heightMapBuilder;
	heightMapBuilder.SetSourceModule(myModule);
	heightMapBuilder.SetDestNoiseMap(heightMap);
	heightMapBuilder.SetDestSize(256, 256); //size of map to generate
	heightMapBuilder.SetBounds(6.0, 10.0, 1.0, 5.0);
	heightMapBuilder.Build(); //build the noise maps

	//rendering the height maps
	utils::RendererImage renderer;
	utils::Image image;
	renderer.SetSourceNoiseMap(heightMap);
	renderer.SetDestImage(image);

	//colour the image
	renderer.ClearGradient();
	renderer.AddGradientPoint(-1.0000, utils::Color(0, 0, 128, 255)); // deeps
	renderer.AddGradientPoint(-0.2500, utils::Color(0, 0, 255, 255)); // shallow
	renderer.AddGradientPoint(0.0000, utils::Color(0, 128, 255, 255)); // shore
	renderer.AddGradientPoint(0.0625, utils::Color(240, 240, 64, 255)); // sand
	renderer.AddGradientPoint(0.1250, utils::Color(32, 160, 0, 255)); // grass
	renderer.AddGradientPoint(0.3750, utils::Color(224, 224, 0, 255)); // dirt
	renderer.AddGradientPoint(0.7500, utils::Color(128, 128, 128, 255)); // rock
	renderer.AddGradientPoint(1.0000, utils::Color(255, 255, 255, 255)); // snow

	//add light source
	renderer.EnableLight();
	renderer.SetLightContrast(3.0); //triple the contrast
	renderer.SetLightBrightness(2.0); //double the brightness

	renderer.Render();

	//writing the image to file
	utils::WriterBMP writer;
	writer.SetSourceImage(image);
	writer.SetDestFilename("tutorial2.bmp");
	writer.WriteDestFile();

	return 0;
}