/*
 *  tHMM.h
 *  HMMBrain
 *
 *  Created by Arend on 9/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _tHMM_h_included_
#define _tHMM_h_included_

#include <vector>
#include <deque>
#include <iostream>
#include "globalConst.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>

using namespace std;

class tHMMU{
public:
	vector<vector<unsigned char> > hmm;
	vector<unsigned int> sums;
	vector<unsigned int> ins,outs;
	unsigned char posFBNode,negFBNode;
	unsigned char nrPos,nrNeg;
	vector<int> posLevelOfFB,negLevelOfFB;
	deque<unsigned char> chosenInPos,chosenInNeg,chosenOutPos,chosenOutNeg;
	
	unsigned char _xDim,_yDim;
	tHMMU();
	virtual ~tHMMU();
	virtual void setup(vector<unsigned char> &genome, int start);
	void setupQuick(vector<unsigned char> &genome, int start);
	virtual void update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap);
	virtual void show(unsigned char *nodeMap);
	
};

class tStack:public tHMMU{
public:
    unsigned char pushNode;
    unsigned char popNode;
    vector<unsigned char> theStack;
    tStack();
    virtual ~tStack();
    virtual void setup(vector<unsigned char> &genome,int start);
	virtual void update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap);
    virtual void show(unsigned char *nodeMap);
};

class tCounter:public tHMMU{
public:
    unsigned int counter;
    tCounter();
    virtual ~tCounter();
    virtual void setup(vector<unsigned char> &genome,int start);
	virtual void update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap);
    virtual void show(unsigned char *nodeMap);
};

class tThreshold:public tHMMU{
public:
    unsigned int counter;
    unsigned int threshold;
    tThreshold();
    virtual ~tThreshold();
    virtual void setup(vector<unsigned char> &genome,int start);
	virtual void update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap);
    virtual void show(unsigned char *nodeMap);
};

class tROM:public tHMMU{
public:
    vector<unsigned char> ROM;
    tROM();
    virtual ~tROM();
    virtual void setup(vector<unsigned char> &genome,int start);
	virtual void update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap);
    virtual void show(unsigned char *nodeMap);
};
class tQueue:public tHMMU{
public:
    vector<unsigned char> theQueue;
    unsigned int length;
    tQueue();
    virtual ~tQueue();
    virtual void setup(vector<unsigned char> &genome,int start);
	virtual void update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap);
    virtual void show(unsigned char *nodeMap);
};
#endif