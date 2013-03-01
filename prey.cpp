//
//  prey.cpp
//  openSavanna
//
//  Created by Eli Strauss on 1/15/13.
//
//

#include "prey.h"

//
void prey::loadPrey(char* filename){
	FILE *f=fopen(filename,"r+t");
	int i;
	genome.clear();
	while(!(feof(f))){
		fscanf(f,"%i	",&i);
		genome.push_back((unsigned char)(i&255));
	}
	//setupPhenotype();
}


void prey::inherit(prey *from, double mutationRate,int theTime){
 	int i,s,o,w = 0;
    int nucleotides;
 	vector<unsigned char> buffer;
 	born=theTime;
	ancestor=from;
    //kin_flag = from->kin_flag;
    //kin_thresh = from->kin_thresh;
    
	//from->nrPointingAtMe++;
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
	//setupPhenotype();
	//fitness=0.0;
}

//
void prey::setupPhenotype(void){
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
//
void prey::setupRandomPrey(int nucleotides){
	int i;
	genome.resize(nucleotides);
	for(i=0;i<nucleotides;i++)
		genome[i]=127;//rand()&255;
	ampUpStartCodons();
	setupPhenotype();
}

void prey::showPhenotype(void){
	for(int i=0;i<hmmus.size();i++)
		hmmus[i]->show(&nodeMap[0]);
	cout<<"------"<<endl;
}
//
void prey::ampUpStartCodons(void){
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
//
//
