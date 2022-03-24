#include "ofxSpacialHash_Test.h"

ofxSpacialHash_Test::ofxSpacialHash_Test()
{
}

ofxSpacialHash_Test::~ofxSpacialHash_Test()
{
}

void ofxSpacialHash_Test::init(float width, float height, float gridSize, int numberOfPoints, bool isPointsRandom)
{
	ofRegisterMouseEvents(this);
	ofRegisterKeyEvents(this);

	std::cout << "Running performace tests\n";
	spatialTest(1000, 1000, 10, 0);

	m_worldWidth = width;
	m_worldHeight = height;
	m_gridSize = gridSize;
	m_gridWidth = m_worldWidth / m_gridSize;
	m_gridHeight = m_worldHeight / m_gridSize;
	int bucketPreallocation = 50;

	m_sh.init(m_worldWidth, m_worldHeight, m_gridSize, bucketPreallocation);

	if (isPointsRandom)
	{
		generateRandomPoints(1000);
	}
	else 
	{
		generateUniformpoints();
	}

	for (auto& p : m_points)
	{
		m_sh.addPoint(p.x, p.y, &p);
	}
}

void ofxSpacialHash_Test::generateRandomPoints(int amount)
{
	for (size_t i = 0; i < amount; i++)
	{
		m_points.push_back({ ofRandom(m_worldWidth), ofRandom(m_worldHeight) });
	}
}

void ofxSpacialHash_Test::generateUniformpoints()
{
	for (int x = 0; x < m_gridSize; x++)
	{
		for (int y = 0; y < m_gridSize; y++)
		{
			float gx = x * m_gridWidth;
			float gy = y * m_gridHeight;
			m_points.push_back({ gx + (m_gridWidth / 2.f) , gy + (m_gridHeight / 2.f) });
		}
	}
}

void ofxSpacialHash_Test::draw()
{
	ofPushStyle();
	ofSetLineWidth(1);
	ofNoFill();

	// Draw grid
	ofDrawRectangle(0.f, 0.f, m_worldWidth, m_worldHeight);
	for (float x = 0.f; x < m_worldWidth; x += m_gridWidth)
	{
		ofDrawLine(x, 0.f, x, m_worldHeight);
	}
	for (float y = 0.f; y < m_worldHeight; y += m_gridHeight)
	{
		ofDrawLine(0.f, y, m_worldWidth, y);
	}
	ofFill();

	// Draw pointsVec2f
	for (auto& p : m_points)
	{
		ofDrawCircle(p, 2);
	}

	// Draw bucket indices top left
	if (drawBucketIndices)
	{
		for (int x = 0; x < m_gridSize; x++)
		{
			for (int y = 0; y < m_gridSize; y++)
			{
				float gx = x * m_gridWidth;
				float gy = y * m_gridHeight;
				ofDrawBitmapString(m_sh.getBucketIndex(gx, gy),gx,gy + 10);
			}
		}
	}

	std::vector<int>& bucketIndices = m_sh.getNearestBuckets(m_mouseX, m_mouseY, m_searchRadius);
	auto points = m_sh.getNearestPoints(m_mouseX, m_mouseY, m_searchRadius);

	ofSetColor(ofColor::green);
	ofNoFill();

	// Draw buckets inside of bounding box
	for (auto& i : bucketIndices)
	{
		// bucket indices to bucket xy coords
		float col = i / (int)m_gridSize;
		float row = i % (int)m_gridSize;

		// bucket coords to screen xy coords
		float y = col * m_gridHeight;
		float x = row * m_gridWidth;

		// Draw bucket
		ofDrawRectangle(x, y, m_gridWidth, m_gridHeight);
	}

	// Draw radius
	ofDrawCircle(m_mouseX, m_mouseY, m_searchRadius);

	ofFill();
	for (auto& p : points)
	{
		if (p->distance({ m_mouseX,m_mouseY }) < m_searchRadius)
		{
			// Points color within radius
			ofSetColor(ofColor::red);
		}
		else
		{
			// Points color outside of radius and inside of bounding box
			ofSetColor(ofColor::green);
		}
		ofDrawCircle(*p, 3);
	}
	ofPopStyle();
}

struct Result
{
	unsigned int loopCount = 0;
	unsigned int distCount = 0;
	unsigned int inRadiusCount = 0;
	long long micro = 0;
	long long nano = 0;
	long long milli = 0;
	long long sec = 0;
	long long min = 0;
};

