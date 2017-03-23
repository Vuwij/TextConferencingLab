PORT=5007
FILENAME=Twice.jpg
VALGRIND=1
TEST="test/call"

# BUILD FILES
make

i=0
for test_dir in test/*;
do
    if [ ! -z "$TEST" ] 
    then 
        if [ "$test_dir" != "$TEST" ]
        then
            continue
        fi
    fi

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
    sleep 5
    #ssh wangj261@ug224 "cd Documents/ECE361/TextConferencingLab/ && build/Client/client '2' < '$test_dir/c2_input.txt' &"
    #sleep .2
    echo "------------------------------------------------------------------------------------------------------------------------------------------------------------------------"
    sleep 60
done