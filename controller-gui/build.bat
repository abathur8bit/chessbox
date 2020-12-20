pscp -r -p src README.md CMakeLists.txt assets pi@millipede:workspace/chessbox/controller-gui
plink -batch pi@millipede "cd workspace/chessbox/controller-gui && cmake -DCMAKE_BUILD_TYPE=Debug -f CMakeLists.txt && make
