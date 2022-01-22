g++ sequential_hough_transform.cpp -Wall -o sequential_hough -std=c++11 \
  -I /usr/include/opencv4/ \
  -L /usr/lib -lopencv_imgcodecs -lopencv_objdetect -lopencv_features2d -lopencv_imgproc -lopencv_highgui -lopencv_core
