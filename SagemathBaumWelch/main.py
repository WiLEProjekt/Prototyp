import sys, json
from sage.all import *

def readTrace(name):
    file = open(name, 'r')
    stringdata = file.readlines()
    file.close()
    arraydata = []
    for i in range(len(stringdata[0])):
        arraydata.append(int(stringdata[0][i]))
    return(arraydata)

def writeJSON(model):
    tmat = model.transition_matrix()
    emat = model.emission_matrix()
    iprobs = model.initial_probabilities()
    data = {}
    data['transition_matrix'] = []
    data['emission_matrix'] = []
    data['initial_probabilities'] = []
    for i in range(tmat.nrows()):
        for b in range(tmat.ncols()):
            data['transition_matrix'].append({
                  'p'+str(i+1)+str(b+1): str(tmat[i][b])
            })

    for i in range(emat.nrows()):
        for b in range(emat.ncols()):
            data['emission_matrix'].append({
                  's'+str(i+1)+'p'+str(b): str(emat[i][b])
            })

    for i in range(len(iprobs)):
        data['initial_probabilities'].append({
            str(i+1):str(iprobs[i])
        })

    with open('sageout.json', 'w') as outfile:
        json.dump(data, outfile)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: sage --python main.py <complete path to binarytrace> <Model gilbert|gilbertelliot|markov>")
        print("applies the baum-welch algorithm on the given binary trace for a selected model")
        sys.exit(1)
    tracedata = readTrace(sys.argv[1])
    modelname=sys.argv[2]
    model = []
    emissions = [0,1]
    if sys.argv[2] == "gilbert":
        model = hmm.DiscreteHiddenMarkovModel([[0.9,0.1],[0.1,0.9]], [[0.0,1.0],[0.95,0.05]], [0.5,0.5], emissions)
        model.baum_welch(tracedata)
    elif sys.argv[2] == "gilbertelliot":
        model = hmm.DiscreteHiddenMarkovModel([[0.9,0.1],[0.1,0.9]], [[0.05,0.95],[0.95,0.05]], [0.5,0.5], emissions)
        model.baum_welch(tracedata)
    elif sys.argv[2] == "markov":
        model = hmm.DiscreteHiddenMarkovModel([[0.7,0.0,0.25,0.05],[0.0,0.05,0.95,0.0],[0.01,0.01,0.98,0.0],[1.0,0.0,0.0,0.0]], [[0.0,1.0],[0.0,1.0],[1.0,0.0],[1.0,0.0]], [0.25,0.25,0.25,0.25], emissions)
        model.baum_welch(tracedata)
    else:
        print("No valid model selected. Select gilbert | gilbertelliot | markov")
        sys.exit(1)
    writeJSON(model)