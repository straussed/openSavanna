/*
 *  tAgent.cpp
 *  HMMBrain
 *
 *  Created by Arend on 9/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "tAgent.h"

tAgent::tAgent(){
	int i;
	nrPointingAtMe=1;
	ancestor=NULL;
	for(i=0;i<maxNodes;i++){
		states[i]=0;
		newStates[i]=0;
        nodeMap[i]=(unsigned char) i;
	}
	hmmus.clear();
	food=0;
}

tAgent::~tAgent(){
	for(int i=0;i<hmmus.size();i++)
		delete hmmus[i];
	if(ancestor!=NULL){
		ancestor->nrPointingAtMe--;
		if(ancestor->nrPointingAtMe==0)
			delete ancestor;
	}
    genome.clear();
}

void tAgent::setupRandomAgent(int nucleotides){
	int i;
	genome.resize(nucleotides);
	for(i=0;i<nucleotides;i++)
		genome[i]=127;//rand()&255;
	ampUpStartCodons();
	setupPhenotype();
}
void tAgent::loadAgent(char* filename){
	FILE *f=fopen(filename,"r+t");
	int i;
	genome.clear();
	while(!(feof(f))){
		fscanf(f,"%i	",&i);
		genome.push_back((unsigned char)(i&255));
	}
	//setupPhenotype();
}
void tAgent::loadAgentWithTrailer(char* filename){
	FILE *f=fopen(filename,"r+t");
	int i;
	genome.clear();
	fscanf(f,"%i	",&i);
	while(!(feof(f))){
		fscanf(f,"%i	",&i);
		genome.push_back((unsigned char)(i&255));
	}
	setupPhenotype();
}


void tAgent::ampUpStartCodons(void){
	int i,j;
	for(i=0;i<genome.size();i++)
		genome[i]=rand()&255;
	for(i=0;i<4;i++)
	{
		j=rand()%(genome.size()-100);
		genome[j]=43;
		genome[j+1]=(255-43);
		for(int k=2;k<20;k++)
			genome[j+k]=rand()&255;
	}
}

void tAgent::inherit(tAgent *from,double mutationRate,int theTime){
 	int i,s,o,w;
    int nucleotides;
 	vector<unsigned char> buffer;
 	born=theTime;
	ancestor=from;
	from->nrPointingAtMe++;
    nucleotides=(int)from->genome.size();
	genome.clear();
	genome.resize(nucleotides);
    for(i=0;i<nucleotides;i++)
        if(((double)rand()/(double)RAND_MAX)<mutationRate)
            genome[i]=rand()&255;
        else
            genome[i]=from->genome[i];
	if((((double)rand()/(double)RAND_MAX)<0.05)&&(genome.size()<20000)){
		//duplication
		w=15+rand()&511;
		s=rand()%((int)genome.size()-w);
		o=rand()%(int)genome.size();
		buffer.clear();
		buffer.insert(buffer.begin(),genome.begin()+s,genome.begin()+s+w);
		genome.insert(genome.begin()+o,buffer.begin(),buffer.end());
	}
	if((((double)rand()/(double)RAND_MAX)<0.02)&&(genome.size()>1000)){
		//deletion
		w=15+rand()&511;
		s=rand()%((int)genome.size()-w);
		genome.erase(genome.begin()+s,genome.begin()+s+w);
	}
	setupPhenotype();
	fitness=0.0;
}
void tAgent::setupPhenotype(void){
	int i,j;
	if(hmmus.size()!=0)
		for(i=0;i<hmmus.size();i++)
			delete hmmus[i];
	hmmus.clear();
	for(i=0;i<genome.size();i++){
        // deterministic logic gate
		if((genome[i]==42)&&(genome[(i+1)%genome.size()]==(255-42))){
			tHMMU* hmmu=new tHMMU;
			hmmu->setupQuick(genome,i);
			hmmus.push_back(hmmu);
		}
        //probabilistic logic gate
		if((genome[i]==43)&&(genome[(i+1)%genome.size()]==(255-43))){
			tHMMU *hmmu=new tHMMU;
			hmmu->setup(genome,i);
			hmmus.push_back(hmmu);
		}
        /*
        //stack "gate"
		if((genome[i]==44)&&(genome[(i+1)%genome.size()]==(255-44))){
			tHMMU *hmmu=new tStack;
			hmmu->setup(genome,i);
			hmmus.push_back(hmmu);
		}
        //counter "gate"
		if((genome[i]==45)&&(genome[(i+1)%genome.size()]==(255-45))){
			tHMMU* hmmu=new tCounter;
			hmmu->setup(genome,i);
			hmmus.push_back(hmmu);
		}
        //threshold "gate"
		if((genome[i]==46)&&(genome[(i+1)%genome.size()]==(255-46))){
			tHMMU* hmmu=new tThreshold;
			hmmu->setup(genome,i);
			hmmus.push_back(hmmu);
		}
        //ROM "gate"
		if((genome[i]==47)&&(genome[(i+1)%genome.size()]==(255-47))){
			tHMMU* hmmu=new tROM;
			hmmu->setup(genome,i);
			hmmus.push_back(hmmu);
		}
        //que "gate"
		if((genome[i]==48)&&(genome[(i+1)%genome.size()]==(255-48))){
			tHMMU* hmmu=new tQueue;
			hmmu->setup(genome,i);
			hmmus.push_back(hmmu);
		}
         */
	}
}

