#include <iostream>
#include <cmath>
#include <vector>
#include <utility>
#include <queue>
#include <cstring>
#include <unistd.h>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "mpi.h"

#define BIN_WIDTH 1
#define NUM_BINS 180 / BIN_WIDTH 
// Canny edge detection parameters
#define KERNEL_SIZE 3
#define THRESHOLD 50
#define RATIO 3


enum MpiHoughTag {
	MPI_EDGE_MATRIX_INTERVAL_START,
	MPI_EDGE_MATRIX_INTERVAL_END,
	MPI_EDGE_MATRIX,
	MPI_MAX_DISTANCE,
	MPI_VOTE_MATRIX
};


const int MAXBYTES=8*1024*1024;
uchar buffer[MAXBYTES];

void sendMatrixThroughMPI(cv::Mat m, int destination){
	int rows  = m.rows;
	int cols  = m.cols;
	int type  = m.type();
	int channels = m.channels();
	memcpy(&buffer[0 * sizeof(int)],(uchar*)&rows,sizeof(int));
	memcpy(&buffer[1 * sizeof(int)],(uchar*)&cols,sizeof(int));
	memcpy(&buffer[2 * sizeof(int)],(uchar*)&type,sizeof(int));

	int bytesPerSample=1; // change if using shorts or floats
	int bytes=m.rows*m.cols*channels*bytesPerSample;

	if(!m.isContinuous())
 		m = m.clone();
	memcpy(&buffer[3*sizeof(int)],m.data,bytes);
	MPI_Send(&buffer, bytes+3*sizeof(int), MPI_UNSIGNED_CHAR, destination, MPI_EDGE_MATRIX, MPI_COMM_WORLD);
}

cv::Mat receiveMatrixThroughMPI(int source){
	MPI_Status status;
	int count,rows,cols,type,channels;

	MPI_Recv(&buffer, sizeof(buffer), MPI_UNSIGNED_CHAR, source, MPI_EDGE_MATRIX, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status,MPI_UNSIGNED_CHAR,&count);
	memcpy((uchar*)&rows,&buffer[0 * sizeof(int)], sizeof(int));
	memcpy((uchar*)&cols,&buffer[1 * sizeof(int)], sizeof(int));
	memcpy((uchar*)&type,&buffer[2 * sizeof(int)], sizeof(int));

	cv::Mat received = cv::Mat(rows,cols,type,(uchar*)&buffer[3*sizeof(int)]);
	return received;
}


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


