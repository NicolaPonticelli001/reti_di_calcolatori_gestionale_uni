cd sqlite-autoconf
./configure --prefix=/usr/local
make
sudo make install
sudo apt update
sudo apt install libsqlite3-dev
