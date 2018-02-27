#include "Terrain.h"

//Enable multitexture blending across the terrain
#ifndef ENABLE_MULTITEXTURE
#define ENABLE_MULTITEXTURE 1
#endif // !ENABLE_MULTITEXTURE

//Enable the blend constants based on the slope of the terrain
#ifndef ENABLE_SLOPE_BASED_BLEND
#define ENABLE_SLOPE_BASED_BLEND 1
#endif

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

bool Terrain::LoadTexture(const string& filename, unsigned int textureStage)
{
	assert(textureStage < m_uiNumTextures);
	DeleteTexture(m_GLTextures[textureStage]);

	m_GLTextures[textureStage] = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (m_GLTextures[textureStage] != 0)
	{
		glBindTexture(GL_TEXTURE_2D, m_GLTextures[textureStage]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return (m_GLTextures[textureStage] != 0);
}

bool Terrain::LoadHeightMap(const string& filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height)
{
	if (!fs::exists(filename))
	{
		cerr << "Could not find file: " << filename << endl;
		return false;
	}

	std::ifstream ifs;
	ifs.open(filename, std::ifstream::binary);

	if (ifs.fail())
	{
		cerr << "Failed to open file: " << filename << endl;
		return false;
	}

	const unsigned int bytesPerPixel = bitsPerPixel / 8;
	const unsigned int expectedFileSize = (bytesPerPixel * width * height);
	const unsigned int fileSize = GetFileLength(ifs);

	if (expectedFileSize != fileSize)
	{
		cerr << "Expected file size [" << expectedFileSize << " bytes] differs from actual file size [" << fileSize << " bytes]" << endl;
		return false;
	}

	unsigned char* heightMap = new unsigned char[fileSize];
	ifs.read((char*)heightMap, fileSize);
	if (ifs.fail())
	{
		cerr << "An error occurred while reading from the height map file: " << filename << endl;
		ifs.close();
		delete[] heightMap;
		return false;
	}
	ifs.close();

	unsigned int numVerts = width * height;
	m_PositionBuffer.resize(numVerts);
	m_ColorBuffer.resize(numVerts);
	m_NormalBuffer.resize(numVerts);
	m_Tex0Buffer.resize(numVerts);

	m_HeightmapDimensions = glm::vec2(width, height);

	//Size of terrain in world units
	float terrainWidth = (width - 1) * m_fBlockScale;
	float terrainHeight = (height - 1) * m_fBlockScale;

	float halfTerrainWidth = terrainWidth * 0.5f;
	float halfTerrainHeight = terrainHeight * 0.5f;

	for (unsigned int j = 0; j < height; j++)
	{
		for (unsigned int i = 0; i < width; i++)
		{
			unsigned int index = (j * width) + i;
			assert(index * bytesPerPixel < fileSize);
			float heightValue = GetHeightValue(&heightMap[index * bytesPerPixel], bytesPerPixel);

			float S = (i / (float)(width - 1));
			float T = (j / (float)(height - 1));

			float X = (S * terrainWidth) - halfTerrainWidth;
			float Y = heightValue * m_fHeightScale;
			float Z = (T * terrainHeight) - halfTerrainHeight;

			//Blend 3 textures depending on the height of the terrain
			float tex0Contribution = 1.0f - GetPercentage(heightValue, 0.0f, 0.75f);
			float tex2Contribution = 1.0f - GetPercentage(heightValue, 0.75f, 1.0f);

			m_NormalBuffer[index] = glm::vec3(0);
			m_PositionBuffer[index] = glm::vec3(X, Y, Z);
#if ENABLE_MULTITEXTURE
			m_ColorBuffer[index] = glm::vec4(tex0Contribution, tex0Contribution, tex0Contribution, tex2Contribution);
#else
			m_ColorBuffer[index] = glm::vec4(1.0f);
#endif
			m_Tex0Buffer[index] = glm::vec2(S, T);
		}
	}

	cout << "Terrain has been loaded!" << endl;
	delete[] heightMap;

	GenerateIndexBuffer();
	GenerateNormals();
	GenerateVertexBuffers();

	return true;
}

void Terrain::GenerateIndexBuffer()
{
	if (m_HeightmapDimensions.x < 2 || m_HeightmapDimensions.y < 2)
	{
		//Terrain hasn't been loaded
		//or is incorrect size
		return;
	}

	const unsigned int terrainWidth = m_HeightmapDimensions.x;
	const unsigned int terrainHeight = m_HeightmapDimensions.y;

	//2 triangles for every quad of the terrain mesh
	const unsigned int numTriangles = (terrainWidth - 1) * (terrainHeight - 1) * 2;

	//3 indices for each triangle in the terrain mesh
	m_IndexBuffer.resize(numTriangles * 3);

	//Index in the index buffer
	unsigned int index = 0;

	for (unsigned int j = 0; j < (terrainHeight - 1); j++)
	{
		for (unsigned int i = 0; i < (terrainWidth - 1); i++)
		{
			int vertexIndex = (j * terrainWidth) + i;

			//Top triangle (T0)
			m_IndexBuffer[index++] = vertexIndex;	// V0
			m_IndexBuffer[index++] = vertexIndex + terrainWidth + 1;	// V3
			m_IndexBuffer[index++] = vertexIndex + 1;	// V1

			//Bottom Triangle
			m_IndexBuffer[index++] = vertexIndex;	// V0
			m_IndexBuffer[index++] = vertexIndex + terrainWidth;	// V2
			m_IndexBuffer[index++] = vertexIndex + terrainWidth + 1;	//V3
		}
	}
}

void Terrain::GenerateNormals()
{
	for (unsigned int i = 0; i < m_IndexBuffer.size(); i += 3)
	{
		glm::vec3 v0 = m_PositionBuffer[ m_IndexBuffer[i + 0] ];
		glm::vec3 v1 = m_PositionBuffer[ m_IndexBuffer[i + 1] ];
		glm::vec3 v2 = m_PositionBuffer[ m_IndexBuffer[i + 2] ];

		glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

		m_NormalBuffer[m_IndexBuffer[i + 0]] += normal;
		m_NormalBuffer[m_IndexBuffer[i + 1]] += normal;
		m_NormalBuffer[m_IndexBuffer[i + 2]] += normal;
	}

	const glm::vec3 UP(0.0f, 1.0f, 0.0f);

	for (unsigned int i = 0; i < m_NormalBuffer.size(); i++)
	{
		m_NormalBuffer[i] = glm::normalize(m_NormalBuffer[i]);

#if ENABLE_SLOPE_BASED_BLEND
		float fTexture0Contribution = saturate(glm::dot(m_NormalBuffer[i], UP) - 0.1f);
		m_ColorBuffer[i] = glm::vec4(fTexture0Contribution, fTexture0Contribution, fTexture0Contribution, m_ColorBuffer[i].w);
#endif
	}
}

void Terrain::GenerateVertexBuffers()
{
	//Generate the buffer object ID's
	CreateVertexBuffer(m_GLVertexBuffer);
	CreateVertexBuffer(m_GLNormalBuffer);
	CreateVertexBuffer(m_GLColorBuffer);
	CreateVertexBuffer(m_GLTex0Buffer);
	CreateVertexBuffer(m_GLTex1Buffer);
	CreateVertexBuffer(m_GLTex2Buffer);
	CreateVertexBuffer(m_GLIndexBuffer);

	//Copy data into vertex buffer objects
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVertexBuffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(glm::vec3) * m_PositionBuffer.size(), &(m_PositionBuffer[0]), GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLColorBuffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(glm::vec4) * m_ColorBuffer.size(), &(m_ColorBuffer[0]), GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLNormalBuffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(glm::vec3) * m_NormalBuffer.size(), &(m_NormalBuffer[0]), GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLTex0Buffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(glm::vec2) * m_Tex0Buffer.size(), &(m_Tex0Buffer[0]), GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLTex1Buffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(glm::vec2) * m_Tex0Buffer.size(), &(m_Tex0Buffer[0]), GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLTex2Buffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(glm::vec2) * m_Tex0Buffer.size(), &(m_Tex0Buffer[0]), GL_STATIC_DRAW_ARB);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLIndexBuffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLuint) * m_IndexBuffer.size(), &(m_IndexBuffer[0]), GL_STATIC_DRAW_ARB);

	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

