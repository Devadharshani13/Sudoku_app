#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 9
#define MAX_UNDO 200
#define TOP_BAR_HEIGHT 80
#define BUTTON_HEIGHT 60
#define NUMBER_PALETTE_HEIGHT 60

typedef struct { int value; bool fixed; } Cell;
typedef struct { int row, col, prevValue, newValue; } Move;
typedef enum { EASY=0, MEDIUM, HARD } Difficulty;

// ---------------- Global Variables ----------------
Cell board[GRID_SIZE][GRID_SIZE];
Move undoStack[MAX_UNDO], redoStack[MAX_UNDO];
int undoTop=-1, redoTop=-1;

int selectedRow=-1, selectedCol=-1;
int lastMoveRow=-1, lastMoveCol=-1;
int moves=0;
Uint32 startTime;

SDL_Window *window=NULL;
SDL_Renderer *renderer=NULL;
TTF_Font *font=NULL;
Difficulty difficulty=EASY;

// ---------------- Sample Puzzles ----------------
int puzzles[3][GRID_SIZE][GRID_SIZE] = {
    {{5,3,0,0,7,0,0,0,0},{6,0,0,1,9,5,0,0,0},{0,9,8,0,0,0,0,6,0},
     {8,0,0,0,6,0,0,0,3},{4,0,0,8,0,3,0,0,1},{7,0,0,0,2,0,0,0,6},
     {0,6,0,0,0,0,2,8,0},{0,0,0,4,1,9,0,0,5},{0,0,0,0,8,0,0,7,9}},
    {{0,2,0,6,0,8,0,0,0},{5,8,0,0,0,9,7,0,0},{0,0,0,0,4,0,0,0,0},
     {3,7,0,0,0,0,5,0,0},{6,0,0,0,0,0,0,0,4},{0,0,8,0,0,0,0,1,3},
     {0,0,0,0,2,0,0,0,0},{0,0,9,8,0,0,0,3,6},{0,0,0,3,0,6,0,9,0}},
    {{0,0,0,2,6,0,7,0,1},{6,8,0,0,7,0,0,9,0},{1,9,0,0,0,4,5,0,0},
     {8,2,0,1,0,0,0,4,0},{0,0,4,6,0,2,9,0,0},{0,5,0,0,0,3,0,2,8},
     {0,0,9,3,0,0,0,7,4},{0,4,0,0,5,0,0,3,6},{7,0,3,0,1,8,0,0,0}}
};

// ---------------- Sudoku Logic ----------------
bool isValidMove(int row,int col,int num){
    if(num==0) return true;
    for(int i=0;i<GRID_SIZE;i++){
        if(i!=col && board[row][i].value==num) return false;
        if(i!=row && board[i][col].value==num) return false;
    }
    int sr=row-row%3, sc=col-col%3;
    for(int i=0;i<3;i++) for(int j=0;j<3;j++){
        int r=sr+i, c=sc+j;
        if((r!=row || c!=col) && board[r][c].value==num) return false;
    }
    return true;
}

bool findEmpty(int *row,int *col){
    for(*row=0;*row<GRID_SIZE;(*row)++) for(*col=0;*col<GRID_SIZE;(*col)++) if(board[*row][*col].value==0) return true;
    return false;
}

bool solveBoard(){
    int row,col;
    if(!findEmpty(&row,&col)) return true;
    for(int n=1;n<=9;n++){
        if(isValidMove(row,col,n)){
            board[row][col].value=n;
            if(solveBoard()) return true;
            board[row][col].value=0;
        }
    }
    return false;
}

void applyMove(int row,int col,int val){
    undoTop++; if(undoTop>=MAX_UNDO) undoTop=MAX_UNDO-1;
    undoStack[undoTop]=(Move){row,col,board[row][col].value,val};
    board[row][col].value=val;
    redoTop=-1; moves++; lastMoveRow=row; lastMoveCol=col;
}

void undoMove(){if(undoTop<0) return; Move m=undoStack[undoTop--]; board[m.row][m.col].value=m.prevValue; redoTop++; redoStack[redoTop]=m; moves--;}
void redoMove(){if(redoTop<0) return; Move m=redoStack[redoTop--]; board[m.row][m.col].value=m.newValue; undoTop++; undoStack[undoTop]=m; moves++;}

void loadPuzzle(int index){
    for(int r=0;r<GRID_SIZE;r++) for(int c=0;c<GRID_SIZE;c++){
        board[r][c].value=puzzles[index][r][c];
        board[r][c].fixed=(puzzles[index][r][c]!=0);
    }
    undoTop=redoTop=-1; moves=0; lastMoveRow=-1; lastMoveCol=-1;
    startTime=SDL_GetTicks();
}