Result test_glm_spatial_hash(ofxSpatialHash<glm::vec2*>& spatialHash, glm::vec2 searchPoint, float searchRadius, int iterations)
{
	Result r;
	using namespace std::chrono;
	steady_clock::time_point begin;
	steady_clock::time_point end;
	auto& shPointsGlm = spatialHash.getNearestPoints(searchPoint.x, searchPoint.y, searchRadius);
	for (size_t i = 0; i < iterations; i++)
	{
		r.loopCount = 0;
		r.distCount = 0;
		r.inRadiusCount = 0;
		begin = std::chrono::steady_clock::now();
		auto& points = spatialHash.getNearestPoints(searchPoint.x, searchPoint.y, searchRadius);

		for (const auto& p : points)
		{
			r.distCount++;
			if (glm::distance(*p, searchPoint) < searchRadius)
			{
				r.inRadiusCount++;
			}
			r.loopCount++;
		}
		end = std::chrono::steady_clock::now();

		r.micro += duration_cast<microseconds>(end - begin).count();
		r.nano += duration_cast<nanoseconds>(end - begin).count();
		r.milli += duration_cast<milliseconds>(end - begin).count();
		r.sec += duration_cast<seconds>(end - begin).count();
		r.min += duration_cast<minutes>(end - begin).count();
	}

	r.micro /= iterations;
	r.nano /= iterations;
	r.milli /= iterations;
	r.sec /= iterations;
	r.min /= iterations;

	return r;
}

Result test_ofVec2f_spatial_hash(ofxSpatialHash<ofVec2f*>& spatialHash, ofVec2f searchPoint, float searchRadius, int iterations)
{
	Result r;
	using namespace std::chrono;
	steady_clock::time_point begin;
	steady_clock::time_point end;
	auto& shPointsGlm = spatialHash.getNearestPoints(searchPoint.x, searchPoint.y, searchRadius);
	for (size_t i = 0; i < iterations; i++)
	{
		r.loopCount = 0;
		r.distCount = 0;
		r.inRadiusCount = 0;
		begin = steady_clock::now();
		auto& points = spatialHash.getNearestPoints(searchPoint.x, searchPoint.y, searchRadius);

		for (const auto& p : points)
		{
			r.distCount++;
			if (p->distance(searchPoint) < searchRadius)
			{
				r.inRadiusCount++;
			}
			r.loopCount++;
		}
		end = steady_clock::now();

		r.micro += duration_cast<microseconds>(end - begin).count();
		r.nano += duration_cast<nanoseconds>(end - begin).count();
		r.milli += duration_cast<milliseconds>(end - begin).count();
		r.sec += duration_cast<seconds>(end - begin).count();
		r.min += duration_cast<minutes>(end - begin).count();
	}

	r.micro /= iterations;
	r.nano /= iterations;
	r.milli /= iterations;
	r.sec /= iterations;
	r.min /= iterations;

	return r;
}

Result ofVec2f_naive(ofxSpatialHash<ofVec2f*>& spatialHash, ofVec2f searchPoint, float searchRadius, int iterations)
{
	Result r;
	using namespace std::chrono;
	steady_clock::time_point begin;
	steady_clock::time_point end;
	auto& shPointsGlm = spatialHash.getNearestPoints(searchPoint.x, searchPoint.y, searchRadius);
	for (size_t i = 0; i < iterations; i++)
	{
		r.loopCount = 0;
		r.distCount = 0;
		r.inRadiusCount = 0;
		begin = std::chrono::steady_clock::now();
		auto& points = spatialHash.getNearestPoints(searchPoint.x, searchPoint.y, searchRadius);

		for (size_t i = 0; i < points.size(); i++)
		{
			for (size_t j = 0; j < points.size(); j++)
			{
				r.loopCount++;
				r.distCount++;
				if (points[j]->distance(*points[i]) < searchRadius)
				{
					r.inRadiusCount++;
				}
			}
		}
		end = std::chrono::steady_clock::now();

		r.micro += duration_cast<microseconds>(end - begin).count();
		r.nano += duration_cast<nanoseconds>(end - begin).count();
		r.milli += duration_cast<milliseconds>(end - begin).count();
		r.sec += duration_cast<seconds>(end - begin).count();
		r.min += duration_cast<minutes>(end - begin).count();
	}

	r.micro /= iterations;
	r.nano /= iterations;
	r.milli /= iterations;
	r.sec /= iterations;
	r.min /= iterations;

	return r;
}

