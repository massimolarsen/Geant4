import numpy as np
import matplotlib.pyplot as plt
import csv

path = 'C:\\Users\\larse\\source\\repos\\data\\'
fileName = '0Detector_4_4MeV'

numBins = 4000
incEnergy = 6.11*10**6
totCounts = np.zeros(numBins)
dubCounts = np.zeros((16, numBins))
total = 0
detectors = np.zeros(16)
multiplicity = np.zeros(16)
incidentDetector = 0



def ezDubs(detectors, dubCounts, index):
    
    double = np.nonzero(detectors)[0]
    dubDetector = double[np.nonzero(double != incidentDetector)[0]][0]
    
    dubCounts[dubDetector][index] = dubCounts[dubDetector][index]+1

    return(dubCounts)


def convert(num, energy):
    num = float(num)
    
    if energy == "MeV":
        num = num*10**6
        
    elif energy == "keV":
        num = num*10**3
        
    return(num)


def parseEnergy(line):
    #find energy units
    k = 0
    for i in line:
        if i == "keV" or i == "MeV" or i == "eV":
            break
        else:
            k = k + 1
            
    #get energy units and energy value
    energy = line[k]
    val = line[k-1]
        
    return(val,energy)


def parseDetector(line):
    #find Edep string
    k = 0
    for i in line:
        if i == "chamberNb:":
            break
        else:
            k = k + 1
    edep = line[k + 1]
    
    #get numbers before Edep
    k = 0
    for i in edep:
        if i == "E":
            break
        else:
            k = k + 1
    return(int(edep[:k]))

with open(path + fileName + '.txt') as infile:
    reader = csv.reader(infile, delimiter=' ')
    for i, line in enumerate(reader):
        
        #go down to collection
        if len(line) > 4 and line[3] == "Collection:":  
            
            for i, line in enumerate(reader):
                #start counting
                if len(line) > 15 and line[4] == "trackID:": 
                    
                    value, units = parseEnergy(line) 
                    detNumber = parseDetector(line)
                    
                    total = total + convert(value, units)
                    detectors[detNumber] = detectors[detNumber] + 1
                
                #stop once you hit the next collection
                if len(line) > 4 and line[3] == "Collection:":
                    if total != 0:
                        index = int(total/incEnergy*numBins)
                        totCounts[index] = totCounts[index] + 1
                        multiplicity[np.count_nonzero(detectors)] = multiplicity[np.count_nonzero(detectors)] + 1
                        
                        if np.count_nonzero(detectors) == 2:
                            dubCounts = ezDubs(detectors, dubCounts, index)
                        
                    detectors = np.zeros(16)
                    total = 0
                    continue
                
plt.yscale("log")
plt.ylabel("Counts")
plt.xlabel("Bin #")
plt.title(fileName)

for i in range(16):
    if np.count_nonzero(dubCounts[i]) != 0:
        plt.step(range(numBins), dubCounts[i], label=i)
        plt.plot(range(numBins), dubCounts[i], '.', alpha = 1)
    
plt.legend(title = 'Detector #:')
plt.savefig(fileName, dpi=1200)
plt.show()

#plt.bar(np.arange(16),multiplicity)      
#plt.savefig(fileName, dpi=1200)
    