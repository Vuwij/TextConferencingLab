PORT=5007
FILENAME=Twice.jpg

# BUILD FILES
make

i=0
for test_dir in test/*;
do
    i=$(( $i + 1 ))
    
    # Close the programs
    pkill server
    pkill -9 $(pgrep client)
    
    echo "Test $i - $test_name"
    echo "Server                           Client 1                         Client 2                          Client 3                          Client 4"
    echo "------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
    
    # Starts the server
    cd build/Server
    ./server $PORT &
    cd ../..
    sleep .5

    # Starts the clients
    build/Client/client "1" < "$test_dir/c1_input.txt" &
    sleep .01
    build/Client/client "2" < "$test_dir/c2_input.txt" &
    sleep .01
    build/Client/client "3" < "$test_dir/c3_input.txt" &
    sleep .01
    build/Client/client "4" < "$test_dir/c4_input.txt" &
    sleep .01
    
    sleep 10
done