// ---------------- UI ----------------
void drawText(const char *txt,int x,int y,SDL_Color color){
    SDL_Surface *surf=TTF_RenderText_Solid(font,txt,color);
    SDL_Texture *tex=SDL_CreateTextureFromSurface(renderer,surf);
    SDL_Rect dst={x,y,surf->w,surf->h};
    SDL_RenderCopy(renderer,tex,NULL,&dst);
    SDL_DestroyTexture(tex); SDL_FreeSurface(surf);
}

bool isComplete(){
    for(int r=0;r<GRID_SIZE;r++) for(int c=0;c<GRID_SIZE;c++)
        if(board[r][c].value==0 || !isValidMove(r,c,board[r][c].value)) return false;
    return true;
}

void drawBoard(int winWidth,int winHeight){
    int gridSize=winWidth<winHeight-TOP_BAR_HEIGHT-BUTTON_HEIGHT-NUMBER_PALETTE_HEIGHT?winWidth:winHeight-TOP_BAR_HEIGHT-BUTTON_HEIGHT-NUMBER_PALETTE_HEIGHT;
    int cellSize=gridSize/GRID_SIZE;
    int gridX=(winWidth-cellSize*GRID_SIZE)/2;
    int gridY=TOP_BAR_HEIGHT;

    SDL_SetRenderDrawColor(renderer,255,255,255,255); SDL_RenderClear(renderer);

    // Top bar
    SDL_SetRenderDrawColor(renderer,220,220,220,255);
    SDL_Rect topBar={0,0,winWidth,TOP_BAR_HEIGHT}; SDL_RenderFillRect(renderer,&topBar);
    char buf[64]; Uint32 elapsed=(SDL_GetTicks()-startTime)/1000;
    sprintf(buf,"Time: %02d:%02d  Moves: %d",elapsed/60,elapsed%60,moves);
    drawText(buf,10,10,(SDL_Color){0,0,0,255});

    // Highlight last move
    if(lastMoveRow!=-1){
        SDL_SetRenderDrawColor(renderer,180,255,180,120);
        SDL_Rect lastRect={gridX+lastMoveCol*cellSize,gridY+lastMoveRow*cellSize,cellSize,cellSize};
        SDL_RenderFillRect(renderer,&lastRect);
    }

    // Highlight selected cell with fade
    if(selectedRow!=-1 && selectedCol!=-1){
        static Uint8 alpha=100; static int delta=5;
        alpha+=delta; if(alpha>200 || alpha<50) delta=-delta;
        SDL_SetRenderDrawColor(renderer,150,150,255,alpha);
        SDL_Rect selRect={gridX+selectedCol*cellSize,gridY+selectedRow*cellSize,cellSize,cellSize};
        SDL_RenderFillRect(renderer,&selRect);
    }

    // Grid lines
    for(int i=0;i<=GRID_SIZE;i++){
        SDL_SetRenderDrawColor(renderer,(i%3==0)?0:180,(i%3==0)?0:180,(i%3==0)?0:180,255);
        SDL_RenderDrawLine(renderer,gridX+i*cellSize,gridY,gridX+i*cellSize,gridY+cellSize*GRID_SIZE);
        SDL_RenderDrawLine(renderer,gridX,gridY+i*cellSize,gridX+cellSize*GRID_SIZE,gridY+i*cellSize);
    }

    // Numbers
    SDL_Color black={0,0,0}, blue={0,0,255}, red={255,0,0}, green={0,150,0};
    for(int r=0;r<GRID_SIZE;r++){
        for(int c=0;c<GRID_SIZE;c++){
            if(board[r][c].value!=0){
                sprintf(buf,"%d",board[r][c].value);
                SDL_Color color=board[r][c].fixed?black:(isValidMove(r,c,board[r][c].value)?blue:red);
                drawText(buf,gridX+c*cellSize+cellSize/3,gridY+r*cellSize+cellSize/4,color);
            }
        }
    }

    // Buttons row
    const char *btns[]={"Undo","Redo","Hint","Solve","New Game"};
    int btnY=gridY+cellSize*GRID_SIZE;
    for(int i=0;i<5;i++){
        SDL_Rect btn={i*(winWidth/5),btnY,winWidth/5,BUTTON_HEIGHT};
        SDL_SetRenderDrawColor(renderer,200,200,200,255); SDL_RenderFillRect(renderer,&btn);
        SDL_SetRenderDrawColor(renderer,0,0,0,255); SDL_RenderDrawRect(renderer,&btn);
        drawText(btns[i],btn.x+10,btn.y+10,black);
    }

    // Number palette
    int paletteY=btnY+BUTTON_HEIGHT;
    for(int i=1;i<=9;i++){
        SDL_Rect numRect={gridX+(i-1)*cellSize,paletteY,cellSize,NUMBER_PALETTE_HEIGHT};
        SDL_SetRenderDrawColor(renderer,180,180,180,255); SDL_RenderFillRect(renderer,&numRect);
        SDL_SetRenderDrawColor(renderer,0,0,0,255); SDL_RenderDrawRect(renderer,&numRect);
        sprintf(buf,"%d",i); drawText(buf,numRect.x+cellSize/3,numRect.y+NUMBER_PALETTE_HEIGHT/4,black);
    }

    SDL_RenderPresent(renderer);
}

