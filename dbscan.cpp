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
#include <concepts>

namespace plt = matplotlibcpp;
using namespace std;

// Used for marking clusters
constexpr int unexplored = -1;
constexpr int noice = 0;;

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template <Arithmetic T>
struct Point {
    T x, y;
    int clusterId; // "unexplored", "noice", 1.. means clusterID
    bool visited;  // "false", "true"

    Point(T x = 0, T y = 0) : x(x), y(y), clusterId(unexplored), visited(false) {}
};

// Get vector of X coordinates from vector of points
template <Arithmetic T>
vector<T> extractXCoordinates(const vector<Point<T>>& points) {
    vector<T> xCoordinates;
    
    transform(points.begin(), points.end(), back_inserter(xCoordinates), [](const Point<T>& p) { return p.x; });
    
    return xCoordinates;
}

// Get vector of Y coordinates from vector of points
template <Arithmetic T>
vector<T> extractYCoordinates(const vector<Point<T>>& points) {
    vector<T> yCoordinates;
    
    transform(points.begin(), points.end(), back_inserter(yCoordinates), [](const Point<T>& p) { return p.y; });
    
    return yCoordinates;
}

// Euclidean distance
template <Arithmetic T>
T distance(const Point<T>& a, const Point<T>& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Manhattan distance
template <Arithmetic T>
T distanceMan(const Point<T>& a, const Point<T>& b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Find all neighbors within epsilon distance
template <Arithmetic T>
vector<int> regionQuery(const vector<Point<T>>& points, const Point<T>& p, T eps) {
    vector<int> neighbors;

    for (unsigned i = 0; i < points.size(); ++i) {
        if (distanceMan(p, points[i]) <= eps) { // Select Euclidean or Manhattan distance
            neighbors.push_back(i);
        }
    }

    return neighbors;
}

// Expand cluster
template <Arithmetic T>
bool expandCluster(vector<Point<T>>& points, const int pointIdx, int& clusterId, const T eps, const unsigned minPts) {
    //cout << "Expand cluster" << endl;
    
    vector<int> seeds = regionQuery(points, points[pointIdx], eps);

    if (seeds.size() < minPts) {
        points[pointIdx].clusterId = noice; // Mark as noice
        //cout << "  Point idx as noice: " << pointIdx << endl;
        --clusterId; // No new cluster created, decrease previously incremented clusterId
        return false;
    }

    for_each(seeds.begin(), seeds.end(), [clusterId, &points](int idx) { points[idx].clusterId = clusterId; });

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
                //cout << "      Found core point with idx: " << currentP << " x: " << points[currentP].x << " y: " << points[currentP].y << endl;
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
template <Arithmetic T>
int dbscan(vector<Point<T>>& points, const T eps, int const minPts) {
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

template <Arithmetic T>
T convStr(const std::string& str) {
    if constexpr (std::is_same_v<T, int>) {
        return std::stoi(str);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::stod(str);
    } else if constexpr (std::is_same_v<T, long>) {
        return std::stol(str);
    } else if constexpr (std::is_same_v<T, float>) {
        return std::stof(str);
    } else {
        static_assert(std::is_arithmetic_v<T>, "Unsupported type for conversion");
    }
}

// Get sample data from CSV-file with semikolon as separator
template <Arithmetic T>
bool readCSV(const string& filename, vector<Point<T>>& points) {
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
            points.emplace_back(convStr<T>(xStr), convStr<T>(yStr));
        }
    }
    
    file.close();

    return true;
}

// Filter out points by clusterId
template <Arithmetic T>
vector<Point<T>> filterPointsByClusterId(const vector<Point<T>>& points, int targetClusterId) {
    vector<Point<T>> result;
    
    copy_if(points.begin(), points.end(), back_inserter(result), [targetClusterId](const Point<T>& p) { return p.clusterId == targetClusterId; });
    
    return result;
}

int main() {
    // Sample points
    vector<Point<double>> points = {
        Point( 1.0,  1.0),
        Point( 1.1,  1.1),
        Point( 0.9,  0.9),
        Point(10.0, 10.0),
        Point(10.1, 10.1),
        Point( 9.9,  9.9),
        Point( 5.0,  5.0)
    };

    // vector<Point<int>> points = {
    //     Point( 1,  1),
    //     Point( 2,  2),
    //     Point( 3,  3),
    //     Point(10, 10),
    //     Point(11, 11),
    //     Point( 9,  9),
    //     Point( 6,  6)
    // };

    if (!readCSV("./smile_face.csv", points))
       return -1;

    // Output of read sample points
    //for (const auto& p : points) {
    //    cout << "Point (x = " << p.x << ", y = " << p.y << ")" << endl;
    //}

    auto x = extractXCoordinates(points);
    auto y = extractYCoordinates(points);

    plt::figure_size(1000, 1000);
    plt::title("Sample points");
    plt::plot(x, y, "bo");

    // The DBSCAN parameter
    /*int*/ double eps = 2;
    unsigned minPts = 13;

    // Run the DBSCAN algorithm
    int maxClusterId = dbscan(points, eps, minPts);

    cout << endl;
    
    cout << "Found max. cluster: " << maxClusterId << endl;

    cout << endl;

    for (const auto& p : points) {
        cout << "Point (" << p.x << ", " << p.y << ") - Cluster ID: " << p.clusterId << endl;
    }

    cout << endl;

    for (int id = 0; id <= maxClusterId; ++id) {
        auto filteredPoints = filterPointsByClusterId(points, id);

        cout << "Cluster " << id << " size: " << filteredPoints.size() << endl;

        auto x = extractXCoordinates(filteredPoints);
        auto y = extractYCoordinates(filteredPoints);

        plt::figure_size(1000, 1000);
        plt::title(string("Cluster ID: ") + to_string(id));
        plt::plot(x, y, "ro");
    }

    plt::show();

    return 0;
}
