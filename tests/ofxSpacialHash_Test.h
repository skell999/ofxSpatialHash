#pragma once
#include "ofMain.h"
#include <ofxSpacialHash.h>
class ofxSpacialHash_Test
{
public:
	ofxSpacialHash_Test();
	ofxSpacialHash_Test(float worldWidth, float worldHeight, float gridSize) : m_worldWidth(worldWidth), m_worldHeight(worldHeight), m_gridSize(gridSize) {};
	~ofxSpacialHash_Test();

	void init();
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

	float m_searchRadius = 100.f;
private:
	float m_worldWidth = 0;
	float m_worldHeight = 0;
	float m_gridSize = 0;
	float m_mouseX = 0;
	float m_mouseY = 0;
	ofVbo m_vbo;
	std::vector<ofVec2f> m_points;
	SpacialHash<ofVec2f*> m_sh;
};

