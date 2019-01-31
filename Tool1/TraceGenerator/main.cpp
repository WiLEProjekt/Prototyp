#include "TraceGenerator.h"
#include "PcapParser/PcapParser.h"

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
            newresult[i] = atof(params[i].c_str());
        }
        result = newresult;
        return result;
    } else if (model == GILBERT_ELLIOT) {
        paramCount = 4;
        double newresult[paramCount];
        for (int i = 0; i < paramCount; i++) {
            newresult[i] = atof(params[i].c_str());
        }
        result = newresult;
        return result;
    } else if (model == GILBERT) {
        paramCount = 4;
        double newresult[paramCount];
        newresult[0] = atof(params[0].c_str());
        newresult[1] = atof(params[1].c_str());
        newresult[2] = 1;
        newresult[3] = atof(params[2].c_str());
        result = newresult;
        return result;
    } else if (model == SIMPLE_GILBERT) {
        paramCount = 4;
        double newresult[paramCount];
        newresult[0] = atof(params[0].c_str());
        newresult[1] = atof(params[1].c_str());
        newresult[2] = 1;
        newresult[3] = 0;
        result = newresult;
        return result;
    } else if (model == BERNOULLI) {
        paramCount = 4;
        double newresult[paramCount];
        newresult[0] = atof(params[0].c_str());
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
 * Mögliche aufrufe:
 * ext outPath clientTrace serverTrace globalClientIp \\(extract)
 * exg outPath clientTrace serverTrace globalClientIp model \\(extract and generate)
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

        if (params[1] == "exg") {
            TraceGenerator generator;
            PacketLossModelType model = parseModelName(params[6]);
            ExtractParameter parameter = generator.extractModelParameter(model, pcapResult.loss, 0);
            vector<bool> result = generate(model, parameter.packetCount, parameter.parameter);
            pcapResult.loss = result;
        }
        writeResultToFile(pcapResult, outPath);
    }

    return 0;
}