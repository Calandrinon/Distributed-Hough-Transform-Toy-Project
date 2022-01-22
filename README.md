# Distributed-Hough-Transform-Toy-Project
A simple implementation of the Hough transform, with the voting part of the algorithm being distributed to 3 workers using MPI. It fits lines on an image based on the output of an edge detection algorithm, in this case the canny edge detection algorithm. 
More details about the algorithm can be found [here](https://en.wikipedia.org/wiki/Hough_transform).

# How to run

In order to run the algorithm, use one of the versions (either the sequential one or the distributed one) by executing the *compile_and_run_version_hough_transform.sh* script with an image as the first parameter to the script.

# Results

### Normal image

![Sudoku image](/Distributed-Hough-Transform-Project/images/sudoku_pencil_2.jpg)

### The image's Hough transform

![Hough transform](/Distributed-Hough-Transform-Project/images/markdown_images/hough_transform_sudoku.png)

### Execution time difference

![Execution time difference](/Distributed-Hough-Transform-Project/time_measurements.png)

