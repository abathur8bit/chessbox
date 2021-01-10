@echo off
pscp -r -p src CMakeLists.txt README.md assets pi@millipede:workspace/chessbox/controller-gui
pscp -r -p ../common/* pi@millipede:workspace/chessbox/common
if "%1" == "chessbox" (
    echo Build and deploy to Chessbox
    pscp -r -p README.md assets pi@chessbox:chessbox
    plink -batch pi@chessbox "kill `ps aux | grep cbgui | grep -v grep | awk -F' ' '{print $2}'`"
    plink -batch pi@millipede "cd workspace/chessbox/controller-gui && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && scp cbgui chessbox:chessbox"
) else (
    echo Build only
    plink -batch pi@millipede "cd workspace/chessbox/controller-gui && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make"
)