Result glmvec2_naive(ofxSpatialHash<glm::vec2*>& spatialHash, glm::vec2 searchPoint, float searchRadius, int iterations)
{
	Result r;
	using namespace std::chrono;
	steady_clock::time_point begin;
	steady_clock::time_point end;
	auto& shPointsGlm = spatialHash.getNearestPoints(searchPoint.x, searchPoint.y, searchRadius);
	for (size_t i = 0; i < iterations; i++)
	{
		r.loopCount = 0;
		r.distCount = 0;
		r.inRadiusCount = 0;
		begin = std::chrono::steady_clock::now();
		auto& points = spatialHash.getNearestPoints(searchPoint.x, searchPoint.y, searchRadius);

		for (size_t i = 0; i < points.size(); i++)
		{
			for (size_t j = 0; j < points.size(); j++)
			{
				r.loopCount++;
				r.distCount++;
				if (glm::distance(*points[j], *points[i]) < searchRadius)
				{
					r.inRadiusCount++;
				}
			}
		}
		end = std::chrono::steady_clock::now();

		r.micro += duration_cast<microseconds>(end - begin).count();
		r.nano += duration_cast<nanoseconds>(end - begin).count();
		r.milli += duration_cast<milliseconds>(end - begin).count();
		r.sec += duration_cast<seconds>(end - begin).count();
		r.min += duration_cast<minutes>(end - begin).count();
	}

	r.micro /= iterations;
	r.nano /= iterations;
	r.milli /= iterations;
	r.sec /= iterations;
	r.min /= iterations;

	return r;
}

