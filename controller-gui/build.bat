@echo off
pscp -r -p src README.md CMakeLists.txt assets pi@millipede:workspace/chessbox/gui
pscp -r -p ../common/* pi@millipede:workspace/chessbox/common
if "%1" == "chessbox" (
    echo Build and deploy to Chessbox
    plink -batch pi@chesslr "kill `ps aux | grep cbgui | grep -v grep | awk -F' ' '{print $2}'`"
    plink -batch pi@millipede "cd workspace/chessbox/gui && cmake -DCMAKE_BUILD_TYPE=Debug -f CMakeLists.txt && make && scp cbgui chesslr:workspace/chessbox/gui"
) else (
    echo Build only
    plink -batch pi@millipede "cd workspace/chessbox/gui && cmake -DCMAKE_BUILD_TYPE=Debug -f CMakeLists.txt && make"
)
