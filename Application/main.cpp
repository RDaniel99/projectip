#include <iostream>
#include <graphics.h>
#include <winbgim.h>
#include <ctime>
#include "personalGraphic.h"

using namespace std;

#define MAX_POINTS 105
#define MAX_NAME   25

///---------------- Geometry structs ---------------------------------------------------------------
struct CPoint {
    int x, y;
};
struct CSegment {
    CPoint A, B;
};
struct CCircle {
    CPoint center;
    int radius;
};
///-------------------------------------------------------------------------------------------------

///---------------- Settings struct of the Game ----------------------------------------------------
struct CSettings {
    int numberOfPoints;
    int colorOfPlayer1,
        colorOfPlayer2;

    int  botLevel        ;
    bool isPlayingWithBot;
    int  firstToWin      ;

    char namePlayer1[MAX_NAME],
         namePlayer2[MAX_NAME];
};
///-------------------------------------------------------------------------------------------------

///---------------- Table game struct --------------------------------------------------------------
struct CTable {
    CPoint   points[MAX_POINTS]     ;
    CSegment segments[MAX_POINTS]   ;
    bool     isSelected[MAX_POINTS] ; // 'isSelected[i] = true' only when 'i' is already in a segment

    int numberOfSegments,
        numberOfPoints  ,
        windowHeight    ,
        windowWidth     ,
        firstWinnings   ,
        secondWinnings  ,
        radiusPoints = 4;

    CSettings settings  ;
};
///-------------------------------------------------------------------------------------------------

///---------------- Game Engine Functions ----------------------------------------------------------
void    StartGame(CTable &table)            ;
bool    TheGameIsOver(CTable &table)        ;
void    GenerateNRandomPoints(CTable &table);
///-------------------------------------------------------------------------------------------------

///---------------- Geometry Functions -------------------------------------------------------------
bool    IsPointOnSegment(CSegment &segment, CPoint &point)    ;
bool    SegmentsAreIntersecting(CSegment &s1, CSegment &s2)   ;
int     ComputeOrientation(CPoint &A, CPoint &B, CPoint &C)   ;
bool    CheckCirclesIntersection(CCircle &c1, CCircle &c2)    ;
int     CalculateSqDistanceBetweenPoints(CPoint &A, CPoint &B);
///-------------------------------------------------------------------------------------------------

///---------------- CSettings Functions ----------------------------------------------------------
void    SetNumberOfPoints(CTable &table, int &newN);
void    SetGameWithBot(CTable &table, bool &status);
void    SetFirstToWin(CTable &table, int &firstToW);
///-------------------------------------------------------------------------------------------------


///---------------- Main Function ------------------------------------------------------------------
int main() {
    initwindow(600, 600, "Segments Game");

    CTable table;

    table.windowHeight = 600;
    table.windowWidth = 600;
    table.numberOfPoints = 25;

    int pageIndex = 0;

    /*
    0 - Main Page
    1 - Settings Page
    2 - Game Page
    */

    outtextxy(300, 0, "Welcome to Segments Game");

    rectangle(50, 50, 164, 90);
    outtextxy(60, 60, "Game Settings");

    rectangle(50, 100, 164, 140);
    outtextxy(90, 110, "Play");

    while(true) {
        int x, y;
        getmouseclick(WM_LBUTTONDOWN, x, y);

        if(!(x < 0 && y < 0)) {
            if(x >= 50 && x <= 164 && y >= 50 && y <= 90 && pageIndex == 0) {
                cleardevice();
                outtextxy(10, 10, "Settings Page. In Progress...");
                pageIndex = 1;
            }
            else {
                if(x >= 50 && x <= 164 && y >= 100 && y <= 140 && pageIndex == 0) {
                    cleardevice();

                    pageIndex = 2;

                    GenerateNRandomPoints(table);

                    for(int pointIndex = 1; pointIndex <= table.numberOfPoints; ++pointIndex) {
                        circle(table.points[pointIndex].x, table.points[pointIndex].y, 4);
                    }
                }
            }
        }
    }

    getch();
    closegraph();

    return 0;
}
///-------------------------------------------------------------------------------------------------


///---------------- Generator of N Random Points ----------------------------------------------------
void GenerateNRandomPoints(CTable &table) {
    srand(time(NULL));

    for(int pointIndex = 1; pointIndex <= table.numberOfPoints; ++pointIndex) {
        int xCoordinate = rand() % table.windowWidth ;
        int yCoordinate = rand() % table.windowHeight;

        CPoint newPoint;

        newPoint.x = xCoordinate;
        newPoint.y = yCoordinate;

        CCircle c1;
        c1.center = newPoint;
        c1.radius = table.radiusPoints;
        int minX = c1.center.x - 3 * c1.radius;
        int maxX = c1.center.x + 3 * c1.radius;
        int minY = c1.center.y - 3 * c1.radius;
        int maxY = c1.center.y + 3 * c1.radius;

        if(minX > 0 && maxX < table.windowWidth && minY > 0 && minY < table.windowHeight) {
            bool flag = false;
            for(int i = 1; i < pointIndex; ++i) {
                CCircle c2;
                c2.center = table.points[i];
                c2.radius = table.radiusPoints;

                if(CheckCirclesIntersection(c1, c2)) {
                    flag = true;
                    break;
                }
            }

            if(flag) {
                --pointIndex;
            }
            else {
                table.points[pointIndex] = newPoint;
            }
        }
        else {
            --pointIndex;
        }
    }
}
///-------------------------------------------------------------------------------------------------

