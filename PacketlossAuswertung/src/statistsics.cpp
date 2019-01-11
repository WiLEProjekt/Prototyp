#include <statistsics.h>
#include <Generator.h>

double calcMean(vector<double> input){
    return(accumulate(input.begin(), input.end(), 0.0)/input.size());
}

double caldStandarddeviation(vector<double> input, double mean){
    double sum=0;
    for(int i=0; i<input.size(); i++){
        sum = sum + ((input.at(i)-mean)*(input.at(i)-mean));
    }
    return(sqrt(1/(input.size()-1)*sum));
}

void calculateECDF(vector<int> &sizes, vector<vector<float> > &ECDF){
    sort(sizes.begin(), sizes.end());
    float cumprob = 0;
    for(int i = sizes.at(0); i <= sizes.at(sizes.size()-1); i++){
        vector<float> temp;
        int counter = count(sizes.begin(), sizes.end(), i);
        float tmp = (float)i;
        temp.push_back(tmp);
        float prob = cumprob + (float)counter/sizes.size();
        temp.push_back(prob);
        cumprob = prob;
        ECDF.push_back(temp);
    }
}

void calcLoss(vector<bool> &trace, vector<int> &overallsizes) {
    int temp = 0;
    for (int i = 0; i < trace.size(); i++) {
        if(i > 0){
            if(!trace[i] && !trace[i-1]){
                temp--;
            }else if(trace[i] && trace[i-1]){
                temp++;
            }else{
                if(temp < 0){
                    overallsizes.push_back(temp);
                }else{
                    overallsizes.push_back(temp);
                }
                temp = 0;
                if(!trace[i]){
                    temp--;
                }
                else if(trace[i]) {
                    temp++;
                }
            }
        }else{
            if(!trace[i]){
                temp--;
            }
            else if(trace[i]){
                temp++;
            }
        }
    }
}

void adjustECDFLengths(vector<vector<float> > &ECDF1, vector<vector<float> > &ECDF2){
    if(ECDF1[0][0] > ECDF2[0][0]){
        vector<vector<float> > newECDF1;
        int diff = ECDF1[0][0]-ECDF2[0][0];
        int start = ECDF1[0][0]-diff;
        for(int i = start; i<ECDF1[0][0]; i++){
            vector<float> temp;
            temp.push_back((float)i);
            temp.push_back(0.0f);
            newECDF1.push_back(temp);
        }
        for(int i = 0; i<ECDF1.size(); i++){
            newECDF1.push_back(ECDF1[i]);
        }
        ECDF1 = newECDF1;
    }else if(ECDF2[0][0] > ECDF1[0][0]){
        vector<vector<float> > newECDF2;
        int diff = ECDF2[0][0]-ECDF1[0][0];
        int start = ECDF2[0][0]-diff;
        for(int i = start; i<ECDF2[0][0]; i++){
            vector<float> temp;
            temp.push_back((float)i);
            temp.push_back(0.0f);
            newECDF2.push_back(temp);
        }
        for(int i = 0; i<ECDF2.size(); i++){
            newECDF2.push_back(ECDF2[i]);
        }
        ECDF2 = newECDF2;
    }

    while(ECDF1[ECDF1.size()-1][0] < ECDF2[ECDF2.size()-1][0]){ //add elements at the end of ECDF1
        vector<float> temp;
        float i = ECDF1[ECDF1.size()-1][0] + 1;
        temp.push_back(i);
        temp.push_back(1.0f);
        ECDF1.push_back(temp);
    }
    while(ECDF2[ECDF2.size()-1][0] < ECDF1[ECDF1.size()-1][0]){ //add elements at the end of ECDF2
        vector<float> temp;
        float i = ECDF2[ECDF2.size()-1][0] + 1;
        temp.push_back(i);
        temp.push_back(1.0f);
        ECDF2.push_back(temp);
    }
}

double calcSquaredDifference(float value1, float value2){
     return((value1 - value2)*(value1 - value2));
}

double calcKolmogorovDistance(float value1, float value2){
    return(fabs(value1-value2));
}

void calcDistance(vector<vector<float> > &ECDF1, vector<vector<float> > &ECDF2, double &kolmogorovDistance, double &squaredDifference){
    if(ECDF1.size() != ECDF2.size()){
        cout << "ECDF1 & ECDF2 have different length. Cannot calculate squared difference. Implementation error somewhere" << endl;
    }else{
        double kolmogorovDistanceLocal=0.0;
        double squaredDifferenceLocal = 0.0;
        for(int i = 0; i<ECDF1.size(); i++){
            //Calculate Kolmogorov distance
            double tempKolmogorovDistance = calcKolmogorovDistance(ECDF1[i][1], ECDF2[i][1]);
            if(tempKolmogorovDistance > kolmogorovDistanceLocal){
                kolmogorovDistanceLocal = tempKolmogorovDistance;
            }
            //Calculate Squared distance
            squaredDifferenceLocal += calcSquaredDifference(ECDF1[i][1], ECDF2[i][1]);
        }
        kolmogorovDistance = kolmogorovDistanceLocal;
        squaredDifference = squaredDifferenceLocal;
    }
}

