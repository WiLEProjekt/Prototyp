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
#include <statistsics.h>
#include <sage.h>
#include <paramCalculator.h>


using namespace std;

const string metrics[2] = {"Kolmogorov", "LeastSquared"};

void usage(){
    cout << "usage: ./packetloss <path to sagemath python script> <path to binary file> <path to output folder> <significanceniveau [0-100]>" << endl;
}

//void writeName(string output, string modelname){

//}

void writeHeader(string output){
    fstream fout;
    fout.open(output, ios::out);
    fout << "Modelname" << ";" << "Metric" << ";" <<  "Best Seed" << ";" << "Best Distance" << ";" << "Confidence Interval lower bound" << ";" << "Confidence Interval upper bound" << endl;
    fout.close();
}

void consolePrint(string output, string model, string metric, int bestSeed, double bestDistance, vector<double> &confInterval){
    cout << "\t" << metric << ":\tBest Seed: " << bestSeed << "\tWith Distance: " << bestDistance << "\tConfidence interval: " << confInterval[0] << "; " << confInterval[1] << endl;
    fstream fout;
    fout.open(output, ios::out | ios::app);
    fout << model << ";" << metric << ";" <<  bestSeed << ";" << bestDistance << ";" << confInterval[0] << ";" << confInterval[1] << endl;
    fout.close();
}

