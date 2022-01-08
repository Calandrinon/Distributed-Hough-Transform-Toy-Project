#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "mpi.h"

using namespace std;
using namespace cv;

const int MAXBYTES=8*1024*1024;
uchar buffer[MAXBYTES];

void matsnd(Mat m, int dest){
	int rows  = m.rows;
	int cols  = m.cols;
	int type  = m.type();
	int channels = m.channels();
	memcpy(&buffer[0 * sizeof(int)],(uchar*)&rows,sizeof(int));
	memcpy(&buffer[1 * sizeof(int)],(uchar*)&cols,sizeof(int));
	memcpy(&buffer[2 * sizeof(int)],(uchar*)&type,sizeof(int));

	int bytespersample=1; // change if using shorts or floats
	int bytes=m.rows*m.cols*channels*bytespersample;
	std::cout << "matsnd: rows=" << rows << endl;
	std::cout << "matsnd: cols=" << cols << endl;
	std::cout << "matsnd: type=" << type << endl;
	std::cout << "matsnd: channels=" << channels << endl;
	std::cout << "matsnd: bytes=" << bytes << endl;

	if(!m.isContinuous())
 		m = m.clone();
	memcpy(&buffer[3*sizeof(int)],m.data,bytes);
	MPI_Send(&buffer,bytes+3*sizeof(int),MPI_UNSIGNED_CHAR,dest,0,MPI_COMM_WORLD);
}

Mat matrcv(int src){
	MPI_Status status;
	int count,rows,cols,type,channels;

	MPI_Recv(&buffer,sizeof(buffer),MPI_UNSIGNED_CHAR,src,0,MPI_COMM_WORLD,&status);
	MPI_Get_count(&status,MPI_UNSIGNED_CHAR,&count);
	memcpy((uchar*)&rows,&buffer[0 * sizeof(int)], sizeof(int));
	memcpy((uchar*)&cols,&buffer[1 * sizeof(int)], sizeof(int));
	memcpy((uchar*)&type,&buffer[2 * sizeof(int)], sizeof(int));

	std::cout << "matrcv: Count=" << count << endl;
	std::cout << "matrcv: rows=" << rows << endl;
	std::cout << "matrcv: cols=" << cols << endl;
	std::cout << "matrcv: type=" << type << endl;

	Mat received = Mat(rows,cols,type,(uchar*)&buffer[3*sizeof(int)]);
	return received;
}

int main ( int argc, char *argv[] ) {
	int rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		Mat received = matrcv(1);
		imwrite("received_sudoku_pencil_2.jpg",received);
	} else {
		Mat image = imread("images/sudoku_pencil_2.jpg",IMREAD_COLOR);
		matsnd(image, 0);
	}

	MPI_Finalize();
}
