
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
int ifood = 1000;
int maxFood = 2000;
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
   
    //Add prey items based on maxFood
    for(int i=0;i<food.size();i++){
		food[i]=new prey;
        food[i]->inherit(masterPrey,.01,0);
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
    //while(update<5000)    
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
                for(j=0;j<4;j++){
                    area[agent[i]->xPos][agent[i]->yPos]=_empty;
                    who[agent[i]->xPos][agent[i]->yPos]=NULL;
                    
                    // put agent's current facing into its sensors
                    agent[i]->states[0]=agent[i]->direction&1;
                    agent[i]->states[1]=(agent[i]->direction>>1)&1;
                    //check whats in the cell directly in front of agent
                    agent[i]->states[2]=(area[agent[i]->xPos+xm[agent[i]->direction]][agent[i]->yPos+ym[agent[i]->direction]])&1;
                    agent[i]->states[3]=(area[agent[i]->xPos+xm[agent[i]->direction]][agent[i]->yPos+ym[agent[i]->direction]]>>1)&1;
                    agent[i]->states[4]=(area[agent[i]->xPos+xm[agent[i]->direction]][agent[i]->yPos+ym[agent[i]->direction]]>>2)&1;
                    //update states
                    agent[i]->updateStates();
                }
                //evaluate actions
                action=((agent[i]->states[5]&1)<<2)+((agent[i]->states[6]&1)<<1)+(agent[i]->states[7]&1);
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
                        int targ_x = agent[i]->xPos+xm[agent[i]->direction];
                        int targ_y = agent[i]->yPos+ym[agent[i]->direction];
                        
                        switch(area[targ_x][targ_y]){
                            case _food:
                                agent[i]->food++;
                                cout << preyWho[targ_x][targ_y] << endl;
                                cout<<"prey location: "<<targ_x<<", "<<targ_y<<endl<<endl;
                                cout<<"agent location: "<<agent[i]->xPos<<", "<<agent[i]->yPos<<endl<<endl;
                                
                                preyWho[targ_x][targ_y]->capacity--;
                                if(preyWho[targ_x][targ_y]->capacity == 0){
                                    //when a carcass is depleted.
                                    for(int z =0; z < food.size();z++){
                                        if(food[z]->capacity == 0){
                                            food.erase(food.begin()+z);
                                            area[food[z]->xPos][food[z]->yPos]=_empty;
                                            preyWho[food[z]->xPos][food[z]->yPos]=NULL;
                                            //break;
                                        }
                                    }
                                    
                                    //preyWho[targ_x][targ_y]=NULL;
                                    //area[targ_x][targ_y]=_empty;

                                }

                                        
                                if(agent[i]->food>=5){
                                    tAgent *offspring=new tAgent();
                                    offspring->inherit(agent[i], 0.01, update);
                                    offspring->xPos=agent[i]->xPos+xm[agent[i]->direction-2];
                                    offspring->yPos=agent[i]->yPos-ym[agent[i]->direction-2];
                                    offspring->direction=rand()&3;
                                    agent[i]->food-=5;
                                    birth.push_back(offspring);
                                    area[offspring->xPos][offspring->yPos]=_agent;
                                    who[offspring->xPos][offspring->yPos]=offspring;
                                    //After reproducing, there is a chance of death
                                    if(randDouble<0.01){
                                        //death of an agent
                                        who[agent[i]->xPos][agent[i]->yPos]=NULL;
                                        area[agent[i]->xPos][agent[i]->yPos]=_empty;
                                        agent[i]->nrPointingAtMe--;
                                        if(agent[i]->nrPointingAtMe==0)
                                            delete agent[i];
                                        agent.erase(agent.begin()+i);
                                    }

                            
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
        j = 0;
        preyBirth.clear();
        while(j<food.size()){
            //prey reproduce, but only up to a certain point
            if(food.size()<maxFood){
                if(((update - (food[j]->born))+1)%(food[j]->reproRate) ==0){
                    prey *preyOffspring = new prey;
                    //cout<<"born"<<endl;
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
            //            //kill old prey
//            if((update - food[j]->born) > food[j]->lifespan){
//                area[food[j]->xPos][food[j]->yPos] = _empty;
//                delete preyWho[food[j]->xPos][food[j]->yPos];
//                //delete food[j];
//                food.erase(food.begin()+j);
//                cout<<"killed"<<endl;
//            }
            //cout<<j<<endl;
            j++;
            //cout<<"end"<<endl;
        }

        //add newborns to population
        agent.insert(agent.end(), birth.begin(), birth.end());
        food.insert(food.end(), preyBirth.begin(), preyBirth.end());
        //cout<<food.size()<<endl;
////        //add food to maintain prey
//        if(food.size() <= 2000){
//            for(i=0;i<10;i++){
//                x=2+(rand()%(xDim-4));
//                y=2+(rand()%(yDim-4));
//                if(area[x][y]==_empty){
//                    area[x][y]=_food;
//                    food.push_back(new prey);
//                    preyWho[x][y] = food.back();
//                }
//            }
//        }
        //cout<<update<<endl;
        //cout<<food.size()<<endl;
        //cout<<agent.size()<<endl;
        //*
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
//                            if(who[i][j]->kin_flag <= .2)
//                                sprintf(line,"%i,%i,255,0,0;",i,j);
//                            else if (who[i][j]->kin_flag <= .4)
//                                sprintf(line,"%i,%i,255,0,255;",i,j);
//                            else if (who[i][j]->kin_flag <= .6)
//                                sprintf(line,"%i,%i,0,0,255;",i,j);
//                            else if (who[i][j]->kin_flag <= .8)
//                                sprintf(line,"%i,%i,255,215,0;",i,j);
//                            else
//                                sprintf(line,"%i,%i,0,255,255;",i,j);
                    
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
