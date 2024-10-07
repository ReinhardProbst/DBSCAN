/*
// Originally based on ChatGPT code creation
*/

#include <matplotlibcpp.h>
#include "dbscan.hpp"

namespace plt = matplotlibcpp;

int main() {
    DBSCAN Dbscan;

    // Sample points
    std::vector<DBSCAN::Point<double>> points = {
        DBSCAN::Point( 1.0,  1.0),
        DBSCAN::Point( 1.1,  1.1),
        DBSCAN::Point( 0.9,  0.9),
        DBSCAN::Point(10.0, 10.0),
        DBSCAN::Point(10.1, 10.1),
        DBSCAN::Point( 9.9,  9.9),
        DBSCAN::Point( 5.0,  5.0)
    };

    // vector<DBSCAN::Point<int>> points = {
    //     DBSCAN::Point( 1,  1),
    //     DBSCAN::Point( 2,  2),
    //     DBSCAN::Point( 3,  3),
    //     DBSCAN::Point(10, 10),
    //     DBSCAN::Point(11, 11),
    //     DBSCAN::Point( 9,  9),
    //     DBSCAN::Point( 6,  6)
    // };

    if (!Dbscan.readCSV("./smile_face.csv", points))
       return -1;

    // Output of read sample points
    //for (const auto& p : points) {
    //    cout << "Point (x = " << p.x << ", y = " << p.y << ")" << endl;
    //}

    auto x = Dbscan.extractXCoordinates(points);
    auto y = Dbscan.extractYCoordinates(points);

    plt::figure_size(1000, 1000);
    plt::title("Sample points");
    plt::plot(x, y, "bo");

    // The DBSCAN parameter
    /*int*/ double eps = 2;
    unsigned minPts = 13;

    // Run the DBSCAN algorithm
    int maxClusterId = Dbscan.dbscan(points, eps, minPts);

    std::cout << std::endl;
    
    std::cout << "Found max. cluster: " << maxClusterId << std::endl;

    std::cout << std::endl;

    for (const auto& p : points) {
        std::cout << "Point (" << p.x << ", " << p.y << ") - Cluster ID: " << p.clusterId << std::endl;
    }

    std::cout << std::endl;

    for (int id = 0; id <= maxClusterId; ++id) {
        auto filteredPoints = Dbscan.filterPointsByClusterId(points, id);

        std::cout << "Cluster " << id << " size: " << filteredPoints.size() << std::endl;

        auto x = Dbscan.extractXCoordinates(filteredPoints);
        auto y = Dbscan.extractYCoordinates(filteredPoints);

        plt::figure_size(1000, 1000);
        plt::title(std::string("Cluster ID: ") + std::to_string(id));
        plt::plot(x, y, "ro");
    }

    plt::show();

    return 0;
}
