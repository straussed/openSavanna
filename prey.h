//
//  prey.h
//  openSavanna
//
//  Created by Eli Strauss on 1/15/13.
//
//

#ifndef openSavanna_prey_h
#define openSavanna_prey_h


#include "globalConst.h"
#include "tHMM.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;


//class tDot{
//public:
//	double xPos,yPos;
//};

class prey{
public:
	vector<tHMMU*> hmmus;
	vector<unsigned char> genome;
    
	
	prey *ancestor;
	unsigned int nrPointingAtMe;
	//unsigned char states[maxNodes],newStates[maxNodes];
	double fitness,convFitness;
	vector<double> fitnesses;
	int capacity = 5;
	int xPos,yPos,direction;
	int ID;
	int born;
//    char sex;
//    unsigned char nodeMap[256];
//    double kin_flag;
//    double kin_thresh;
//    double get_random(double min, double max);
    
//	tAgent();
//	~tAgent();
	void setupRandomPrey(int nucleotides);
//    //    virtual void setupNodeMap(void);
	void loadPrey(char* filename);
//	void loadAgentWithTrailer(char* filename);
	void setupPhenotype(void);
	void inherit(prey *from,double mutationRate,int theTime);
//	unsigned char * getStatesPointer(void);
	void updateStates(void);
//	void resetBrain(void);
	void ampUpStartCodons(void);
//	void showBrain(void);
//	void showPhenotype(void);
//	void saveToDot(char *filename);
	//void saveToDotFullLayout(char *filename);
    //void saveEdgeList(char *filename);
    
	//void saveLOD(FILE *statsFile,FILE *genomeFile);
	//void saveGenome(FILE *f);
};



#endif
