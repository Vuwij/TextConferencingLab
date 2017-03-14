PORT=5007
FILENAME=Twice.jpg

# BUILD FILES
make

i=0
for test_dir in test/*;
do
    i=$(( $i + 1 ))
    
    # Close the programs
    pkill client
    pkill server
    
    echo "------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
    echo "                                                                     Test $i - $test_dir"
    echo "------------------------------------------------------------------------------------------------------------------------------------------------------------------------"     
    echo "Server                           Client 1                         Client 2                          Client 3                          Client 4"
    echo "------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
    
    # Starts the server
    ./build/Server/server $PORT &
    sleep .5

    # Starts the clients
    build/Client/client "1" < "$test_dir/c1_input.txt" &
    sleep .01
    build/Client/client "2" < "$test_dir/c2_input.txt" &
    sleep .01
    build/Client/client "3" < "$test_dir/c3_input.txt" &
    sleep .01
    build/Client/client "4" < "$test_dir/c4_input.txt" &
    sleep 1
    echo "------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
    sleep 5
done
