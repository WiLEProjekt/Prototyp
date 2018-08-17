
/*
 * This header contains some constant values and util functions which are used in client and server to make live easier.
 */

#ifndef MESSTOOL_MEASUREMENT_CONSTS_H
#define MESSTOOL_MEASUREMENT_CONSTS_H

#include <string>
#include <cstring>
#include <sys/stat.h> // exist directory test
#include <vector>
#include <sstream> // split-function
#include <regex>

using namespace std;

#define PACKETSIZE 1440 // TODO: What value makes sense?
#define CLIENT_LOCATION "/home/caspar/Desktop/measurement_data_client"
#define SERVER_LOCATION "/home/caspar/Desktop/measurement_data_server"

/**
 * Create a directory at a specific path 'dirpath'.
 * @param dirpath the directory path
 * @return 0 if sucessfull, else -1
 */
int makeDirectoryForMeasurement(string dirpath);

/**
 * It's a bad joke that a standard string to char function (that does not result in 'const char')
 * does not exists.
 * @param str
 * @return
 */
char* stringToChar(string str);

/**
 * Splits a string at a defined delimiter and stores strings in a vector.
 * @param s the string which will be splitted by delimiter
 * @param delimiter the delimiter as a simple char
 * @return a vector containing a sequence of strings
 */
vector<string> split(const string& s, char delimiter);

/**
 * Assure that written a string a float point number.
 * @param str the string which will be checked
 * @return true if string resembles float point number, else false
 */
bool is_positiv_number(string str);

#endif //MESSTOOL_MEASUREMENT_CONSTS_H