int main(int argc, char* argv[]){
    if (argc < 5) {
        usage();
        return 0;
    } else {

        clock_t start = clock();
        string sagescript = argv[1];
        string pathToGivenTrace = argv[2];
        string outputPath = argv[3];
        int significanceniveau = stoi(argv[4]);
        string resultFilename = outputPath+"/FittingResults.txt";
        writeHeader(resultFilename);
        cout << "Processing Inputtrace" << endl;
        vector<bool> originalTrace = readBinaryTrace(pathToGivenTrace);
        vector<int> origSizes;
        calcLoss(originalTrace, origSizes);
        vector<vector<float> > origECDF;
        calculateECDF(origSizes, origECDF);
        writeVector(outputPath+"/OriginalECDF.txt", origSizes);

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
        vector<int> bernoulliBurstsKolmogorov, bernoulliBurstsLeastSquared;
        vector<double> bernoulliKolmogorovDistances, bernoulliLeastSquaresDistances;
        int bernoulliSeedKolmogorov, bernoulliSeedLeastSquared;
        double bernoulliMinKolmogorovDistance, bernoulliMinLeastSquaredDifference;
        fitGilbert(tracesize, origECDF, pBernoulli, rBernoulli, 1.0, 0.0, bernoulliBurstsKolmogorov, bernoulliSeedKolmogorov, bernoulliMinKolmogorovDistance, bernoulliBurstsLeastSquared, bernoulliSeedLeastSquared, bernoulliMinLeastSquaredDifference, bernoulliKolmogorovDistances, bernoulliLeastSquaresDistances);
        vector<double> bernoulliConfidenceKolmogorov = calcConfidenceIntervall(bernoulliKolmogorovDistances, significanceniveau);
        vector<double> bernoulliConfidenceLeastSquared = calcConfidenceIntervall(bernoulliLeastSquaresDistances, significanceniveau);
        writeVector(outputPath+"/BernoulliECDFKolmogorov.txt", bernoulliBurstsKolmogorov);
        writeVector(outputPath+"/BernoulliECDFLeastSquared.txt", bernoulliBurstsLeastSquared);
        writeVector(outputPath+"/BernoulliKolmogorovDistances.txt", bernoulliKolmogorovDistances);
        writeVector(outputPath+"/BernoulliLeastSquaredDistances.txt", bernoulliLeastSquaresDistances);
        cout << "Bernoulli:" << endl;
        consolePrint(resultFilename, "Bernoulli", metrics[0], bernoulliSeedKolmogorov, bernoulliMinKolmogorovDistance, bernoulliConfidenceKolmogorov);
        consolePrint(resultFilename, "Bernoulli", metrics[1], bernoulliSeedLeastSquared, bernoulliMinLeastSquaredDifference, bernoulliConfidenceLeastSquared);


        cout << "Fitting Simple-Gilbert" << endl;
        vector<int> simpleGilbertBurstsKolmogorov, simpleGilbertBurstsLeastSquared;
        vector<double> simpleGilbertKolmogorovDistances, simpleGilbertLeastSquaresDistances;
        int simpleGilbertSeedKolmogorov, simpleGilbertSeedLeastSquared;
        double simpleGilbertMinKolmogorovDistance, simpleGilbertMinLeastSquaredDifference;
        fitGilbert(tracesize, origECDF, pSimpleGilbert, rSimpleGilbert, 1.0, 0.0, simpleGilbertBurstsKolmogorov, simpleGilbertSeedKolmogorov, simpleGilbertMinKolmogorovDistance, simpleGilbertBurstsLeastSquared, simpleGilbertSeedLeastSquared, simpleGilbertMinLeastSquaredDifference, simpleGilbertKolmogorovDistances, simpleGilbertLeastSquaresDistances);
        vector<double> simpleGilbertConfidenceKolmogorov = calcConfidenceIntervall(simpleGilbertKolmogorovDistances, significanceniveau);
        vector<double> simpleGilbertConfidenceLeastSquared = calcConfidenceIntervall(simpleGilbertLeastSquaresDistances, significanceniveau);
        writeVector(outputPath+"/SimpleGilbertECDFKolmogorov.txt", simpleGilbertBurstsKolmogorov);
        writeVector(outputPath+"/SimpleGilbertECDFLeastSquared.txt", simpleGilbertBurstsLeastSquared);
        writeVector(outputPath+"/SimpleGilbertKolmogorovDistances.txt", simpleGilbertKolmogorovDistances);
        writeVector(outputPath+"/SimpleGilbertLeastSquaredDistances.txt", simpleGilbertLeastSquaresDistances);
        cout << "Simple-Gilbert:" << endl;
        consolePrint(resultFilename, "SimpleGilbert", metrics[0], simpleGilbertSeedKolmogorov, simpleGilbertMinKolmogorovDistance, simpleGilbertConfidenceKolmogorov);
        consolePrint(resultFilename, "SimpleGilbert", metrics[1], simpleGilbertSeedLeastSquared, simpleGilbertMinLeastSquaredDifference, simpleGilbertConfidenceLeastSquared);


        cout << "Fitting Gilbert" << endl;
        vector<int> GilbertBurstsKolmogorov, GilbertBurstsLeastSquared;
        vector<double> GilbertKolmogorovDistances, GilbertLeastSquaresDistances;
        int GilbertSeedKolmogorov, GilbertSeedLeastSquared;
        double GilbertMinKolmogorovDistance, GilbertMinLeastSquaredDifference;
        fitGilbert(tracesize, origECDF, pGilbert, rGilbert, kGilbert, hGilbert,GilbertBurstsKolmogorov, GilbertSeedKolmogorov, GilbertMinKolmogorovDistance, GilbertBurstsLeastSquared, GilbertSeedLeastSquared, GilbertMinLeastSquaredDifference, GilbertKolmogorovDistances, GilbertLeastSquaresDistances);
        vector<double> GilbertConfidenceKolmogorov = calcConfidenceIntervall(GilbertKolmogorovDistances, significanceniveau);
        vector<double> GilbertConfidenceLeastSquared = calcConfidenceIntervall(GilbertLeastSquaresDistances, significanceniveau);
        writeVector(outputPath+"/GilbertECDFKolmogorov.txt", GilbertBurstsKolmogorov);
        writeVector(outputPath+"/GilbertECDFLeastSquared.txt", GilbertBurstsLeastSquared);
        writeVector(outputPath+"/GilbertKolmogorovDistances.txt", GilbertKolmogorovDistances);
        writeVector(outputPath+"/GilbertLeastSquaredDistances.txt", GilbertLeastSquaresDistances);
        cout << "Gilbert:" << endl;
        consolePrint(resultFilename, "Gilbert", metrics[0], GilbertSeedKolmogorov, GilbertMinKolmogorovDistance, GilbertConfidenceKolmogorov);
        consolePrint(resultFilename, "Gilbert", metrics[1], GilbertSeedLeastSquared, GilbertMinLeastSquaredDifference, GilbertConfidenceLeastSquared);


        cout << "Fitting Gilbert-Elliot" << endl;
        vector<int> GilbertElliotBurstsKolmogorov, GilbertElliotBurstsLeastSquared;
        vector<double> GilbertElliotKolmogorovDistances, GilbertElliotLeastSquaresDistances;
        int GilbertElliotSeedKolmogorov, GilbertElliotSeedLeastSquared;
        double GilbertElliotMinKolmogorovDistance, GilbertElliotMinLeastSquaredDifference;
        fitGilbert(tracesize, origECDF, pGilbertElliot, rGilbertElliot, kGilbertElliot, hGilbertElliot, GilbertElliotBurstsKolmogorov, GilbertElliotSeedKolmogorov, GilbertElliotMinKolmogorovDistance, GilbertElliotBurstsLeastSquared, GilbertElliotSeedLeastSquared, GilbertElliotMinLeastSquaredDifference, GilbertElliotKolmogorovDistances, GilbertElliotLeastSquaresDistances);
        vector<double> GilbertElliotConfidenceKolmogorov = calcConfidenceIntervall(GilbertElliotKolmogorovDistances, significanceniveau);
        vector<double> GilbertElliotConfidenceLeastSquared = calcConfidenceIntervall(GilbertElliotLeastSquaresDistances, significanceniveau);
        writeVector(outputPath+"/GilbertElliotECDFKolmogorov.txt", GilbertElliotBurstsKolmogorov);
        writeVector(outputPath+"/GilbertElliotECDFLeastSquared.txt", GilbertElliotBurstsLeastSquared);
        writeVector(outputPath+"/GilbertElliotKolmogorovDistances.txt", GilbertElliotKolmogorovDistances);
        writeVector(outputPath+"/GilbertElliotLeastSquaredDistances.txt", GilbertElliotLeastSquaresDistances);
        cout << "Gilbert-Elliot:" << endl;
        consolePrint(resultFilename, "GilbertElliot", metrics[0], GilbertElliotSeedKolmogorov, GilbertElliotMinKolmogorovDistance, GilbertElliotConfidenceKolmogorov);
        consolePrint(resultFilename, "GilbertElliot", metrics[1], GilbertElliotSeedLeastSquared, GilbertElliotMinLeastSquaredDifference, GilbertElliotConfidenceLeastSquared);


        cout << "Fitting Markov" << endl;
        vector<int> markovBurstsKolmogorov, markovBurstsLeastSquared;
        vector<double> markovKolmogorovDistances, markovLeastSquaresDistances;
        int markovSeedKolmogorov, markovSeedLeastSquared;
        double markovMinKolmogorovDistance, markovMinLeastSquaredDifference;
        fitMarkov(tracesize, origECDF, p13, p31, p32, p23, p14, markovBurstsKolmogorov, markovSeedKolmogorov, markovMinKolmogorovDistance, markovBurstsLeastSquared, markovSeedLeastSquared, markovMinLeastSquaredDifference, markovKolmogorovDistances, markovLeastSquaresDistances);
        vector<double> markovConfidenceKolmogorov = calcConfidenceIntervall(markovKolmogorovDistances, significanceniveau);
        vector<double> markovConfidenceLeastSquared = calcConfidenceIntervall(markovLeastSquaresDistances, significanceniveau);
        writeVector(outputPath+"/MarkovECDFKolmogorov.txt", markovBurstsKolmogorov);
        writeVector(outputPath+"/MarkovECDFLeastSquared.txt", markovBurstsLeastSquared);
        writeVector(outputPath+"/MarkovKolmogorovDistances.txt", markovKolmogorovDistances);
        writeVector(outputPath+"/MarkovLeastSquaredDistances.txt", markovLeastSquaresDistances);
        cout << "Markov:" << endl;
        consolePrint(resultFilename, "Markov", metrics[0], markovSeedKolmogorov, markovMinKolmogorovDistance, markovConfidenceKolmogorov);
        consolePrint(resultFilename, "Markov", metrics[1], markovSeedLeastSquared, markovMinLeastSquaredDifference, markovConfidenceLeastSquared);


        cout << "Calculations are for " << 100-significanceniveau << "% confidence interval" << endl;

        clock_t stop = clock();
        double elapsed = (double) (stop-start)/CLOCKS_PER_SEC;
        cout << elapsed << "s" << endl;
    }

}