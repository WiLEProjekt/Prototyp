# Prototyp - TraceGenerator
## Systemvorraussetzungen
Zum erstellen des Programms wird libpcap, build-essential benötigt.

sudo apt-get install libpcap-dev build-essential

Für den Buildprozess wird cmake genutzt.

## Aufruf
./TraceGenerator

zeigt alle möglichen Parameterangaben an.

### Optionen
Im folgenden ist ein Trace eine Datei, die nur aus 0 (Paketverlust) und 1 (Kein Packetverlust) besteht.

Das Programm lässt sich mit folgenden Optionen ausführen:
#### -gen (Hauptfunktionalität)
./TraceGenerator -gen [outputfile] [model] [Modellargumente (siehe -showmodel)]

generiert einen Trace mit den Model [model] und nutzt die Parameter für das Model. Der Trace wird dann in der Datei [outputfile] gespeichert.
#### -showmodel (Hilfsfunktionalität)
./TraceGenerator -showmodel

zeigt alle verfügbaren Paketverlustmodelle und die dafür benötigten Parameter an.
#### -extract (Hilfsfunktionalität - primär für uns zum Debuggen der Parameterschätzungsfunktionen)
./TraceGenerator -extract [tcp/icmp] [filename] [modelname] ([gMin])

extrahiert Modellparameter für das Model [modelname] aus der .pcap-Datei [filename]. Der erste Parameter [tcp/icmp] gibt an, welche Pakete im pcap-File untersucht werden sollen. Der Parameter ([gMin]) ist optional und gibt den Schwellwert für die Burstlänge für Markov- oder Gilbert-Elliot-Modelle an. Der Standardwert ist bei Markov 4 und bei Gilbert-Elliot 16 (RFC3611).

Wird der parameter "ping" übergeben, erwartet das Programm die in ein File geschriebene Terminalausgabe eines pings als Inputfile.

der Parameter [packetCount] gibt an wie viele Pakete beim Ping insgesamt gesendet wurden.

./TraceGenerator -extract ping [packetCount] [filename] [modelname] ([gMin])

#### -import (Hauptfunktionalität)
./TraceGenerator -import [tcp/icmp] [filename] [modelname] [outputfile] ([gMin]) ([seed])

extrahiert die Modellparameter genau wie -extract und benutzt die extrahierten Parameter als Eingabe für unseren Generator um mit ihnen einen neuen Trace zu erstellen [outputfile]. Mit dem optionalen Parameter [seed] kann ein seed für den Zufallsgenerator angegeben werden.

Alternative Eingabe ist wieder die Terminalausgabe eines pings.

./TraceGenerator -import ping [packetCount] [filename] [modelname] [outputfile] ([gMin]) ([seed])
#### -parse (Hauptfunktionalität)
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

# Prototyp - Validationtool
## Aufrufe
./validationtool inputfile outputfile

## Beschreibung
Das Validationtool ist ein kleines Hilfsprogramm zur Validierung der implementierten Paketverlustmodelle.

Es zählt die burst und gap sizes und schreibt sie in eine Datei.

Eine -4 steht z.B. für 4 aufeinanderfolgend verlorengegangene Pakete, eine 4 für 4 aufeinanderfolgend angekommene Pakete.

Diese Datei wird mit dem R-Script eingelesen und die ECDF berechnet