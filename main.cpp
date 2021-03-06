/* 
   File:   main
   Author: Omar Hernandez
   Created on November 28, 2016, 10:00 PM
   Purpose:  Sudoku
 */

//System Libraries
#include <iostream>   //Input/Output objects
#include <cstdlib>    //Random number generator
#include <ctime>      //rand
#include <fstream>    //File I/O
#include <string>     //String Library
#include <iomanip>    //formatting
#include <cmath>      //square root function
#include <vector>    //vectors for rankings
using namespace std;  //Name-space used in the System Library

//User Libraries
//#include "Victors.h"

//Global Constants
short const SIZE=9;     //2 dimensional array. As it is Sudoku, same amount of rows and columns

//Function prototypes
void rndPuz(char [][SIZE], bool);
char doRand9(char [][SIZE], int, int, short &);
char doRand4(char [][SIZE], int, int, short &);
void dsp9x9(char [][SIZE], char [][SIZE]);
void dsp4x4(char [][SIZE], char [][SIZE]);
void dspPuzz(char [][SIZE], bool, char[][SIZE]);
void chInput(char [][SIZE], char [][SIZE], char, char, short);
bool isSaved(char [][SIZE], char [][SIZE], char [][SIZE], bool=true);
bool isLoadd(char [][SIZE], char [][SIZE], char [][SIZE], bool *);
short difSel(bool);
void addRec();
void ldrBrd();
void markSrt(vector<int> &,vector<int> &,int);
void prntScr(vector <int> &, vector<string> &,vector <int> &,int);
void escape();

