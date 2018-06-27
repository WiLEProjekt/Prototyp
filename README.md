# Prototyp
## Systemvorraussetzungen
Zum erstellen des Programms wird libpcap benötigt.
Es kann mit sudo apt-get install libpcap-dev installiert werden.

Für den Buildprozess wird cmake genutzt.

## Aufrufe 
./TraceGenerator 

zeigt alle möglichen Parameterangaben an.

### Optionen
Im folgenden ist ein Trace eine Datei, die nur aus 0 (Paketverlust) und 1 (Kein Packetverlust) besteht.

Das Programm lässt sich mit folgenden Optionen ausführen:
#### -gen
-gen [outputfile] [model] [args...]

generiert einen Trace mit den Model [model] und nutzt die Parameter [args...] für das Model. Der Trace wird dann in die Datei [outputfile] gespeichert.
#### -showmodel
-showmodel 

zeigt alle verfügbaren Modelle an.
#### -extract
-extract [tcp/icmp] [filename] [modelname] ([gMin])

extrahiert Modellparameter für das Model [modelname] aus der .pcap-Datei [filename]. Der erste Parameter [tcp/icmp] gibt an, welche Packete untersucht werden sollen. Der Parameter ([gMin]) ist optional und gibt den schwellwert für die Burstlänge für Markov- oder Gilbert-Elliot-Modelle an. Der Standardwert ist bei Markov 4 und bei Gilbert-Elliot 16.

-extract ping [packetCount] [filename] [modelname] ([gMin])

extrahiert Modellparameter für das Model [modelname] aus einem ping-log [filename], in dem [packetCount] viele Packete per Ping verschickt wurden. Der Parameter ([gMin]) ist optional und gibt den schwellwert für die Burstlänge für Markov- oder Gilbert-Elliot-Modelle an. Der Standardwert ist bei Markov 4 und bei Gilbert-Elliot 16.
#### -import
-import [tcp/icmp] [filename] [modelname] [outputfile] ([gMin]) ([seed])

extrahiert die Modellparameter genau wie -extract und generiert daraus eine neue Trace-Datei [outputfile]. Mit dem optionalen Parameter [seed] kann ein seed für die Trace-Generierung angegeben werden. 

-import ping [packetCount] [filename] [modelname] [outputfile] ([gMin]) ([seed])

extrahiert die Modellparameter genau wie -extract und generiert daraus eine neue Trace-Datei [outputfile]. Mit dem optionalen Parameter [seed] kann ein seed für die Trace-Generierung angegeben werden. 
#### -parse
Nimmt die Terminal-Ausgabe eines pings (z.B. ping -c 20 -i 0 131.173.33.228 >output.txt) als Eingabe und gibt 0,1 Losstrace aus
-parse -ping [filename] [packetCount] [outputfile]

Wandelt eine Pinglog-Datei [filename] mit [packetCount] vielen Packeten in eine Trace-Datei [outputfile] um.

-parse -pcap [tcp|icmp] [filename] [outputfile]

Wandelt eine .pcap-Datei [filename] in eine Trace-Datei [outputfile] um. [tcp/icmp] gibt an, ob tcp- oder icmp-Pakete untersucht werden sollen.

### Wichtige Beispiele
#### Trace aus Modellparameter als Eingabe generieren
./TraceGenerator -gen [outputfile] [modelname] [seed] [number of packets] [p] [r] [k] [h]
./TraceGenerator -gen GilbertElliotTrace.txt gilbertelliot 1 200000 0.2 0.7 0.97 0.05

#### PCAP Trace als Eingabe in einen 0,1 Losstrace umwandeln
./TraceGenerator -parse -pcap icmp input.pcap output.txt

#### PCAP Trace als Eingabe, Parameterschätzung durchführen und damit mit unseren Modellen einen neuen Trace generieren
./TraceGenerator -import [tcp/icmp] [filename] [modelname] [outputfile] ([gMin]) ([seed])

./TraceGenerator -import icmp inputfile.pcap gilbertelliot genGE.txt 16 1