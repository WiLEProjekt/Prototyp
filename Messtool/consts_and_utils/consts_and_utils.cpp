#include "consts_and_utils.h"

int makeDirectoryForMeasurement(string dirpath)
{
        string mkdir = "sudo mkdir ";
        mkdir = mkdir + dirpath;
        system(mkdir.c_str());
        string rw_permission = "sudo chmod 777 "; // set rw permission to folder
        rw_permission = rw_permission + CLIENT_LOCATION;
        system(rw_permission.c_str());

        struct stat info;
        if( stat( dirpath.c_str(), &info ) != 0 )
        {
        //no access case
        return -1;
        } else if( info.st_mode & S_IFDIR )
        {
        // is directory
        return 0;
        } else
        {
        // no directory case
        return -1;
    }
}


char* stringToChar(string str)
{
    char* cstr = &str[0u];
    return cstr;
}

vector<string> split(const string& s, char delimiter)
{
        vector<string> tokens;
        string token;
        istringstream tokenStream(s);
        while (getline(tokenStream, token, delimiter))
        {
                tokens.push_back(token);
        }
        return tokens;
}

bool is_positiv_number(string str)
{
    regex db("([[:digit:]]+)(\\.(([[:digit:]]+)?))?");
    if(regex_match(str,db))
        return true;
    else
        return false;
}