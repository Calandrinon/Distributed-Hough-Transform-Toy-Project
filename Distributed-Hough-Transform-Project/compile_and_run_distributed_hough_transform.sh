mpicxx -o distributed_hough distributed_hough_transform.cpp \
  -I /usr/include/opencv4/ \
  -L /usr/lib -lopencv_imgcodecs -lopencv_objdetect -lopencv_features2d -lopencv_imgproc -lopencv_highgui -lopencv_core

mpirun -np 4 ./distributed_hough $1 200
rm distributed_hough

