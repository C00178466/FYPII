#pragma once
#include "GLM\glm\glm.hpp"
#include "GLM\glm\gtx\compatibility.hpp"
#include "GLM\glm\gtc\type_ptr.hpp"
#include "GLEW\GL\glew.h"
#include "SOIL\SOIL.h"
#include <string>
#include <vector>
#include <iostream>
#include <experimental\filesystem>
#include <fstream>

using namespace std;

namespace fs = std::experimental::filesystem;

template <typename T> GLM_FUNC_QUALIFIER T saturate(T x) { return clamp(x, T(0), T(1)); }

class Terrain
{
public:
	Terrain(float heightscale = 500.0f, float blockScale = 2.0f)
	{

	}
	virtual ~Terrain() {}

	void Terminate();
	bool LoadHeightMap(const string& filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height);
	bool LoadTexture(const string& filename, unsigned int textureStage = 0);

	float GetHeightAt(const glm::vec3& position);

	void Render();
	//In debug builds, the terrain normals will be rendered.
	void DebugRenderer();

protected:
	void GenerateIndexBuffer();
	void GenerateNormals();

	//Generates the vertex buffer objects from the
	//position, normal, texture and vertex buffers
	void GenerateVertexBuffers();

	void RenderNormals();

private:
	typedef vector<glm::vec3> PositionBuffer;
	typedef vector<glm::vec4> ColorBuffer;
	typedef vector<glm::vec3> NormalBuffer;
	typedef vector<glm::vec2> TexCoordBuffer;
	typedef vector<GLuint> IndexBuffer;

	PositionBuffer m_PositionBuffer;
	ColorBuffer m_ColorBuffer;
	NormalBuffer m_NormalBuffer;
	TexCoordBuffer m_Tex0Buffer;
	IndexBuffer m_IndexBuffer;

	//ID's for the VBO's
	GLuint m_GLVertexBuffer;
	GLuint m_GLNormalBuffer;
	GLuint m_GLColorBuffer;
	GLuint m_GLTex0Buffer;
	GLuint m_GLTex1Buffer;
	GLuint m_GLTex2Buffer;
	GLuint m_GLIndexBuffer;

	static const unsigned int m_uiNumTextures = 3;
	GLuint m_GLTextures[m_uiNumTextures];

	glm::mat4x4 m_LocalToWorldMatrix;

	//The dimensions of the heightmap texture
	glm::vec2 m_HeightmapDimensions;

	//The height-map value will be multiplied by this value
	//before it is assigned to the vertex's Y-Coordinate
	float m_fHeightScale;

	//The vertex's X and Z coordinates will be multiplied by this
	//for each step when building the terrain
	float m_fBlockScale;
};