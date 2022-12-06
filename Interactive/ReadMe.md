Interactive Write-Up
--------------------

Bug Fix: texID storage issue solved (in content.cpp / content.hdd), in BindMesh texID wasn't being stored, so just applied same tex to each loaded material, solved by including a GLunit variable (textureID)
in content.hdd and then in content.cpp line 109 stored the texID to my GLunit variable, then in drawMesh function (content.cpp) bound the texture2d to the textureID stored (loaded materials for each object
individually) solving this issue - took some time to work out but got there in the end. Maybe look at the template for next year and go about doing this so students don't run into the same issue!

Interactive Camera: using lab work, I created an interactive camera using WASD keys alongside fixing an issue in the modelmatrix for offsetting z axis for the camera (zoom out and in) function

renderObject struct: created a struct to make things easier when doing modelMatrix for each loaded object, allowed editable translations, rotations and scales for each loaded object individually! great
for saving time when doing render for each object (saves time and makes code more efficient to use for the student)
