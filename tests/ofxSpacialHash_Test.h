#pragma once
#include "ofMain.h"
#include <ofxSpatialHash.h>

void spatialTest(float worldW, float worldH, float gridSize, int preAllocSize);

class ofxSpacialHash_Test
{
public:
	ofxSpacialHash_Test();
	~ofxSpacialHash_Test();

	void init(float width, float height, float gridSize, int numberOfPoints, bool isPointsRandom);
	void draw();

	void mouseMoved(ofMouseEventArgs& args) 
	{
		m_mouseX = args.x;
		m_mouseY = args.y;
	};
	void mouseDragged(ofMouseEventArgs& args) {};
	void mousePressed(ofMouseEventArgs& args) {};
	void mouseReleased(ofMouseEventArgs& args) {};
	void mouseScrolled(ofMouseEventArgs& args) 
	{
		m_searchRadius += 10.f * args.scrollY;
		if (m_searchRadius < 0.f)
		{
			m_searchRadius = 0.f;
		}
	};
	void mouseEntered(ofMouseEventArgs& args) {};
	void mouseExited(ofMouseEventArgs& args) {};

	void keyPressed(ofKeyEventArgs& args) 
	{
		if (args.key == '1' || args.key == '!')
		{
			drawBucketIndices = !drawBucketIndices;
		}
	};
	void keyReleased(ofKeyEventArgs& args) {};

	float m_searchRadius = 100.f;
	bool drawBucketIndices = false;
private:
	void generateRandomPoints(int amount);
	void generateUniformpoints();
	float m_worldWidth = 0;
	float m_worldHeight = 0;
	float m_gridSize = 0;
	float m_gridWidth = 0;
	float m_gridHeight = 0;
	float m_mouseX = 0;
	float m_mouseY = 0;
	ofVbo m_vbo;
	std::vector<ofVec2f> m_points;
	ofxSpatialHash<ofVec2f*> m_sh;
};


