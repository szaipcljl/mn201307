#!/bin/bash
#clear project
echo "Do you want to build a user version?(Y/N, default is Yes)"
  read userversion
  echo $userversion
echo "Do you want to build a version with ota?(Y/N, default is N)"
  read haveota
  echo $haveota
echo "git clean -df"
git clean -df
echo "git checkout -f"
git checkout -f
echo "git pull"
git pull

#copy source [modify]
echo "cp -rvf ../private/c901/b9a/* ."
cp -rvf ../private/c901/b9a/* .

#build cmd
echo ". /build/envsetup.sh"
source build/envsetup.sh
#lunch project [modify]
echo "lunch sp9832iea_4m_volte_dt-userdebug"
lunch sp9832iea_4m_volte_dt-userdebug
#1 user; 2 userdebug [modify]
echo "choosevariant userdebug"
if [ "$userversion" == "n" ] || [ "$userversion" == "N" ]; then
 choosevariant userdebug
else
 choosevariant user
fi

#
echo "kheader"
kheader
echo "kdconfig"
kdconfig
echo "make -j24"
make -j24 | tee log.txt
if [ "$haveota" == "Y" ] || [ "$haveota" == "y" ]; then
 echo "make otapackage"
 make otapackage
fi
echo "make pac"
make pac



