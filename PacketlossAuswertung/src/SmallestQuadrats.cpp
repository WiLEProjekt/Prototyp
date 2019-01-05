#include <SmallestQuadrats.h>
#include <Generator.h>

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
    while(ECDF1[0][0] > ECDF2[0][0]){ //add elements at the beginning of ECDF1
        vector<float> temp;
        float i = ECDF1[0][0] - 1;
        temp.push_back(i);
        temp.push_back(0.0f);
        ECDF1.insert(ECDF1.begin(), temp);
    }
    while(ECDF2[0][0] > ECDF1[0][0]){ //add elements at the beginning of ECDF2
        vector<float> temp;
        float i = ECDF2[0][0] - 1;
        temp.push_back(i);
        temp.push_back(0.0f);
        ECDF2.insert(ECDF2.begin(), temp);
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

double calcSquaredDifference(vector<vector<float> > &ECDF1, vector<vector<float> > &ECDF2){
    double difference=0.0;
    if(ECDF1.size() != ECDF2.size()){
        cout << "ECDF1 & ECDF2 have different length. Cannot calculate squared difference. Implementation error somewhere" << endl;
        return -1.0;
    }else{
        for(int i = 0; i<ECDF1.size(); i++){
            difference += (ECDF1[i][1] - ECDF2[i][1])*(ECDF1[i][1] - ECDF2[i][1]);
        }
    }
    return(difference);
}

//TODO: Evtl. auch lowestDiff zurückgeben
int fitGilbert(long length, vector<vector<float> > origECDF, float p, float r, float k, float h){
    int bestSeed;
    double lowestDiff = DBL_MAX;
    for(int i = 0; i < MAXSEED; i++){
        vector<vector<float> > tempOrigECDF = origECDF;
        setSeed(i);
        vector<int> burstsizes = buildGilbertElliot(length, p, r, k, h);
        vector<vector<float> > calcECDF;
        calculateECDF(burstsizes, calcECDF);
        adjustECDFLengths(tempOrigECDF, calcECDF);
        double squaredDiff = calcSquaredDifference(tempOrigECDF, calcECDF);
        if(squaredDiff<lowestDiff){
            bestSeed = i;
            lowestDiff = squaredDiff;
        }
    }
    return(bestSeed);
}
//2x fast das gleiche um Vergleiche zu sparen
int fitMarkov(long length, vector<vector<float> > origECDF, float p13, float p31, float p32, float p23, float p14){
    int bestSeed;
    double lowestDiff = DBL_MAX;
    for(int i = 0; i < MAXSEED; i++){
        vector<vector<float> > tempOrigECDF = origECDF;
        setSeed(i);
        vector<int> burstsizes = buildMarkov(length, p13, p31, p32, p23, p14);
        vector<vector<float> > calcECDF;
        calculateECDF(burstsizes, calcECDF);
        adjustECDFLengths(tempOrigECDF, calcECDF);
        double squaredDiff = calcSquaredDifference(tempOrigECDF, calcECDF);
        if(squaredDiff<lowestDiff){
            bestSeed = i;
            lowestDiff = squaredDiff;
        }
    }
    return(bestSeed);
}