build/Server/server 5007 &
sleep 1
build/Client/client 1 < test/call/c1_input.txt &
sleep 0.1
#build/Client/client 2 < test/call/c2_input.txt &
sleep 100
