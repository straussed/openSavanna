/*
 *  tHMM.cpp
 *  HMMBrain
 *
 *  Created by Arend on 9/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "tHMM.h"
//#define feedbackON

tHMMU::tHMMU(){
}

tHMMU::~tHMMU(){
	hmm.clear();
	sums.clear();
	ins.clear();
	outs.clear();
	posLevelOfFB.clear();
	negLevelOfFB.clear();
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
    ins.clear();
    outs.clear();
}
void tHMMU::setup(vector<unsigned char> &genome, int start){
	int i,j,k;
	ins.clear();
	outs.clear();
	k=(start+2)%genome.size();

	_xDim=1+(genome[(k++)%genome.size()]&3);
	_yDim=1+(genome[(k++)%genome.size()]&3);
	posFBNode=genome[(k++)%genome.size()]&(maxNodes-1);
	negFBNode=genome[(k++)%genome.size()]&(maxNodes-1);
	nrPos=genome[(k++)%genome.size()]&3;
	nrNeg=genome[(k++)%genome.size()]&3;
	//cout<<"setup "<<(int)genome[start+2]<<" "<<(int)xDim<<" "<<(int)yDim<<endl;
	ins.resize(_yDim);
	outs.resize(_xDim);
	posLevelOfFB.resize(nrPos);
	negLevelOfFB.resize(nrNeg);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%genome.size()]&(maxNodes-1);
	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+4+i)%genome.size()]&(maxNodes-1);
	for(i=0;i<nrPos;i++)
		posLevelOfFB[i]=(int)(1+genome[(k+8+i)%genome.size()]);
	for(i=0;i<nrNeg;i++)
		negLevelOfFB[i]=(int)(1+genome[(k+12+i)%genome.size()]);
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
	
	k=k+16;
	hmm.resize(1<<_yDim);
	sums.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++){
		hmm[i].resize(1<<_xDim);
		for(j=0;j<(1<<_xDim);j++){
//			hmm[i][j]=(genome[(k+j+((1<<yDim)*i))%genome.size()]&1)*255;
			hmm[i][j]=genome[(k+j+((1<<_xDim)*i))%genome.size()];
			if(hmm[i][j]==0) hmm[i][j]=1;
			sums[i]+=hmm[i][j];
		}
	}
}

void tHMMU::setupQuick(vector<unsigned char> &genome, int start){
	int i,j,k;
	ins.clear();
	outs.clear();
	k=(start+2)%genome.size();
	
	_xDim=1+(genome[(k++)%genome.size()]&3);
	_yDim=1+(genome[(k++)%genome.size()]&3);
	posFBNode=genome[(k++)%genome.size()]&(maxNodes-1);
	negFBNode=genome[(k++)%genome.size()]&(maxNodes-1);
	nrPos=genome[(k++)%genome.size()]&3;
	nrNeg=genome[(k++)%genome.size()]&3;
	//cout<<"setup "<<(int)genome[start+2]<<" "<<(int)xDim<<" "<<(int)yDim<<endl;
	ins.resize(_yDim);
	outs.resize(_xDim);
	posLevelOfFB.resize(nrPos);
	negLevelOfFB.resize(nrNeg);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%genome.size()]&(maxNodes-1);
	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+4+i)%genome.size()]&(maxNodes-1);
	for(i=0;i<nrPos;i++)
		posLevelOfFB[i]=(int)(1+genome[(k+8+i)%genome.size()]);
	for(i=0;i<nrNeg;i++)
		negLevelOfFB[i]=(int)(1+genome[(k+12+i)%genome.size()]);
	chosenInPos.clear();
	chosenInNeg.clear();
	chosenOutPos.clear();
	chosenOutNeg.clear();
	
	k=k+16;
	hmm.resize(1<<_yDim);
	sums.resize(1<<_yDim);
	for(i=0;i<(1<<_yDim);i++){
		hmm[i].resize(1<<_xDim);
		for(j=0;j<(1<<_xDim);j++)
			hmm[i][j]=0;
		hmm[i][genome[(k+j+((1<<_xDim)*i))%genome.size()]&((1<<_xDim)-1)]=255;
		sums[i]=255;
	}
	
}

void tHMMU::update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap)
{
	int I=0;
	int i,j,r;
#ifdef feedbackON
    unsigned char mod;
    
	if((nrPos!=0)&&(states[nodeMap[posFBNode]]==1))
    {
		for(i=0;i<chosenInPos.size();i++)
        {
			mod=(unsigned char)(rand()%(int)posLevelOfFB[i]);
			if((hmm[chosenInPos[i]][chosenOutPos[i]]+mod)<255)
            {
				hmm[chosenInPos[i]][chosenOutPos[i]]+=mod;
				sums[chosenInPos[i]]+=mod;
			}
		}
	}
	if((nrNeg!=0)&&(states[nodeMap[negFBNode]]==1))
    {
		for(i=0;i<chosenInNeg.size();i++)
        {
			mod=(unsigned char)(rand()%(int)negLevelOfFB[i]);
			if((hmm[chosenInNeg[i]][chosenOutNeg[i]]-mod)>0)
            {
				hmm[chosenInNeg[i]][chosenOutNeg[i]]-=mod;
				sums[chosenInNeg[i]]-=mod;
			}
		}
	}
#endif
    
	for(vector<unsigned int>::iterator it = ins.begin(), end = ins.end(); it != end; ++it)
    {
		I=(I<<1)+((states[nodeMap[*it]])&1);
    }
    
	r=1+(rand()%(sums[I]-1));
	j=0;
    //	cout<<I<<" "<<(int)hmm.size()<<" "<<(int)hmm[0].size()<<endl;
	while(r > hmm[I][j])
    {
		r -= hmm[I][j];
		++j;
	}
    
	for(i = 0; i < outs.size(); ++i)
    {
		newStates[nodeMap[outs[i]]] |= (j >> i) & 1;
    }
#ifdef feedbackON
	chosenInPos.push_back(I);
	chosenInNeg.push_back(I);
	chosenOutPos.push_back(j);
	chosenOutNeg.push_back(j);
	while(chosenInPos.size()>nrPos)
    {
        chosenInPos.pop_front();
    }
	while(chosenOutPos.size()>nrPos)
    {
        chosenOutPos.pop_front();
    }
	while(chosenInNeg.size()>nrNeg)
    {
        chosenInNeg.pop_front();
    }
	while(chosenOutNeg.size()>nrNeg)
    {
        chosenOutNeg.pop_front();
    }
#endif
}

void tHMMU::show(unsigned char *nodeMap){
	int i,j;
	cout<<"INS: ";
	for(i=0;i<ins.size();i++)
		cout<<(int)nodeMap[ins[i]]<<" ";
	cout<<endl;
	cout<<"OUTS: ";
	for(i=0;i<outs.size();i++)
		cout<<(int)nodeMap[outs[i]]<<" ";
	cout<<endl;
	for(i=0;i<hmm.size();i++){
		for(j=0;j<hmm[i].size();j++)
			cout<<" "<<(double)hmm[i][j]/sums[i];
		cout<<endl;
	}
	cout<<endl;
	cout<<"posFB: "<<(int)nodeMap[posFBNode]<<" negFB: "<<(int)nodeMap[negFBNode]<<endl;
	cout<<"posQue:"<<endl;
	for(i=0;i<posLevelOfFB.size();i++)
		cout<<(int)nodeMap[posLevelOfFB[i]]<<" ";
	cout<<endl;
	cout<<"negQue:"<<endl;
	for(i=0;i<negLevelOfFB.size();i++)
		cout<<(int)nodeMap[negLevelOfFB[i]]<<" ";
	cout<<endl;
/*
	for(i=0;i<hmm.size();i++){
		for(j=0;j<hmm[i].size();j++)
			cout<<(int)hmm[i][j]<<" ";
		cout<<endl;
	}
	*/