float Terrain::GetHeightAt(const glm::vec3 & position)
{
	float height = -FLT_MAX;

	//check if terrain dimensions are valid
	if (m_HeightmapDimensions.x < 2 || m_HeightmapDimensions.y < 2)
	{
		return height;
	}

	//Width and heigt of the terrain in world units
	float terrainWidth = (m_HeightmapDimensions.x - 1) * m_fBlockScale;
	float terrainHeight = (m_HeightmapDimensions.y - 1) * m_fBlockScale;
	float halfWidth = terrainWidth * 0.5f;
	float halfHeight = terrainHeight * 0.5f;

	//Multiply the position of the inverse of the terrain matrix
	//to get the position in terrain local space
	glm::vec3 terrainPos = glm::vec3(m_LocalToWorldMatrix * glm::vec4(position, 1.0f));
	glm::vec3 invBlockScale(1.0f / m_fBlockScale, 0.0f, 1.0f / m_fBlockScale);

	//Calculate offset and scale to get the vertex indices
	glm::vec3 offset(halfWidth, 0.0f, halfHeight);

	//Get the 4 vertices of the triangle we're over
	glm::vec3 vertexIndices = (terrainPos + offset) * invBlockScale;

	int u0 = (int)floorf(vertexIndices.x);
	int u1 = u0 + 1;
	int v0 = (int)floorf(vertexIndices.z);
	int v1 = v0 + 1;

	if (u0 >= 0 && u1 < m_HeightmapDimensions.x && v0 >= 0 && v1 < m_HeightmapDimensions.y)
	{
		glm::vec3 p00 = m_PositionBuffer[(v0 * m_HeightmapDimensions.x) + u0]; // Top-Left Vertex
		glm::vec3 p10 = m_PositionBuffer[(v0 * m_HeightmapDimensions.x) + u1]; // Top-Right Vertex
		glm::vec3 p01 = m_PositionBuffer[(v1 * m_HeightmapDimensions.x) + u0]; // Bottom-Left Vertex
		glm::vec3 p11 = m_PositionBuffer[(v1 * m_HeightmapDimensions.x) + u1]; // Bottom-Right Vertex

		//check which triange we're over
		float percentU = vertexIndices.x - u0;
		float percentV = vertexIndices.z - v0;

		glm::vec3 dU, dV;
		if (percentU > percentV)
		{
			//Top Triangle
			dU = p10 - p00;
			dV = p11 - p10;
		}
		else
		{
			//Bottom Triangle
			dU = p11 - p01;
			dV = p01 - p00;
		}

		glm::vec3 heightPos = p00 + (dU * percentU) + (dV * percentV);
		//Convert back to world-space 
		heightPos = glm::vec3(m_LocalToWorldMatrix * glm::vec4(heightPos, 1));
		height = heightPos.y;
	}

	return height;
}

