Interaction Write-Up
--------------------

During this phase of the coursework, I was tasked to use OpenGL to get experience with graphic programming.

Notably, with little OpenGL experience I found the overall task rather enjoyable. I enjoy taking on new programming challenges such as this one, and luckily with the help of Stefano's template, I was able to get to grips with the task at hand.

This section required key steps in order to achieve the outcome of the scene I rendered using OpenGL. The following will document each step, detailing the problem solving used by myself in order to tackle each step and hopefully guide the marker throughout my code and understand why I chose to tackle this task in certain ways.

I want to note here for the reader that as I am a master's student, I will provide critical analysis and refelct on any of my explanataions regarding the topic at the hand. Throughout the analysis, I will discuss methods, code, procedures, designs and development processes.

1. Step One (Exporting Models From Blender)
-------------------------------------------
This task was fairly simple. The way I chose to do this was very much the standard way for exporting gltf objects from blender. During a lab this semester, we were taught how to do this, hence, I used this method here. The way to go about doing so is by selecting a model in blender, then in the export settings, you can just select to export as gltf seperate (this creates a .bin file too which could be handy in the future so I thought this was the best option). I did modify some settings however, exlcluding normals from the geometrty. The reason behind this was to do with the way normals were being exported and causing objects to render incorrectly through openGL. I also chose to automatically export related textures and materials alongside any animation data for an object. This was the most optimal and easy way to do this process and since I had several objects (models) to export, I repeated  the process throughout for all relative models in the scene.

You can see in my assets folder I have the exported gltf files there for each model I render within my openGL scene, feel free to browse through these for yourself!

2. Step Two (importing the exported gltf models into the graphic API template provided, how I rendered each object and drew them, and also refactoring my code for computational time speed up!)
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
In order to import the models, I went through a couple of different processes. Eventually refactoring my code at the end to speed up computation time and make things tidier. Originally I created a variable of type Content and then used modelName.LoadGLTF("assets/modelName"); to load in the model, after this, I would look to edit this model's starting position and rotation, and in the render function then use a modelMatrix to give some translation, rotation and scale values for the model then just draw the model. However, I found there to be a lot of issues regarding how the template was set up for using one model matrix or each model so I decided to create a struct at the top of my code in source.cpp and called this renderObject, the reason behind this was so that I could provide my struct with data types such as Content c, vec3 scale, modelRotation, modelPosition and also a mat4 modelMatrix. This allowed me to then load in models of type renderObject and each of them could have their own scale, model matrix and things such as a position and rotation. Please look at my code to see how I done this - it creates a tidier structure and saves time retyping lines of similar code. 

After this, in startup() I would look to set a model rotation, position and scale for each object. Then in the render function, I would create a model matrix for each object. This again, led to me thinking of new ways to provide a tidier and much more computational speedy procees for this. So I then refactored my code to use an array list. The reason behind this is that I can just create an array list with size n, where n is the num of models required to import and draw. I called my arraylist renderObjectsList and gave it a size of 7. 

