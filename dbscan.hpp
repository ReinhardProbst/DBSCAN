/*
// Originally based on ChatGPT code creation
*/

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <concepts>

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

struct DBSCAN {
    DBSCAN() = default;

    // Used for marking clusters
    static constexpr int unexplored = -1;
    static constexpr int noice = 0;

    template <Arithmetic T>
    struct Point {
        T x, y;
        int clusterId; // "unexplored", "noice", 1.. means valid clusterID
        bool visited;  // "false", "true"

        Point(T x = 0, T y = 0) : x(x), y(y), clusterId(unexplored), visited(false) {}
    };

    // Get vector of X coordinates from vector of points
    template <Arithmetic T>
    std::vector<T> extractXCoordinates(const std::vector<Point<T>>& points) {
        std::vector<T> xCoordinates;
        
        std::transform(points.begin(), points.end(), std::back_inserter(xCoordinates), [](const Point<T>& p) { return p.x; });
        
        return xCoordinates;
    }

    // Get vector of Y coordinates from vector of points
    template <Arithmetic T>
    std::vector<T> extractYCoordinates(const std::vector<Point<T>>& points) {
        std::vector<T> yCoordinates;
        
        std::transform(points.begin(), points.end(), std::back_inserter(yCoordinates), [](const Point<T>& p) { return p.y; });
        
        return yCoordinates;
    }

    // Euclidean distance
    template <Arithmetic T>
    T distance(const Point<T>& a, const Point<T>& b) {
        return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
    }

    // Manhattan distance
    template <Arithmetic T>
    T distanceMan(const Point<T>& a, const Point<T>& b) {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y);
    }

    // Find all neighbors within epsilon distance
    template <Arithmetic T>
    std::vector<int> regionQuery(const std::vector<Point<T>>& points, const Point<T>& p, T eps) {
        std::vector<int> neighbors;

        for (unsigned i = 0; i < points.size(); ++i) {
            if (distanceMan(p, points[i]) <= eps) { // Select Euclidean or Manhattan distance
                neighbors.push_back(i);
            }
        }

        return neighbors;
    }

    // Expand cluster
    template <Arithmetic T>
    bool expandCluster(std::vector<Point<T>>& points, const int pointIdx, int& clusterId, const T eps, const unsigned minPts) {
        //cout << "Expand cluster" << endl;
        
        std::vector<int> seeds = regionQuery(points, points[pointIdx], eps);

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
                std::vector<int> result = regionQuery(points, points[currentP], eps);

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
    int dbscan(std::vector<Point<T>>& points, const T eps, int const minPts) {
        int clusterId = 0;
        for (unsigned i = 0; i < points.size(); ++i) {
            if (!points[i].visited) {
                points[i].visited = true;

                if (expandCluster(points, i, ++clusterId, eps, minPts)) {
                    std::cout << "New cluster found and expanded: " << clusterId << std::endl;
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
    bool readCSV(const std::string& filename, std::vector<Point<T>>& points) {
        std::ifstream file(filename);
        std::string line;

        points.clear();

        if (!file.is_open()) {
            std::cerr << "Error with: " << filename << std::endl;
            return false;
        }

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string xStr, yStr;
            
            if (getline(ss, xStr, ';') && std::getline(ss, yStr, ';')) {
                points.emplace_back(convStr<T>(xStr), convStr<T>(yStr));
            }
        }
        
        file.close();

        return true;
    }

    // Filter out points by clusterId
    template <Arithmetic T>
    std::vector<Point<T>> filterPointsByClusterId(const std::vector<Point<T>>& points, int targetClusterId) {
        std::vector<Point<T>> result;
        
        copy_if(points.begin(), points.end(), std::back_inserter(result), [targetClusterId](const Point<T>& p) { return p.clusterId == targetClusterId; });
        
        return result;
    }
};


















