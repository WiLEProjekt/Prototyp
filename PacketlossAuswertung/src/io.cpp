#include <io.h>

vector<bool> readBinaryTrace(string filename){
    vector<bool> trace;
    ifstream file;
    file.open(filename, ios::in);
    char c;
    while (file.get(c)) {
        trace.push_back(c != '0');
    }
    return trace;
}

void writeVector(string output, vector<int> &outvec){
    fstream fout;
    fout.open(output, ios::out);
    for(int i = 0; i<outvec.size(); i++){
        fout << outvec[i] << endl;
    }
    fout.close();
}

void writeVector(string output, vector<double> &outvec){
    fstream fout;
    fout.open(output, ios::out);
    for(int i = 0; i<outvec.size(); i++){
        fout << outvec[i] << endl;
    }
    fout.close();
}

void writeParams(string output, double pBernoulli, double rBernoulli, double pSimpleGilbert, double rSimpleGilbert, double pGilbert, double rGilbert, double kGilbert, double hGilbert, double pGilbertElliot, double rGilbertElliot, double kGilbertElliot, double hGilbertElliot, double p13, double p31, double p32, double p23, double p14, double p41){
    fstream fout;
    fout.open(output, ios::out);
    fout << "Bernoulli: p; r" << endl;
    fout << pBernoulli << ";" << rBernoulli << endl;
    fout << "Simple-Gilbert: p; r" << endl;
    fout << pSimpleGilbert << ";" << rSimpleGilbert << endl;
    fout << "Gilbert: p; r; k; h" << endl;
    fout << pGilbert << ";" << rGilbert << ";" << kGilbert << ";" << hGilbert << endl;
    fout << "Gilbert-Elliot: p; r; k; h" << endl;
    fout << pGilbertElliot << ";" << rGilbertElliot << ";" << kGilbertElliot << ";" << hGilbertElliot << endl;
    fout << "Markov: p13; p31; p32; p23; p14; p41" << endl;
    fout << p13 << ";" << p31 << ";" << p32 << ";" << p23 << ";" << p14 << ";" << p41 << endl;
    fout.close();
}

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

void writeStatesHeader(string output){
    fstream fout;
    fout.open(output, ios::out);
    fout << "for Gilbert-based models: good-states; bad-states; 0; 0" << endl;
    fout << "for Markov: state one; state two; state three; state four" << endl;
    fout << "1. line: Bernoulli" << endl;
    fout << "2. line: Simple-Gilbert" << endl;
    fout << "3. line: Gilbert" << endl;
    fout << "4. line: Gilbert-Elliot" << endl;
    fout << "5. line: Markov" << endl;
    fout.close();
}

void writeStates(string output, unsigned long long one, unsigned long long two, unsigned long long three, unsigned long long four){
    fstream fout;
    fout.open(output, ios::out | ios::app);
    fout << one << ";" << two << ";" << three << ";" << four <<  endl;
}