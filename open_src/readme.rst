
-------------------------------------
kcov: code coverage for fuzzing
kernel:
	Documentation/dev-tools/kcov.rst

-------------------------------------
The Kernel Address Sanitizer (KASAN) 
kernel:
	Documentation/dev-tools/kasan.rst

-------------------------------------
========
# myrepo
## how to install myrepo
REPO_DIR=${HOME}/bin  
mkdir $REPO_DIR  

curl https://raw.githubusercontent.com/sky8336/git-repo/master/repo > $REPO_DIR/myrepo  
chmod a+x $REPO_DIR/myrepo  

echo "PATH=$PATH:$REPO_DIR" >> $HOME/.bashrc  
echo "PATH=$PATH:$REPO_DIR" >> $HOME/.zshrc  

using myrepo to avoid overwriting your repo in $REPO_DIR  

NOTE: mostly, you just need do it once.  

##### get repo from gitlab with your private token
//curl 121.196.xxx.xxx:8080/Boardx/git-repo/raw/master/repo?private_token=G5NUJ9Hxxxxxxxxxxxxx > myrepo  

-------------------
## repo init & sync
mkdir your-project-root-dir  
cd your-project-root-dir  

myrepo init --no-repo-verify -u http://github.com/sky8336/manifest  
myrepo sync  

---------------------
### vimcfg_bundle.xml
myrepo init --no-repo-verify -u http://github.com/sky8336/manifest --manifest-name=vimcfg_bundle.xml  
myrepo sync  

---------------------
### wowotechX.xml
myrepo init --no-repo-verify -u http://github.com/sky8336/manifest -m wowotechX.xml  
myrepo sync  