unsigned char * tAgent::getStatesPointer(void){
	return states;
}

void tAgent::resetBrain(void){
	for(int i=0;i<maxNodes;i++){
		states[i]=0;
    }
}

void tAgent::updateStates(void){
    int i;
    for(i=0;i<hmmus.size();i++)
        hmmus[i]->update(&states[0],&newStates[0],&nodeMap[0]);
    for(i=0;i<maxNodes;i++){
        states[i]=newStates[i];
        newStates[i]=0;
    }
}

void tAgent::showBrain(void){
	for(int i=0;i<maxNodes;i++)
		cout<<(int)states[i];
	cout<<endl;
}

void tAgent::showPhenotype(void){
	for(int i=0;i<hmmus.size();i++)
		hmmus[i]->show(&nodeMap[0]);
	cout<<"------"<<endl;
}

void tAgent::saveToDot(char *filename){
	FILE *f=fopen(filename,"w+t");
	int i,j,k;
	fprintf(f,"digraph brain {\n");
	fprintf(f,"	ranksep=2.0;\n");
	for(i=0;i<4;i++)
		fprintf(f,"	%i [shape=invtriangle,style=filled,color=red];\n",i);
	for(i=4;i<14;i++)
		fprintf(f,"	%i [shape=circle,color=blue];\n",i);
	for(i=14;i<16;i++)
		fprintf(f,"	%i [shape=circle,style=filled,color=green];\n",i);
	for(i=0;i<hmmus.size();i++){
	//	fprintf(f,"	{\n");
		for(j=0;j<hmmus[i]->ins.size();j++){
			for(k=0;k<hmmus[i]->outs.size();k++)
				fprintf(f,"	%i	->	%i;\n",hmmus[i]->ins[j],hmmus[i]->outs[k]);
		}
	//	fprintf(f,"	}\n");
	}
	fprintf(f,"	{ rank=same; 0; 1; 2; 3;}\n"); 
	fprintf(f,"	{ rank=same; 4; 5; 6; 7; 8; 9; 10; 11; 12; 13;}\n"); 
	fprintf(f,"	{ rank=same; 14; 15; }\n"); 
	fprintf(f,"}\n");
	fclose(f);
}

void tAgent::saveEdgeList(char *filename){
	FILE *f=fopen(filename,"w+t");
	int i,j,k;
	for(i=0;i<hmmus.size();i++){
		for(j=0;j<hmmus[i]->ins.size();j++){
			for(k=0;k<hmmus[i]->outs.size();k++)
				fprintf(f,"%i   %i\n",hmmus[i]->ins[j],hmmus[i]->outs[k]);
		}
	}
	fclose(f);
}

void tAgent::saveToDotFullLayout(char *filename){
	FILE *f=fopen(filename,"w+t");
	int i,j,k;
	fprintf(f,"digraph brain {\n");
	fprintf(f,"	ranksep=2.0;\n");
	for(i=0;i<hmmus.size();i++){
		fprintf(f,"MM_%i [shape=box]\n",i);
		for(j=0;j<hmmus[i]->ins.size();j++)
			fprintf(f,"	t0_%i -> MM_%i\n",hmmus[i]->ins[j],i);
		for(k=0;k<hmmus[i]->outs.size();k++)
			fprintf(f,"	MM_%i -> t1_%i\n",i,hmmus[i]->outs[k]);
		
	}
	fprintf(f,"}\n");
}
void tAgent::saveGenome(FILE *f){
	int i;
	for(i=0;i<genome.size();i++)
		fprintf(f,"%i	",genome[i]);
	fprintf(f,"\n");
}




