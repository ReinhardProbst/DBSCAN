/*
// Originally based on ChatGPT code creation
*/

#include "matplotlibcpp.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>

namespace plt = matplotlibcpp;
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

// Get vector of X coordinates from vector of points
vector<double> getPointsX(const vector<Point>& points) {
    vector<double> pointsX;

    for (const auto& p : points)
        pointsX.push_back(p.x);

    return pointsX;
}

// Get vector of Y coordinates from vector of points
vector<double> getPointsY(const vector<Point>& points) {
    vector<double> pointsY;

    for (const auto& p : points)
        pointsY.push_back(p.y);

    return pointsY;
}

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
        if (distanceMan(p, points[i]) <= eps) { // Select Euclidean or Manhattan distance
            neighbors.push_back(i);
        }
    }

    return neighbors;
}

// Expand cluster
bool expandCluster(vector<Point>& points, const int pointIdx, int& clusterId, const double eps, const unsigned minPts) {
    //cout << "Expand cluster" << endl;
    
    vector<int> seeds = regionQuery(points, points[pointIdx], eps);

    if (seeds.size() < minPts) {
        points[pointIdx].clusterId = noice; // Mark as noice
        //cout << "  Point idx as noice: " << pointIdx << endl;
        --clusterId; // No new cluster created, decrease previously incremented clusterId
        return false;
    }

    for (int idx : seeds) {
        points[idx].clusterId = clusterId;
        //cout << "  Point seed idx: " << idx << " in clusterID: " << clusterId << endl;
    }

    //for (int idx : seeds)
    //    cout << "  Seed point idx before: " << idx << endl;

    seeds.erase(remove(seeds.begin(), seeds.end(), pointIdx), seeds.end());
    
    //for (int idx : seeds)
    //    cout << "  Seed point idx after: " << idx << endl;

    while (!seeds.empty()) {
        int currentP = seeds.back();
        seeds.pop_back();

        //cout << "    Remainng point idx: " << currentP << endl;

        if (!points[currentP].visited) {
            points[currentP].visited = true;
            vector<int> result = regionQuery(points, points[currentP], eps);

            if (result.size() >= minPts) {
                seeds.insert(seeds.end(), result.begin(), result.end()); // Add new neighbors to list
                //for (int idx : seeds)
                //    cout << "      Neighbor point idx: " << idx << endl;
            }
            else {
                //for (int idx : result)
                //    cout << "      No neighbor point idx: " << idx << endl;
            }
        }
        else {
            //cout << "    Visited point idx: " << currentP << endl;
        }

        if (points[currentP].clusterId == unexplored) {
            points[currentP].clusterId = clusterId; // Set to cluster if not yet explored
            //cout << "    Unexplored point idx: " << currentP << endl;
        }
    }
    return true;
}

// DBSCAN algorithm
int dbscan(vector<Point>& points, const double eps, int const minPts) {
    int clusterId = 0;
    for (unsigned i = 0; i < points.size(); ++i) {
        if (!points[i].visited) {
            points[i].visited = true;

            if (expandCluster(points, i, ++clusterId, eps, minPts)) {
                cout << "New cluster found and expanded: " << clusterId << endl;
            }
        }
    }

    return clusterId;
}

// Get sample data from CSV-file with semikolon as separator
bool readCSV(const string& filename, vector<Point>& points) {
    ifstream file(filename);
    string line;

    points.clear();

    if (!file.is_open()) {
        cerr << "Error with: " << filename << endl;
        return false;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string xStr, yStr;
        
        if (getline(ss, xStr, ';') && getline(ss, yStr, ';')) {
            points.emplace_back(stod(xStr), stod(yStr));
        }
    }
    
    file.close();

    return true;
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

    if (!readCSV("./smile_face.csv", points))
        return -1;

    // Output of read sample points
    //for (const auto& p : points) {
    //    cout << "Point (x = " << p.x << ", y = " << p.y << ")" << endl;
    //}

    vector<double> x = getPointsX(points);
    vector<double> y = getPointsY(points);

    plt::figure_size(1000, 1000);
    plt::plot(x, y, "bx");

    // The DBSCAN parameter
    double eps = 2;
    unsigned minPts = 10;

    // Run the DBSCAN algorithm
    int maxClusterId = dbscan(points, eps, minPts);
    
    cout << "Found max. cluster: " << maxClusterId << endl;

    for (const auto& p : points) {
        cout << "Point (" << p.x << ", " << p.y << ") - Cluster ID: " << p.clusterId << endl;
    }

    plt::show();

    return 0;
}
