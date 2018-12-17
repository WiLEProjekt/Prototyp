import sys
from sage.all import *

def readTrace(name):
    file = open(name, 'r')
    stringdata = file.readlines()
    file.close()
    arraydata = []
    for i in range(len(stringdata[0])):
        arraydata.append(int(stringdata[0][i]))
    return(arraydata)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: sage --python main.py <complete path to binarytrace> <Model gilbert|gilbertelliot|markov>")
        print("applies the baum-welch algorithm on the given binary trace for a selected model")
        sys.exit(1)
    tracedata = readTrace(sys.argv[1])
    emissions = [0,1]
    if sys.argv[2] == "gilbert":
        gilbertmodel = hmm.DiscreteHiddenMarkovModel([[0.9,0.1],[0.1,0.9]], [[0.0,1.0],[0.95,0.05]], [0.5,0.5], emissions)
        print("Initial Model:")
        print(gilbertmodel)
        gilbertmodel.baum_welch(tracedata)
        print("Baum Welch Model:")
        print(gilbertmodel)
    elif sys.argv[2] == "gilbertelliot":
        gilbertelliotmodel = hmm.DiscreteHiddenMarkovModel([[0.9,0.1],[0.1,0.9]], [[0.05,0.95],[0.95,0.05]], [0.5,0.5], emissions)
        print(gilbertelliotmodel)
        gilbertelliotmodel.baum_welch(tracedata)
        print("Baum Welch Model:")
        print(gilbertelliotmodel)
    elif sys.argv[2] == "markov":
        markovmodel = hmm.DiscreteHiddenMarkovModel([[0.7,0.0,0.25,0.05],[0.0,0.05,0.95,0.0],[0.01,0.01,0.98,0.0],[1.0,0.0,0.0,0.0]], [[0.0,1.0],[0.0,1.0],[1.0,0.0],[1.0,0.0]], [0.25,0.25,0.25,0.25], emissions)
        print(markovmodel)
        markovmodel.baum_welch(tracedata)
        print("Baum Welch Model:")
        print(markovmodel)
    else:
        print("No valid model selected. Select gilbert | gilbertelliot | markov")