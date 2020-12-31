@echo off
pscp -r -p src CMakeLists.txt README.md assets pi@millipede:workspace/chessbox/gui
pscp -r -p ../common/* pi@millipede:workspace/chessbox/common
if "%1" == "chessbox" (
    echo Build and deploy to Chessbox
    pscp -r -p README.md assets pi@chessbox:workspace/chessbox/gui
    plink -batch pi@chessbox "kill `ps aux | grep cbgui | grep -v grep | awk -F' ' '{print $2}'`"
    plink -batch pi@millipede "cd workspace/chessbox/gui && cmake -DCMAKE_BUILD_TYPE=Debug -f CMakeLists.txt && make && scp cbgui chessbox:workspace/chessbox/gui"
) else (
    echo Build only
    plink -batch pi@millipede "cd workspace/chessbox/gui && cmake -DCMAKE_BUILD_TYPE=Debug -f CMakeLists.txt && make"
)
