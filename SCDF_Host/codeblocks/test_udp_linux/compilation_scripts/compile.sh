rm -fr ./obj
rm -fr ./exe
mkdir ./obj
mkdir ./exe

g++ -c ../ip/IpEndpointName.cpp -Wall -Wextra -O3 -o ./obj/IpEndpointName.o
g++ -c ../ip/posix/NetworkingUtils.cpp -Wall -Wextra -O3 -I.. -o ./obj/NetworkingUtils.o
g++ -c ../ip/posix/UdpSocket.cpp -Wall -Wextra -O3 -I.. -o ./obj/UdpSocket.o
g++ -c ../osc/OscOutboundPacketStream.cpp -Wall -Wextra -O3 -o ./obj/OscOutboundPacketStream.o
g++ -c ../osc/OscPrintReceivedElements.cpp -Wall -Wextra -O3 -o ./obj/OscPrintReceivedElements.o
g++ -c ../osc/OscReceivedElements.cpp -Wall -Wextra -O3 -o ./obj/OscReceivedElements.o
g++ -c ../osc/OscTypes.cpp -Wall -Wextra -O3 -o ./obj/OscTypes.o
gcc -c ../udp_receiver.c -Wall -Wextra -O3 -o ./obj/udp_receiver.o
g++ ../main.cpp -Wall -Wextra -O3 -o ./exe/receiver_test ./obj/IpEndpointName.o ./obj/NetworkingUtils.o ./obj/UdpSocket.o ./obj/OscOutboundPacketStream.o ./obj/OscPrintReceivedElements.o ./obj/OscReceivedElements.o ./obj/OscTypes.o ./obj/udp_receiver.o