void printResult(std::string testName, Result& r, int worldW, int worldH, int gridSize, int numPoints, int isDebug)
{
	using namespace std;
	cout << " [Ms] = " << (float)r.micro / 1000.f;
}
//[World Width] 1000[World Height] 1000[Grid Size] 10[Search Radius] 150
//Spatial hash add point
//[Num Points] = 100[Ms] = 0.055
//[Num Points] = 1000[Ms] = 0.301
//[Num Points] = 10000[Ms] = 1.276
//[Num Points] = 50000[Ms] = 5.269
//[Num Points] = 100000[Ms] = 9.23
//[Num Points] = 500000[Ms] = 55.71
//[Num Points] = 1000000[Ms] = 107.168
//[Num Points] = 10000000[Ms] = 1116.19
//[Ms] = 0.008[Ms] = 0.004[Ms] = 0.012[Ms] = 0.008
//[Ms] = 0.022[Ms] = 0.014[Ms] = 1.219[Ms] = 0.643
//[Ms] = 0.137[Ms] = 0.104[Ms] = 106.859[Ms] = 61.337
//[Ms] = 0.787[Ms] = 0.533[Ms] = 2631.26[Ms] = 1363.52
//[Ms] = 1.484[Ms] = 1.082[Ms] = 10929.2[Ms] = 5672.39
//[Ms] = 7.921[Ms] = 5.779[Ms] = 320844[Ms] = 168197
//[Ms] = 23.674[Ms] = 12.711 none[Ms] = 208.988[Ms] = 138.787 none
void spatialTest(float worldW, float worldH, float gridSize, int preAllocSize)
{
	std::array<int, 8> numPoints = { 100, 1000, 10'000, 50'000, 100'000, 500'000, 1'000'000, 10'000'000 };

	float cx = worldW / 2.f;
	float cy = worldH / 2.f;
	ofVec2f centerOfVec2f{ cx,cy };
	glm::vec2 centerGlm{ cx, cy };

	float searchRadius = 150;
	ofVec2f dummyForce = { 0,1 };

	unsigned int loopCount = 0;
	unsigned int calcCount = 0;
	unsigned int foundCount = 0;

	int isDebug = 1;

#ifdef DEBUG
	isDebug = 0;
#endif // DEBUG

	std::cout << "[World Width] " << worldW;
	std::cout << " [World Height] " << worldH;
	std::cout << " [Grid Size] " << gridSize;
	std::cout << " [Search Radius] " << searchRadius;
	std::cout << " [isDebug] = " << isDebug;
	std::cout << std::endl;

	{
		using namespace std::chrono;
		using namespace std;
		steady_clock::time_point begin;
		steady_clock::time_point end;
		ofxSpatialHash<ofVec2f*> hash;
		hash.init(worldW, worldH, gridSize, 100'000);
		std::vector<ofVec2f> points;

		for (size_t i = 0; i < numPoints.back(); i++)
		{
			points.push_back({ofRandom(worldW), ofRandom(worldH)});
		}


		cout << "Spatial hash add point\n";
		for (size_t i = 0; i < numPoints.size(); i++)
		{
			begin = std::chrono::steady_clock::now();
			hash.clear();
			for (size_t j = 0; j < numPoints[i]; j++)
			{
				hash.addPoint(points[j].x, points[j].y, &points[j]);
			}
			end = std::chrono::steady_clock::now();
			cout << "[Num Points] = " << numPoints[i];
			cout << " [Ms] = " << (float)duration_cast<microseconds>(end - begin).count() / 1000.f << endl;
		}
	}

	std::cout << "   \t[glm spatial hash]  ";
	std::cout << "   [ofVec2f spatial hash]  ";
	std::cout << "   [glm naive]  ";
	std::cout << "   [ofVec2f naive]  ";
	std::cout << "\n";

	int iterations = 1;
	{
		for (size_t i = 0; i < numPoints.size(); i++)
		{
			std::cout << "[Num Points] = " << numPoints[i];
			{
				ofSeedRandom(3286428356);
				std::vector<glm::vec2> points;
				ofxSpatialHash<glm::vec2*> hash;
				hash.init(worldW, worldH, gridSize, 100'000);
				for (size_t j = 0; j < numPoints[i]; j++)
				{
					points.push_back({ ofRandom(worldW) , ofRandom(worldH) });
				}
				for (auto& p : points)
				{
					hash.addPoint(p.x, p.y, &p);
				}
				auto spatialHashGlm = test_glm_spatial_hash(hash, centerGlm, searchRadius, iterations);
				printResult("Spatial GLM", spatialHashGlm, worldW, worldH, gridSize, numPoints[i], isDebug);
			}

			{
				ofSeedRandom(3286428356);
				std::vector<ofVec2f> points;
				ofxSpatialHash<ofVec2f*> hash;
				hash.init(worldW, worldH, gridSize, 100'000);
				for (size_t j = 0; j < numPoints[i]; j++)
				{
					points.push_back({ ofRandom(worldW) , ofRandom(worldH) });
				}
				for (auto& p : points)
				{
					hash.addPoint(p.x, p.y, &p);
				}
				auto spatialHashGlm = test_ofVec2f_spatial_hash(hash, centerOfVec2f, searchRadius, iterations);
				printResult("Spatial GLM", spatialHashGlm, worldW, worldH, gridSize, numPoints[i], isDebug);
			}

			{
				if (numPoints[i] > 500'000) { std::cout << " none "; continue; }
				ofSeedRandom(3286428356);
				std::vector<glm::vec2> points;
				ofxSpatialHash<glm::vec2*> hash;
				hash.init(worldW, worldH, gridSize, 100'000);
				for (size_t j = 0; j < numPoints[i]; j++)
				{
					points.push_back({ ofRandom(worldW) , ofRandom(worldH) });
				}
				for (auto& p : points)
				{
					hash.addPoint(p.x, p.y, &p);
				}
				auto spatialHashGlm = glmvec2_naive(hash, centerOfVec2f, searchRadius, iterations);
				printResult("Spatial GLM", spatialHashGlm, worldW, worldH, gridSize, numPoints[i], isDebug);
			}

			{
				if (numPoints[i] > 500'000) { std::cout << " none "; continue; }
				ofSeedRandom(3286428356);
				std::vector<ofVec2f> points;
				ofxSpatialHash<ofVec2f*> hash;
				hash.init(worldW, worldH, gridSize, 100'000);
				for (size_t j = 0; j < numPoints[i]; j++)
				{
					points.push_back({ ofRandom(worldW) , ofRandom(worldH) });
				}
				for (auto& p : points)
				{
					hash.addPoint(p.x, p.y, &p);
				}
				auto spatialHashOfVec2fNaive = ofVec2f_naive(hash, centerOfVec2f, searchRadius, 1);
				printResult("OfVec2f Naive", spatialHashOfVec2fNaive, worldW, worldH, gridSize, numPoints[i], isDebug);
			}
			std::cout << "\n";
		}
	}
}
