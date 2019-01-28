/**
 * Hilfstool zum Analysieren des Paketverlusts von einem gebebenen Binärtrace
 * Generiert mit unterschiedlichen Seeds des Zufallszahlengenerators ein Vergleichstrace
 * Gibt den Seed und den kleinsten quadratischen Abstand aus, mit dem die ECDF des generierten Traces mit der ECDF des original Traces am besten passt
 * Abstandsmaß: Methode der kleinsten Quadrate und Kolmogorov Abstand
 * @param Pfad zum Sage Script
 * @param Pfad zum Binärtrace
 * @param Signifikanzniveau. Unterstützte Werte siehe statistics.h
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

void usage() {
    cout << "usage: ./packetloss <path to sagemath python script> <path to binary file> <path to output folder> <significanceniveau [0-100]>" << endl;
}


int main(int argc, char *argv[]) {
    if (argc < 5) {
        usage();
        return 0;
    } else {
        clock_t start = clock();
        string sagescript = argv[1];
        string pathToGivenTrace = argv[2];
        string outputPath = argv[3];
        int significanceniveau = stoi(argv[4]);
        string resultFilename = outputPath + "/FittingResults.txt";
        writeHeader(resultFilename);
        cout << "Processing Inputtrace" << endl;
        vector<bool> originalTrace = readBinaryTrace(pathToGivenTrace);
        long tracesize = originalTrace.size();
        vector<int> origSizes;
        calcLoss(originalTrace, origSizes);
        vector<vector<float> > origECDF;
        calculateECDF(origSizes, origECDF);
        writeVector(outputPath + "/OriginalECDF.txt", origSizes);

        cout << "Calculating Bernoulli Model-Parameter" << endl;
        double pBernoulli, rBernoulli;
        getBernoulliParams(originalTrace, pBernoulli);
        rBernoulli = 1.0 - pBernoulli;

        cout << "Calculating Simple-Gilbert Model-Parameter" << endl;
        double pSimpleGilbert, rSimpleGilbert;
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

        cout << "Writing Parameter to file" << endl;
        writeParams(outputPath + "/ModelParams.txt", pBernoulli, rBernoulli, pSimpleGilbert, rSimpleGilbert, pGilbert,
                    rGilbert, kGilbert, hGilbert, pGilbertElliot, rGilbertElliot, kGilbertElliot, hGilbertElliot, p13,
                    p31, p32, p23, p14, p41);

        writeStatesHeader(outputPath + "/states.txt");

        cout << "Fitting Bernoulli" << endl;
        vector<int> bernoulliBurstsKolmogorov, bernoulliBurstsLeastSquared;
        vector<double> bernoulliKolmogorovDistances, bernoulliLeastSquaresDistances;
        int bernoulliSeedKolmogorov, bernoulliSeedLeastSquared;
        double bernoulliMinKolmogorovDistance, bernoulliMinLeastSquaredDifference;
        unsigned long long bernoulliGoods=0, bernoulliBads=0;
        fitGilbert(tracesize, origECDF, pBernoulli, rBernoulli, 1.0, 0.0, bernoulliBurstsKolmogorov,
                   bernoulliSeedKolmogorov, bernoulliMinKolmogorovDistance, bernoulliBurstsLeastSquared,
                   bernoulliSeedLeastSquared, bernoulliMinLeastSquaredDifference, bernoulliKolmogorovDistances,
                   bernoulliLeastSquaresDistances, bernoulliGoods, bernoulliBads);
        vector<double> bernoulliConfidenceKolmogorov = calcConfidenceIntervall(bernoulliKolmogorovDistances,
                                                                               significanceniveau);
        vector<double> bernoulliConfidenceLeastSquared = calcConfidenceIntervall(bernoulliLeastSquaresDistances,
                                                                                 significanceniveau);
        writeVector(outputPath + "/BernoulliECDFKolmogorov.txt", bernoulliBurstsKolmogorov);
        writeVector(outputPath + "/BernoulliECDFLeastSquared.txt", bernoulliBurstsLeastSquared);
        writeVector(outputPath + "/BernoulliKolmogorovDistances.txt", bernoulliKolmogorovDistances);
        writeVector(outputPath + "/BernoulliLeastSquaredDistances.txt", bernoulliLeastSquaresDistances);
        cout << "Bernoulli:" << endl;
        consolePrint(resultFilename, "Bernoulli", metrics[0], bernoulliSeedKolmogorov, bernoulliMinKolmogorovDistance,
                     bernoulliConfidenceKolmogorov);
        consolePrint(resultFilename, "Bernoulli", metrics[1], bernoulliSeedLeastSquared,
                     bernoulliMinLeastSquaredDifference, bernoulliConfidenceLeastSquared);
        writeStates(outputPath + "/states.txt", bernoulliGoods, bernoulliBads);

        cout << "Fitting Simple-Gilbert" << endl;
        vector<int> simpleGilbertBurstsKolmogorov, simpleGilbertBurstsLeastSquared;
        vector<double> simpleGilbertKolmogorovDistances, simpleGilbertLeastSquaresDistances;
        int simpleGilbertSeedKolmogorov, simpleGilbertSeedLeastSquared;
        double simpleGilbertMinKolmogorovDistance, simpleGilbertMinLeastSquaredDifference;
        unsigned long long simpleGilbertGoods=0, simpleGilbertBads=0;
        fitGilbert(tracesize, origECDF, pSimpleGilbert, rSimpleGilbert, 1.0, 0.0, simpleGilbertBurstsKolmogorov,
                   simpleGilbertSeedKolmogorov, simpleGilbertMinKolmogorovDistance, simpleGilbertBurstsLeastSquared,
                   simpleGilbertSeedLeastSquared, simpleGilbertMinLeastSquaredDifference,
                   simpleGilbertKolmogorovDistances, simpleGilbertLeastSquaresDistances, simpleGilbertGoods,
                   simpleGilbertBads);
        vector<double> simpleGilbertConfidenceKolmogorov = calcConfidenceIntervall(simpleGilbertKolmogorovDistances,
                                                                                   significanceniveau);
        vector<double> simpleGilbertConfidenceLeastSquared = calcConfidenceIntervall(simpleGilbertLeastSquaresDistances,
                                                                                     significanceniveau);
        writeVector(outputPath + "/SimpleGilbertECDFKolmogorov.txt", simpleGilbertBurstsKolmogorov);
        writeVector(outputPath + "/SimpleGilbertECDFLeastSquared.txt", simpleGilbertBurstsLeastSquared);
        writeVector(outputPath + "/SimpleGilbertKolmogorovDistances.txt", simpleGilbertKolmogorovDistances);
        writeVector(outputPath + "/SimpleGilbertLeastSquaredDistances.txt", simpleGilbertLeastSquaresDistances);
        cout << "Simple-Gilbert:" << endl;
        consolePrint(resultFilename, "SimpleGilbert", metrics[0], simpleGilbertSeedKolmogorov,
                     simpleGilbertMinKolmogorovDistance, simpleGilbertConfidenceKolmogorov);
        consolePrint(resultFilename, "SimpleGilbert", metrics[1], simpleGilbertSeedLeastSquared,
                     simpleGilbertMinLeastSquaredDifference, simpleGilbertConfidenceLeastSquared);
        writeStates(outputPath + "/states.txt", simpleGilbertGoods, simpleGilbertBads);

        cout << "Fitting Gilbert" << endl;
        if (!isnan(pGilbert) && !isnan(rGilbert) && !isnan(kGilbert) && !isnan(hGilbert)) {
            vector<int> GilbertBurstsKolmogorov, GilbertBurstsLeastSquared;
            vector<double> GilbertKolmogorovDistances, GilbertLeastSquaresDistances;
            int GilbertSeedKolmogorov, GilbertSeedLeastSquared;
            double GilbertMinKolmogorovDistance, GilbertMinLeastSquaredDifference;
            unsigned long long GilbertGoods=0, GilbertBads=0;
            fitGilbert(tracesize, origECDF, pGilbert, rGilbert, kGilbert, hGilbert, GilbertBurstsKolmogorov,
                       GilbertSeedKolmogorov, GilbertMinKolmogorovDistance, GilbertBurstsLeastSquared,
                       GilbertSeedLeastSquared, GilbertMinLeastSquaredDifference, GilbertKolmogorovDistances,
                       GilbertLeastSquaresDistances, GilbertGoods, GilbertBads);
            vector<double> GilbertConfidenceKolmogorov = calcConfidenceIntervall(GilbertKolmogorovDistances,
                                                                                 significanceniveau);
            vector<double> GilbertConfidenceLeastSquared = calcConfidenceIntervall(GilbertLeastSquaresDistances,
                                                                                   significanceniveau);
            writeVector(outputPath + "/GilbertECDFKolmogorov.txt", GilbertBurstsKolmogorov);
            writeVector(outputPath + "/GilbertECDFLeastSquared.txt", GilbertBurstsLeastSquared);
            writeVector(outputPath + "/GilbertKolmogorovDistances.txt", GilbertKolmogorovDistances);
            writeVector(outputPath + "/GilbertLeastSquaredDistances.txt", GilbertLeastSquaresDistances);
            cout << "Gilbert:" << endl;
            consolePrint(resultFilename, "Gilbert", metrics[0], GilbertSeedKolmogorov, GilbertMinKolmogorovDistance,
                         GilbertConfidenceKolmogorov);
            consolePrint(resultFilename, "Gilbert", metrics[1], GilbertSeedLeastSquared,
                         GilbertMinLeastSquaredDifference, GilbertConfidenceLeastSquared);
            writeStates(outputPath + "/states.txt", GilbertGoods, GilbertBads);
        } else {
            cout << "Failed fitting Gilbert Model. Baum-Welch estimated NaN-Values for one or more model Parameter" << endl;
        }


        cout << "Fitting Gilbert-Elliot" << endl;
        if (!isnan(pGilbertElliot) && !isnan(rGilbertElliot) && !isnan(kGilbertElliot) && !isnan(hGilbertElliot)) {
            vector<int> GilbertElliotBurstsKolmogorov, GilbertElliotBurstsLeastSquared;
            vector<double> GilbertElliotKolmogorovDistances, GilbertElliotLeastSquaresDistances;
            int GilbertElliotSeedKolmogorov, GilbertElliotSeedLeastSquared;
            double GilbertElliotMinKolmogorovDistance, GilbertElliotMinLeastSquaredDifference;
            unsigned long long GilbertElliotGoods=0, GilbertElliotBads=0;
            fitGilbert(tracesize, origECDF, pGilbertElliot, rGilbertElliot, kGilbertElliot, hGilbertElliot,
                       GilbertElliotBurstsKolmogorov, GilbertElliotSeedKolmogorov, GilbertElliotMinKolmogorovDistance,
                       GilbertElliotBurstsLeastSquared, GilbertElliotSeedLeastSquared,
                       GilbertElliotMinLeastSquaredDifference, GilbertElliotKolmogorovDistances,
                       GilbertElliotLeastSquaresDistances, GilbertElliotGoods, GilbertElliotBads);
            vector<double> GilbertElliotConfidenceKolmogorov = calcConfidenceIntervall(GilbertElliotKolmogorovDistances,
                                                                                       significanceniveau);
            vector<double> GilbertElliotConfidenceLeastSquared = calcConfidenceIntervall(
                    GilbertElliotLeastSquaresDistances, significanceniveau);
            writeVector(outputPath + "/GilbertElliotECDFKolmogorov.txt", GilbertElliotBurstsKolmogorov);
            writeVector(outputPath + "/GilbertElliotECDFLeastSquared.txt", GilbertElliotBurstsLeastSquared);
            writeVector(outputPath + "/GilbertElliotKolmogorovDistances.txt", GilbertElliotKolmogorovDistances);
            writeVector(outputPath + "/GilbertElliotLeastSquaredDistances.txt", GilbertElliotLeastSquaresDistances);
            cout << "Gilbert-Elliot:" << endl;
            consolePrint(resultFilename, "GilbertElliot", metrics[0], GilbertElliotSeedKolmogorov,
                         GilbertElliotMinKolmogorovDistance, GilbertElliotConfidenceKolmogorov);
            consolePrint(resultFilename, "GilbertElliot", metrics[1], GilbertElliotSeedLeastSquared,
                         GilbertElliotMinLeastSquaredDifference, GilbertElliotConfidenceLeastSquared);
            writeStates(outputPath + "/states.txt", GilbertElliotGoods, GilbertElliotBads);
        } else {
            cout << "Failed fitting Gilbert-Elliot Model. Baum-Welch estimated NaN-Values for one or more model Parameter" << endl;
        }


        cout << "Fitting Markov" << endl;
        if (!isnan(p13) && !isnan(p31) && !isnan(p32) && !isnan(p23) && !isnan(p14)) {
            vector<int> markovBurstsKolmogorov, markovBurstsLeastSquared;
            vector<double> markovKolmogorovDistances, markovLeastSquaresDistances;
            int markovSeedKolmogorov, markovSeedLeastSquared;
            double markovMinKolmogorovDistance, markovMinLeastSquaredDifference;
            unsigned long long stateOne=0, stateTwo=0, stateThree=0, stateFour=0;
            fitMarkov(tracesize, origECDF, p13, p31, p32, p23, p14, markovBurstsKolmogorov, markovSeedKolmogorov,
                      markovMinKolmogorovDistance, markovBurstsLeastSquared, markovSeedLeastSquared,
                      markovMinLeastSquaredDifference, markovKolmogorovDistances, markovLeastSquaresDistances, stateOne,
                      stateTwo, stateThree, stateFour);
            vector<double> markovConfidenceKolmogorov = calcConfidenceIntervall(markovKolmogorovDistances,
                                                                                significanceniveau);
            vector<double> markovConfidenceLeastSquared = calcConfidenceIntervall(markovLeastSquaresDistances,
                                                                                  significanceniveau);
            writeVector(outputPath + "/MarkovECDFKolmogorov.txt", markovBurstsKolmogorov);
            writeVector(outputPath + "/MarkovECDFLeastSquared.txt", markovBurstsLeastSquared);
            writeVector(outputPath + "/MarkovKolmogorovDistances.txt", markovKolmogorovDistances);
            writeVector(outputPath + "/MarkovLeastSquaredDistances.txt", markovLeastSquaresDistances);
            cout << "Markov:" << endl;
            consolePrint(resultFilename, "Markov", metrics[0], markovSeedKolmogorov, markovMinKolmogorovDistance,
                         markovConfidenceKolmogorov);
            consolePrint(resultFilename, "Markov", metrics[1], markovSeedLeastSquared, markovMinLeastSquaredDifference,
                         markovConfidenceLeastSquared);
            writeStates(outputPath + "/states.txt", stateOne, stateTwo, stateThree, stateFour);
        } else {
            cout << "Failed fitting Markov Model. Baum-Welch estimated NaN-Values for one or more model Parameter" << endl;
        }

        cout << "Calculations are for " << 100 - significanceniveau << "% confidence interval" << endl;

        clock_t stop = clock();
        double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
        cout << elapsed << "s" << endl;
    }

}