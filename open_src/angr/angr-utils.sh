#!/bin/bash

mkdir $HOME/angr_dir
angr_dir=$HOME/angr_dir

cd $andr_dir

# angr-utils
#1. Environment setup
#(1)Install bingraphvis
git clone https://github.com/axt/bingraphvis
cd $angr_dir/bingraphvis
sudo python setup.py install

#(2) install graphviz
sudo apt-get install graphviz

#(3) install pydot
cd $angr_dir
git clone https://github.com/erocarrera/pydot
cd $angr_dir/pydot
sudo python setup.py install

#(4) install simuvex
cd $angr_dir
git clone https://github.com/angr/simuvex
cd $angr_dir/simuvex
sudo python setup.py install

#(5)install angr-utils
cd $angr_dir
git clone https://github.com/axt/angr-utils
cd $angr_dir/angr-utils
sudo python setup.py install


#(6)angr-utils/examples/plot_cfg_region[master]$ python plot_cfg_region_example.py 
#ImportError: cannot import name arm:
pip install -I --no-use-wheel capstone


#2. Test angr-utils
cd $angr_dir/angr-utils/examples/plot_func_graph
python plot_func_graph_example.py

#Cfg-explorer
#1.flask
cd $angr_dir
git clone https://github.com/pallets/flask
cd $angr_dir/flask
sudo python setup.py install