//Execution Begins Here!
int main(int argc, char** argv) {
    //set random number seed
    srand(static_cast<int>(time(0)));
    //declare variables
    char    puzzle[SIZE][SIZE];
    char    dfaults[SIZE][SIZE];   //values that have been left by randomization; parallel array
    char    cmplte[SIZE][SIZE];    //store completed puzzle for reference
    short   difLoop;      //difficulty
    bool    is9x9, isDone, isPlybl, isSolvd; //is 9x9 or 4x4, is done with the puzzle, is playable, is solved
    short   gridT;      //4x4 or 9x9
    char    menuIn;     //menu input
    char    lowerC, upperC, digit;    //number submit
    char    x, y;       //used in algorithm that determines which cells to edit
    string  wchGrid;    //difficulty select, which grid (4x4 or 9x9)
    
    //intro
    //fill intro with ? for intro
    for(int n=0; n<SIZE; n++){
        for(int i=0; i<SIZE; i++){
            puzzle[n][i]='?';
        }
    }
    
    cout<<"Sudoku!"<<endl;
    dspPuzz(puzzle, is9x9, dfaults);    //display puzzle with question marks for intro
    
    do{
        cout<<"Input P to create puzzle and play, L to load saved"<<endl
            <<"puzzle, B to display leaderboard, or E to exit."<<endl;
        cin>>menuIn;
    
        switch(menuIn){
            case'P':{
                //puzzle preference selection
                //choose 4x4 or 9x9 by inputting a string
                cout<<"Play 4x4 or 9x9?"<<endl;
                do{
                    cin.clear();
                    getline(cin, wchGrid);
                }while(wchGrid!="4x4"&&wchGrid!="9x9");

                //if input matches "9x9", set grid variables to suit 9x9
                if (wchGrid=="9x9"){
                    is9x9=true;
                    gridT=9;
                }
                //else "4x4", set grid variables to suit 4x4
                else {
                    is9x9=false;
                    gridT=4;
                }
                
                rndPuz(puzzle, is9x9);
                //store array to complete to give solution to player's that concede
                for(int n=0; n<SIZE; n++){
                    for(int i=0; i<SIZE; i++){
                        cmplte[n][i]=puzzle[n][i];
                    }
                }
                difLoop=difSel(is9x9);//difficulty select do-while loop
                
                //determine which cells to empty
                do{
                    //x -> row, y -> column
                    x=rand()%(gridT);
                    y=rand()%(gridT);

                    //if cells are already empty, do loop again
                    if(puzzle[x][y]!=' '){
                        puzzle[x][y]=' ';
                        difLoop--;
                    }
                }while(difLoop>0);

                //store array as is to be able to distinguish between values added by the
                //player and the values generated by the program
                for(int n=0; n<SIZE; n++){
                    for(int i=0; i<SIZE; i++){
                        dfaults[n][i]=puzzle[n][i];
                    }
                }
                isPlybl=true;
                break;
            }
            case 'B':ldrBrd(); break;
            case 'L': {
                isPlybl=isLoadd(puzzle, dfaults, cmplte, &isPlybl);
                //set grid type
                is9x9=true;
                gridT=9;
                break;
            }
            case 'E':{
                escape();
            }
        }
    }while(isPlybl==false);
    
    
    
    //now play the game
    do{
        dspPuzz(puzzle, is9x9, dfaults);
        isSolvd=false;
        isDone=false;        //escape from the loop
        cout<<endl;
        cout<<"Input column and row you want to edit."<<endl
            <<"Input"<<((is9x9)?" S to save progress and exit, ":" ")<<"X to give up."<<endl;
        cin.clear();
        cin>>upperC;
        if(upperC!='X'&&upperC!='S')cin>>lowerC;
        
        //save puzzle and exit
        if(upperC=='S'||lowerC=='S'){
            isDone=isSaved(puzzle, dfaults, cmplte, is9x9);
            isSolvd=false;
        }
        
        //exit puzzle and give solution
        else if(upperC=='X'||lowerC=='X'){
                isDone=true;
                isSolvd=false;
                cout<<"Solution: "<<endl;
                dspPuzz(cmplte, is9x9, cmplte);
        }

        else if ((upperC>='A'&&upperC<='I')&&(lowerC>='a'&&lowerC<='i')){
            //set column and row input appropriate value for loop
            upperC-='A';
            lowerC-='a';

            chInput(puzzle, dfaults, upperC, lowerC, gridT);
            isSolvd=true;
            
            isDone=true;
            for(int n=0; n<gridT; n++){
                for(int i=0; i<gridT; i++){
                    if(puzzle[n][i]==' ')isDone=false;
                }
            }
        }
    }while(isDone==false);
    
    if(isSolvd)dspPuzz(puzzle, is9x9, dfaults);
    cout<<((isSolvd)?"Victory!":"Play again!")<<endl;
    
    if(isSolvd&&is9x9)addRec();
    
    //Exit Program
    return 0;
}

//******************************* Leaderboard **********************************
//Purpose: read in names and scores of previous players and display
//Inputs:   Inputs to the function here -> Description, Range, Units
//  string and score vectors to allow for storage of contents in file
//Output:   Outputs to the function here -> Description, Range, Units
//  display names and scores, sorted
//******************************************************************************
void ldrBrd(){
    //read names from file
    ifstream in;
    in.open("names.dat");
    string data;
    //Victors victor;
    
    vector<string> names;
    vector<int>    scores;
    
    
    if(in){
        while(in>>data) {
            names.push_back(data);
        }
    }
    in.close();
    
    //display score
    in.open("score.dat");
    int points;
    if(in){
        while(in>>points){
            scores.push_back(points);
        }
    }
    
    //create index to facilitate sorting
    vector <int> index(names.size());
    for(int i=0;i<scores.size();i++){
        index[i]=i;
    }
    
    //sort
    markSrt(scores, index, names.size());
    
    //print sorted scores
    prntScr(scores, names, index, names.size());
}

