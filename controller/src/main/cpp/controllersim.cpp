class ControllerSim : public TelnetServer {
}

int main(int argc,char* argv[]) {
    bool swap=false;
    bool turnOffLeds=false;
    unsigned16 wPort = 9999;
    for(int i=0; i<argc; i++) {
        if(!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help"))
            usage(argv[0]);
        else if(!strcmp(argv[i],"-s")) {
            swap=true;
        } else if(!strcmp(argv[i],"-p")) {
            wPort = atoi(argv[++i]);
        } else if(!strcmp(argv[i],"-l") || !strcmp(argv[i],"--leds-off")) {
            turnOffLeds=true;
        }
    }
    printf("Binding to port %d\n",wPort);
    SockAddr saBind((ULONG)INADDR_ANY,wPort);
    printf("%s runs on port %d\n",TITLE,wPort);
    ControllerServer server(saBind,swap);
    if(turnOffLeds) {
        printf("Turning off leds\n");
        server.turnOffLeds();
    } else {
        server.initGame();
        server.startServer();
    }
    printf("%s finished\n",TITLE);

//    auto output = make_shared<string>();
//    auto error = make_shared<string>();
//
//Process p("/home/pi/workspace/chessengine/stockfish-8-linux/src/stockfish","",[output](const char *bytes, size_t n) {
//    *output = string(bytes, n);
//    printf("--> %s\n",output->c_str());
//}, nullptr,true);
//p.write("uci\n");
//p.write("quit\n");
//printf("process exit=%d output=[%s]\n",p.get_exit_status(),output->c_str());

    return 0;
}