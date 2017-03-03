PORT=5007
FILENAME=Twice.jpg

# BUILD FILES
make

# CLOSE TESTING PROGRAMS
pkill server

echo "Server                           Client 1                         Client 2                          Client 3                          Client 4"

# Starts the server
cd build/Server
./server $PORT &
cd ../..
sleep .5

# Starts the client
echo ""

build/Client/client "1" << 'EOF'
/login 1 User1 127.0.0.1 5007
/joinsession 1
/createsession 1
EOF

build/Client/client "2" << 'EOF'
/login 1 User2 127.0.0.1 5007
/joinsession 1
EOF

build/Client/client "3" << 'EOF'
/login 1 User3 127.0.0.1 5007
/joinsession 2
/joinsession 1
EOF

build/Client/client "4" << 'EOF'
/login 1 User4 127.0.0.1 5007
/joinsession 1
Hello team
EOF