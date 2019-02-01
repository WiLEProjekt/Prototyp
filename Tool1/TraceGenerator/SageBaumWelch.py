#
# This Script executes the Baum-Welch Algorithm of Sagemath on a Binary Loss Trace
# Currently the Packetlossmodels Gilbert, Gilbert-Elliot and 4-State Markov are supported, to be compatible with Netem
#
import sys, json
from sage.all import *

######
# Reads the binary loss trace as input, converts each element into an int and stores result in an array
######
def readTrace(name):
    file = open(name, 'r')
    stringdata = file.readlines()
    file.close()
    arraydata = []
    for i in range(len(stringdata[0])):
        arraydata.append(int(stringdata[0][i]))
    return(arraydata)

######
# Converts the Matrices into a string and writes them to file
######
def writeTextfile(model):
    tmat = model.transition_matrix()
    emat = model.emission_matrix()
    iprobs = model.initial_probabilities()
    tstring = ""
    estring = ""
    istring = ""
    for i in range(tmat.nrows()):
        for b in range(tmat.ncols()):
            tstring = tstring + str(tmat[i][b])+';'

    for i in range(emat.nrows()):
        for b in range(emat.ncols()):
            estring = estring + str(emat[i][b])+';'

    for i in range(len(iprobs)):
        istring = istring + str(iprobs[i])+';'

    with open('sageout.txt', 'w') as outfile:
        outfile.write(tstring+'\n')
        outfile.write(estring+'\n')
        outfile.write(istring)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: sage --python SageBaumWelch.py <complete path to binarytrace> <Model gilbert|gilbertelliot|markov>")
        print("applies the baum-welch algorithm on the given binary trace for a selected model")
        sys.exit(1)
    tracedata = readTrace(sys.argv[1])
    modelname = sys.argv[2]
    model = []
    emissions = [0,1]
    if modelname == "gilbert":
        model = hmm.DiscreteHiddenMarkovModel([[0.9,0.1],[0.1,0.9]], [[0.0,1.0],[0.95,0.05]], [0.5,0.5], emissions)
        model.baum_welch(tracedata)
    elif modelname == "gilbertelliot":
        model = hmm.DiscreteHiddenMarkovModel([[0.9,0.1],[0.1,0.9]], [[0.05,0.95],[0.95,0.05]], [0.5,0.5], emissions)
        model.baum_welch(tracedata)
    elif modelname == "markov":
        model = hmm.DiscreteHiddenMarkovModel([[0.7,0.0,0.25,0.05],[0.0,0.05,0.95,0.0],[0.01,0.01,0.98,0.0],[1.0,0.0,0.0,0.0]], [[0.0,1.0],[0.0,1.0],[1.0,0.0],[1.0,0.0]], [0.25,0.25,0.25,0.25], emissions)
        model.baum_welch(tracedata)
    else:
        print("No valid model selected. Select gilbert | gilbertelliot | markov")
        sys.exit(1)
    writeTextfile(model)
    sys.exit()