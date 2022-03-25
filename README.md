ofxSpatialHash
=====================================
Fast nearest neighbour lookup for 2d particle systems

# Tests
## Debug Build
World size 1000 x 1000. Grid size 10. Search radius 150. <br />
Adding ofVec2f as pointers `ofxSpatialHash::addPoint()` <br />
| Points | Milliseconds |
| --- | --- |
| 100 | 0.054 |
| 1,000 | 0.302 |
| 10,000 | 1.245 |
| 50,000 | 5.035 |
| 100,000 | 9.392 |
| 500,000 | 47.302 |
| 1,000,000 | 95.738 |
| 10,000,000 | 995.733 |

Nearest neighbour search. Milliseconds.
| Points | Hash `glm` | Hash `ofVec2f` | Naive `glm` | Naive `ofVec2f` |
| --- | --- | --- | --- | --- |
| 100 | 0.004 | 0.004 | 0.012 | 0.008 |
| 1,000 | 0.018 | 0.015 | 1.204 | 0.573 |
| 10,000 | 0.136 | 0.105 | 104.703 | 51.006 |
| 50,000 | 0.683 | 0.548 | 2609.96 | 1305.25 |
| 100,000 | 1.463 | 1.087 | 10890.3 | 5574.67 |
| 500,000 | 7.769 | 5.615 | 330835 | 168685 |
| 1,000,000 | 19.107 | 12.219 | n/a | n/a |
| 10,000,000 | 207.801 | 129.039 |n/a | n/a |

## Release Build
Adding ofVec2f as pointers `ofxSpatialHash::addPoint()` <br />
| Points | Milliseconds |
| --- | --- |
| 100 | 0.01 |
| 1,000 | 0.07 |
| 10,000 | 0.241 |
| 50,000 | 0.968 |
| 100,000 | 1.582 |
| 500,000 | 12.341 |
| 1,000,000 | 14.337 |
| 10,000,000 | 268.345 |

Nearest neighbour search. Milliseconds.
| Points | Hash `glm` | Hash `ofVec2f` | Naive `glm` | Naive `ofVec2f` |
| --- | --- | --- | --- | --- |
| 100 | 0 | 0 | 0.001 |
| 1,000 | 0 | 0 | 0.14 | 0.251 |
| 10,000 | 0.005 | 0.005 | 19.369 | 25.852 |
| 50,000 | 0.043 | 0.057 | 326.108 | 295.424 |
| 100,000 | 0.154 | 0.143 | 1103.8 | 1029.82 |
| 500,000 | 0.828 | 0.572 | 28465.3 | 30356.9 |
| 1,000,000 | 1.707 | 1.711 | n/a | n/a | 
| 10,000,000 | 23.678 | 23.955 | n/a | n/a |

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


