@echo off

echo Generating Visual studio files (.vcproj)

cd src
qmake -t vcapp

cd core
qmake -t vclib

cd network
qmake -t vclib

cd ..

cd jobs
qmake -t vclib

cd ..\..

cd dialogs
qmake -t vclib

cd ..

cd widgets
qmake -t vclib

cd photowidgets
qmake -t vclib

cd ..

cd wizard
qmake -t vclib

cd ..\..

echo Updating and generating translations
cd translations
qmake

cd ..\..

echo Done
pause