void computeHoughTransformMasterTask(int argc, char** argv, int numberOfProcesses) {
	auto start = std::chrono::steady_clock::now();

    int theta;      // angle for the line written in polar coordinates
    double rho;     // distance (radius) for the line written in polar coordinates

    cv::Mat source, edges, output;

    int lineThreshold = atoi(argv[2]);

    source = cv::imread(argv[1], cv::IMREAD_GRAYSCALE);
    cv::namedWindow("source image", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);

    output = source.clone();
    cv::namedWindow("output image", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);

    int maxDistance = hypot(source.rows, source.cols);

    // voting matrix
	int votes[2 * maxDistance][NUM_BINS]; 
	int receivedVotes[2 * maxDistance][NUM_BINS]; 
	std::memset(votes, 0, sizeof votes);
	std::memset(votes, 0, sizeof receivedVotes);

    // transforms the image such that only the edges are kept
    detectEdge(source, edges);
    cv::namedWindow("edge detection result", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);

	int edgeMatrixRowsPerProcess = (int)(edges.rows / (numberOfProcesses - 1));

    // vote
	int processIndex = 0;
    for (int i = 0; i < edges.rows; i += edgeMatrixRowsPerProcess) {
		processIndex++;
		int intervalStart = i, intervalEnd = i + edgeMatrixRowsPerProcess;
		printf("Master sends intervalStart to %d...\n", processIndex);	
		MPI_Send(&intervalStart, 1, MPI_INT, processIndex, MPI_EDGE_MATRIX_INTERVAL_START, MPI_COMM_WORLD);
		printf("Master sends intervalEnd to %d...\n", processIndex);	
		MPI_Send(&intervalEnd, 1, MPI_INT, processIndex, MPI_EDGE_MATRIX_INTERVAL_END, MPI_COMM_WORLD);		
		printf("Master sends maxDistance to %d...\n", processIndex);	
		MPI_Send(&maxDistance, 1, MPI_INT, processIndex, MPI_MAX_DISTANCE, MPI_COMM_WORLD);		
		printf("Master sends matrix to %d...\n", processIndex);	
		sendMatrixThroughMPI(edges, processIndex);
    }

	for (processIndex = 1; processIndex < numberOfProcesses; processIndex++) {		
		printf("Master receives votes from %d...\n", processIndex);	
		MPI_Recv(receivedVotes, 2 * maxDistance * NUM_BINS, MPI_INT, processIndex, MPI_VOTE_MATRIX, MPI_COMM_WORLD, NULL);	
		printf("Master received votes from %d...\n", processIndex);	
		for (int row = 0; row < 2 * maxDistance; row++) {
			for (int column = 0; column < NUM_BINS; column++)
				votes[row][column] += receivedVotes[row][column];
		}
	}

	// find peaks
    for (int i = 0; i < 2 * maxDistance; ++i) {
        for (int j = 0; j < NUM_BINS; ++j) {
            if (votes[i][j] >= lineThreshold) {
                rho = i - maxDistance;
                theta = j - 90;
                // converts the line from the polar coordinates to Cartesian coordinates
                cv::Point p1, p2;
				polarToCartesian(rho, theta, p1, p2);
                cv::line(output, p1, p2, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
            }
        }
    }

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "Distributed Hough transform - elapsed time: " << elapsed_seconds.count() << " seconds\n";

    cv::Point dummy1(10, 10), dummy2(100, 100);
    cv::line(output, dummy1, dummy2, cv::Scalar(0, 0, 255), 1, cv::LINE_AA);
    cv::imshow("source image", source);
    cv::imshow("output image", output);
    cv::imshow("edge detection result", edges);

    cv::waitKey();
}


void computeHoughTransformWorkerTask(int rank, int numberOfProcesses, char* argv[]) {		
	int intervalStart, intervalEnd, maxDistance;
	MPI_Status status;
	MPI_Recv(&intervalStart, 1, MPI_INT, 0, MPI_EDGE_MATRIX_INTERVAL_START, MPI_COMM_WORLD, &status);
	printf("Worker receives intervalStart: %d\n", intervalStart);	
	MPI_Recv(&intervalEnd, 1, MPI_INT, 0, MPI_EDGE_MATRIX_INTERVAL_END, MPI_COMM_WORLD, &status);		
	printf("Worker receives intervalEnd...%d\n", intervalEnd);	
	MPI_Recv(&maxDistance, 1, MPI_INT, 0, MPI_MAX_DISTANCE, MPI_COMM_WORLD, &status);	
	printf("Worker receives maxDistance: %d\n", maxDistance);	
	cv::Mat edges = receiveMatrixThroughMPI(0);
	printf("Worker receives matrix\n");	

    int lineThreshold = atoi(argv[2]);
	int votes[2 * maxDistance][NUM_BINS]; 
	std::memset(votes, 0, sizeof votes);

	// vote
    for (int i = intervalStart; i < intervalEnd; ++i) {
        for (int j = 0; j < edges.cols; ++j) {
            if (edges.at<uchar>(i, j) == 255) {  // if we find an edge point
                // we go through all angles between -90 degrees and 90 degrees
                for (int theta = 0; theta <= 180; theta += BIN_WIDTH) {
                    int rho = (int)round(j * cos(theta - 90) + i * sin(theta - 90)) + maxDistance;
                    votes[rho][theta]++;
                }
            }
        }
    }

	// send vote matrix back to the master, so that the master can update the cells of 
	// the old matrix where the values are smaller than the ones in the sent matrix 	
	printf("Worker sending vote matrix\n");	
	MPI_Send(votes, 2 * maxDistance * NUM_BINS, MPI_INT, 0, MPI_VOTE_MATRIX, MPI_COMM_WORLD);
	printf("Worker sent vote matrix\n");	
}


int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage: hough [filename] [threshold]\n");
        return 1;
    }
	printf("\n");
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (!rank) {
		computeHoughTransformMasterTask(argc, argv, size);	
    } else {	
		computeHoughTransformWorkerTask(rank, size, argv);	
    }

	MPI_Finalize();

    return 0;
}

