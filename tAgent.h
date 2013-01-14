/*
 *  tAgent.h
 *  HMMBrain
 *
 *  Created by Arend on 9/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _tAgent_h_included_
#define _tAgent_h_included_

#include "globalConst.h"
#include "tHMM.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define useANN
//#define freeNeurons

using namespace std;

class tDot{
public:
	double xPos,yPos;
};


class tAgent{
public:
	vector<tHMMU*> hmmus;
	vector<unsigned char> genome;

	
	tAgent *ancestor;
	unsigned int nrPointingAtMe;
	unsigned char states[maxNodes],newStates[maxNodes];
	double fitness,convFitness;
	vector<double> fitnesses;
	int food;
	int xPos,yPos,direction; 
	int ID;
	int born;
    char sex;
    unsigned char nodeMap[256];
    
	tAgent();
	~tAgent();
	void setupRandomAgent(int nucleotides);
//    virtual void setupNodeMap(void);
	void loadAgent(char* filename);
	void loadAgentWithTrailer(char* filename);
	void setupPhenotype(void);
	void inherit(tAgent *from,double mutationRate,int theTime);
	unsigned char * getStatesPointer(void);
	void updateStates(void);
	void resetBrain(void);
	void ampUpStartCodons(void);
	void showBrain(void);
	void showPhenotype(void);
	void saveToDot(char *filename);
	void saveToDotFullLayout(char *filename);
    void saveEdgeList(char *filename);
	
	void saveLOD(FILE *statsFile,FILE *genomeFile);
	void saveGenome(FILE *f);
};

#endif