Interaction Write-Up
--------------------

During this phase of the coursework, I was tasked to use OpenGL to get experience with graphic programming.

Notably, with little OpenGL experience I found the overall task rather enjoyable. I enjoy taking on new programming challenges such as this one, and luckily with the help of Stefano's template, I was able to get to grips with the task at hand.

This section required key steps in order to achieve the outcome of the scene I rendered using OpenGL. The following will document each step, detailing the problem solving used by myself in order to tackle each step and hopefully guide the marker throughout my code and understand why I chose to tackle this task in certain ways.

I want to note here for the reader that as I am a master's student, I will provide critical analysis and refelct on any of my explanataions regarding the topic at the hand. Throughout the analysis, I will discuss methods, code, procedures, designs and development processes.

1. Step One (Exporting Models From Blender)
-------------------------------------------
This task was fairly simple. The way I chose to do this was very much the standard way for exporting gltf objects from blender. During a lab this semester, we were taught how to do this, hence, I used this method here. The way to go about doing so is by selecting a model in blender, then in the export settings, you can just select to export as gltf seperate (this creates a .bin file too which could be handy in the future so I thought this was the best option). I did modify some settings however, exlcluding normals from the geometrty. The reason behind this was to do with the way normals were being exported and causing objects to render incorrectly through openGL. I also chose to automatically export related textures and materials alongside any animation data for an object. This was the most optimal and easy way to do this process and since I had several objects (models) to export, I repeated  the process throughout for all relative models in the scene.
