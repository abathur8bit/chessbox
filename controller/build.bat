pscp -r -p src README.md CMakeLists.txt pi@millipede:workspace/chesslrcontroller
plink -batch pi@chesslr "kill `ps aux | grep chesslrcontroller | grep -v grep | awk -F' ' '{print $2}'`"
plink -batch pi@millipede "cd workspace/chesslrcontroller && cmake -DCMAKE_BUILD_TYPE=Debug -f CMakeLists.txt && make && scp chesslrcontroller chesslr:workspace/chesslrcontroller"
