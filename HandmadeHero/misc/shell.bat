@echo off
doskey debug=devenv "C:\Users\loomisdf\Documents\Visual Studio 2013\Projects\HandmadeHero\build\win32_handmade.exe"
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64
set path="C:\Users\loomisdf\Documents\Visual Studio 2013\Projects\HandmadeHero\HandmadeHero\misc";%path%
