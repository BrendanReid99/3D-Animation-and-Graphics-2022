ReadMe - Animation

Readme.md (Explain how the objects were  imported to Unreal, any pipeline fixes and issues sorted, and how you created your animation (e.g. 
moving objects, moving camera, other animations). 

How I imported objects: For this, I actually included a screen shot within the render file (my mistake but you can access it from there) I just deselected the cameras and lighting and then using 'A' selected all objects within the scene. Then with the settings I just selected for mesh only and exported into a FBX objects file. 

In Unreal Engine 5, I then just used the import button and dragged in my FBX file which transferred over all the objects needed. I then selected each mesh from the top to bottom and dragged them into the scene (which I created a new blank level for) and the materials loaded onto each object. 

Pipeline fixes: I had to manually reset all the pivot points of the objects as they get thrown off when importing, this is a simple fix! Use the alt + middle mouse button to drag the pivot point to the centre of the related object and then use right click, pivots and set the pivot to the current origin.

Master's student comment: I will include some screen shots to show case what I meant about fixing this.

Animation: So initially I animated using blender and then when importing discovered an error I couldn't quite work out how to fix, it was to do with frames and I thought I would just use UE5's animation sequencer to animate fully - which worked a treat! 

So for this I animated the cine machine camera (FLY-Through Camera) and the raft. I did this differently for each one. 

For the raft: I set a good length of frames to allow for a smooth animation, I set a location keyframe and a rotation keyframe at 0000 and then at certain points of the frame, moved the object and updated it's location and rotation accordingly until the animation ends. This was really fun as I haven't done much animation before and although rather basic, did create the effect I was after which was to see the raft moving like it would in shallow water - I think it was good! 

For the camera: I used a level sequence, then added a cine machine camera as an actor and dragged it into the sequencer, then I did much the same as I did for the raft however I had to give it a 'fly-through' variation as per the CW spec, so to do this I just altered the transform at different parts and tried to make it as smooth as possible. I then rendered the animation using the sequencer and changed the output settings to compress at a quality of around 75%.

Master's Student Overview and Thoughts looking back
---------------------------------------------------

Originally I found it easier to animate in blender and had a fire movement too but when moving to UE5 found to have pipeline issues surrounding the animations so I decided to focus on the camera and the raft. In the future, there are a lot of things I would change here. I would animate each individual object in the scene and have multiple fly-through camera to do interesting cuts and create nice looking angle variations like you see on TV show's such as Game of Thrones. 

I also would like to do something with the sea, however, for the time being thought it would be best to leave out as I wasn't happy with the material I used.

Finally, I found UE5 quite tricky to get used to and actually really enjoyed using the sequencer. This has gave me ideas for prototype animations I could use in my master's projects over the years and try and improve my portfolio so that I could show game companies I have a rough understanding of how 3D modelling and animations actually work within engines such as UE5!

Also refer to the LINK TO ALL PROJECTS section to find the animation video and UE5 project!

Cheers,
Brendan Reid (H00258678)
