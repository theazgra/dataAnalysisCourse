@echo off
echo Pulling repository
git pull

cd AzgraCppLibrary
echo Pulling AzgraCppLibrary submodule
git pull
cd ../