This was one of the better times coding this, as now instead of rewriting lines of render code (model matrix's etc...) I could just use a simple for loop and render each model in the array list I created, looking at this now, I think this is something a master's student would think of compared to someone in their first year of undergrad etc.. Refactoring is a key part of programming and is required in the industry so when reflecting on this now, I am pleased I found this solution - also this saves you having to read multiple lines of code over and over again (you're welcome! haha)

2.1 A bug (found and fixed)
---------------------------
There was a bug in your content.cpp file I noticed in regard to how textures were being saved - they werent! look in my content.cpp to see how I fixed this for you. I just created a GLuint textureID variable in the header file of content and then in your bindMesh function, on my line 109, you can see how I just added in textureID = texID (storing each unique texID). The reason this is quite crucial is it stops overwriting happening on each model material. I was able to load each item's texture with ease due to this fix. However, I know some students have been struggling with this part however, that's up to them to find the problem and have a little dig themselves. 

I recommend adding this into your API template for future. You're welcome! 

3. Step three (interactive camera)
----------------------------------
After all the objects were loaded in the scene, I thought it would be time to get a fun camera added so that the user can wonder around the scene in any direction.

To do this, I just used some if(keyStatus) code (you can see it from lines 389 -> 408 in my source.cpp file, all I did here was modify the camera position on x and y axis by a relatively small number. Pretty easy and makes the scene better. I also added some keys (O, P) for zoom in and zoom out (offsetting)

4. Step three (selectable models, and making them interactive)
--------------------------------------------------------------
In order to have each model be selectable in the scene, I did much the same as I did with the camera however, I used my array list and a variable I created at the top (for indexing through the object list), I then set it for a simple if statement which when you press tab, it will go through each model in the scene, 1 per tab, and I had to use a modulo operand here in order for it to continue cycling (not just stop at 7). 

The next step was to add some keys and change the model rotations and positions. You can see how I did this from lines 355 -> 378.

Note here that interaction in the scene is pretty similar for most things.

5. Step four (lighting, and how to create interactive lighting in the scene)
----------------------------------------------------------------------------
This part was a little tricky to understand at first but once I had a look over lecture 12 of the course materials, I understood how to implements phong's lighting.

I edited both my vs_model and fs_model files (you can check these out in the shaders folder on my github repo), I used the code that was given to use in lecture 12's correlated lab and then saved the txt files accordingly. 

After this, within my source.cpp file, I added some variables related to the phong's equation, you can see this on lines 122 -> 130. This adds the constants and values for each of those to allow phong's lighting to be implemented
correctly. 

Then I had to render the lighting before I drew any of my models in render() so you can see how I did this in lines 479 -> 487 in source.cpp - just passing through each of the lighting values through the pipeline and to my shaders. 
Again, this stuff was taught to us during the term so not much to explain here that you won't know already! 

The fun I did have with this however, was creating some interaction with the lighting. You can see how I did this in my video contained within this repo! But to explain here, I just added some lines to the ImGUI code near the bottom of the source.cpp file so that the user can watch the ambient, diffuse and specular lighting values change (this shows up in the box on the bottom right of the screen). The reason I did this was because I want the user to be able to 
see the change (numerically) of each value and how it affects the scene.

After this, I added some keyStatus again (lines 412 -> 433) so that the user can play around with positioning of the lights and change the values of phong's lighting! Very fun and good for the scene - can create dramatic element.

Conclusion and overall extra information
----------------------------------------

Although this task was done well programming wise, you can clearly see my model scales are not really the best within the scene - this was an issue with the export from blender (I believe) and I tried multiple times different export options to fix this, however I just couldn't work it out! The models are actually fine and just need their scales fixed (some models could need positioning sorted etc..) but again, I tried forever to just fix this issue from the get-go and ended up deciding to just work with what I had and show my abilities else where. 

I feel like I have achieved a very high standard of programming here - even if I say so myself. I am rather proud of how I problem solved this. As a master's student, it is nice to see my time spent in undergrad paying off, as I can tell I am more suited to reading and understanding code, also looking at things from a different persepective - how to refactor etc... which for a long time was something I just didn't even think to do. So I hope as my course director and lecturer for this course, you can appreciate how I went around this!

I would have liked to have been able to get my scales correct so that the scene wasn't so messed up for some objects. Apologies here! I know this must be annoying to view but I hope you can look at how I tackled the other aspects such as materials / textures, interaction, buffer rates, refactor, lighting, etc.. and other things mentioned in the rubic.

Note here that I do understand I did not implement some animation for my raft in the scene. This is my own fault, I can only apologise again but with the way the raft scale had started, I didn't see it to be visably attractive or even
noticable for you to watch (haha), I did look over the lecture for animation progrmming in openGL however, I don't really have a great knowledge of openGL so I found this to be a little out my depths! Something I definitely need to work on as a master's level student.

Thank you for running a great course, throughout my time at university you stand out to me as your lectures are always interesting and fun to view, and I found these modules and the game programming one from last year to be the best I had sat so far! I really enjoy them - hence why I am doing the masters! 

Cheers,
Brendan Reid (H00258678)