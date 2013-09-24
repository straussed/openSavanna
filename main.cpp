
#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <map>
#include <math.h>
#include <time.h>
#include <iostream>
#include "globalConst.h"
#include "tHMM.h"
#include "tAgent.h"
#include "prey.h"
#define randDouble ((double)rand()/(double)RAND_MAX)

using namespace std;

//double replacementRate=0.1;
double perSiteMutationRate=0.005;
int update=0;
int repeats=1;
int maxAgent=1000;
int ifood = 4000;
int maxFood = 220;
int totalGenerations=100;
int successfull=0;

int xm[4]={0,1,0,-1};
int ym[4]={-1,0,1,0};


#define xDim 256
#define yDim 256
#define _empty 0
#define _agent 1
#define _food 2
#define _wall 3

#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "helper.h"           /*  our own helper functions  */

#define ECHO_PORT          (2002)
#define MAX_LINE           (100000)

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

int       list_s;                /*  listening socket          */
int       conn_s;                /*  connection socket         */
short int port;                  /*  port number               */
struct    sockaddr_in servaddr;  /*  socket address structure  */
char      buffer[MAX_LINE];      /*  character buffer          */
char     *endptr;                /*  for strtol()              */

void setupBroadcast(void);
void doBroadcast(string data);
int get_distance(tAgent* A1, tAgent* A2, int cd);

