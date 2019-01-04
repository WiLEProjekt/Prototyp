/**
 * Hilfstool zum Analysieren des Paketverlusts von einem gebebenen Binärtrace
 * Generiert mit unterschiedlichen Seeds des Zufallszahlengenerators Ein Vergleichstrace
 * Gibt den Seed zurück, mit dem die ECDF des generierten Traces mit der ECDF des original Traces am besten passt
 * Abstandsmaß: Methode der kleinsten Quadrate
 * @param Pfad zum gegebenen Binärtrace
 * @param Modellname und dann die Modellparameter Bernoulli | SimpleGilbert | Gilbert | GilbertElliot | Markov
 */
#include <iostream>
#include <string>
#include <Generator.h>
#include <io.h>
#include <SmallestQuadrats.h>

using namespace std;

void usage(){
    cout << "usage: ./packetloss <path to binary file> <model> <list of modelparameter>" << endl;
    cout << "Models:" << endl;
    cout << "\tGilbertElliot\t <p [0-1]> <r [0-1]> <k [0-1]> <h [0-1]>" << endl
                                    << "\tGilbert\t\t\t <p [0-1]> <r [0-1]> <h [0-1]>" << endl
                                    << "\tSimpleGilbert\t <p [0-1]> <r [0-1]" << endl
                                    << "\tBernoulli\t\t <p [0-1]>" << endl
                                    << "\tMarkov\t\t\t <p13> <p31> <p32> <p23> <p14>" << endl;
}

void wrongArguments(){
    cout << "Wrong Arguments given" << endl;
}

int main(int argc, char* argv[]){
    if(argc < 4) {
        usage();
    }else{
        string pathToGivenTrace = argv[1];
        string model = argv[2];
        float p, r, k, h, p13, p31, p32, p23, p14;
        if(model == "Bernoulli"){
            if(argc != 4){
                wrongArguments();
                return -1;
            }else{
                p = stof(argv[3]);

            }
        }else if(model == "SimpleGilbert"){
            if(argc != 5){
                wrongArguments();
                return -1;
            }else{
                p = stof(argv[3]);
                r = stof(argv[4]);
            }
        }else if(model == "Gilbert"){
            if(argc != 6){
                wrongArguments();
                return -1;
            }else{
                p = stof(argv[3]);
                r = stof(argv[4]);
                h = stof(argv[5]);
            }
        }else if(model == "GilbertElliot"){
            if(argc != 7){
                wrongArguments();
                return -1;
            }else{
                p = stof(argv[3]);
                r = stof(argv[4]);
                k = stof(argv[5]);
                h = stof(argv[6]);
            }
        }else if(model == "Markov"){
            if(argc != 8){
                wrongArguments();
                return -1;
            }else{
                p13 = stof(argv[3]);
                p31 = stof(argv[4]);
                p32 = stof(argv[5]);
                p23 = stof(argv[6]);
                p14 = stof(argv[7]);
            }
        }else{
            wrongArguments();
            return -1;
        }
        vector<bool> originalTrace = readBinaryTrace(pathToGivenTrace);
        vector<int> origSizes;
        calcLoss(originalTrace, origSizes);
        vector<vector<float> > origECDF;
        calculateECDF(origSizes, origECDF);
    }

}