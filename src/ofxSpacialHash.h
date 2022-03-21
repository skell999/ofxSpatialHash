#pragma once

#include "ofMain.h"


// 1 Set size of the world 
// 2 Divide world into squares
// 3 Create a vector for each square
// 3 For each point find out which square it is in
// 4 Hash the points square to get an index
// 5 Use the index to place the point in a vector/bucket

// 6 Write a nearest neaighbour function

// http://www.cs.ucf.edu/~jmesit/publications/scsc%202005.pdf

template <class T>
class SpacialHash
{
public:
	SpacialHash();
	~SpacialHash();
	void init(float worldWidth, float worldHeight, float gridSize);
	void addPoint(float x, float y, T value);
	std::vector<T>& getNearestPoints(float x, float y, float radius);
	std::vector<int>& getNearestBuckets(float x, float y, float radius);


private:
	std::vector<T>& getBucket(float x, float y);
	int getBucketIndex(float x, float y);
	float clip(float n, float lower, float upper);
	float m_worldWidth = 0;
	float m_worldHeight = 0;
	float m_gridSize = 0;
	float m_cellWidth = 0;
	float m_cellHeight = 0;
	std::vector<std::vector<T>> m_buckets;
	std::vector<int> m_bucketIndexReturnBuffer;
	std::vector<T> m_returnBuffer;
};

const int BUCKET_PREALLOCATE_SIZE = 20;

template <class T>
SpacialHash<T>::SpacialHash()
{

}

template <class T>
SpacialHash<T>::~SpacialHash()
{
}

template<class T>
inline void SpacialHash<T>::init(float worldWidth, float worldHeight, float gridSize)
{
	m_worldWidth = worldWidth;
	m_worldHeight = worldHeight;
	m_gridSize = gridSize;
	m_cellWidth = m_worldWidth / m_gridSize;
	m_cellHeight = m_worldHeight / m_gridSize;
	m_buckets.clear();
	for (size_t i = 0; i < std::pow(gridSize,2.f); i++)
	{
		std::vector<T> bucket;
		bucket.reserve(BUCKET_PREALLOCATE_SIZE);
		m_buckets.emplace_back(bucket);
	}
}

template<class T>
void SpacialHash<T>::addPoint(float x, float y, T value)
{
	// dont add points outside the area?
	int index = getBucketIndex(x, y);
	m_buckets[index].push_back(value);
}

template<class T>
inline std::vector<T>& SpacialHash<T>::getNearestPoints(float x, float y, float radius)
{
	m_returnBuffer.clear();
	auto indices = getNearestBuckets(x, y, radius);

	for (auto& i : indices)
	{
		for (auto& point : m_buckets[i])
		{
			m_returnBuffer.push_back(point);
		}
	}

	return m_returnBuffer;
}

template<class T>
inline std::vector<int>& SpacialHash<T>::getNearestBuckets(float x, float y, float radius)
{
	m_bucketIndexReturnBuffer.clear();

	// Screen space. Bounding box corners
	float topLeftX = x - radius;
	float topLeftY = y - radius;
	float bottomRightX = x + radius;
	float bottomRightY = y + radius;

	// Grid space. Clip to stay inside grid
	float gridTopLeftX = clip(topLeftX / 100.f,0.f,9.f);
	float gridTopLeftY = clip(topLeftY / 100.f, 0.f, 9.f);
	float gridBottomRightX = clip(bottomRightX / 100.f, 0.f, 9.f);
	float gridBottomRightY = clip(bottomRightY / 100.f, 0.f, 9.f);

	// Bounding box
	float width = (floorf(gridBottomRightX)) - (floorf(gridTopLeftX)) + 1.f;
	float height = (floorf(gridBottomRightY)) - (floorf(gridTopLeftY)) + 1.f;

	// Iterate over bounding box
	for (size_t gx = 0; gx < width; gx++)
	{
		for (size_t gy = 0; gy < height; gy++)
		{
			// Translate from 0,0 to actual grid coordinates
			float gridX = gx + std::floorf(gridTopLeftX);
			float gridY = gy + std::floorf(gridTopLeftY);
			// Get bucket index
			int index = (gridY * 10) + gridX;
			m_bucketIndexReturnBuffer.push_back(index);
		}
	}
	std::cout << "gtlx: " << gridTopLeftX << " gtly: " << gridTopLeftY << " gbrx: " << gridBottomRightX << " gbry: " << gridBottomRightY << " ";
	std::cout << "bw: " << width << " bh: " << height << " gx: " << x / 100 << " gy: " << x / 100 << " radius: " << radius * 2.f << " \n";
	return m_bucketIndexReturnBuffer;
}

template<class T>
inline std::vector<T>& SpacialHash<T>::getBucket(float x, float y)
{
	// Bug here x and y need to be in bounds
	int index = getBucketIndex(x, y);
	return m_buckets[index];
}

template<class T>
int SpacialHash<T>::getBucketIndex(float x, float y)
{
	float cellSize = 100.f;
	float numBuckets = 10.f;
	int bucketID = static_cast<int>((std::floorf(x / 100.f)) + (std::floorf(y / 100.f)) * 10);
	return bucketID;
}

template<class T>
float SpacialHash<T>::clip(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}