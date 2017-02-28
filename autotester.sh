PORT=5007
FILENAME=Twice.jpg

# BUILD FILES
make

# CLOSE TESTING PROGRAMS
pkill server

echo "Server                 Client 1               Client 2                Client 3                Client 4"

# Starts the server
cd build/Server
./server $PORT &
cd ../..
sleep .5

# Starts the client
echo ""

build/Client/client "1" << 'EOF'
EOF

build/Client/client "2" << 'EOF'
EOF

build/Client/client "3" << 'EOF'
EOF

build/Client/client "4" << 'EOF'
EOF