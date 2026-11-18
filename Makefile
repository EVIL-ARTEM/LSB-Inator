CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

LSB-inator: fileio.h fileio.cpp image.h image.cpp lodepng.h lodepng.cpp auth.h auth.cpp rc4.h rc4.cpp lsb.h lsb.cpp menu.h menu.cpp Main_inator.cpp
	$(CXX) $(CXXFLAGS) fileio.cpp image.cpp lodepng.cpp auth.cpp rc4.cpp lsb.cpp menu.cpp Main_inator.cpp -o LSB-inator

test_fileio: test_fileio.cpp fileio.cpp fileio.h
	$(CXX) $(CXXFLAGS) fileio.cpp test_fileio.cpp -o test_fileio

test_rc4: test_rc4.cpp rc4.cpp rc4.h fileio.cpp fileio.h
	$(CXX) $(CXXFLAGS) test_rc4.cpp rc4.cpp fileio.cpp -o test_rc4

test_image: test_image.cpp image.cpp image.h fileio.cpp fileio.h lodepng.cpp lodepng.h
	$(CXX) $(CXXFLAGS) test_image.cpp image.cpp fileio.cpp lodepng.cpp -o test_image

test_lsb: test_lsb.cpp lsb.cpp lsb.h image.cpp image.h fileio.cpp fileio.h lodepng.cpp lodepng.h
	$(CXX) $(CXXFLAGS) test_lsb.cpp lsb.cpp image.cpp fileio.cpp lodepng.cpp -o test_lsb

test_auth: test_auth.cpp auth.cpp auth.h fileio.cpp fileio.h
	$(CXX) $(CXXFLAGS) test_auth.cpp auth.cpp fileio.cpp -o test_auth

test_menu: test_menu.cpp menu.h menu.cpp fileio.h fileio.cpp rc4.h rc4.cpp lsb.h lsb.cpp image.h image.cpp lodepng.cpp lodepng.h
	$(CXX) $(CXXFLAGS) test_menu.cpp menu.cpp fileio.cpp rc4.cpp lsb.cpp image.cpp lodepng.cpp -o test_menu

clean:
	rm -f test_fileio test_rc4 test_image test_lsb test_auth test_menu LSB-inator *.o users.dat