//******************************* Add Record ***********************************
//Purpose: add record or increment if it is a new entry, store to file
//Inputs:   Inputs to the function here -> Description, Range, Units
//  total, name, isIn
//Output:   Outputs to the function here -> Description, Range, Units
//  store new entry to file
//******************************************************************************
void addRec(){
    int total=0;
    string name;
    bool isIn;  //vector push flag
    
    cout<<"Enter your name to the hall of fame."<<endl;
    do{
        getline(cin, name);
    }while(name.empty());  //check if input is empty
    
    vector<string> names;
    vector<int>    scores;
    
    //read names from file
    ifstream in;
    in.open("names.dat");
    string data;
    if(in){
        while(in>>data) {
            names.push_back(data);
        }
    }
    in.close();
    
    //display score
    in.open("score.dat");
    int points;
    if(in){
        while(in>>points){
            scores.push_back(points);
        }
    }
    
    //search vector to find if name is already registered
    for(int i=0;i<names.size(); i++){
        if(names[i]==name){
            isIn=true;
            scores[i]++;
        }
    }
    
    //push.back or add new entry
    if(!isIn){
        total++;
        names.push_back(name);
        scores.push_back(total);
    }
    
    //set index 0 - size of score vector
    vector <int> index(names.size());
    for(int i=0;i<scores.size();i++){
        index[i]=i;
    }
    
    markSrt(scores, index, names.size());
    
    prntScr(scores, names, index, names.size());
    
    //store names into file
    ofstream out;
    out.open("names.dat");
    for(int i=0;i<names.size(); i++){
        out<<names[i]<<endl;
    }
    out.close();
    //store scores into file
    out.open("score.dat");
    for(int i=0;i<scores.size(); i++){
        out<<scores[i]<<endl;
    }
    out.close();
}

//******************************* Print Score **********************************
//Purpose: print scores from greatest to least
//Inputs:   Inputs to the function here -> Description, Range, Units
//  names vector, scores vector, and size of vectors
//Output:   Outputs to the function here -> Description, Range, Units
//  display scores
//******************************************************************************
void prntScr(vector <int> &a, vector<string> &names,vector <int> &indx,int SIZE){
    //from greatest to smallest
    cout<<"Hall of Fame:\nName--Total Puzzles Completed"<<endl;
    for(int i=0;i<SIZE;i++){
        cout<<names[indx[(SIZE-1-i)]]<<"--";
        cout<<a[indx[(SIZE-1-i)]]<<endl;
    }
    cout<<endl;
}

//******************************* Mark Sort ************************************
//Purpose: sort indexes that lets us sort from greatest to least
//Inputs:   Inputs to the function here -> Description, Range, Units
//  score and index vectors -- the variables to account for storage
//Output:   Outputs to the function here -> Description, Range, Units
//  return indexes sorted
//******************************************************************************
void markSrt(vector <int> &a,vector <int> &indx,int SIZE){
    for(int i=0;i<SIZE-1;i++){
        for(int j=i+1;j<SIZE;j++){
            if(a[indx[i]]>a[indx[j]]){
                indx[i]=indx[i]^indx[j];
                indx[j]=indx[i]^indx[j];
                indx[i]=indx[i]^indx[j];
            }
        }
    }
}

//******************************* Load Puzzle **********************************
//Purpose: load puzzle
//Inputs:   Inputs to the function here -> Description, Range, Units
//  puzzle, dfaults, cmplte -> puzzle and boundaries
//Output:   Outputs to the function here -> Description, Range, Units
//  array filled with contents from save file
//******************************************************************************
bool isLoadd(char puzzle [SIZE][SIZE], char dfaults [SIZE][SIZE], char cmplte [SIZE][SIZE], bool *isPlybl){
    ifstream inFile;        //load from file
    inFile.open("prev.dat");
    
    if(inFile){
        inFile.unsetf(ios_base::skipws);    //skip white space
        //inFile>>is9x9;
        *isPlybl=true;

        for(int n=0; n<SIZE; n++){
            for(int i=0; i<SIZE; i++){
                inFile>>puzzle[n][i];
            }
        }
        

        for(int n=0; n<SIZE; n++){
            for(int i=0; i<SIZE; i++){
                inFile>>dfaults[n][i];
            }
        }

        for(int n=0; n<SIZE; n++){
            for(int i=0; i<SIZE; i++){
                inFile>>cmplte[n][i];
            }
        }
        
        inFile.close();
    }
    else {
        //if no load puzzle, set menu=x so it continues the loop
        cout<<endl<<"No saved puzzle."<<endl;
        *isPlybl=false;
    }
}