//	cout<<"------"<<endl;
}


/******** tStack ********/
tStack::tStack(){
    
}
tStack::~tStack(){
    theStack.clear();
    ins.clear();
    outs.clear();
}
void tStack::setup(vector<unsigned char> &genome,int start){
	int i,k;
	ins.clear();
	outs.clear();
	k=(start+2)%(int)genome.size();
    
	_xDim=1+(genome[(k++)%(int)genome.size()]&3);
	_yDim=_xDim;
	ins.resize(_yDim);
	outs.resize(_xDim);
    pushNode=genome[(k++)%(int)genome.size()]&(maxNodes-1);
    popNode=genome[(k++)%(int)genome.size()]&(maxNodes-1);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%(int)genome.size()]&(maxNodes-1);
	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+4+i)%(int)genome.size()]&(maxNodes-1);
    theStack.clear();
}
void tStack::update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap){
    unsigned int V;
    int i;
    if(states[nodeMap[popNode]]==1){
        V=0;
        if(theStack.size()>0){
            V=theStack.back();
            theStack.pop_back();
        }
        for(i=0;i<outs.size();i++)
            newStates[nodeMap[outs[i]]]|=(V>>i)&1;
    }
    if(states[nodeMap[pushNode]]==1){
        V=0;
        for(i=0;i<outs.size();i++)
            V=(V<<1)+(states[nodeMap[ins[i]]]&1);
        theStack.push_back(V);
    }
}
void tStack::show(unsigned char *nodeMap){
    int i,j;
    cout<<"PUSH BIT: "<<(int)nodeMap[pushNode]<<endl;
    cout<<"POP BIT: "<<(int)nodeMap[popNode]<<endl;
	cout<<"INS: ";
	for(i=0;i<ins.size();i++)
		cout<<(int)nodeMap[ins[i]]<<" ";
	cout<<endl;
	cout<<"OUTS: ";
	for(i=0;i<outs.size();i++)
		cout<<(int)nodeMap[outs[i]]<<" ";
	cout<<endl;
}

