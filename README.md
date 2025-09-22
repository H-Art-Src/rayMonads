# Infinite depth of monads (objects) with variable connections (functors) between them at any depth.

Use the mouse wheel to change the depth.

Click any object/connection to select it.

-If you're selecting an object currently hovering **over** the current depth, right clicking will add objects to it.\
-If you're selecting an object currently **at** the current depth, right clicking another object at the same depth will create a one-way connection travelling **to** the right-clicked object.\
-It will instead create a one way connection **from** the right-clicked object if it is in a different category (container object).\
-You can change a link's travelling **to** object if you are selecting it and its containing object (usually will also be selected by clicking the link) by right clicking an object at the same depth.
-When selecting an object, you can rename it by simply typing and using backspace.

- Key 'Delete' to delete a selected object and recursively delete all objects contained within that object (and so on) and their connections from and to.
- The Alt keys will clear the last action message.

The following commands need a control key held down to function:
- Key 'B' to delete all connections from and to a selected object.
If you are selecting a link it will delete that instead of an object.
- Key 'T' will rename the selected object to your clipboard contents.
- Key 'C' will copy the selected object's text data recursively to your clipboard.
- Key 'V' will paste the text data recursively as a new object contained by the selected object.
- Key 'A' will advance the selected link's end object to its neighboring one in its stead.

Holding a shift key will always select the object you right clicked for an operation.
If you hold a shift key while left clicking an object, you will go to its depth.
Command: gcc monads.c -o monads.exe -lraylib -lopengl32 -lgdi32 -lwinmm
