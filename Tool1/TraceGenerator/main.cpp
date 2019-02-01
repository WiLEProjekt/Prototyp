#include "TraceGenerator.h"
#include "PcapParser/PcapParser.h"
#include "Packetloss/PacketLossModelType.h"
#include "Packetloss/PacketlossParameterParser/BernoulliParser.h"
#include "Packetloss/PacketlossParameterParser/SimpleGilbertParser.h"

PacketLossModelType parseModelName(const string &modelName) {
    if (modelName == "markov") {
        return MARKOV;
    }
    if (modelName == "bernoulli") {
        return BERNOULLI;
    }
    if (modelName == "simplegilbert") {
        return SIMPLE_GILBERT;
    }
    if (modelName == "gilbert") {
        return GILBERT;
    }
    if (modelName == "gilbertelliot") {
        return GILBERT_ELLIOT;
    }
    cout << "No valid modelname: " << modelName << endl;
}

void writeDelayFile(const string &filename, vector<int64_t> delays) {
    ofstream uploadDelayFile;
    uploadDelayFile.open(filename);
    for (int64_t ts: delays) {
        uploadDelayFile << ts << " ";
    }
    uploadDelayFile.flush();
    uploadDelayFile.close();
}

void writeResultToFile(const result &download, const string &path) {
    ofstream uploadLossFile;
    string pathcommand = "mkdir -p " + path;
    system(pathcommand.c_str());

    ofstream downloadLossFile;
    downloadLossFile.open(path + "/downloadLoss.txt");
    for (bool b : download.loss) {
        downloadLossFile << b;
    }
    downloadLossFile.flush();
    downloadLossFile.close();

    string downloadDelayFilename = path + "/downloadDelays.csv";
    writeDelayFile(downloadDelayFilename, download.delays);

    ofstream downloadDuplicationFile;
    downloadDuplicationFile.open(path + "/downloadDuplication.txt");
    downloadDuplicationFile << download.duplication << endl;
    downloadDuplicationFile.flush();
    downloadDuplicationFile.close();

    ofstream downloadReorderingFile;
    downloadReorderingFile.open(path + "/downloadReordering.txt");
    downloadReorderingFile << download.reordering << endl;
    downloadReorderingFile.flush();
    downloadReorderingFile.close();
}

vector<bool> generate(PacketLossModelType model, unsigned long numPackets, double params[]) {
    TraceGenerator traceGenerator;
    return traceGenerator.generateTrace(model, numPackets, 0, params);
}

double *parseParams(PacketLossModelType model, const string *params, double *result) {
    int paramCount = 0;
    if (model == MARKOV) {
        paramCount = 5;
        double newresult[paramCount];
        for (int i = 0; i < paramCount; i++) {
            newresult[i] = stof(params[i]);
        }
        result = newresult;
        return result;
    } else if (model == GILBERT_ELLIOT) {
        paramCount = 4;
        double newresult[paramCount];
        for (int i = 0; i < paramCount; i++) {
            newresult[i] = stof(params[i]);
        }
        result = newresult;
        return result;
    } else if (model == GILBERT) {
        paramCount = 4;
        double newresult[paramCount];
        newresult[0] = stof(params[0]);
        newresult[1] = stof(params[1]);
        newresult[2] = 1;
        newresult[3] = stof(params[2]);
        result = newresult;
        return result;
    } else if (model == SIMPLE_GILBERT) {
        paramCount = 4;
        double newresult[paramCount];
        newresult[0] = stof(params[0]);
        newresult[1] = stof(params[1]);
        newresult[2] = 1;
        newresult[3] = 0;
        result = newresult;
        return result;
    } else if (model == BERNOULLI) {
        paramCount = 4;
        double newresult[paramCount];
        newresult[0] = stof(params[0]);
        newresult[1] = 1 - newresult[0];
        newresult[2] = 1;
        newresult[3] = 0;
        result = newresult;
        return result;
    } else {
        return nullptr;
    }
}

void writeLossToFile(vector<bool> loss, const string &path) {
    string pathcommand = "mkdir -p " + path;
    system(pathcommand.c_str());

    ofstream downloadLossFile;
    downloadLossFile.open(path + "/loss.txt");
    for (bool b : loss) {
        downloadLossFile << b;
    }
    downloadLossFile.flush();
    downloadLossFile.close();
}

/**
 * Reads the temprary file, builds the matrices and deletes the temporary file
 */
void readFile(vector<vector<double>> &transitionmatrix, vector<vector<double>> &emissionmatrix,
              vector<double> &initialprobabilities) {
    ifstream infile;
    string filename = "sageout.txt";
    infile.open(filename);
    string line;
    int i = 0;
    vector<double> temptrans;
    vector<double> tempemission;
    string tempstring;
    //Push all probabilities into tempral vectors, then build the matrices
    while (getline(infile, line)) {
        if (i == 0) { //First line = Transition Matrix
            for (int i = 0; i < line.size(); i++) {
                if (line[i] != ';') {
                    tempstring = tempstring + line[i];
                } else {
                    temptrans.push_back(stod(tempstring));
                    tempstring = "";
                }
            }
        } else if (i == 1) { //Second line = Emission Matrix
            for (int i = 0; i < line.size(); i++) {
                if (line[i] != ';') {
                    tempstring = tempstring + line[i];
                } else {
                    tempemission.push_back(stod(tempstring));
                    tempstring = "";
                }
            }

        } else if (i == 2) { //Third line = Initial Probabilities
            for (int i = 0; i < line.size(); i++) {
                if (line[i] != ';') {
                    tempstring = tempstring + line[i];
                } else {
                    initialprobabilities.push_back(stod(tempstring));
                    tempstring = "";
                }
            }
        } else {
            break;
        }
        i++;
    }
    infile.close();
    //Build Transitionmatrix
    int dimension = sqrt(temptrans.size());
    vector<double> tmp;
    int counter = 0;
    for (int i = 0; i < temptrans.size(); i++) {
        if (counter == dimension) {
            counter = 0;
            transitionmatrix.push_back(tmp);
            tmp.clear();
        }
        tmp.push_back(temptrans[i]);
        counter++;
    }
    transitionmatrix.push_back(tmp);
    tmp.clear();
    counter = 0;

    //Build Emissionmatrix
    for (int i = 0; i < tempemission.size(); i++) {
        if (counter == 2) {
            counter = 0;
            emissionmatrix.push_back(tmp);
            tmp.clear();
        }
        tmp.push_back(tempemission[i]);
        counter++;
    }
    emissionmatrix.push_back(tmp);
    tmp.clear();
    if (remove(filename.c_str()) != 0) { //delete temporary file
        perror("Error deleting the temporary file");
    }
}

