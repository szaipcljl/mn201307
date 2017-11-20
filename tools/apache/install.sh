#!/bin/bash

##########################
# Step 1: Install Apache #
##########################
sudo apt-get update
sudo apt-get install apache2

###############################
# Step 2: Adjust the Firewall #
###############################
# enable UFW
sudo ufw enable

# list the ufw application profiles
sudo ufw app list

# we will allow incoming traffic for the Apache Full profile by typing:
sudo ufw allow 'Apache Full'

# You can verify the change by typing:
sudo ufw status


#################################
# Step 3: Check your Web Server #
#################################

# We can check with the systemd init system to make sure the service is running by
# typing:
sudo systemctl status apache2

# Try typing this at your server's command prompt:
hostname -I
# You will get back a few addresses separated by spaces. You can try each in your
# web browser to see if they work.


# An alternative is typing this, which should give you your public IP address as
# seen from another location on the internet:
sudo apt-get install curl
curl -4 icanhazip.com
# When you have your server's IP address or domain, enter it into your browser's
# address bar:

# http://server_domain_or_IP


#####################################
# Step 4: Manage the Apache Process #
#####################################
# Now that you have your web server up and running, we can go over some basic
# management commandsNow that you have your web server up and running, we can go
# over some basic management commands.

# To stop your web server, you can type:

sudo systemctl stop apache2
# To start the web server when it is stopped, type:

# sudo systemctl start apache2
# To stop and then start the service again, type:
sudo systemctl restart apache2

# If you are simply making configuration changes, Apache can often reload without
# dropping connections. To do this, you can use this command:
sudo systemctl reload apache2

# By default, Apache is configured to start automatically when the server
# boots. If this is not what you want, you can disable this behavior by
# typing:
sudo systemctl disable apache2

# To re-enable the service to start up at boot, you can type:
sudo systemctl enable apache2

# Apache should now start automatically when the server boots again..

# To stop your web server, you can type:
sudo systemctl stop apache2

# To start the web server when it is stopped, type:
sudo systemctl start apache2

# To stop and then start the service again, type:
sudo systemctl restart apache2

# If you are simply making configuration changes, Apache can often reload without
# dropping connections. To do this, you can use this command:
sudo systemctl reload apache2

# By default, Apache is configured to start automatically when the server
# boots. If this is not what you want, you can disable this behavior by
# typing:
sudo systemctl disable apache2

# To re-enable the service to start up at boot, you can type:
sudo systemctl enable apache2

# Apache should now start automatically when the server boots again.

####################################################################
# Step 5: Get Familiar with Important Apache Files and Directories #
####################################################################

# Now that you know how to manage the service itself, you should take a few
# minutes to familiarize yourself with a few important directories and files.

# Content

# /var/www/html: The actual web content, which by default only consists of the
# default Apache page you saw earlier, is served out of the /var/www/html
# directory. This can be changed by altering Apache configuration files.
# Server Configuration

# /etc/apache2: The Apache configuration directory. All of the Apache
# configuration files reside here.

# /etc/apache2/apache2.conf: The main Apache configuration file. This can be
# modified to make changes to the Apache global configuration. This file is
# responsible for loading many of the other files in the configuration directory.

# /etc/apache2/ports.conf: This file specifies the ports that Apache will listen
# on. By default, Apache listens on port 80 and additionally listens on port 443
# when a module providing SSL capabilities is enabled.

# /etc/apache2/sites-available/: The directory where per-site "Virtual Hosts" can
# be stored. Apache will not use the configuration files found in this directory
# unless they are linked to the sites-enabled directory (see below). Typically,
# all server block configuration is done in this directory, and then enabled by
# linking to the other directory with the a2ensite command.

# /etc/apache2/sites-enabled/: The directory where enabled per-site "Virtual
# Hosts" are stored. Typically, these are created by linking to configuration
# files found in the sites-available directory with the a2ensite. Apache reads the
# configuration files and links found in this directory when it starts or reloads
# to compile a complete configuration.

# /etc/apache2/conf-available/, /etc/apache2/conf-enabled/: These directories have
# the same relationship as the sites-available and sites-enabled directories, but
# are used to store configuration fragments that do not belong in a Virtual Host.
# Files in the conf-available directory can be enabled with the a2enconf command
# and disabled with the a2disconf command.

# /etc/apache2/mods-available/, /etc/apache2/mods-enabled/: These directories
# contain the available and enabled modules, respectively. Files in ending in
# .load contain fragments to load specific modules, while files ending in .conf
# contain the configuration for those modules. Modules can be enabled and disabled
# using the a2enmod and a2dismod command.
# Server Logs

# /var/log/apache2/access.log: By default, every request to your web server is
# recorded in this log file unless Apache is configured to do otherwise.
# /var/log/apache2/error.log: By default, all errors are recorded in this file.
# The LogLevel directive in the Apache configuration specifies how much detail the
# error logs will contain.