//******************************* Save Puzzle **********************************
//Purpose: save Puzzle
//Inputs:   Inputs to the function here -> Description, Range, Units
//  puzzle, defaults, complete -> progress, values set by program, and completed
//Output:   Outputs to the function here -> Description, Range, Units
//  store arrays to file
//******************************************************************************
bool isSaved(char puzzle [][SIZE], char dfaults [][SIZE], char cmplte [][SIZE], bool is9x9){
    if(!is9x9) return false;
    
    ofstream out;
    out.open("prev.dat");
    
    //out<<is9x9;
    
    for(int n=0; n<SIZE; n++){
        for(int i=0; i<SIZE; i++){
            out<<puzzle[n][i];
        }
    }
    
    for(int n=0; n<SIZE; n++){
        for(int i=0; i<SIZE; i++){
            out<<dfaults[n][i];
        }
    }
    
    for(int n=0; n<SIZE; n++){
        for(int i=0; i<SIZE; i++){
            out<<cmplte[n][i];
        }
    }
    
    out.close();
    cout<<"Saved!"<<endl;
    return true;
}

//******************************* Difficulty Select ***************************
//Purpose: allow difficulty selection
//Inputs:   Inputs to the function here -> Description, Range, Units
//  is9x9 -> if true, erase more values
//Output:   Outputs to the function here -> Description, Range, Units
//  return number of cells to empty
//******************************************************************************
short difSel(bool is9x9){
    short dif;
    
    //difficulty select do-while loop
    do{
        cout<<"Select a difficulty (1-5), 1 being the easiest, 5 being the hardest."<<endl;
        cin>>dif;
    }while(dif<=0&&dif>5);

    //empty cells depending on difficulty selected  
    if (is9x9) return dif*12;
    else return dif*2;    
}

//******************************* Check Sudoku Input ***************************
//Purpose: check Sudoku input
//Inputs:   Inputs to the function here -> Description, Range, Units
//  puzzle, dfaults, upperC, lowerC, gridT -> validate entry 
//Output:   Outputs to the function here -> Description, Range, Units
//  if value is in accordance with Sudoku rules, change cell in puzzle array
//******************************************************************************
void chInput(char puzzle[][SIZE], char dfaults[][SIZE], char upperC, char lowerC, short gridT){
    //get the square root of the grid type for use in algorithm that scans square
    //if 4x4, each square has two columns and rows
    //if 9x9, each square has three columns and rows
    char digit;
    static short puzDiv;
    
    
    if ((puzDiv!=2&&gridT==4)||(puzDiv!=3&&gridT==9)) puzDiv=sqrt(gridT);
    
    //check if cell is empty or if it is a value placed by the computer
    if(puzzle[lowerC][upperC]!=' '&&puzzle[lowerC][upperC]==dfaults[lowerC][upperC]){
        cout<<"The cell you want to edit has a value set by the program."<<endl;
        return;
    }
    
    //player submits a value to be added to cell
    cout<<"Input number to fill this cell. (1-"<<gridT<<")"<<endl;
    //input validation
    do{
        cin>>digit;
    }while(digit<'1'||digit>(gridT+'0'));
    
    //check columns
    for(int n=0; n<gridT; n++){
        if(puzzle[n][upperC]==digit){
            cout<<"The value already exists in the same column."<<endl;
            return;
        }
    }

    //ensure no duplicates exist in row
    for(int i=0; i<gridT; i++){
        if(puzzle[lowerC][i]==digit){
            cout<<"The value already exists in the same row."<<endl;
            return;
        }
    }

    //check if there are any of the same values in the square
    //x -> beginning row of square
    //y -> beginning column of square
    //n & i for the loop
    for(int x=lowerC-lowerC%puzDiv, n=0; n<puzDiv; x++, n++){
        for(int y=upperC-upperC%puzDiv, i=0; i<puzDiv; y++, i++){
            if(puzzle[x][y]==digit) {
            cout<<"The value already exists in the same square."<<endl;
            return;
            }
        }
    }
    
    puzzle[lowerC][upperC]=digit;
}

