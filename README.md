ofxSpatialHash
=====================================
Fast nearest neighbour lookup for 2d particle systems

License
-------
[MIT License](https://en.wikipedia.org/wiki/MIT_License)

Installation
------------
Git clone or copy into `openFrameworks/addons/` folder.

Usage
------------
#### First use
First initialise the hash with the correct dimensions `ofxSpatialHash::init()` <br />
Points can be added to the hash with `ofxSpatialHash::addPoint(float x, float y, T value)` <br />
Search for points `getNearestPoints(float x, float y, float radius)` and test for distance if necessary <br />

#### Update points
Clear hash `ofxSpatialHash::clear()` <br />
Add points `ofxSpatialHash::addPoint(float x, float y, T value)` <br />

#### Example
All points contained in the green squares will be returned from a call to `ofxSpatialHash::getNearestPoints(float x, float y, float radius)`<br />
Users need to test the returned points to see if they are inside the radius with a call to `ofVec2f::distance` or `glm::distaance`

![Example](https://github.com/skell999/ofxSpatialHash/blob/main/docs/example.jpg?raw=true)

Known issues
------------
- 2d only.
- Points must be positive in x and y. 
- The spatial hash top left corner is anchored to 0,0.
- The class needs a predefined space width and height. Points outside this space will cause errors.
- The returned points from a nearest neighbour search will contain points outside of the search radius.
- An extra distance check provided by the user is needed to make sure you have points contained inside the radius.
