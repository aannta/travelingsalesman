This program demonstrates the processing of command-line arguments, the use of strings/arrays/files and a provided library to implement heuristic solutions to the traveling salesman problem (the travelling salesman problem asks the following question: "Given a list of cities and the distances between each pair of cities, what is the shortest possible route that visits each city and returns to the origin city?").

tsp.c & makefile are written by me. The remaining files are obtained from CS223 teaching materials. 

This program reads a filename and several arguments provided in the command line argument and returns a list of cities optimized with the heuristic methods called for in the command line. The file contains the number of cities, a list of 3-character codenames, and a list of corresponding latitude and longitude values.

The '-nearest' method calculates a route starting from the first city in the input file, appended with the next closest city in the list until all cities are visited, then back to the first city.
The '-insert nearst' or '-insert farthest' method first finds the closest/farthest pair of city to start with, and continuously insert the next nearest/farthest city into the existing tour where the total tour distance is minimized. The output is then reordered so that the optimized tour starts with the original first city in the input, and the second city in the input needs to come before the penultimate city in the input.

Where input is invalid, the program returns some error message to standard error.

Example:
Input in the file 8_cities.in:
8
SEA YVR MCI STL BWI DCA IAH DAL
47.450000 -122.311667
49.194722 -123.183889
39.297500 -94.713889
38.748611 -90.370000
39.175278 -76.668333
38.851944 -77.037778
29.984444 -95.341389
32.847222 -96.851667

Input from command line:
$ ./TSP 8_cities.in -nearest -insert nearest -insert farthest -optimal

Output:
-nearest        :  10067.11 SEA YVR MCI STL DAL IAH DCA BWI SEA
-insert nearest :   9320.21 SEA YVR MCI STL DCA BWI IAH DAL SEA
-insert farthest:   9306.22 SEA YVR MCI STL BWI DCA IAH DAL SEA
-optimal        :   9306.22 SEA YVR MCI STL BWI DCA IAH DAL SEA