g++ sequential_hough_transform.cpp -Wall -o sequential_hough -std=c++11 \
  -I /usr/include/opencv4/ \
  -L /usr/lib -lopencv_imgcodecs -lopencv_objdetect -lopencv_features2d -lopencv_imgproc -lopencv_highgui -lopencv_core

./sequential_hough images/sudoku_pencil_2.jpg 225
rm sequential_hough
