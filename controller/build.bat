@echo off
if "%1" == "chessbox" (
    echo Build on Chessbox
    rem pscp -r -p ../common/* pi@chessbox:workspace/chessbox/common
    rem pscp -C ../common/json.hpp pi@chessbox:workspace/chessbox/common
    rem pscp -C ../common/thc.cpp pi@chessbox:workspace/chessbox/common
    rem pscp -C ../common/thc.h pi@chessbox:workspace/chessbox/common
    pscp -C ../common/actiontest.cpp pi@chessbox:workspace/chessbox/common
    pscp -C ../common/BoardRules.h pi@chessbox:workspace/chessbox/common
    pscp -C ../common/chessaction.hpp pi@chessbox:workspace/chessbox/common
    pscp -C ../common/chessmove.hpp pi@chessbox:workspace/chessbox/common
    pscp -C ../common/defs.h pi@chessbox:workspace/chessbox/common
    pscp -C ../common/generalexception.cpp pi@chessbox:workspace/chessbox/common
    pscp -C ../common/generalexception.h pi@chessbox:workspace/chessbox/common
    pscp -C ../common/msdefs.h pi@chessbox:workspace/chessbox/common
    pscp -C ../common/PGNUtils.cpp pi@chessbox:workspace/chessbox/common
    pscp -C ../common/PGNUtils.h pi@chessbox:workspace/chessbox/common
    pscp -C ../common/README.md pi@chessbox:workspace/chessbox/common
    pscp -C ../common/socketinstance.cpp pi@chessbox:workspace/chessbox/common
    pscp -C ../common/socketinstance.h pi@chessbox:workspace/chessbox/common
    pscp -C ../common/UCIClient.cpp pi@chessbox:workspace/chessbox/common
    pscp -C ../common/UCIClient.h pi@chessbox:workspace/chessbox/common
    pscp -C ../common/ucitest.hpp pi@chessbox:workspace/chessbox/common

    pscp -r -p src README.md CMakeLists.txt pi@chessbox:workspace/chessbox/controller
    echo Killing any existing processes
    plink -batch pi@chessbox "kill `ps aux | grep cbcontroller | grep -v grep | awk -F' ' '{print $2}'`"
    echo Building
    plink -batch pi@chessbox "cd ~/workspace/chessbox/controller && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make \;"
    echo Done
) else if "%1" == "chessboxshort" (
    echo Build on Chessbox only controller
    pscp -C -r src pi@chessbox:workspace/chessbox/controller
    echo Killing any existing processes
    plink -batch pi@chessbox "kill `ps aux | grep cbcontroller | grep -v grep | awk -F' ' '{print $2}'`"
    echo Building
    plink -batch pi@chessbox "cd ~/workspace/chessbox/controller && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make"
    echo Done
) else if "%1" == "millipede_to_chessbox" (
    echo Build on millipede and deploy to Chessbox
    pscp -r -C ../common/* pi@millipede:workspace/chessbox/common
    pscp -r -C src README.md CMakeLists.txt pi@millipede:workspace/chessbox/controller
    plink -batch pi@chesslr "kill `ps aux | grep cbcontroller | grep -v grep | awk -F' ' '{print $2}'`"
    plink -batch pi@millipede "cd workspace/chessbox/controller && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make && echo 'Sending to Chessbox' && find . -maxdepth 1 -type f -executable -exec scp {} chesslr:chessbox \;"
) else (
    echo Build only
    plink -batch pi@millipede "cd workspace/chessbox/controller && mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make"
)

:done