/**
 * Hilfstool zum Analysieren des Paketverlusts von einem gebebenen Binärtrace
 * Generiert mit unterschiedlichen Seeds des Zufallszahlengenerators ein Vergleichstrace
 * Gibt den Seed und den kleinsten quadratischen Abstand aus, mit dem die ECDF des generierten Traces mit der ECDF des original Traces am besten passt
 * Abstandsmaß: Methode der kleinsten Quadrate und Kolmogorov Abstand
 * @param Pfad zum Sage Script
 * @param Pfad zum Binärtrace
 */
#include <iostream>
#include <string>
#include <time.h>
#include <io.h>
#include <SmallestQuadrats.h>
#include <sage.h>
#include <paramCalculator.h>


using namespace std;

void usage(){
    cout << "usage: ./packetloss <path to sagemath python script> <path to binary file> <path to output folder>" << endl;
}

int main(int argc, char* argv[]){
    if (argc < 4) {
        usage();
        return 0;
    } else {
        clock_t start = clock();
        string sagescript = argv[1];
        string pathToGivenTrace = argv[2];
        string outputPath = argv[3];

        cout << "Processing Inputtrace" << endl;
        vector<bool> originalTrace = readBinaryTrace(pathToGivenTrace);
        vector<int> origSizes;
        calcLoss(originalTrace, origSizes);
        vector<vector<float> > origECDF;
        calculateECDF(origSizes, origECDF);

        cout << "Calculating Bernoulli Model-Parameter" << endl;
        double pBernoulli, rBernoulli;
        getBernoulliParams(originalTrace, pBernoulli);
        rBernoulli = 1.0 - pBernoulli;

        cout << "Calculating Simple-Gilbert Model-Parameter" << endl;
        double pSimpleGilbert,rSimpleGilbert;
        getSimpleGilbertParams(originalTrace, pSimpleGilbert, rSimpleGilbert);

        cout << "Calculating Gilbert Model-Parameter" << endl;
        double pGilbert, rGilbert, kGilbert, hGilbert;
        string pythonCommand = "sage --python " + sagescript + " " + pathToGivenTrace + " gilbert";
        int sageStatus = system(pythonCommand.c_str());
        if (sageStatus < 0) {
            cout << "An Error occured while executing Sagemath" << endl;
            return 0;
        }
        vector<vector<double>> transitionmatrixGilbert;
        vector<vector<double>> emissionmatrixGilbert;
        vector<double> initialprobabilitiesGilbert;
        readMatrixFile(transitionmatrixGilbert, emissionmatrixGilbert, initialprobabilitiesGilbert);
        pGilbert = transitionmatrixGilbert[0][1];
        rGilbert = transitionmatrixGilbert[1][0];
        kGilbert = emissionmatrixGilbert[0][1];
        hGilbert = emissionmatrixGilbert[1][1];

        cout << "Calculating Gilbert-Elliot Model-Parameter" << endl;
        double pGilbertElliot, rGilbertElliot, kGilbertElliot, hGilbertElliot;
        pythonCommand = "sage --python " + sagescript + " " + pathToGivenTrace + " gilbertelliot";
        sageStatus = system(pythonCommand.c_str());
        if (sageStatus < 0) {
            cout << "An Error occured while executing Sagemath" << endl;
            return 0;
        }
        vector<vector<double>> transitionmatrixGilbertElliot;
        vector<vector<double>> emissionmatrixGilbertElliot;
        vector<double> initialprobabilitiesGilbertElliot;
        readMatrixFile(transitionmatrixGilbertElliot, emissionmatrixGilbertElliot, initialprobabilitiesGilbertElliot);
        pGilbertElliot = transitionmatrixGilbertElliot[0][1];
        rGilbertElliot = transitionmatrixGilbertElliot[1][0];
        kGilbertElliot = emissionmatrixGilbertElliot[0][1];
        hGilbertElliot = emissionmatrixGilbertElliot[1][1];

        cout << "Calculating Markov Model-Parameter" << endl;
        double p13, p31, p32, p23, p14, p41;
        pythonCommand = "sage --python " + sagescript + " " + pathToGivenTrace + " markov";
        sageStatus = system(pythonCommand.c_str());
        if (sageStatus < 0) {
            cout << "An Error occured while executing Sagemath" << endl;
            return 0;
        }
        vector<vector<double>> transitionmatrixMarkov;
        vector<vector<double>> emissionmatrixMarkov;
        vector<double> initialprobabilitiesMarkov;
        readMatrixFile(transitionmatrixMarkov, emissionmatrixMarkov, initialprobabilitiesMarkov);
        p13 = transitionmatrixMarkov[0][2];
        p31 = transitionmatrixMarkov[2][0];
        p32 = transitionmatrixMarkov[2][1];
        p23 = transitionmatrixMarkov[1][2];
        p14 = transitionmatrixMarkov[0][3];
        p41 = transitionmatrixMarkov[3][0];

        cout << "Fitting Bernoulli" << endl;
        long tracesize = originalTrace.size();
        vector<int> bernoulliBursts;
        int seedBernoulli = fitGilbert(tracesize, origECDF, pBernoulli, rBernoulli, 1.0, 0.0, bernoulliBursts);
        writeBursts(outputPath+"/FittedBernoulli.txt", bernoulliBursts);

        cout << "Fitting Simple-Gilbert" << endl;
        vector<int> simpleGilbertBursts;
        int seedSimpleGilbert = fitGilbert(tracesize, origECDF, pSimpleGilbert, rSimpleGilbert, 1.0, 0.0, simpleGilbertBursts);
        writeBursts(outputPath+"/FittedSimpleGilbert.txt", simpleGilbertBursts);

        cout << "Fitting Gilbert" << endl;
        vector<int> gilbertBursts;
        int seedGilbert = fitGilbert(tracesize, origECDF, pGilbert, rGilbert, kGilbert, hGilbert, gilbertBursts);
        writeBursts(outputPath+"/FittedGilbert.txt", gilbertBursts);

        cout << "Fitting Gilbert-Elliot" << endl;
        vector<int> gilbertElliotBursts;
        int seedGilbertElliot = fitGilbert(tracesize, origECDF, pGilbertElliot, rGilbertElliot, kGilbertElliot, hGilbertElliot, gilbertElliotBursts);
        writeBursts(outputPath+"/FittedGilbertElliot.txt", gilbertElliotBursts);

        cout << "Fitting Markov" << endl;
        vector<int> markovBursts;
        int seedMarkov = fitMarkov(tracesize, origECDF, p13, p31, p32, p23, p14, markovBursts);
        writeBursts(outputPath+"/FittedMarkov.txt", markovBursts);

        clock_t stop = clock();
        double elapsed = (double) (stop-start)/CLOCKS_PER_SEC;
        cout << elapsed << "s" << endl;
    }

}