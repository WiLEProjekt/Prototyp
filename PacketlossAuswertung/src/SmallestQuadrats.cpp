#include <SmallestQuadrats.h>

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