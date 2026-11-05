cls

del *.exe

del *.obj

cl.exe /c /EHsc /I "D:\RTR-6\Practice\DS-Demo\02-SecondScene\stb-master" OGL.c

cl.exe /c /EHsc stb_image.c

rc.exe OGL.rc

link.exe OGL.obj stb_image.obj OGL.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

