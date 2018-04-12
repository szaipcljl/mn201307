 Command line instructions
 Git global setup

 git config --global user.name "Ning.Ma"
 git config --global user.email "ning.ma@roadefend.com"

 Create a new repository

 git clone git@121.196.213.107:ning.ma/my_prj_test.git
 cd my_prj_test
 touch README.md
 git add README.md
 git commit -m "add README"
 git push -u origin master

 Existing folder

 cd existing_folder
 git init
 git remote add origin git@121.196.213.107:ning.ma/my_prj_test.git
 git add .
 git commit -m "Initial commit"
 git push -u origin master

 Existing Git repository

 cd existing_repo
 git remote add origin git@121.196.213.107:ning.ma/my_prj_test.git
 git push -u origin --all
 git push -u origin --tags

