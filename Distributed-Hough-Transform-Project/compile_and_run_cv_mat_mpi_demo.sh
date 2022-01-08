mpicxx -o cv_mat_mpi_demo sending_cv_mat_through_mpi.cpp \
  -I /usr/include/opencv4/ \
  -L /usr/lib -lopencv_imgcodecs -lopencv_objdetect -lopencv_features2d -lopencv_imgproc -lopencv_highgui -lopencv_core

mpirun -n 2 ./cv_mat_mpi_demo
rm cv_mat_mpi_demo
