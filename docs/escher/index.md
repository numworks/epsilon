---
title: Escher
---
# Escher

## Overview

Escher is a layer which creates an UI (User Interface), with an high level solution. Instead of just settings pixels on screen, the apps use Escher, and its MVC system (Model View Controller). The app has a viewController, which has to handle the event (key press, USB connection, ...), and this controller has a View. Also, Escher has some other class, like clipboard, app, some usefull views, which will be used a lot of time (editable_text_cell, table_view, ...), Appcontainer (the environnment in which the apps will run), and run_loop (the loop which will make the firmware tick).

## viewController

The controller is like the root of the UI : it has to handle the keypress, via `handleEvent()`, but has also to handle input views' event, like with which events it has to leave the input, what to do when typing is finished,... when heriting from `TextFieldDelegate`. 

### stackViewController : a very usefull one

The `stackViewController` is a very special controller, which has to be the main controller of the app, but has also the root controller, which will be the real main controller. Its advantage is that you can `push` a viewController on it, and it will become the current viewController in the UI. You can also `pop` the last viewController pushed, which will be removed of the `stackViewController`, and the previous viewController will become the current one. This behavious is used, for exemple, when you enter in the graph range controller : the `parameterController` will be pushed, and when you press back, or you click on OK, it will be poped.

## view

The current viewController (if it is the toppest one on the stackViewController, or if it is the main one of the app) has always a function called `view()`, which returns the view handled by the viewController.
The view has some essential elements :
* a `drawRect` function, which will, like its name say, draw the view
* a `bounds` function, which will return the size of the view (because some views aren't the size of the screen)
* a `numberOfSubviews` function
* a `subviewAtIndex` function
* a `layoutSubviews` function

The last 3 functions are very interresting, because they will describe how a view is made.
If you take a look at [`title_bar_view`](https://github.com/numworks/epsilon/blob/master/apps/title_bar_view.cpp) for exemple, you'll see that it is made of 5 subviews. If we look how epsilon will draw this view, we'll see that it is made of steps :
* It will begin by calling the `layoutSubviews` function. This function defines the size and the position of each subviews. We can notice that when `layoutSubviews` is executed, it will also execute it in the subviews (it is recursive).
* After that, the `drawRect` function of the view will be called. In this case : it will fill the size of the view with YellowDark
* To finish, it will look how many subviews there are, and get them individualy (via `subviewAtIndex`). For each, it will execute the `drawRect` function.

### markAsDirty : a very usefull optimization

Some views can be very long to draw, and this can create issue, because the user don't want to wait. The solution used in epsilon is not to redraw the complete screen, when something has changed (for exemple, when you move the cursor in the graph view), but to `markAsDirty` the changed zone, and so escher, when it will try to redraw these parts will execute `drawRect` of each subviews, but with the changed rect in parameter, in order not to redraw the other unchanged parts.