//******************************* Randomize Puzzle 9x9 *************************
//Purpose: fill 9x9 array with random numbers within Sudoku boundaries
//Inputs:   Inputs to the function here -> Description, Range, Units
//  puzzke, r, c, counter, isRep, digit -> puzzle and boundaries
//Output:   Outputs to the function here -> Description, Range, Units
//  cell with random
//******************************************************************************
char doRand9(char puzzle[][SIZE], int r,int c, short & counter){
    bool isRep;
    char digit;
    
    counter=0;
    
    do{
        isRep=false;
        digit=rand()%9+49;  //numbers 1-9u
        
        //ensure no duplicates exist in column
        for(int n=0; n<SIZE; n++){
            if(puzzle[n][c]==digit) isRep=true;
        }
        
        //ensure no duplicates exist in row
        for(int i=0; i<SIZE; i++){
            if(puzzle[r][i]==digit) isRep=true;
        }
        
        //check if there are any of the same values in the square
        //x -> beginning row of square
        //y -> beginning column of square
        //n & i for the loop
        for(int x=r-r%3, n=0; n<3; x++, n++){
            for(int y=c-c%3, i=0; i<3; y++, i++){
                if(puzzle[x][y]==digit) isRep=true;
            }
        }
        if(isRep) counter++;
    }while(isRep && counter<=30);
    
    //dspPuzz(puzzle, true);    //display puzzle with random numbers
    
    return digit;
}

//******************************* Randomize Puzzle 4x4 *************************
//Purpose: fill 4x4 array with random numbers within Sudoku boundaries
//Inputs:   Inputs to the function here -> Description, Range, Units
//  puzzke, r, c, counter, isRep, digit -> puzzle and boundaries
//Output:   Outputs to the function here -> Description, Range, Units
//  cell with random
//******************************************************************************
char doRand4(char puzzle[][SIZE], int r,int c, short & counter){
    bool isRep;
    char digit;
    
    counter=0;
    
    do{
        isRep=false;
        digit=rand()%4+49;  //numbers 1-9u
        
        //ensure no duplicates exist in column
        for(int n=0; n<4; n++){
            if(puzzle[n][c]==digit) isRep=true;
        }
        
        //ensure no duplicates exist in row
        for(int i=0; i<4; i++){
            if(puzzle[r][i]==digit) isRep=true;
        }
        
        //check if there are any of the same values in the square
        //x -> beginning row of square
        //y -> beginning column of square
        //n & i for the loop
        for(int x=r-r%2, n=0; n<2; x++, n++){
            for(int y=c-c%2, i=0; i<2; y++, i++){
                if(puzzle[x][y]==digit) isRep=true;
            }
        }
        if(isRep) counter++;
    }while(isRep && counter<=30);
    
    //dspPuzz(puzzle, true);    //display puzzle with random numbers
    
    return digit;
}