int main(int argc, char *argv[])
{

	vector<tAgent*>agent,birth;
    vector<prey*>food, preyBirth;
    prey* preyWho[xDim][yDim];
    tAgent* who[xDim][yDim];
    unsigned char area[xDim][yDim];
	tAgent *masterAgent;
    prey *masterPrey;
	int i,j,k,x,y,action;
	double maxFitness;
	int newBorn=0;
	FILE *resFile;
	FILE *LOD;
	FILE *genomeFile;
    FILE *F,*G,*R;
    srand(getpid());
    cout<<"attempt socked connection"<<endl;
    setupBroadcast();
    cout<<"attempt complete"<<endl;
	srand(getpid());
    //setup area and all
    agent.clear();
    for (i=0; i<xDim; i++) {
        for(j=0;j<yDim;j++){
            area[i][j]=_empty;
            who[i][j]=NULL;
            preyWho[i][j]=NULL;
            if(randDouble<0.01)
                area[i][j]=_wall;
//            else
//                if(randDouble<0.1){
//                    area[i][j]=_food;
//                    food[i] = new prey;
//                    food[i]->xPos = i;
//                    food[i]->yPos = j;
//                    food[i]->capacity = 5;
//                    preyWho[i][j] = food[i];
                //}
            
            if((i==0)||(j==0)||(i==xDim-1)||(j==yDim-1))
                area[i][j]=_wall;
        }
    }
	agent.resize(maxAgent);
    food.resize(ifood);
	masterAgent=new tAgent;
    masterPrey = new prey;
//	masterAgent->setupRandomAgent(5000);
    masterAgent->loadAgent((char*)"startGenome.txt");
    masterPrey->loadPrey((char*)"startGenome.txt");
    masterPrey->setupPhenotype();
	masterAgent->setupPhenotype();
    masterAgent->showPhenotype();
    masterPrey->showPhenotype();
    //exit(0);
//    masterAgent->saveGenome(genomeFile);
//    fclose(genomeFile);
	for(i=0;i<agent.size();i++){
		agent[i]=new tAgent;
		agent[i]->inherit(masterAgent,0.01,0);
        agent[i]->kin_flagR = agent[i]->get_random(0,255);
        agent[i]->kin_flagG = agent[i]->get_random(0,255);
        agent[i]->kin_flagB = agent[i]->get_random(0,255);
        agent[i]->kin_thresh = agent[i]->get_random(0,1);
        do{
            agent[i]->xPos=rand()%xDim;
            agent[i]->yPos=rand()%yDim;
            
        } while(area[agent[i]->xPos][agent[i]->yPos]!=_empty);
        agent[i]->direction=rand()&3;
        area[agent[i]->xPos][agent[i]->yPos]=_agent;
        who[agent[i]->xPos][agent[i]->yPos]=agent[i];
        //agent[i]->showPhenotype();
	}
   
    //Add prey items based on iFood
    for(int i=0;i<food.size();i++){
		food[i]=new prey;
        food[i]->inherit(masterPrey,0.01,0);
        do{
            food[i]->xPos=rand()%xDim;
            food[i]->yPos=rand()%yDim;
        } while(area[food[i]->xPos][food[i]->yPos] !=_empty);
        food[i]->direction=rand()&3;
        area[food[i]->xPos][food[i]->yPos]=_food;
        preyWho[food[i]->xPos][food[i]->yPos]=food[i];
        //agent[i]->showPhenotype();
	}

	masterAgent->nrPointingAtMe--;
	cout<<"setup complete"<<endl;
    while(agent.size()>0)
    //while(update<50000)
    {
        update++;
        i=0;
        birth.clear();
        //run through predators and do stuff
        while(i<agent.size()){
            if(randDouble<0.001){
                //death of an agent
                who[agent[i]->xPos][agent[i]->yPos]=NULL;
                area[agent[i]->xPos][agent[i]->yPos]=_empty;
                agent[i]->nrPointingAtMe--;
                if(agent[i]->nrPointingAtMe==0)
                    delete agent[i];
                agent.erase(agent.begin()+i);
            }
            else{
                //do agent
                //make inputs
                area[agent[i]->xPos][agent[i]->yPos]=_empty;
                who[agent[i]->xPos][agent[i]->yPos]=NULL;
                int localDist=0;
                if(who[agent[i]->xPos+xm[agent[i]->direction]][agent[i]->yPos+ym[agent[i]->direction]])
                    localDist=get_distance(agent[i], who[agent[i]->xPos+xm[agent[i]->direction]][agent[i]->yPos+ym[agent[i]->direction]], 0);
                for(j=0;j<4;j++){
                    // put agent's current facing into its sensors
                    agent[i]->states[0]=agent[i]->direction&1;
                    agent[i]->states[1]=(agent[i]->direction>>1)&1;
                    //check whats in the cell directly in front of agent
                    agent[i]->states[2]=(area[agent[i]->xPos+xm[agent[i]->direction]][agent[i]->yPos+ym[agent[i]->direction]])&1;
                    agent[i]->states[3]=(area[agent[i]->xPos+xm[agent[i]->direction]][agent[i]->yPos+ym[agent[i]->direction]]>>1)&1;
                    agent[i]->states[4]=(area[agent[i]->xPos+xm[agent[i]->direction]][agent[i]->yPos+ym[agent[i]->direction]]>>2)&1;
                    if(localDist>10)
                        agent[i]->states[5]=0;
                    else
                        agent[i]->states[5]=1;
                    //update states
                    agent[i]->updateStates();
                }
                //evaluate actions
                action=((agent[i]->states[6]&1)<<2)+((agent[i]->states[7]&1)<<1)+(agent[i]->states[8]&1);
                //action=rand()&3;
                switch(action){
                    case 0: // nop
                        break;
                    case 1: // turn left
                        agent[i]->direction=(agent[i]->direction+1)&3;
                        break;
                    case 2: // turn right
                        agent[i]->direction=(agent[i]->direction-1)&3;
                        break;
                    case 3: //move forward. If food in front, eat food and remain in same position
                        int targ_x = (agent[i]->xPos+xm[agent[i]->direction])&(xDim-1);
                        int targ_y = (agent[i]->yPos+ym[agent[i]->direction])&(yDim-1);
                        
                        switch(area[targ_x][targ_y]){
                            case _food:
                                if(preyWho[targ_x][targ_y]->capacity>0){
                                    agent[i]->food++;
                                    preyWho[targ_x][targ_y]->capacity--;
                                }

                                        
                                if(agent[i]->food>=5){
                                    tAgent *offspring=new tAgent();
                                    offspring->inherit(agent[i], 0.01, update);
                                    offspring->xPos=agent[i]->xPos;//+xm[agent[i]->direction-2];
                                    offspring->yPos=agent[i]->yPos;//-ym[agent[i]->direction-2];
                                    offspring->direction=rand()&3;
                                    agent[i]->food-=5;
                                    birth.push_back(offspring);
                                    //area[offspring->xPos][offspring->yPos]=_agent;
                                    //who[offspring->xPos][offspring->yPos]=offspring;
                                    
                            
                                }
                                break;
                            case _empty:
                                agent[i]->xPos+=xm[agent[i]->direction];
                                agent[i]->yPos+=ym[agent[i]->direction];
                                break;
                            default: //do nothing if not empty or food
                                break;
                        }
                }
                area[agent[i]->xPos][agent[i]->yPos]=_agent;
                who[agent[i]->xPos][agent[i]->yPos]=agent[i];
                i++;
            }
                
        }
        
        //Go through prey and do stuff
        j = food.size()-1;
        while(j>0){
            if(food[j]->capacity<=0){
                area[food[j]->xPos][food[j]->yPos]=_empty;
                preyWho[food[j]->xPos][food[j]->yPos]=NULL;
                delete food[j];
                food.erase(food.begin()+j);
            }
            j--;
        }
        j= 0;
        while(j < food.size()) {
            area[food[j]->xPos][food[j]->yPos]=_empty;
            preyWho[food[j]->xPos][food[j]->yPos]=NULL;
            //cout<<food[j]->xPos<<" "<<food[j]->yPos<<endl;
            //cout<<food[j]->xPos+xm[food[j]->direction]<<" "<<food[j]->yPos+ym[food[j]->direction]<<endl;
            for(k=0;k<4;k++){
                // put prey's current facing into its sensors
                food[j]->states[0]=food[j]->direction&1;
                food[j]->states[1]=(food[j]->direction>>1)&1;
                food[j]->states[2]=(area[food[j]->xPos+xm[food[j]->direction]][food[j]->yPos+ym[food[j]->direction]])&1;
                food[j]->states[3]=(area[food[j]->xPos+xm[food[j]->direction]][food[j]->yPos+ym[food[j]->direction]]>>1)&1;
                food[j]->states[4]=(area[food[j]->xPos+xm[food[j]->direction]][food[j]->yPos+ym[food[j]->direction]]>>2)&1;
                //update states
                food[j]->updateStates();
            }
            
            //evaluate actions
            action=((food[j]->states[6]&1)<<2)+((food[j]->states[7]&1)<<1)+(food[j]->states[8]&1);
            switch(action){
                case 0: // nop
                    break;
                case 1: // turn left
                    food[j]->direction=(food[j]->direction+1)&3;
                    break;
                case 2: // turn right
                    food[j]->direction=(food[j]->direction-1)&3;
                    break;
                case 3: //move forward if space is open
                    if(area[food[j]->xPos+xm[food[j]->direction]][food[j]->yPos+ym[food[j]->direction]] == _empty){
                        food[j]->xPos+=xm[food[j]->direction];
                        food[j]->yPos+=ym[food[j]->direction];
                        break;
                    }
                    else break;
            }
            area[food[j]->xPos][food[j]->yPos]=_food;
            preyWho[food[j]->xPos][food[j]->yPos]=food[j];
            j++;
        }
        preyBirth.clear();
        while(food.size()+preyBirth.size()<maxFood){
            j=rand()%food.size();
            {
                prey *preyOffspring = new prey;
                preyOffspring->inherit(food[j],0.01,update);
                //preyOffspring->capacity = preyOffspring->maxCapacity;
                do{
                    preyOffspring->xPos=rand()%xDim;
                    preyOffspring->yPos=rand()%yDim;
                    
                } while(area[preyOffspring->xPos][preyOffspring->yPos]!=_empty);
                preyBirth.push_back(preyOffspring);
                area[preyOffspring->xPos][preyOffspring->yPos]=_food;
                preyWho[preyOffspring->xPos][preyOffspring->yPos]=preyOffspring;
                
            }
        }

        //add newborns to population
        agent.insert(agent.end(), birth.begin(), birth.end());
        food.insert(food.end(), preyBirth.begin(), preyBirth.end());
        if((update&15)==0){
            //doBroadcast("a c string");
            string S;
            char line[100];
            int R,B,G;
            for(i=0;i<xDim;i++)
                for(j=0;j<yDim;j++){
                    switch(area[i][j]&3){//change the 3 to add more cases
                        case _food: sprintf(line,"%i,%i,34,139,34;",i,j); break;
                        case _agent:
                            R = who[i][j]->kin_flagR;
                            G = who[i][j]->kin_flagG;
                            B = who[i][j]->kin_flagB;
                            sprintf(line,"%i,%i,%i,%i,%i;",i,j,R,G,B); break;
                    
                        case _wall: sprintf(line,"%i,%i,255,255,255;",i,j); break;
                        case _empty: break;
                    }
                        S.append(line);
                    }
            doBroadcast(S);
          
        }
         //*/
        if((update&127)==0){
            cout<<update<<" "<<(int)agent.size()<<endl;
        }
        
        if(update == 15000){
            int distances[agent.size()][agent.size()];
            for (i = 0; i < agent.size(); i++){
                for (j =0; j < agent.size();j++) {
                    distances[i][j] = get_distance(agent[i],agent[j],0);
                    cout<<distances[i][j]<<endl;
                }
            }
        
        }
    }
    /*
    F=fopen("startGenome.txt","w+t");
    agent[0]->saveGenome(F);
    fclose(F);
     */
 
    return 0;
}


