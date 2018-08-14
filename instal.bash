#!/bin/bash
apt-cache search libsdl2
sudo apt-get install libsdl2-dev

apt-cache search libsdl2-image
sudo apt-get install libsdl2-image-dev

make
