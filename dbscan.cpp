/*
// Originally based on ChatGPT code creation
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

// Used for marking clusters
constexpr int unexplored = -1;
constexpr int noice = 0;;

struct Point {
    double x, y;
    int clusterId; // "unexplored", "noice", 1.. means clusterID
    bool visited;  // "false", "true"

    Point(double x = 0, double y = 0) : x(x), y(y), clusterId(unexplored), visited(false) {}
};

// Euclidean distance
double distance(const Point& a, const Point& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Manhattan distance
double distanceMan(const Point& a, const Point& b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Find all neighbors within epsilon distance
vector<int> regionQuery(const vector<Point>& points, const Point& p, double eps) {
    vector<int> neighbors;
    for (unsigned i = 0; i < points.size(); ++i) {
        if (distanceMan(p, points[i]) <= eps) { // Euclidean or Manhattan distance
            neighbors.push_back(i);
        }
    }
    return neighbors;
}

// Expand cluster
bool expandCluster(vector<Point>& points, int pointIdx, int clusterId, double eps, unsigned minPts) {
	cout << "Expand cluster" << endl;
	
    vector<int> seeds = regionQuery(points, points[pointIdx], eps);

    if (seeds.size() < minPts) {
        points[pointIdx].clusterId = noice; // Mark as noice
        cout << "  Point idx as noice: " << pointIdx << endl;
        return false;
    }

    for (int idx : seeds) {
        points[idx].clusterId = clusterId;
        cout << "  Point seed idx: " << idx << " in clusterID: " << clusterId << endl;
    }

    for (int idx : seeds) {
        cout << "  Seed point idx before: " << idx << endl;
    }

    seeds.erase(remove(seeds.begin(), seeds.end(), pointIdx), seeds.end());
    
    for (int idx : seeds) {
        cout << "  Seed point idx after: " << idx << endl;
    }

    while (!seeds.empty()) {
        int currentP = seeds.back();
        seeds.pop_back();

        cout << "    Remainng point idx: " << currentP << endl;

        if (!points[currentP].visited) {
            points[currentP].visited = true;
            vector<int> result = regionQuery(points, points[currentP], eps);

            if (result.size() >= minPts) {
                seeds.insert(seeds.end(), result.begin(), result.end()); // Add new neighbors to list
                for (int idx : seeds) {
                    cout << "      Neighbor point idx: " << idx << endl;
                }
            }
            else {
                for (int idx : result) {
                    cout << "      No neighbor point idx: " << idx << endl;
                }
            }
        }
        else {
            cout << "    Visited point idx: " << currentP << endl;
        }

        if (points[currentP].clusterId == unexplored) {
            points[currentP].clusterId = clusterId; // Set to cluster if not yet explored
            cout << "    Unexplored point idx: " << currentP << endl;
        }
    }
    return true;
}

// Run DBSCAN algorithm
void dbscan(vector<Point>& points, double eps, unsigned minPts) {
    int clusterId = 0;
    for (unsigned i = 0; i < points.size(); ++i) {
        if (!points[i].visited) {
            points[i].visited = true;

            if (expandCluster(points, i, ++clusterId, eps, minPts)) {
                // New cluster found and expanded
            }
        }
    }
}

int main() {
    // Sample points
    vector<Point> points = {
        Point( 1.0,  1.0),
        Point( 1.1,  1.1),
        Point( 0.9,  0.9),
        Point(10.0, 10.0),
        Point(10.1, 10.1),
        Point( 9.9,  9.9),
        Point( 5.0,  5.0)
    };

	// The DBSCAN parameter
    double eps = 1.5;
    unsigned minPts = 2;

    dbscan(points, eps, minPts);
    
    cout << endl;

    for (const auto& p : points) {
        cout << "Point (" << p.x << ", " << p.y << ") - Cluster ID: " << p.clusterId << endl;
    }

    return 0;
}