void setupBroadcast(void)
{
    port = ECHO_PORT;
	if ( (list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
		fprintf(stderr, "ECHOSERV: Error creating listening socket.\n");
		//exit(EXIT_FAILURE);
    }
	/*  Set all bytes in socket address structure to
	 zero, and fill in the relevant data members   */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(port);
	/*  Bind our socket addresss to the 
	 listening socket, and call listen()  */
    if ( bind(list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 )
    {
		fprintf(stderr, "ECHOSERV: Error calling bind()\n");
		exit(EXIT_FAILURE);
    }
    
    if ( listen(list_s, LISTENQ) < 0 )
    {
		fprintf(stderr, "ECHOSERV: Error calling listen()\n");
		exit(EXIT_FAILURE);
    }
    
}

int get_distance(tAgent *A1, tAgent *A2, int cd) {
    // Returns pedigree distance between A1 and A2
    if(A1 == A2)
        return 0;
    else if((A1->ancestor == A2) || (A2->ancestor == A1)) {
        return cd + 1;}
    else if(A1->born >= A2->born) {return get_distance(A1->ancestor, A2, cd+1);}
    else {return get_distance(A1, A2->ancestor, cd+1);}
    
    
}



void doBroadcast(string data)
{
    if ( (conn_s = accept(list_s, NULL, NULL) ) < 0 )
    {
        fprintf(stderr, "ECHOSERV: Error calling accept()\n");
        //exit(EXIT_FAILURE);
    }
    Writeline(conn_s, data.c_str(), data.length());
    
    if ( close(conn_s) < 0 )
    {
        fprintf(stderr, "ECHOSERV: Error calling close()\n");
        //exit(EXIT_FAILURE);
    }
}
