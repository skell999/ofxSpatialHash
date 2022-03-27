#pragma once

/**
 * @brief ofxSpatialHash Fast nearest neighbour lookup for 2d partical systems
 * 
 * ### Restrictions
 * - 2d only.
 * - Points must be positive in x and y. 
 * - The spatial hash top left corner is anchored to 0,0.
 * - The class needs a predefined space width and height. Points outside this space will cause errors.
 * - The returned points from a nearest neighbour search will contain points outside of the search radius.
 * - An extra distance check provided by the user is needed to make sure you have points contained inside the radius.
 * 
 * ### Dependency
 * The class is written without any openframeworks dependency and can be used in any system with an origin in the top left.
 * 
 * @tparam T Point data or a pointer to point data
 * 
 * @see http://www.cs.ucf.edu/~jmesit/publications/scsc%202005.pdf
*/
#include <vector>
#include <cmath>
template <class T>
class ofxSpatialHash
{
public:

	/**
	 * @brief Initialise a spatial hash
	 * 
	 * @param worldWidth				The maximum width of the spatial hash, starting at 0,0
	 * @param worldHeight				The maximum height of the the spatial hash, starting at 0,0
	 * @param gridSize					A grid of buckets that is gridSize * gridSize
	 * @param bucketPreallocationSize	Avoid syscall trading memory for time
	 * 
	 * @note Internally this sets up the array of buckets. 
	 * So that it will contain the correct amount of buckets for a given grid size
	*/
	void init(float worldWidth, float worldHeight, float gridSize, int bucketPreallocationSize);

	/**
	 * @brief Add a value of type T to the spatial hash
	 * @param x Point x
	 * @param y Point y
	 * @param value Point value
	 * 
	 * @note Typically type T would be a pointer to a point
	*/
	void addPoint(float x, float y, T value);

	/**
	 * @brief Fast point lookup
	 * @param x Circle center x
	 * @param y Circle center y
	 * @param radius Circle radius
	 * @return A referance to a vector of type T
	 * 
	 * @note Use this to search a circular area for points. This will return points outside of the
	 * circle so an extra distance check is needed eg. `ofVec2f::distance()` or `glm::distance()`
	*/
	std::vector<T>& getNearestPoints(float x, float y, float radius);

	/**
	 * @brief Returns the bucket indices for a given circular search area
	 * @param x Circle center x
	 * @param y Circle center y
	 * @param radius Circle radius
	 * @return A vector of bucket indices
	*/
	std::vector<int>& getNearestBuckets(float x, float y, float radius);

	/**
	 * @brief Get a bucket index for a given point
	 * @param x Point x
	 * @param y Point y
	 * @return Bucket index
	*/
	int getBucketIndex(float x, float y);

	/**
	 * @brief Get the bucket for given point
	 * @param x Point x
	 * @param y Point y
	 * @return A vector<T> bucket.
	*/
	std::vector<T>& getBucket(float x, float y);

	/**
	 * @brief Clears the contents of every bucket.
	 * @note Use this when you need to rebuild the spatial hash with new positions for every point
	*/
	void clear();

private:
	std::vector<std::vector<T>> m_buckets;
	std::vector<int> m_bucketIndexReturnBuffer;
	std::vector<T> m_returnBuffer;
	float clip(float n, float lower, float upper);
	float m_worldWidth = 0;
	float m_worldHeight = 0;
	float m_gridSize = 0;
	float m_cellWidth = 0;
	float m_cellHeight = 0;
};

template<class T>
inline void ofxSpatialHash<T>::init(float worldWidth, float worldHeight, float gridSize, int bucketPreallocationSize)
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
		bucket.reserve(bucketPreallocationSize);
		m_buckets.emplace_back(bucket);
	}
}

template<class T>
void ofxSpatialHash<T>::addPoint(float x, float y, T value)
{
	int index = getBucketIndex(x, y);
	m_buckets[index].push_back(value);
}

template<class T>
inline std::vector<T>& ofxSpatialHash<T>::getNearestPoints(float x, float y, float radius)
{
	m_returnBuffer.clear();
	m_bucketIndexReturnBuffer = getNearestBuckets(x, y, radius);

	for (auto& i : m_bucketIndexReturnBuffer)
	{
		m_returnBuffer.insert(m_returnBuffer.end(), m_buckets[i].begin(), m_buckets[i].end());
	}
	return m_returnBuffer;
}

template<class T>
inline std::vector<int>& ofxSpatialHash<T>::getNearestBuckets(float x, float y, float radius)
{
	m_bucketIndexReturnBuffer.clear();

	// Screen space. Bounding box corners
	float topLeftX = x - radius;
	float topLeftY = y - radius;
	float bottomRightX = x + radius;
	float bottomRightY = y + radius;

	// Grid space. Clip to stay inside grid
	float gridTopLeftX = clip(topLeftX / m_cellWidth,0.f, m_gridSize - 1);
	float gridTopLeftY = clip(topLeftY / m_cellHeight, 0.f, m_gridSize - 1);
	float gridBottomRightX = clip(bottomRightX / m_cellWidth, 0.f, m_gridSize - 1);
	float gridBottomRightY = clip(bottomRightY / m_cellHeight, 0.f, m_gridSize - 1);

	// Bounding box dimensions
	float width = (std::floorf(gridBottomRightX)) - (std::floorf(gridTopLeftX)) + 1.f;
	float height = (std::floorf(gridBottomRightY)) - (std::floorf(gridTopLeftY)) + 1.f;

	// Iterate over bounding box
	for (size_t gx = 0; gx < width; gx++)
	{
		for (size_t gy = 0; gy < height; gy++)
		{
			// Translate from 0,0 to actual grid coordinates
			float gridX = gx + std::floorf(gridTopLeftX);
			float gridY = gy + std::floorf(gridTopLeftY);
			// Get bucket index
			int index = (gridY * m_gridSize) + gridX;
			m_bucketIndexReturnBuffer.push_back(index);
		}
	}
	return m_bucketIndexReturnBuffer;
}

template<class T>
int ofxSpatialHash<T>::getBucketIndex(float x, float y)
{
	int bucketID = static_cast<int>((std::floorf(x / m_cellWidth)) + (std::floorf(y / m_cellHeight)) * m_gridSize);
	return bucketID;
}

template<class T>
inline std::vector<T>& ofxSpatialHash<T>::getBucket(float x, float y)
{
	int index = getBucketIndex(x, y);
	return m_buckets[index];
}

template<class T>
inline void ofxSpatialHash<T>::clear()
{
	for (auto& bucket : m_buckets)
	{
		bucket.clear();
	}
}

template<class T>
float ofxSpatialHash<T>::clip(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}