/********* tCounter ************/
tCounter::tCounter(){
    
}
tCounter::~tCounter(){
    ins.clear();
    outs.clear();
}
void tCounter::setup(vector<unsigned char> &genome,int start){
	int i,k;
	ins.clear();
	outs.clear();
	k=(start+2)%(int)genome.size();
	_xDim=1+(genome[(k++)%(int)genome.size()]&3);
	_yDim=1+(genome[(k++)%(int)genome.size()]&7);
	ins.resize(_yDim);
	outs.resize(_xDim);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%genome.size()]&(maxNodes-1);
 	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+8+i)%genome.size()]&(maxNodes-1);
    counter=0;
}
void tCounter::update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap){
    unsigned int I=0,i=0;
    for(vector<unsigned int>::iterator it = ins.begin(), end = ins.end(); it != end; ++it)
		I=(I<<1)+((states[nodeMap[*it]])&1);
    counter+=I;
    for(i=0;i<outs.size();i++)
		newStates[nodeMap[outs[i]]] |= (counter>>i) & 1;
   // printf(".");
}
void tCounter::show(unsigned char *nodeMap){
    int i,j;
	cout<<"INS: ";
	for(i=0;i<ins.size();i++)
		cout<<(int)nodeMap[ins[i]]<<" ";
	cout<<endl;
	cout<<"OUTS: ";
	for(i=0;i<outs.size();i++)
		cout<<(int)nodeMap[outs[i]]<<" ";
	cout<<endl;
}

/***** tThreshold *****/
tThreshold::tThreshold(){
    threshold=0;
    counter=0;
}
tThreshold::~tThreshold(){
    ins.clear();
    outs.clear();
}
void tThreshold::setup(vector<unsigned char> &genome,int start){
	int i,k;
	ins.clear();
	outs.clear();
	k=(start+2)%(int)genome.size();
	_xDim=1+(genome[(k++)%(int)genome.size()]&3);
	_yDim=1+(genome[(k++)%(int)genome.size()]&3);
	threshold=(genome[(k++)%(int)genome.size()]&255);
	ins.resize(_yDim);
	outs.resize(_xDim);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%genome.size()]&(maxNodes-1);
 	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+4+i)%genome.size()]&(maxNodes-1);
    counter=0;
}
void tThreshold::update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap){
    unsigned int I=0,i=0;
    for(vector<unsigned int>::iterator it = ins.begin(), end = ins.end(); it != end; ++it)
		I=(I<<1)+((states[nodeMap[*it]])&1);
    counter+=I;
    if(counter>threshold){
        for(i=0;i<outs.size();i++)
            newStates[nodeMap[outs[i]]] |= 1;
        counter=0;
    }
   // printf(".");
}
void tThreshold::show(unsigned char *nodeMap){
    int i,j;
    cout<<"THRESHOLD: "<<(int)threshold<<endl;
	cout<<"INS: ";
	for(i=0;i<ins.size();i++)
		cout<<(int)nodeMap[ins[i]]<<" ";
	cout<<endl;
	cout<<"OUTS: ";
	for(i=0;i<outs.size();i++)
		cout<<(int)nodeMap[outs[i]]<<" ";
	cout<<endl;
}

