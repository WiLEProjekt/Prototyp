# Prototyp
## OneDrive Verzeichnis
https://1drv.ms/f/s!Ag0XrlR639VItTpaLc9gP4oNXFr4
## Systemvorraussetzungen
Zum erstellen des Programms wird libpcap benötigt.

```
 sudo apt-get install libpcap-dev
```

Für den Buildprozess wird [cmake](https://cmake.org/) genutzt.

## Aufrufe 
Startet den ModelParameteriser. Die Parameter werden über ein Menü abgefragt.
```
./ModelParameteriser 
```
### Optionen
Im folgenden ist ein Trace eine Datei, die nur aus 0 (Paketverlust) und 1 (Kein Packetverlust) besteht.

Das Programm lässt sich mit folgenden Optionen ausführen:
#### gen
```
gen [outputfile] [number of packets] [model] [args...]
```
generiert einen Trace mit [number of packets] Paketen, den Model [model] und nutzt die Parameter [args...] für das Model. Der Trace wird dann in die Datei [outputfile] gespeichert.

#### ext
```
ext [output-path] [clientTrace.pcap] [serverTrace.pcap] [global client-ip]
```
extrahiert Netzwerkparameter aus den .pcap-Dateien [clientTrace.pcap] und [serverTrace.pcap]. Die [global client-ip] ist die globale IP-Adresse des Clients. Die extrahierten Parameter werden in den Ordner [output-path] geschrieben.

#### exg
```
exg [output-path] [clientTrace.pcap] [serverTrace.pcap] [global client-ip] [model] [number of packets]
```
extrahiert Netzwerkparameter aus den .pcap-Dateien [clientTrace.pcap] und [serverTrace.pcap], erzeugt aus den Loss-Trace Parameter für das Modell [model] und generiert einen neuen Trace mit [number of packets] Paketen. Die [global client-ip] ist die globale IP-Adresse des Clients. Die extrahierten Parameter und der generierte Loss-Trace werden in den Ordner [output-path] geschrieben.

### Wichtige Beispiele
#### Menü starten um weitere Parameter einzugeben
```
./ModelParameteriser
```
#### Trace aus Modellparameter generieren
./ModelParameteriser gen [outputfile] [number of packets] [modelname] [p] [r] [k] [h]
```
./ModelParameteriser gen /output/ gilbertelliot 200000 0.2 0.7 0.97 0.05
```
#### Netzwerkparameter aus Netzwerkmittschnitt extrahieren.
./TraceGenerator ext [output-path] [clientTrace.pcap] [serverTrace.pcap] [global client-ip]
```
./TraceGenerator ext /output/ client.pcap server.pcap 84.135.166.24
```
#### Netzwerkparameter aus Netzwerkmittschnitt extrahieren. Parameterschätzung durchführen und damit mit unseren Modellen einen neuen Trace generieren
./ModelParameteriser exg [output-path] [clientTrace.pcap] [serverTrace.pcap] [global client-ip] [model] [number of packets]
```
./ModelParameteriser exg /output/ client.pcap server.pcap 84.135.166.24 markov 200000
```

## Autoren
* **Leonhard Brüggemann**
* **Alexander Ditt**
* **Domenic Laniewski**
* **Dennis Rieke** 
