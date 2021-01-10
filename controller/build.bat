@echo off
pscp -r -p ../common/* pi@millipede:workspace/chessbox/common
pscp -r -p src README.md CMakeLists.txt pi@millipede:workspace/chessbox/controller
if "%1" == "chessbox" (
    echo Build and deploy to Chessbox
    plink -batch pi@chesslr "kill `ps aux | grep cbcontroller | grep -v grep | awk -F' ' '{print $2}'`"
    plink -batch pi@millipede "cd workspace/chessbox/controller && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && echo 'Sending to Chessbox' && find . -maxdepth 1 -type f -executable -exec scp {} chesslr:chessbox \;"
) else (
    echo Build only
    plink -batch pi@millipede "cd workspace/chessbox/controller && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make"
)

:done