# Engine3D Editor

## Overview
This is the UI editor for Engine3D. Where it is going to be considered as a seperate project, as I have some plans for using Engine3D \
for other things in the future. During the development of Engine3D, this will be used to test Engine3D in the form of a UI Editor. Showing \
different capabilities of what Engine3D can do.

## UI Editor Layout
![Screenshot 2024-03-12 at 5 36 15 AM](https://github.com/SpinnerX/Engine3D-Editor/assets/56617292/fd0f8aa1-aeb7-441b-bd12-21aead8600dd)


# Running the editor

Go to GitHub Releases. Then unzip the downloaded zip file called, `Windows.zip`.

Then inside of the Windows directory. Click the Editor file to run the editor.

Once you run the editor, the following tutorial `How to use` is going to be how to use the editor to create and run your own scenes with physics simulation enabled.

# Tutorial to Use

## Creating a Camera Object

Engine3D does not create a camera object for you. So, this is how you create a simple camera scene object using the editor.

* Right-click the scene heirarchy panel. Then click `Create Empty Entity`.
* Click `Add Component` to add the camera component to that game object.
* Once created, rename the entity to Camera for clarity.

https://github.com/user-attachments/assets/6fdc0fd2-df47-43ae-b8cb-7ccea2b80f5b

There you go! You created your first object using the editor.

## Creating a Game Object Renderable

Now, you created a camera. I'll show you how to create a game object that you can see. In this example it'll be a square since this project has many limitations. You can only create squares.

* Right-click scene heirarchy panel, again.
* Click `Create Empty Entity` to create the a new object again.
* Add the `SpriteRendererComponent` from the editor.

Then, you can interact with that square by modifying the color by dragging the color palette itself or adding a texture

To add a texture you can dragging, an image from the content browser panel.

https://github.com/user-attachments/assets/18c6565c-d412-473d-b82f-61246b5bd14b

## Setting up game objects for running physics simulation

In this section, we are going to create a second object. This object will act as our platform to showcase some fundamental physics operations that can be done through the editor.

To run physics:

For the object called `Same Game Object` add these components from the editor:
* RigidBody2D Component
* BoxCollider2D Component

For the object called `Platform`, add the same components from the editor:
* RigidBody2D Component
* BoxCollider2D Component

## Running Physics

To run physics, you can just run the play button after adding the components.

This video shows a follow-up example of how to setup the objects. Then running the physics simulation.


https://github.com/user-attachments/assets/a97961a0-b273-478d-a9e5-2b8fbe2f9a9c

# Saving a scene

The editor allows capabilities of saving, clearing, and loading a scene. This is how you can save this new scene created.

To save a scene clicked `File` at the top. Then click `Save As`, then select the path of where you would like to save your scene.

## Loading A Scene

To load a scene, click `File`. Then click `Open` and choose the type as `*` to see scenes inside of the editor assets.



