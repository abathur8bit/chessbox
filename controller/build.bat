@echo off
pscp -r -p src README.md CMakeLists.txt pi@millipede:workspace/chessbox/controller
if "%1" == "chesslr" (
    echo Build and deploy to Chessbox
    plink -batch pi@chesslr "kill `ps aux | grep cbcontroller | grep -v grep | awk -F' ' '{print $2}'`"
    plink -batch pi@millipede "cd workspace/chessbox/controller && cmake -DCMAKE_BUILD_TYPE=Debug -f CMakeLists.txt && make && scp cbcontroller chesslr:workspace/chessbox/controller"
) else (
    echo Build only
    plink -batch pi@millipede "cd workspace/chessbox/controller && cmake -DCMAKE_BUILD_TYPE=Debug -f CMakeLists.txt && make"
)

:done