void fitGilbert(long length, vector<vector<float> > origECDF, double p, double r, double k, double h, vector<int> &returnBurstsizesKolmogorov, int &seedKolmogorov, double &kolmogorovDistance, vector<int> &returnBurstsizesLeastSquared, int &seedLeastSquared, double &LeastSquaredDifference, vector<double> &KolmogorovDistances, vector<double> &LeastSquaresDistances){
    int bestSeedKolmogorov, bestSeedLeastSquared;
    vector<int> bestBurstsizesKolmogorov, bestBurstsizesLeastSquared; //Burstsizes corresponding to generated trace with bestSeed
    double lowestDiffKolmogorov = DBL_MAX;
    double lowestDiffLeastSquared = DBL_MAX;
    double tempKolmogorovDistance;
    double tempLeastSquared;
    for(int i = 0; i < MAXSEED; i++){
        vector<vector<float> > tempOrigECDF = origECDF;
        setSeed(i);
        vector<int> burstsizes = buildGilbertElliot(length, p, r, k, h);
        vector<vector<float> > calcECDF;
        calculateECDF(burstsizes, calcECDF);
        adjustECDFLengths(tempOrigECDF, calcECDF);

        calcDistance(tempOrigECDF, calcECDF, tempKolmogorovDistance, tempLeastSquared);
        KolmogorovDistances.push_back(tempKolmogorovDistance);
        LeastSquaresDistances.push_back(tempLeastSquared);
        //kolmogorov
        if(tempKolmogorovDistance<lowestDiffKolmogorov){
            bestSeedKolmogorov = i;
            bestBurstsizesKolmogorov = burstsizes;
            lowestDiffKolmogorov = tempKolmogorovDistance;
        }
        //Least Squared
        if(tempLeastSquared < lowestDiffLeastSquared){
            bestSeedLeastSquared = i;
            bestBurstsizesLeastSquared = burstsizes;
            lowestDiffLeastSquared = tempLeastSquared;
        }
    }
    returnBurstsizesKolmogorov = bestBurstsizesKolmogorov;
    returnBurstsizesLeastSquared = bestBurstsizesLeastSquared;
    seedKolmogorov = bestSeedKolmogorov;
    kolmogorovDistance = lowestDiffKolmogorov;
    seedLeastSquared = bestSeedLeastSquared;
    LeastSquaredDifference = lowestDiffLeastSquared;
}
//Similar to fitGilbert, however saves many comparisons
void fitMarkov(long length, vector<vector<float> > origECDF, double p13, double p31, double p32, double p23, double p14, vector<int> &returnBurstsizesKolmogorov, int &seedKolmogorov, double &kolmogorovDistance, vector<int> &returnBurstsizesLeastSquared, int &seedLeastSquared, double &LeastSquaredDifference, vector<double> &KolmogorovDistances, vector<double> &LeastSquaresDistances){
    int bestSeedKolmogorov, bestSeedLeastSquared;
    vector<int> bestBurstsizesKolmogorov, bestBurstsizesLeastSquared; //Burstsizes corresponding to generated trace with bestSeed
    double lowestDiffKolmogorov = DBL_MAX;
    double lowestDiffLeastSquared = DBL_MAX;
    double tempKolmogorovDistance;
    double tempLeastSquared;
    for(int i = 0; i < MAXSEED; i++){
        vector<vector<float> > tempOrigECDF = origECDF;
        setSeed(i);
        vector<int> burstsizes = buildMarkov(length, p13, p31, p32, p23, p14);
        vector<vector<float> > calcECDF;
        calculateECDF(burstsizes, calcECDF);
        adjustECDFLengths(tempOrigECDF, calcECDF);

        calcDistance(tempOrigECDF, calcECDF, tempKolmogorovDistance, tempLeastSquared);
        KolmogorovDistances.push_back(tempKolmogorovDistance);
        LeastSquaresDistances.push_back(tempLeastSquared);
        //kolmogorov
        if(tempKolmogorovDistance<lowestDiffKolmogorov){
            bestSeedKolmogorov = i;
            bestBurstsizesKolmogorov = burstsizes;
            lowestDiffKolmogorov = tempKolmogorovDistance;
        }
        //Least Squared
        if(tempLeastSquared < lowestDiffLeastSquared){
            bestSeedLeastSquared = i;
            bestBurstsizesLeastSquared = burstsizes;
            lowestDiffLeastSquared = tempLeastSquared;
        }
    }
    returnBurstsizesKolmogorov = bestBurstsizesKolmogorov;
    returnBurstsizesLeastSquared = bestBurstsizesLeastSquared;
    seedKolmogorov = bestSeedKolmogorov;
    kolmogorovDistance = lowestDiffKolmogorov;
    seedLeastSquared = bestSeedLeastSquared;
    LeastSquaredDifference = lowestDiffLeastSquared;
}