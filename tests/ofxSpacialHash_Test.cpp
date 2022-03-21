#include "ofxSpacialHash_Test.h"

ofxSpacialHash_Test::ofxSpacialHash_Test()
{
}

ofxSpacialHash_Test::~ofxSpacialHash_Test()
{
}

void ofxSpacialHash_Test::init()
{
	ofRegisterMouseEvents(this);
	//ofAddListener(ofEvents().mousePressed, this, &ofxSpacialHash_Test::mousePressed);
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]" << std::endl;

	m_sh.init(1000, 1000, 10);

	// Point for every bucket
	//for (size_t i = 0; i < 100; i++)
	//{
	//	float x = (i * 100) % 1000;
	//	float y = (i / 10) * 100;
	//	m_points.push_back({ x + 50.f , y + 50.f });
	//}

	for (size_t i = 0; i < 1000; i++)
	{
		m_points.push_back({ ofRandom(1000), ofRandom(1000) });
	}

	for (auto& p : m_points)
	{
		m_sh.addPoint(p.x, p.y, &p);
	}
	
	int i = 0;
}

void ofxSpacialHash_Test::draw()
{
	ofPushStyle();
	ofSetLineWidth(1);
	for (size_t x = 0; x < 1100; x += 100)
	{
		ofDrawLine(x, 0.f, x, 1000);
	}
	for (size_t y = 0; y < 1100; y += 100)
	{
		ofDrawLine(0.f, y, 1000, y);
	}

	for (auto& p : m_points)
	{
		ofDrawCircle(p, 2);
	}

	//m_mouseX = 310.f;
	//m_mouseY = 310.f;

	std::vector<int>& bucketIndices = m_sh.getNearestBuckets(m_mouseX, m_mouseY, m_searchRadius);
	auto points = m_sh.getNearestPoints(m_mouseX, m_mouseY, m_searchRadius);

	ofSetColor(ofColor::green);
	ofNoFill();
	for (auto& i : bucketIndices)
	{
		// bucket indices to bucket xy coords
		int row = i / 10;
		int col = i % 10;

		// bucket coords to screen xy coords
		int y = row * 100;
		int x = col * 100;
		int goo = 0;
		ofDrawRectangle(x, y, 100, 100);
	}

	ofDrawCircle(m_mouseX, m_mouseY, m_searchRadius);
	ofDrawRectangle(m_mouseX - m_searchRadius, m_mouseY - m_searchRadius, m_searchRadius * 2.f, m_searchRadius * 2.f);

	ofFill();
	for (auto& p : points)
	{
		ofDrawCircle(*p, 2);
	}
	ofPopStyle();
}