///---------------- Check if the game is over ------------------------------------------------------
bool TheGameIsOver(CTable &table) {
    for(int firstPoint = 1; firstPoint <= table.numberOfPoints; ++firstPoint) {

        if(!table.isSelected[firstPoint]) {

            for(int secondPoint = 1; secondPoint <= table.numberOfPoints; ++secondPoint) {

                if(!table.isSelected[secondPoint]) {
                    CSegment segmentToCheck;
                    segmentToCheck.A = table.points[firstPoint];
                    segmentToCheck.B = table.points[secondPoint];

                    bool flag = false;
                    for(int segmentIndex = 1; segmentIndex <= table.numberOfSegments; ++segmentIndex) {
                        if(SegmentsAreIntersecting(segmentToCheck, table.segments[segmentIndex]) == true) {
                            flag = true;
                            break;
                        }
                    }

                    if(!flag) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}
///-------------------------------------------------------------------------------------------------

///---------------- Check if two given segments are intersecting -----------------------------------
bool SegmentsAreIntersecting(CSegment &s1, CSegment &s2) {
    int o1 = ComputeOrientation(s1.A, s1.B, s2.A);
    int o2 = ComputeOrientation(s1.A, s1.B, s2.B);
    int o3 = ComputeOrientation(s2.A, s2.B, s1.A);
    int o4 = ComputeOrientation(s2.A, s2.B, s1.B);

    if(o1 != o2 && o3 != o4) return true;

    if(o1 == 0 && IsPointOnSegment(s1, s2.A) == true) return true;
    if(o2 == 0 && IsPointOnSegment(s1, s2.B) == true) return true;
    if(o3 == 0 && IsPointOnSegment(s2, s1.A) == true) return true;
    if(o4 == 0 && IsPointOnSegment(s2, s1.B) == true) return true;

    return false;
}
///-------------------------------------------------------------------------------------------------

///---------------- Check if a given point is on a given segment -----------------------------------
bool IsPointOnSegment(CSegment &segment, CPoint &point) {
    if(max(segment.A.x, segment.B.x) >= point.x &&
       min(segment.A.x, segment.B.x) <= point.x &&
       max(segment.A.y, segment.B.y) >= point.y &&
       min(segment.A.y, segment.B.y) <= point.y)
        return true;

    return false;
}
///-------------------------------------------------------------------------------------------------

///---------------- Check if point C is on the right/left side of segment [AB] ---------------------
int ComputeOrientation(CPoint &A, CPoint &B, CPoint &C) {
    int rez = (B.y - A.y) * (C.x - B.x) -
              (B.x - A.x) * (C.y - B.y) ;

    if(!rez) return 0;

    return rez / abs(rez);
}
///-------------------------------------------------------------------------------------------------

///---------------- Set number of points on the table ----------------------------------------------
void SetNumberOfPoints(CTable &table, int &newN) {
    table.settings.numberOfPoints = newN;
}
///-------------------------------------------------------------------------------------------------

///---------------- Set if user is playing with BOT ------------------------------------------------
void SetGameWithBot(CTable &table, bool &status) {
    table.settings.isPlayingWithBot = status;
}
///-------------------------------------------------------------------------------------------------

///---------------- Set the winning score ----------------------------------------------------------
void SetFirstToWin(CTable &table, int &firstToW) {
    table.settings.firstToWin = firstToW;
}
///-------------------------------------------------------------------------------------------------

///---------------- Engine of the game -------------------------------------------------------------
void StartGame(CTable &table) {
    do {
        /// ... Initializari etc
        /// Functie de clear a ecranului
        GenerateNRandomPoints(table);

        int playerToMove = 0; // 0 - first, 1 - second

        while(!TheGameIsOver(table)) {

            /// ... Partea de grafica, selectie a punctelor etc

            playerToMove = 1 - playerToMove;
        }

        if(!playerToMove) {
            table.firstWinnings++;
        }
        else {
            table.secondWinnings++;
        }
    } while(max(table.firstWinnings, table.secondWinnings) < table.settings.firstToWin);
}
///-------------------------------------------------------------------------------------------------

///---------------- Check What Point Is Clicked ----------------------------------------------------

int CheckWhatPointIsClicked(CTable &table) {
    int x, y;
    getmouseclick(WM_LBUTTONDOWN, x, y);

    if(x < 0 && y < 0) return -1;

    CCircle clickCircle;
    clickCircle.center.x = x;
    clickCircle.center.y = y;
    clickCircle.radius = 0;

    for(int pointIndex = 1; pointIndex <= table.numberOfPoints; ++pointIndex) {
        CCircle pointCircle;
        pointCircle.center = table.points[pointIndex];
        pointCircle.radius = table.radiusPoints;
        if(CheckCirclesIntersection(pointCircle, clickCircle)) {
            return pointIndex;
        }
    }

    return -1;
}

///-------------------------------------------------------------------------------------------------

///---------------- Verify Two Circles Are Intersecting --------------------------------------------
bool CheckCirclesIntersection(CCircle &c1, CCircle &c2) {
    return (CalculateSqDistanceBetweenPoints(c1.center, c2.center) -
            (c1.radius + c2.radius) * (c1.radius + c2.radius)
           ) < 0;
}
///-------------------------------------------------------------------------------------------------

///---------------- Calculate Square Dist Between Two Points ---------------------------------------
int CalculateSqDistanceBetweenPoints(CPoint &A, CPoint &B) {
    return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}
///-------------------------------------------------------------------------------------------------
