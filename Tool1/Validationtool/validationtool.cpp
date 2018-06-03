//
// Reads a binary trace, extracts its' packetlossrate, calculates burst sizes, writes them to a file
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
int main(int argc, char** argv){
    if(argc < 3){
        cout << "usage: ./validationtool [inputfilename] [outputfilename]" << endl;
        return -1;
    }
    string input = argv[1];
    string output = argv[2];
    fstream fileStream;
    fileStream.open(input, ios::in);
    string text;
    fileStream >> text;
    fileStream.close();
    cout << text.size() << endl;

    int losscounter = 0;
    int receivedcounter = 0;
    int temp = 0;
    vector<int> trace;
    //calculate burst sizes
    for(unsigned int i = 0; i<text.size(); i++){
        if(i > 0){
            if(text[i] == '0' && text[i-1] == '0'){
                losscounter++;
                temp--;
            }else if(text[i] == '1' && text[i-1]=='1'){
                receivedcounter++;
                temp++;
            }else{
                trace.push_back(temp);
                temp = 0;
                if(text[i] == '0'){
                    losscounter++;
                    temp--;
                }
                else if(text[i] == '1') {
                    receivedcounter++;
                    temp++;
                }
            }
        }else{
            if(text[i] == '0'){
                losscounter++;
                temp--;
            }
            else if(text[i] == '1'){
                receivedcounter++;
                temp++;
            }
        }

    }
    trace.push_back(temp);
    float packetloss = (float)losscounter/(losscounter+receivedcounter)*100;
    cout << "packets lost: " << losscounter << endl;
    cout << "packets received: " << receivedcounter << endl;
    cout << "packetloss: " << packetloss << endl;

    //write burst sizes to file
    fstream fout;
    fout.open(output, ios::out);
    for(int i = 0; i<trace.size(); i++){
        fout << trace[i];
        fout << "\n";
    }
    fout.close();
    return 0;
}