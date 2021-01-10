@echo off
pscp -r -p common/* pi@millipede:workspace/chessbox/common
cd controller
pscp -r -p src README.md CMakeLists.txt pi@millipede:workspace/chessbox/controller
cd ..\controller-gui
pscp -r -p src CMakeLists.txt README.md pi@millipede:workspace/chessbox/controller-gui
cd ..
echo Running tests
plink -batch pi@millipede "cd workspace/chessbox/ && ./tests.sh"

:done