void Terrain::Render()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixf(glm::value_ptr(m_LocalToWorldMatrix));

	//Texture Stage 0
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glScalef(32.0f, 32.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_GLTextures[0]);

	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLTex0Buffer);
	glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

#if ENABLE_MULTITEXTURE

	//Disable Lighting
	glDisable(GL_LIGHTING);

	//Texture Stage 1
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glScalef(32.0f, 32.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_GLTextures[1]);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_COLOR);

	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLTex1Buffer);
	glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

	//Texture Stage 2
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glScalef(32.0f, 32.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_GLTextures[2]);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_ALPHA);

	glClientActiveTextureARB(GL_TEXTURE2_ARB);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLTex2Buffer);
	glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));

#else
	glEnable(GL_TEXTURE)
	glEnable(GL_LIGHTING);
#endif

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLVertexBuffer);
	glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLColorBuffer);
	glColorPointer(4, GL_FLOAT, 0, BUFFER_OFFSET(0));
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_GLNormalBuffer);
	glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));

	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_GLIndexBuffer);
	glDrawElements(GL_TRIANGLES, m_IndexBuffer.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

	//Restore States
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

#if ENABLE_MULTITEXTURE
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glClientActiveTextureARB(GL_TEXTURE2_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER_ARB, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

inline float GetPercentage(float value, const float min, const float max)
{
	value = glm::clamp(value, min, max);
	return (value - min) / (max - min);
}

inline int GetFileLength(istream& file)
{
	int pos = file.tellg();
	file.seekg(0, ios::end);
	int length = file.tellg();

	//Restore position of the get pointer
	file.seekg(pos);

	return length;
}

inline void DeleteVertexBuffer(GLuint& vboID)
{
	if (vboID != 0)
	{
		glDeleteBuffersARB(1, &vboID);
		vboID = 0;
	}
}

inline void CreateVertexBuffer(GLuint& vboID)
{
	//Make sure dont lose the reference to the previous VBO, if there is one
	DeleteVertexBuffer(vboID);
	glGenBuffersARB(1, &vboID);
}

inline void DeleteTexture(GLuint& texID)
{
	if (texID != 0)
	{
		glDeleteTextures(1, &texID);
		texID = 0;
	}
}

inline void CreateTexture(GLuint& texID)
{
	DeleteTexture(texID);
	glGenTextures(1, &texID);
}

inline float GetHeightValue(const unsigned char* data, unsigned char numBytes)
{
	switch (numBytes)
	{
	case 1:
		{
			return (unsigned char)(data[0]) / (float)0xff;
		}
		break;
	case 2:
		{
			return (unsigned short)(data[1] << 8 | data[0]) / (float)0xffff;
		}
		break;
	case 3:
		{
			return (unsigned int)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0]) / (float)0xffffffff;
		}
		break;
	default:
		{
			assert(false);
		}
		break;
	}

	return 0.0f;
}