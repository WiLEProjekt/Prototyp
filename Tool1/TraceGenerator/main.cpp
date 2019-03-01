#include "TraceGenerator.h"
#include "PcapParser/PcapParser.h"
#include "Packetloss/PacketLossModelType.h"
#include "Packetloss/PacketlossParameterParser/BernoulliParser.h"
#include <string>
#include <iostream>
#include "Packetloss/PacketlossParameterParser/SimpleGilbertParser.h"

enum ProgramFunction{
    EXTRACT = 1,
    GENERATE = 2,
    EXTRACT_GENERATE = 3
};

int main(int argc, char** argv);

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

vector<string> listAllTraces(){
    vector<string> directories;
    string path = "../Traces/";
    DIR *dir;
    int counter = 1;
    struct dirent *ent;
    if ((dir = opendir (path.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            directories.push_back(ent->d_name);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror (("Could not open directory: " + path).c_str());
    }
    directories.pop_back();
    directories.pop_back();
    for(const string &directory : directories){
        cout << "[" << counter++ << "] " << directory << endl;
    }
    return directories;
}

string getClientIpFromDirectory(const string& directory){
    string path = "../Traces/" + directory;
    ifstream ipFile;
    ipFile.open(path + "/clientIp.txt");
    string clientIp;
    ipFile >> clientIp;
    ipFile.close();
    return clientIp;
}

void getExtractParams(){
    string outpath;
    string clientPcap;
    string serverPcap;
    string clientIp;
    cout << "output path:" << endl;
    cin >> outpath;
    cout << "Trace:" << endl;
    vector<string> directories = listAllTraces();
    int directoryId;
    cin >> directoryId;
    string directory = directories[directoryId - 1];
    clientPcap = "../Traces/" + directory + "/client.pcap";
    serverPcap = "../Traces/" + directory + "/server.pcap";
    clientIp = getClientIpFromDirectory(directory);

    int argc = 6;
    const auto **argv = new const char*[argc];
    argv[0] = "TraceGenerator";
    argv[1] = "ext";
    argv[2] = outpath.c_str();
    argv[3] = clientPcap.c_str();
    argv[4] = serverPcap.c_str();
    argv[5] = clientIp.c_str();
    main(argc, (char**)argv);
}
/**
 * gen outPath [numPacktes] [model] [params]
 */
void getGenerateParams(){
    string outpath;
    string numPackets;
    string modelname;
    cout << "output path:" << endl;
    cin >> outpath;
    cout << "Number of generated packets" << endl;
    cin >> numPackets;
    cout << "Model:" << endl
         << "Bernoulli" << endl
         << "SimpleGilbert" << endl
         << "Gilbert" << endl
         << "GilbertElliot" << endl
         << "Markov" << endl;
    cin >> modelname;

    PacketLossModelType model = parseModelName(modelname);
    int argc = 5;
    const auto **argv = new const char*[9];
    argv[0] = "TraceGenerator";
    argv[1] = "gen";
    argv[2] = outpath.c_str();
    argv[3] = numPackets.c_str();
    argv[4] = modelname.c_str();
    if(model == BERNOULLI || model == SIMPLE_GILBERT || model == GILBERT || model == GILBERT_ELLIOT) {
        cout << "p:" << endl;
        string p;
        cin >> p;
        argv[5] = p.c_str();
        argc += 1;
        if(model != BERNOULLI) {
            cout << "r:" << endl;
            string r;
            cin >> r;
            argv[6] = r.c_str();
            argc += 1;
            if(model != SIMPLE_GILBERT) {
                cout << "h:" << endl;
                string h;
                cin >> h;
                argv[7] = h.c_str();
                argc += 1;
                if(model != GILBERT) {
                    cout << "k:" << endl;
                    string k;
                    cin >> k;
                    argv[8] = argv[7];
                    argv[7] = k.c_str();
                    argc += 1;
                }
            }
        }
    } else if(model == MARKOV){
        argc = 10;
        string p13;
        string p31;
        string p32;
        string p23;
        string p14;
        cout << "p13:" << endl;
        cin >> p13;
        cout << "p31:" << endl;
        cin >> p31;
        cout << "p32:" << endl;
        cin >> p32;
        cout << "p23:" << endl;
        cin >> p23;
        cout << "p14:" << endl;
        cin >> p14;
        argv[5] = p13.c_str();
        argv[6] = p31.c_str();
        argv[7] = p32.c_str();
        argv[8] = p23.c_str();
        argv[9] = p14.c_str();
    }
    main(argc, (char**)argv);
}

/**
 * 1. Funktion (extract/generate/extract-generate)
 * 2. Funktionsparams..
 * 3. output path
 */
void startMenu(){
    bool validCommand = false;
    while(!validCommand) {
        cout << "What do you need?" << endl << "[1] extract real trace" << endl << "[2] generate trace with model"
             << endl << "[3] extract model parameters from real trace" << endl;
        int command;
        cin >> command;
        switch (command) {
            case EXTRACT:
                getExtractParams();
                validCommand = true;
                break;
            case GENERATE:
                getGenerateParams();
                validCommand = true;
                break;
            case EXTRACT_GENERATE:
                validCommand = true;
                break;
            default:
                break;
        }
    }
}

/**
 * Mögliche aufrufe:
 * //menu
 * ext outPath clientTrace serverTrace globalClientIp \\(extract)
 * exg outPath clientTrace serverTrace globalClientIp model numPackets\\(extract and generate)
 * gen outPath [numPacktes] [model] [params] \\(generate)
 */
int main(int argc, char **argv) {
    if(argc == 1){
        startMenu();
    } else {
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
                    parameter = startSageMath("../SageBaumWelch.py", outPath + "/downloadLoss.txt", params[6],
                                              parameter);
                }
                vector<bool> result = generate(model, stoll(params[7]), parameter);
                pcapResult.loss = result;
                writeResultToFile(pcapResult, outPath);
            }
        } else {
            cout << "Invalid params" << endl;
        }
    }

    return 0;
}