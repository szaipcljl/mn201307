#!/bin/bash

mkdir ubuntu_docker
cd ubuntu_docker

docker pull ubuntu:latest
docker run -t -i ubuntu /bin/bash

# confirmed that ubuntu-desktop is not installed in the Docker official ubuntu
# image
dpkg -l ubuntu-desktop
# dpkg-query: no packages found matching ubuntu-desktop
