# chessbox
Source for Chess Box eboard controller and gui

# Samples
modified board
echo '{"action":"setposition","fen":"rnbqkbnr/pppppppp/8/8/8/PPPPPPPP/RNBQKBNR/8 w kq - 0 1"}' | nc -C -N chessbox 9999
normal board:
echo '{"action":"setposition","fen":"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w kq - 0 1"}' | nc -C -N chessbox 9999
echo '{"action":"move","description":null,"moves":[{"from":"b3","long":"b3b4","san":"b4","to":"b4","type":"move"}]}' | nc -C -N chessbox 9999
echo '{"action":"move","description":null,"moves":[{"from":"a2","long":"a2a3","san":"a3","to":"a3","type":"move"}]}' | nc -C -N chessbox 9999
echo '{"action":"setmode","mode":"inspect"}' | nc -C -N chessbox 9999