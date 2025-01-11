/*
    WELCOME raylib EXAMPLES CONTRIBUTOR!

    This is a basic template to anyone ready to contribute with some code example for the library,
    here there are some guidelines on how to create an example to be included in raylib

    1. File naming: <module>_<description> - Lower case filename, words separated by underscore,
       no more than 3-4 words in total to describe the example. <module> referes to the primary
       raylib module the example is more related with (code, shapes, textures, models, shaders, raudio).
       i.e: core_input_multitouch, shapes_lines_bezier, shaders_palette_switch

    2. Follow below template structure, example info should list the module, the short description
       and the author of the example, twitter or github info could be also provided for the author.
       Short description should also be used on the title of the window.

    3. Code should be organized by sections:[Initialization]- [Update] - [Draw] - [De-Initialization]
       Place your code between the dotted lines for every section, please don't mix update logic with drawing
       and remember to unload all loaded resources.

    4. Code should follow raylib conventions: https://github.com/raysan5/raylib/wiki/raylib-coding-conventions
       Try to be very organized, using line-breaks appropiately.

    5. Add comments to the specific parts of code the example is focus on.
       Don't abuse with comments, try to be clear and impersonal on the comments.

    6. Try to keep the example simple, under 300 code lines if possible. Try to avoid external dependencies.
       Try to avoid defining functions outside the main(). Example should be as self-contained as possible.

    7. About external resources, they should be placed in a [resources] folder and those resources
       should be open and free for use and distribution. Avoid propietary content.

    8. Try to keep the example simple but with a creative touch.
       Simple but beautiful examples are more appealing to users!

    9. In case of additional information is required, just come to raylib Discord channel: example-contributions

    10. Have fun!

    The following files should be updated when adding a new example, it's planned to create some
    script to automatize this process but not available yet.

     - raylib/examples/<category>/<category>_example_name.c
     - raylib/examples/<category>/<category>_example_name.png
     - raylib/examples/<category>/resources/*.*
     - raylib/examples/Makefile
     - raylib/examples/Makefile.Web
     - raylib/examples/README.md
     - raylib/projects/VS2022/examples/<category>_example_name.vcxproj
     - raylib/projects/VS2022/raylib.sln
     - raylib.com/common/examples.js
     - raylib.com/examples/<category>/<category>_example_name.html
     - raylib.com/examples/<category>/<category>_example_name.data
     - raylib.com/examples/<category>/<category>_example_name.wasm
     - raylib.com/examples/<category>/<category>_example_name.js
*/

