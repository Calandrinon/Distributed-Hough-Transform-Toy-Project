#include <iostream>
#include <cmath>
#include <vector>
#include <utility>
#include <queue>
#include <cstring>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#define BIN_WIDTH 1
#define NUM_BINS 180 / BIN_WIDTH 
// Canny edge detection parameters
#define KERNEL_SIZE 3
#define THRESHOLD 50
#define RATIO 3


void detectEdge(const cv::Mat& in, cv::Mat& out) {
	cv::blur(in, out, cv::Size(3, 3)); 
	cv::Canny(out, out, THRESHOLD, THRESHOLD*RATIO, KERNEL_SIZE);
}


std::ostream& operator<<(std::ostream& out, std::vector<std::vector<int>> v) {
	for (int i = 0; i < v.size(); ++i) {
		for (int j = 0; j < v[i].size(); ++j) {
			std::cout << v[i][j] << " ";
		}
		std::cout << "\n";
	}

	return out;
}


void polarToCartesian(double rho, int theta, cv::Point& p1, cv::Point& p2) {
	int x0 = cvRound(rho * cos(theta));
	int y0 = cvRound(rho * sin(theta));

	p1.x = cvRound(x0 + 1000 * (-sin(theta)));
	p1.y = cvRound(y0 + 1000 * (cos(theta)));

	p2.x = cvRound(x0 - 1000 * (-sin(theta)));
	p2.y = cvRound(y0 - 1000 * (cos(theta)));
}


void computeHoughTransform(int argc, char** argv) {
    int theta;      // angle for the line written in polar coordinates
    double rho;     // distance (radius) for the line written in polar coordinates

    cv::Mat source, edges, output;

    int lineThreshold = atoi(argv[2]);
    std::deque<std::pair<int, int>> edgePoints;  // <row, col>

    source = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    cv::namedWindow("source image", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);

    output = source.clone();
    cv::namedWindow("output image", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);

    int maxDistance = hypot(source.rows, source.cols);
    std::cout << maxDistance <<"\n";

    // voting matrix
    std::vector<std::vector<int>> votes(2 * maxDistance, std::vector<int>(NUM_BINS, 0));
    std::cout << votes.size() << ", " << votes[0].size() << "\n";

    // transforms the image such that only the edges are kept
    detectEdge(source, edges);
    cv::namedWindow("edge detection result", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);

    // vote
    for(int i = 0; i < edges.rows; ++i) {
        for(int j = 0; j < edges.cols; ++j) {
            if(edges.at<uchar>(i, j) == 255) {  // if we find an edge point
                // we go through all angles between -90 degrees and 90 degrees
                for(theta = 0; theta <= 180; theta += BIN_WIDTH) {
                    rho = round(j * cos(theta - 90) + i * sin(theta - 90)) + maxDistance;
                    votes[rho][theta]++;
                }
            }
        }
    }

    // find peaks
    for(int i = 0; i < votes.size(); ++i) {
        for(int j = 0; j < votes[i].size(); ++j) {
            if(votes[i][j] >= lineThreshold) {
                rho = i - maxDistance;
                theta = j - 90;
                std::cout << "found line with rho = " << rho << " and theta = " << theta << "\n";
                // converts the line from the polar coordinates to Cartesian coordinates
                cv::Point p1, p2;
                polarToCartesian(rho, theta, p1, p2);
                cv::line(output, p1, p2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
            }
        }
    }

    cv::Point dummy1(10, 10), dummy2(100, 100);
    cv::line(output, dummy1, dummy2, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
    cv::imshow("source image", source);
    cv::imshow("output image", output);
    cv::imshow("edge detection result", edges);

    cv::waitKey();
}


void computeHoughTransformMasterTask(int argc, char** argv, int numberOfProcesses) {
    int theta;      // angle for the line written in polar coordinates
    double rho;     // distance (radius) for the line written in polar coordinates

    cv::Mat source, edges, output;

    int lineThreshold = atoi(argv[2]);
    std::deque<std::pair<int, int>> edgePoints;  // <row, col>

    source = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    cv::namedWindow("source image", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);

    output = source.clone();
    cv::namedWindow("output image", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);

    int maxDistance = hypot(source.rows, source.cols);
    std::cout << maxDistance <<"\n";

    // voting matrix
    std::vector<std::vector<int>> votes(2 * maxDistance, std::vector<int>(NUM_BINS, 0));
    std::cout << votes.size() << ", " << votes[0].size() << "\n";

    // transforms the image such that only the edges are kept
    detectEdge(source, edges);
    cv::namedWindow("edge detection result", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);

    // vote
    for(int i = 0; i < edges.rows; ++i) {
        for(int j = 0; j < edges.cols; ++j) {
            if(edges.at<uchar>(i, j) == 255) {  // if we find an edge point
                // we go through all angles between -90 degrees and 90 degrees
                for(theta = 0; theta <= 180; theta += BIN_WIDTH) {
                    rho = round(j * cos(theta - 90) + i * sin(theta - 90)) + maxDistance;
                    votes[rho][theta]++;
                }
            }
        }
    }

    // find peaks
    for(int i = 0; i < votes.size(); ++i) {
        for(int j = 0; j < votes[i].size(); ++j) {
            if(votes[i][j] >= lineThreshold) {
                rho = i - maxDistance;
                theta = j - 90;
                std::cout << "found line with rho = " << rho << " and theta = " << theta << "\n";
                // converts the line from the polar coordinates to Cartesian coordinates
                cv::Point p1, p2;
                polarToCartesian(rho, theta, p1, p2);
                cv::line(output, p1, p2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
            }
        }
    }

    cv::Point dummy1(10, 10), dummy2(100, 100);
    cv::line(output, dummy1, dummy2, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
    cv::imshow("source image", source);
    cv::imshow("output image", output);
    cv::imshow("edge detection result", edges);

    cv::waitKey();
}


int main(int argc, char** argv) {
    if(argc < 3) {
        printf("USAGE: hough [filename] [threshold]\n");
        return 1;
    }

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (!rank) {

    } else {

    }

    return 0;
}