// ---------------- Main ----------------
int main(int argc,char *argv[]){
    srand(time(NULL));
    if(SDL_Init(SDL_INIT_VIDEO)<0){printf("SDL init failed: %s\n",SDL_GetError()); return 1;}
    if(TTF_Init()<0){printf("TTF init failed: %s\n",TTF_GetError()); return 1;}

    window=SDL_CreateWindow("Sudoku",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,600,800,SDL_WINDOW_RESIZABLE);
    renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    font=TTF_OpenFont("C:\\Users\\yamin\\Desktop\\sudoku_app\\assets\\font.ttf",32);
    if(!font){printf("Font failed: %s\n",TTF_GetError()); return 1;}

    loadPuzzle(rand()%3);

    bool running=true; SDL_Event e;
    while(running){
        int w,h; SDL_GetWindowSize(window,&w,&h);
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) running=false;

            if(e.type==SDL_MOUSEBUTTONDOWN){
                int mx=e.button.x,my=e.button.y;

                // Buttons
                int btnY=TOP_BAR_HEIGHT+(w<h? w : h-TOP_BAR_HEIGHT-BUTTON_HEIGHT-NUMBER_PALETTE_HEIGHT)/GRID_SIZE*GRID_SIZE;
                for(int i=0;i<5;i++){
                    SDL_Rect btn={i*(w/5),btnY,w/5,BUTTON_HEIGHT};
                    if(mx>=btn.x && mx<=btn.x+btn.w && my>=btn.y && my<=btn.y+btn.h){
                        switch(i){
                            case 0: undoMove(); break;
                            case 1: redoMove(); break;
                            case 2: // hint
                            { int r,c; if(findEmpty(&r,&c)){ for(int n=1;n<=9;n++){ if(isValidMove(r,c,n)){ applyMove(r,c,n); break; }}}} break;
                            case 3: solveBoard(); break;
                            case 4: loadPuzzle(rand()%3); break;
                        }
                    }
                }

                // Number palette
                int paletteY=btnY+BUTTON_HEIGHT;
                int gridSize=w<h? w : h-TOP_BAR_HEIGHT-BUTTON_HEIGHT-NUMBER_PALETTE_HEIGHT;
                int cellSize=gridSize/GRID_SIZE; int gridX=(w-cellSize*GRID_SIZE)/2;
                for(int i=1;i<=9;i++){
                    SDL_Rect numRect={gridX+(i-1)*cellSize,paletteY,cellSize,NUMBER_PALETTE_HEIGHT};
                    if(mx>=numRect.x && mx<=numRect.x+numRect.w && my>=numRect.y && my<=numRect.y+numRect.h){
                        if(selectedRow!=-1 && selectedCol!=-1 && !board[selectedRow][selectedCol].fixed)
                            applyMove(selectedRow,selectedCol,i);
                    }
                }

                // Grid selection
                if(mx>=gridX && mx<=gridX+cellSize*GRID_SIZE && my>=TOP_BAR_HEIGHT && my<=TOP_BAR_HEIGHT+cellSize*GRID_SIZE){
                    selectedCol=(mx-gridX)/cellSize; selectedRow=(my-TOP_BAR_HEIGHT)/cellSize;
                }
            }

            if(e.type==SDL_KEYDOWN && selectedRow!=-1 && selectedCol!=-1){
                int num=e.key.keysym.sym-SDLK_0;
                if(num>=1 && num<=9 && !board[selectedRow][selectedCol].fixed) applyMove(selectedRow,selectedCol,num);
                if(e.key.keysym.sym==SDLK_0) applyMove(selectedRow,selectedCol,0);
            }
        }

        drawBoard(w,h);

        if(isComplete()){
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Sudoku","Congratulations! Puzzle solved!",window);
            loadPuzzle(rand()%3);
        }
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit(); SDL_Quit();
    return 0;
}