/*******************************************************************************************
*
*   raylib [core] example - Monads
*
*   Example originally created with raylib 4.5, last time updated with raylib 4.5
*
*   Example contributed by H-Art-Src (@<H-Art-Src>)
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2025 H-Art-Src (@<H-Art-Src>)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum
{
    DELETE_OFF,
    DELETE_POSTONLYLINK,
    DELETE_ONLYLINK,
    DELETE_PRELINK,
    DELETE_FINAL
};

// 1. A dot cannot have multiple container dots.
// 2. rootSubLinks can only have starting dots that exist within rootSubDots.
// 3. Links cannot comprise of dots of different depths.
// 4. Only one combination of a link can exist in totality.
#define MONAD_NAME_SIZE 32
#define MONAD_LINK_MIDDLE_LERP 0.35f
typedef struct structDot
{
    char name[MONAD_NAME_SIZE];
    Vector2 avgCenter , defaultCenter;
    struct structDot* rootSubDots;
    struct structDot* prev;
    struct structDot* next;
    struct structLink* rootSubLinks;
    float radius;
    int depth;
    int deleteFrame;
}  structDot;

typedef struct structLink
{
    struct structDot* startDot;
    struct structDot* endDot;
    struct structLink* prev;
    struct structLink* next;
} structLink;

enum Response
{
    RESULT_NONE,
    RESULT_CLICK,
    RESULT_RCLICK
};

typedef struct structActiveResult
{
   struct structDot* resultDot;
   struct structDot* resultContainerDot;
   struct structLink* resultLink;
   int resultKey , resultDepth;
}structActiveResult;

//ContainingDotPtr must not be null.
struct structDot* AddDot( Vector2 canvasPosition , structDot* containingDotPtr )
{
    if(Vector2Distance(canvasPosition , containingDotPtr->avgCenter) <= 30.0f) //deny if too close to container.
        return NULL;
     
    //malloc and initialize new dot. Always initialize variables that are not being overwritten.
    structDot* newDotPtr = (structDot*) malloc(sizeof(structDot));
    newDotPtr->defaultCenter = canvasPosition;
    newDotPtr->avgCenter = canvasPosition;
    newDotPtr->rootSubDots = NULL;
    newDotPtr->rootSubLinks = NULL;
    newDotPtr->radius = 10.0f;
    newDotPtr->depth = containingDotPtr->depth + 1;
    newDotPtr->deleteFrame = DELETE_OFF;
    
    newDotPtr->name[0] = containingDotPtr->rootSubDots ? containingDotPtr->rootSubDots->prev->name[0] + 1 : 'A';
    newDotPtr->name[1] = 0;
    
    //insert new dot in list entry.
    structDot* rootPtr = containingDotPtr->rootSubDots;
    if(rootPtr) //has entries.
    {
        structDot* rootNextPtrUnchanged = rootPtr->next;
        structDot* rootPrevPtrUnchanged = rootPtr->prev;
        if(rootPtr == rootNextPtrUnchanged || rootPtr == rootPrevPtrUnchanged) //after one entry
        {
            newDotPtr->next = rootPtr;
            newDotPtr->prev = rootPtr;
            rootPtr->next = newDotPtr;
            rootPtr->prev = newDotPtr;
        }
        else //after two or more entries.
        {
            newDotPtr->next = rootPtr;
            newDotPtr->prev = rootPrevPtrUnchanged;
            rootPtr->prev = newDotPtr;
            rootPrevPtrUnchanged->next = newDotPtr;
        }
    }
    else //after zero entries
    {
        containingDotPtr->rootSubDots = rootPtr = newDotPtr;
        rootPtr->next = newDotPtr;
        rootPtr->prev = newDotPtr;
    }
    
    //containing dot data
    float prospectDistance = Vector2Distance(containingDotPtr->avgCenter , canvasPosition) * 1.5f;
    if(prospectDistance > containingDotPtr->radius)
    {
        containingDotPtr->radius = prospectDistance;
    }
    containingDotPtr->avgCenter = Vector2Scale(Vector2Add(containingDotPtr->avgCenter , canvasPosition) , 0.5f);
    
    return newDotPtr;
}

void  RemoveSubDotsRecursive(structDot* dotPtr)
{
    structDot* rootDot = dotPtr->rootSubDots;
    if(rootDot)
    {
        structDot* iterator = rootDot;
        do
        {
            structDot* next = iterator->next;
            RemoveSubDotsRecursive(iterator);
            iterator = next;
        } while (iterator != rootDot);
    }
    free(dotPtr);
}

//Remove subdot from containing dot. All parameters must not be null..
bool RemoveDot(structDot* dotPtr , structDot* containingDotPtr)
{
    structDot* rootDot = containingDotPtr->rootSubDots;
    if(rootDot)
    {
        structDot* iterator = rootDot;
        do
        {
            if (iterator == dotPtr)
            {
                if (rootDot == rootDot->next) //is root and sole sub dot.
                    containingDotPtr->rootSubDots = NULL;
                else if (rootDot == iterator) //is root and NOT sole sub dot.
                    containingDotPtr->rootSubDots = rootDot->next;
                iterator->next->prev = iterator->prev;
                iterator->prev->next = iterator->next;
                RemoveSubDotsRecursive(iterator);
                return true;
            }
            iterator = iterator->next;
        } while (iterator != rootDot);
    }
    return false;
}

//All parameters must not be null.
bool SameCategory(structDot* dotPtr , structDot* dotMatePtr)
{
    structDot* iterator = dotMatePtr;
    if(iterator)
    {
        do
        {
            if (iterator == dotPtr)
                return true;
            iterator = iterator->next;
        } while (iterator != dotMatePtr);
    }
    return false;
}

//All parameters must not be null.
struct structLink* AddLink( structDot* start ,  structDot* end , structDot* containingDotPtr )
{
    structLink* rootPtr = containingDotPtr->rootSubLinks;

    //Return nothing if it already exists
    if(rootPtr) //has entries.
    {
        structLink* iterator = rootPtr;
        do
        {
            if (iterator->startDot == start && iterator->endDot == end)
            {
                printf("Link already exists.\n");
                return NULL;
            }
            iterator = iterator->next;
        } while (iterator != rootPtr);
    }
    //malloc and initialize new Link. Always initialize variables that are not being overwritten.
    structLink* newLinkPtr = (structLink*) malloc(sizeof(structLink));
    newLinkPtr->startDot = start;
    newLinkPtr->endDot = end;
        
    //insert new Link in list entry.
    if(rootPtr) //has entries.
    {
        structLink* rootNextPtrUnchanged = rootPtr->next;
        structLink* rootPrevPtrUnchanged = rootPtr->prev;
        if(rootPtr == rootNextPtrUnchanged || rootPtr == rootPrevPtrUnchanged) //after one entry
        {
            newLinkPtr->next = rootPtr;
            newLinkPtr->prev = rootPtr;
            rootPtr->next = newLinkPtr;
            rootPtr->prev = newLinkPtr;
        }
        else //after two or more entries.
        {
            newLinkPtr->next = rootPtr;
            newLinkPtr->prev = rootPrevPtrUnchanged;
            rootPtr->prev = newLinkPtr;
            rootPrevPtrUnchanged->next = newLinkPtr;
        }
    }
    else //after zero entries
    {
        containingDotPtr->rootSubLinks = rootPtr = newLinkPtr;
        rootPtr->next = newLinkPtr;
        rootPtr->prev = newLinkPtr;
    }    
    return newLinkPtr;
}

bool RemoveLink(structLink* linkPtr , structDot* containingDotPtr)
{
    structLink* rootLink = containingDotPtr->rootSubLinks;
    if(rootLink)
    {
        structLink* iterator = rootLink;
        do
        {
            if (iterator == linkPtr)
            {
                if (rootLink == rootLink->next) //is root and sole sub Link.
                    containingDotPtr->rootSubLinks = NULL;
                else if (rootLink == iterator) //is root and NOT sole sub Link.
                    containingDotPtr->rootSubLinks = rootLink->next;
                iterator->next->prev = iterator->prev;
                iterator->prev->next = iterator->next;
                free(iterator);
                return true;
            }
            iterator = iterator->next;
        } while (iterator != rootLink);
    }
    return false;
}

//Renders all dots and links. Returns activated dot.
#define OUTSCOPED functionDepth > selectedDepth + 1
#define SUBSCOPE functionDepth == selectedDepth + 1
#define INSCOPE functionDepth == selectedDepth
#define PRESCOPE functionDepth < selectedDepth
struct structActiveResult RecursiveDraw(structDot* dotPtr , int functionDepth , int selectedDepth)
{
    //check collision with mouse, generate first part of activeResult.
    structActiveResult activeResult = (structActiveResult){0};
    activeResult.resultKey = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ? RESULT_CLICK : (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) ? RESULT_RCLICK : RESULT_NONE);
    activeResult.resultDepth = functionDepth;
    if (functionDepth >= selectedDepth && CheckCollisionPointCircle( GetMousePosition() , dotPtr->avgCenter , 30.0f))
    {
        activeResult.resultDot = dotPtr;
    }

     //iterate through the functors in the category.
    structLink* rootLinkPtr = dotPtr->rootSubLinks;
    if (rootLinkPtr)
    {
        structLink* iterator = rootLinkPtr;
        do
        {
            if (INSCOPE)
            {
                bool linkHit = false;
                if (iterator->startDot == iterator->endDot)
                {
                    linkHit = CheckCollisionPointCircle( GetMousePosition() , Vector2Add(iterator->startDot->avgCenter , (Vector2){15.0f,15.0f}) , 30.0f );
                    DrawRectangleV(iterator->startDot->avgCenter , (Vector2){10.0f,10.0f}, linkHit ? RED : BLACK);
                }
                else
                {
                    Vector2 midPoint = Vector2Lerp(iterator->startDot->avgCenter , iterator->endDot->avgCenter , MONAD_LINK_MIDDLE_LERP);
                    linkHit = CheckCollisionPointCircle( GetMousePosition() , midPoint , 30.0f );
                    DrawLineBezier(iterator->startDot->avgCenter , midPoint ,  2.0f , linkHit ? PURPLE : BLUE);
                    DrawLineBezier(midPoint , iterator->endDot->avgCenter ,  1.0f ,  SameCategory( iterator->endDot , iterator->startDot ) ? BLACK : RED);
                }
                if (linkHit)
                {
                    activeResult.resultLink = iterator;
                    activeResult.resultDot = dotPtr;
                }
            }
            
            structLink* nextSaved = iterator->next;

            if (iterator->startDot->deleteFrame >= DELETE_POSTONLYLINK || iterator->endDot->deleteFrame >= DELETE_POSTONLYLINK)
            {
                if(RemoveLink(iterator , dotPtr) && !(rootLinkPtr = dotPtr->rootSubLinks))
                {
                    break;
                }
            }
            iterator = nextSaved;

        } while (iterator != rootLinkPtr);
    }
 
    //iterate through the objects with this object treated as a category.
    structDot* rootDotPtr = dotPtr->rootSubDots;
    if (rootDotPtr)
    {
        structDot* iterator = rootDotPtr;
        do
        {
            structDot* next = iterator->next;
            
            if (iterator->deleteFrame >= DELETE_FINAL)
            {
                if (RemoveDot(iterator ,  dotPtr) && !(rootDotPtr = dotPtr->rootSubDots))
                {    
                    break;
                }
                else
                    DrawLineV(dotPtr->avgCenter , iterator->avgCenter , RED); // something went wrong if still shows.
                iterator = next;
                continue;
            }
            else if (INSCOPE)
            {
                DrawLineV(dotPtr->avgCenter , iterator->avgCenter , iterator == rootDotPtr ? VIOLET : GREEN);
                DrawLineV(next->avgCenter , iterator->avgCenter , Fade(YELLOW , 0.5f));
            }

            //--------------------------------
            structActiveResult activeOveride = RecursiveDraw(iterator , functionDepth + 1 , selectedDepth);
            //--------------------------------

            if (activeOveride.resultDot && !activeResult.resultLink)
            {
                activeResult = activeOveride;
            }
            else if(activeOveride.resultLink)
            {
                activeResult.resultLink = activeOveride.resultLink;
                activeResult.resultDot = dotPtr;
            }

            iterator = next;
        } while (iterator != rootDotPtr);
    }
  
    //mark for deletion progression
    if (dotPtr->deleteFrame >= DELETE_PRELINK)
    {
            dotPtr->deleteFrame++;
            return (structActiveResult){0};
    }
    else if(dotPtr->deleteFrame >= DELETE_POSTONLYLINK)
    {
        dotPtr->deleteFrame--;
    }

   // cancel any more drawing.
    if (OUTSCOPED)
        return (structActiveResult){0};

    //we have returned back to the container, since this is null, we know that this is the container.
    if (!activeResult.resultContainerDot && activeResult.resultDot != dotPtr)
        activeResult.resultContainerDot = dotPtr;

    if(INSCOPE)
    {
        //DrawCircleLinesV(dotPtr->avgCenter , dotPtr->radius , GREEN);
        DrawPoly(dotPtr->avgCenter, 3 ,  5.0f ,   0 , PURPLE);
        DrawText(dotPtr->name , dotPtr->avgCenter.x + 10 , dotPtr->avgCenter.y + 10 , 24 , Fade(PURPLE , 0.5f));
    }
    else if(PRESCOPE)
    {
        DrawCircleLinesV(dotPtr->avgCenter , dotPtr->radius , Fade(GRAY , (float){functionDepth} / (float){selectedDepth}) );
    }
    else if(SUBSCOPE)
    {     
        DrawCircleV(dotPtr->avgCenter , 5.0f , BLUE);
        DrawText(dotPtr->name , dotPtr->avgCenter.x + 10 , dotPtr->avgCenter.y + 10 , 16 , Fade(SKYBLUE , 0.5f));
    }
    if(activeResult.resultDot == dotPtr)
        DrawCircleLinesV(dotPtr->avgCenter , 20.0f , ORANGE); 

    return activeResult;
}

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Monads");
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Variables
    //--------------------------------------------------------------------------------------
    structDot GodDot = (structDot){0};
    GodDot.avgCenter.x = (float){screenWidth}/2.0;
    GodDot.avgCenter.y = (float){screenHeight}/2.0;
    GodDot.defaultCenter = GodDot.avgCenter;
    GodDot.prev = &GodDot;
    GodDot.next = &GodDot;
    strcpy(GodDot.name , "Monad 0");
    
    char monadLog[MONAD_NAME_SIZE*3] = "Session started.";
    structDot* selectedDot = NULL;
    structLink* selectedLink = NULL;
    int selectedDepth = 0;
    structActiveResult mainResult =  (structActiveResult){0};
    //--------------------------------------------------------------------------------------

    // Testing
    //--------------------------------------------------------------------------------------    
    AddDot((Vector2){600,500}, &GodDot);
    AddDot((Vector2){200,400}, &GodDot);
    AddDot((Vector2){100,100} , AddDot((Vector2){350,200}, &GodDot));

    structDot* example =  AddDot((Vector2){400,400}, &GodDot);
    AddDot((Vector2){440,410}, example);
    AddDot((Vector2){400,450}, example);
    AddDot((Vector2){500,500}, example);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (selectedDot)
        { 
            if (selectedLink && IsKeyPressed(KEY_DELETE))
            {
                strcpy(monadLog , "Link [");
                strcat(monadLog , selectedLink->startDot->name);
                strcat(monadLog , "] -> [");
                strcat(monadLog , selectedLink->endDot->name);
                if (RemoveLink(selectedLink , selectedDot))
                    {
                        selectedLink = NULL;
                        strcat(monadLog , "] deleted.");
                    }
                else
                    strcat(monadLog , "] failed to delete.");
            }
            else if (IsKeyPressed(KEY_DELETE) && selectedDot != &GodDot)
            {
                strcpy(monadLog , "Deleting object [");
                strcat(monadLog , selectedDot->name);
                strcat(monadLog , "].");
                selectedDot->deleteFrame = DELETE_PRELINK;
            }
           else if (IsKeyPressed(KEY_B))
            {
                strcpy(monadLog , "Broke all links from and to [");
                strcat(monadLog , selectedDot->name);
                strcat(monadLog , "].");
                selectedDot->deleteFrame = DELETE_ONLYLINK;
            }
           else if (IsKeyPressed(KEY_V))
            {
                strcpy(monadLog , "Renamed [");
                strcat(monadLog , selectedDot->name);
                strcat(monadLog , "] to [");
                strncpy(selectedDot->name , GetClipboardText() , MONAD_NAME_SIZE);
                selectedDot->name[MONAD_NAME_SIZE - 1] = '\0'; //ensures NULL termination.
                strcat(monadLog , selectedDot->name);
                strcat(monadLog , "].");
            }
        }
        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            mainResult = RecursiveDraw(&GodDot  , 0 , selectedDepth);

            DrawText( monadLog , 48 , 16, 16 , GRAY);

            if(selectedDot)
            {
                int determineMode = selectedDot->depth - selectedDepth;
                DrawText( !determineMode ? "Adding" : determineMode == 1 ? "Linking" : "Edit Only" , 32, 32, 20, SKYBLUE);
                DrawPoly(selectedDot->avgCenter, 4 ,  2.0f ,   0 , RED);
            }
            else
            {
                DrawText( "Null Selection" , 32, 32, 20, ORANGE);
            }
            if(selectedLink)
            {
                DrawText("Edit Link" , 32, 64, 20, PURPLE);
                Vector2 midPoint = Vector2Lerp(selectedLink->startDot->avgCenter , selectedLink->endDot->avgCenter , MONAD_LINK_MIDDLE_LERP);
                DrawLineBezier(selectedLink->startDot->avgCenter , midPoint ,  4.0f , Fade(RED,0.5f));
                DrawLineBezier(midPoint , selectedLink->endDot->avgCenter ,  2.0f ,  Fade(SameCategory( selectedLink->endDot , selectedLink->startDot ) ? RED : PURPLE , 0.5f) );
                DrawRectangleV(midPoint , (Vector2){25.0f,25.0f}, Fade(RED , 0.5f));
            }

            for (int m = 1, d = 1 ; m <= selectedDepth ; m *= 10 , d++)
            {
                char digit[2] = {'0' + (selectedDepth / m) % 10 ,  0};
                DrawText(digit, GetScreenWidth() - 32*d , 64 , 20 , SKYBLUE);
            }
        EndDrawing();

        switch (mainResult.resultKey)
        {
            case RESULT_NONE:
            break;
            case RESULT_CLICK:
                selectedDot = mainResult.resultDot;
                selectedLink = mainResult.resultLink;
                printf("Object %p, Link %p\n" , selectedDot , selectedLink);
            break;
            case RESULT_RCLICK:
                if(selectedDot)
                {
                   if (mainResult.resultDot && mainResult.resultContainerDot && mainResult.resultDepth == selectedDot->depth)
                   {
                       if(SameCategory(selectedDot , mainResult.resultDot))
                            AddLink(selectedDot , mainResult.resultDot , mainResult.resultContainerDot);
                        else
                            AddLink(mainResult.resultDot , selectedDot , mainResult.resultContainerDot);
                        selectedDot = mainResult.resultDot;
                   }
                   else if (selectedDepth == selectedDot->depth)
                   {
                        if (!mainResult.resultDot)
                            AddDot(GetMousePosition() ,  selectedDot);
                        else if (selectedLink && selectedLink->startDot->depth == mainResult.resultDot->depth)
                            selectedLink->endDot = mainResult.resultDot;
                   }
                }
            break;
        }
        
        float mouseMove = GetMouseWheelMove();
        if(mouseMove != 0)
        {
            selectedDepth += mouseMove > 0 ? 1 : -1;
            if (selectedDepth < 0)
                selectedDepth = 0;
        }
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------

    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
