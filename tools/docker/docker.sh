#!/bin/bash
# https://www.digitalocean.com/community/tutorials/how-to-install-and-use-docker-on-ubuntu-16-04
# https://docs.docker.com/engine/admin/systemd/

#
# Step 1 — Installing Docker
#

# First, add the GPG key for the official Docker repository to the system:
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -

# Add the Docker repository to APT sources:
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"


# Next, update the package database with the Docker packages from the newly added repo:
sudo apt-get update

# Make sure you are about to install from the Docker repo instead of the default
# Ubuntu 16.04 repo:
apt-cache policy docker-ce

# Finally, install Docker:
sudo apt-get install -y docker-ce

# Docker should now be installed, the daemon started, and the process enabled to
# start on boot. Check that it's running:
#sudo systemctl status docker


#
# Step 2 — Executing the Docker Command Without Sudo (Optional)
#

# If you want to avoid typing sudo whenever you run the docker command, add your
# username to the docker group:
sudo usermod -aG docker ${USER}

#To apply the new group membership, you can log out of the server and back in, or
#you can type the following:
su - ${USER}

# You will be prompted to enter your user's password to continue. Afterwards, you
# can confirm that your user is now added to the docker group by typing:
id -nG

# If you need to add a user to the docker group that you're not logged in as,
# declare that username explicitly using:
#sudo usermod -aG docker username


# To check whether you can access and download images from Docker Hub, type:
#docker run hello-world

# You can search for images available on Docker Hub by using the docker command
# with the search subcommand. For example, to search for the Ubuntu image, type:
#docker search ubuntu



#
# 3.HTTP/HTTPS proxy
#

# 1.Create a systemd drop-in directory for the docker service:
sudo mkdir -p /etc/systemd/system/docker.service.d


# 2.Create a file called /etc/systemd/system/docker.service.d/http-proxy.conf that
#adds the HTTP_PROXY environment variable:
sudo echo "
[Service]
Environment=\"HTTP_PROXY=http://child-prc.intel.com:913/\"
Environment=\"HTTPS_PROXY=https://child-prc.intel.com:913/\"" \
	> /etc/systemd/system/docker.service.d/http-proxy.conf

# Flush changes:
sudo systemctl daemon-reload

# Restart Docker:
sudo systemctl restart docker

#
# Dockerfile proxy
#
#ENV HTTPS_PROXY=http://10.239.4.160:913/


#
# copy files from host to docker
#
docker run -v ~/local_dir:/home/angr/docker_dir -it bootstomp
