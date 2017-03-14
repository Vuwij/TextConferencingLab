PORT=5000
FILENAME=Twice.jpg

# BUILD FILES
make

# CLOSE TESTING PROGRAMS
pkill server

echo "Server                           Client 1                         Client 2                          Client 3                          Client 4"
echo "------------------------------------------------------------------------------------------------------------------------------------------------------------------------"

# Starts the server
cd build/Server
./server $PORT &
cd ../..
sleep .5

# Starts the clients
build/Client/client "1" < 'test/c1_input.txt' &
sleep .01
#build/Client/client "2" < 'test/c2_input.txt' &
#sleep .01
#build/Client/client "3" < 'test/c3_input.txt' &
#sleep .01
#build/Client/client "4" < 'test/c4_input.txt' &
#sleep .01

sleep 10