double *startSageMath(const string &sagescript, const string &traceFile, const string &model, double *params) {
    if (!(model == "gilbert" || model == "gilbertelliot" || model == "markov")) {
        cout << "wrong model. choose gilbert | gilbertelliot | markov" << endl;
        return nullptr;
    }
    cout << "Starting Sagemath" << endl;
    string pythonCommand = "sage --python " + sagescript + " " + traceFile + " " + model;
    int sageStatus = system(pythonCommand.c_str());
    if (sageStatus < 0) {
        cout << "An Error occured while executing Sagemath" << endl;
        return nullptr;
    }
    cout << "Sagemath executed successfully. Calculating Results" << endl;
    vector<vector<double>> transitionmatrix;
    vector<vector<double>> emissionmatrix;
    vector<double> initialprobabilities;
    readFile(transitionmatrix, emissionmatrix, initialprobabilities);

    if (model == "gilbert") {
        params[0] = transitionmatrix[0][1];
        params[1] = transitionmatrix[1][0];
        params[2] = emissionmatrix[0][1];
        params[3] = emissionmatrix[1][1];
        cout << "p=" << transitionmatrix[0][1] << " r=" << transitionmatrix[1][0] << " k=" << emissionmatrix[0][1]
             << " h=" << emissionmatrix[1][1] << endl;
    } else if (model == "gilbertelliot") {
        params[0] = transitionmatrix[0][1];
        params[1] = transitionmatrix[1][0];
        params[2] = emissionmatrix[0][1];
        params[3] = emissionmatrix[1][1];
        cout << "p=" << transitionmatrix[0][1] << " r=" << transitionmatrix[1][0] << " k=" << emissionmatrix[0][1]
             << " h=" << emissionmatrix[1][1] << endl;
    } else if (model == "markov") {
        params[0] = transitionmatrix[0][2];
        params[1] = transitionmatrix[2][0];
        params[2] = transitionmatrix[2][1];
        params[3] = transitionmatrix[1][2];
        params[4] = transitionmatrix[0][3];
        cout << "p13=" << transitionmatrix[0][2] << " p31=" << transitionmatrix[2][0] << " p32="
             << transitionmatrix[2][1] << " p23=" << transitionmatrix[1][2] << " p14=" << transitionmatrix[0][3]
             << " p41=" << transitionmatrix[3][0] << endl;
    }
    return params;
}

/**
 * Mögliche aufrufe:
 * ext outPath clientTrace serverTrace globalClientIp \\(extract)
 * exg outPath clientTrace serverTrace globalClientIp model numPackets\\(extract and generate)
 * gen outPath [numPacktes] [model] [params] \\(generate)
 */
int main(int argc, char **argv) {
    string params[argc];
    for (int i = 0; i < argc; i++) {
        params[i] = argv[i];
    }

    string outPath = argv[2];

    if (params[1] == "gen") {
        /*
         * Modell aus Parametern generieren
         */
        auto numPackets = static_cast<unsigned long>(atoll(argv[3]));
        string modelName = params[4];
        int paramCount = argc - 5;
        string modelParams[paramCount];
        for (int i = 0; i < paramCount; i++) {
            modelParams[i] = argv[i + 5];
        }
        double *parsedParams;
        parsedParams = parseParams(parseModelName(modelName), modelParams, parsedParams);
        double wtfParams[5];
        for (int i = 0; i < 5; i++) {
            wtfParams[i] = parsedParams[i];
        }
        vector<bool> result = generate(parseModelName(modelName), numPackets, wtfParams);
        writeLossToFile(result, outPath);
    } else if (params[1] == "ext" || params[1] == "exg") {
        /*
         * Real Trace extrahieren
         */
        string clientTrace = params[3];
        string serverTrace = params[4];
        string globalClientIp = params[5];

        PcapParser parser;
        struct result pcapResult = parser.startParsing(clientTrace, serverTrace, globalClientIp);

        writeResultToFile(pcapResult, outPath);

        if (params[1] == "exg") {
            auto *parameter = new double[5];
            PacketLossModelType model = parseModelName(params[6]);
            if (model == BERNOULLI) {
                BernoulliParser bernoulliParser;
                parameter = bernoulliParser.parseParameter(pcapResult.loss);
                cout << "p: " << parameter[0] << endl;
            } else if (model == SIMPLE_GILBERT) {
                SimpleGilbertParser simpleGilbertParser;
                parameter = simpleGilbertParser.parseParameter(pcapResult.loss);
                cout << "p: " << parameter[0] << endl;
                cout << "r: " << parameter[1] << endl;
            } else {
                parameter = startSageMath("../SageBaumWelch.py", outPath + "/downloadLoss.txt", params[6], parameter);
            }
            vector<bool> result = generate(model, stoll(params[7]), parameter);
            pcapResult.loss = result;
        }
    }

    return 0;
}