//******************************* Randomize Puzzle *****************************
//Purpose: fill Sudoku array within the boundaries of the game
//Inputs:   Inputs to the function here -> Description, Range, Units
//  puzzle, is9x9 -> array to change, if 9x9 or not
//Output:   Outputs to the function here -> Description, Range, Units
//  arrays
//******************************************************************************
void rndPuz(char puzzle[][SIZE], bool is9x9){
    short counter;
    short cells;
    
    if(is9x9) cells=9;
    else cells=4;
    
    do{
        counter=0;  //used to escape puzzles computer cannot complete
        //fill array with spaces
        for(int n=0; n<SIZE; n++){
            for(int i=0; i<SIZE; i++){
                puzzle[n][i]=' ';
            }
        }

        //complete Sudoku puzzle
        for(int n=0; n<cells; n++){
            for (int i=0; i<cells; i++){
                //9x9 fill
                if (is9x9) puzzle[n][i]=doRand9(puzzle, n, i, counter);
                //4x4 fill
                else puzzle[n][i]=doRand4(puzzle, n, i, counter);
                if(counter>30) n=i=20;      //multiple assignment
            }
        }
    }while(counter>30);
}

//******************************* Display Puzzle *******************************
//Purpose: determine whether to display 4x4 or 9x9
//Inputs:   Inputs to the function here -> Description, Range, Units
//  is9x9 -> if true, display 9x9 grid, else display 4x4
//Output:   Outputs to the function here -> Description, Range, Units
//  branch to appropriate function
//******************************************************************************
void dspPuzz(char puzzle[][SIZE], bool is9x9, char dfaults[][SIZE]){
    if (is9x9) dsp9x9(puzzle, dfaults);
    else dsp4x4(puzzle, dfaults);
}

//******************************* Display 9x9 ***********************************
//Purpose: Display 9x9 puzzle
//Inputs:   Inputs to the function here -> Description, Range, Units
//  puzzle, rows, cols -> puzzle itself
//Output:   Outputs to the function here -> Description, Range, Units
//  display 9x9 puzzle
//******************************************************************************
void dsp9x9(char puzzle[][SIZE], char dfaults[][SIZE]){
    char temp='a';      //display navigation character for rows
    cout<<setw(50)<<"A B C   D E F   G H I";    //column display index
    
    for(int n=0; n<SIZE; n++){
        if(n%3==0) cout<<setw(52)<<endl<<"- - - - - - - - - - - -";
        cout<<setw(33)<<endl;
        for (int i=0; i<SIZE; i++){
            cout<<((puzzle[n][i]==dfaults[n][i])?"\e[1m":"\e[0m")
                <<puzzle[n][i]<<((i%3==2)?"\e[0m | ":" ");
        }
        cout<<"\e[0m "<<temp++;  //row display index
    }
    
    cout<<endl<<endl;   //create two lines after display
}

//******************************* Display 4x4 **********************************
//Purpose: Display 4x4 puzzle
//Inputs:   Inputs to the function here -> Description, Range, Units
//  puzzle, rows, cols -> puzzle itself
//Output:   Outputs to the function here -> Description, Range, Units
//  display 4x4 puzzle
//******************************************************************************
void dsp4x4(char puzzle[][SIZE], char dfaults[][SIZE]){
    char temp='a';      //display navigation character for rows
    cout<<setw(38)<<"A B   C D";    //column display index
    
    for(int n=0; n<4; n++){
        if(n%2==0) cout<<setw(38)<<endl<<"- -   - -";
        cout<<setw(33)<<endl;
        for (int i=0; i<4; i++){
            cout<<((puzzle[n][i]==dfaults[n][i])?"\e[1m":"\e[0m")
                <<puzzle[n][i]<<((i%2==1)?"\e[0m | ":" ");
        }
        cout<<"\e[0m "<<temp++;  //row display index
    }
    
    cout<<endl<<endl;   //create two lines after display
}

//******************************* Escape ***************************************
//Purpose: option to exit out of program if user decides to just display score
//Inputs:   Inputs to the function here -> Description, Range, Units
//Output:   Outputs to the function here -> Description, Range, Units
//******************************************************************************
void escape(){
    cout<<"Come back later!"<<endl;
    exit(0);
}