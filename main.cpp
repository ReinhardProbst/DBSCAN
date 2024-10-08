#include <matplotlibcpp.h>

#include <boost/timer/timer.hpp>

#include "dbscan.hpp"

namespace plt = matplotlibcpp;

// Choose the number type, implemented are float, double, int or long
using number_t = double;

int main() {
    DBSCAN Dbscan;

    std::vector<DBSCAN::Point<number_t>> points;

    // // Sample points
    // std::vector<DBSCAN::Point<number_t>> points = {
    //     DBSCAN::Point( 1.0,  1.0),
    //     DBSCAN::Point( 1.1,  1.1),
    //     DBSCAN::Point( 0.9,  0.9),
    //     DBSCAN::Point(10.0, 10.0),
    //     DBSCAN::Point(10.1, 10.1),
    //     DBSCAN::Point( 9.9,  9.9),
    //     DBSCAN::Point( 5.0,  5.0)
    // };

    // std::vector<DBSCAN::Point<number_t>> points = {
    //     DBSCAN::Point( 1,  1),
    //     DBSCAN::Point( 2,  2),
    //     DBSCAN::Point( 3,  3),
    //     DBSCAN::Point(10, 10),
    //     DBSCAN::Point(11, 11),
    //     DBSCAN::Point( 9,  9),
    //     DBSCAN::Point( 6,  6)
    // };

    std::string fn = "";
    number_t eps = 0;
    unsigned minPts = 0; 

    if constexpr (std::is_same_v<number_t, int> || std::is_same_v<number_t, long>) {
        fn = "./smile_face_int.csv";
        // The DBSCAN parameter
        eps = 15;
        minPts = 13; 
    } else if constexpr (std::is_same_v<number_t, float> || std::is_same_v<number_t, double>) {
        fn = "./smile_face_double.csv";
        // The DBSCAN parameter
        eps = 2.0;
        minPts = 13; 
    } else {
        std::cout << "Unsupported data type for DBSCAN algorithm" << std::endl;
        return -1;
    }

    if (!Dbscan.readCSV(fn, points))
       return -1;

    // Output of read sample points
    //for (const auto& p : points) {
    //    std::cout << "Point (x = " << p.x << ", y = " << p.y << ")" << std::endl;
    //}

    auto x = Dbscan.extractXCoordinates(points);
    auto y = Dbscan.extractYCoordinates(points);

    plt::figure_size(1000, 1000);
    plt::title("Sample points");
    plt::plot(x, y, "bo");

    // Run the DBSCAN algorithm incl. benchmark
    boost::timer::auto_cpu_timer ac;
    int maxClusterId = Dbscan.dbscan(points, eps, minPts);
    ac.report();

    std::cout << std::endl;
    
    std::cout << "Found max. cluster: " << maxClusterId << std::endl;

    std::cout << std::endl;

    // for (const auto& p : points) {
    //     std::cout << "Point (" << p.x << ", " << p.y << ") - Cluster ID: " << p.clusterId << std::endl;
    // }

    // std::cout << std::endl;

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