/***** tROM *****/
tROM::tROM(){
}
tROM::~tROM(){
    ins.clear();
    outs.clear();
    ROM.clear();
}
void tROM::setup(vector<unsigned char> &genome,int start){
    int i,k;
	ins.clear();
	outs.clear();
    ROM.clear();
	k=(start+2)%(int)genome.size();
	_xDim=1+(genome[(k++)%(int)genome.size()]&7);
	_yDim=_xDim;
	ins.resize(_yDim);
	outs.resize(_xDim);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%(int)genome.size()]&(maxNodes-1);
	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+7+i)%(int)genome.size()]&(maxNodes-1);
    for(i=0;i<1<<_xDim;i++)
        ROM.push_back(genome[(k++)%(int)genome.size()]);
}
void tROM::update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap){
    unsigned int I=0,i=0;
    for(vector<unsigned int>::iterator it = ins.begin(), end = ins.end(); it != end; ++it)
		I=(I<<1)+((states[nodeMap[*it]])&1);
    for(i=0;i<outs.size();i++)
		newStates[nodeMap[outs[i]]] |= (ROM[I]>>i) & 1;
}
void tROM::show(unsigned char *nodeMap){
    int i,j;
    unsigned char mask;
    mask=(1<<ins.size())-1;
	cout<<"INS: ";
	for(i=0;i<ins.size();i++)
		cout<<(int)nodeMap[ins[i]]<<" ";
	cout<<endl;
	cout<<"OUTS: ";
	for(i=0;i<outs.size();i++)
		cout<<(int)nodeMap[outs[i]]<<" ";
	cout<<endl;
	for(i=0;i<ROM.size();i++)
        cout<<(int)(ROM[i]&mask)<<" ";
	cout<<endl;
}

/***** buffer queue *****/
tQueue::tQueue(){
    
}
tQueue::~tQueue(){
    ins.clear();
    outs.clear();
    theQueue.clear(); 
}
void tQueue::setup(vector<unsigned char> &genome,int start){
    int i,k;
	ins.clear();
	outs.clear();
	k=(start+2)%(int)genome.size();
	_xDim=1+(genome[(k++)%(int)genome.size()]&7);
	_yDim=_xDim;
    length=1+(genome[(k++)%(int)genome.size()]&15);
	ins.resize(_yDim);
	outs.resize(_xDim);
	for(i=0;i<_yDim;i++)
		ins[i]=genome[(k+i)%(int)genome.size()]&(maxNodes-1);
	for(i=0;i<_xDim;i++)
		outs[i]=genome[(k+7+i)%(int)genome.size()]&(maxNodes-1);
}
void tQueue::update(unsigned char *states, unsigned char *newStates,unsigned char *nodeMap){
    unsigned int I=0,i=0;
    for(vector<unsigned int>::iterator it = ins.begin(), end = ins.end(); it != end; ++it)
		I=(I<<1)+((states[nodeMap[*it]])&1);
    theQueue.push_back(I);
    if(theQueue.size()>length){
        I=theQueue.front();
        theQueue.erase(theQueue.begin());
    }
    else
        I=0;
    for(i=0;i<outs.size();i++)
		newStates[nodeMap[outs[i]]] |= (I>>i) & 1;
}
void tQueue::show(unsigned char *nodeMap){
    
}

