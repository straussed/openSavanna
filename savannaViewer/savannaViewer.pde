//import processing.video.*;


//MovieMaker mm;  // Declare MovieMaker object
int xDim=256;
int yDim=256;
int[][] area=new int[xDim][yDim];

color[] col=new color[4];

int currentStep=-1;

String data;

int frame=1000000;


void drawArea(){
  background(0);
  String[] L=splitTokens(data,";");
  for(int t=0;t<L.length;t++){
    String[] D=splitTokens(L[t],",");
    int x=Integer.parseInt(D[0]);
    int y=Integer.parseInt(D[1]);
    int r=Integer.parseInt(D[2]);
    int g=Integer.parseInt(D[3]);
    int b=Integer.parseInt(D[4]);
    fill(color(r,g,b));
    rect(x*2,y*2,2,2);
  }
}


void setup()
{
  size(xDim*2,yDim*2);
// mm = new MovieMaker(this, width, height, "drawing.mov",
//                       30, MovieMaker.H263, MovieMaker.HIGH);
  background(#000000);
  fill(0);
  noStroke();
  col[0]=color(0,0,0);
  col[1]=color(255,100,100);
  col[2]=color(100,255,50);
  col[3]=color(255,255,255);
}

void keyReleased(){
  currentStep=0;
}
 
void draw()
{
    BufferedReader in;
    PrintStream out;
    Socket s;
    try{
      String host = "127.0.0.1";   // change
 
      //text( "Connecting to " + host + " ...", 10, 20 );
      s = new Socket( host, 2002 );
 
      //text( "Connection established to " + s.getInetAddress(), 10, 30 );
 
      in = new BufferedReader( new InputStreamReader( s.getInputStream() ) );
      //out = new PrintStream( s.getOutputStream() );
      data=in.readLine();
      in.close();
      //println(data);
      currentStep=0;
      drawArea();
    }
    catch ( Exception e )
    {
      //println( "Error" + e );
    }
} 

void keyPressed() {
  if (key == ' ') {
   // mm.finish();  // Finish the movie if space bar is pressed!
  }
}

