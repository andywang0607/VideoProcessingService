#env
PROJECTPATH=/home/ubuntu/workspace/github/VideoProcessingService
PROJECTNAME=VideoProcessingService
VCPKG_ROOT=/home/ubuntu/ThirdParty/vcpkg
# build this project
cd $PROJECTPATH
mkdir build
cd $PROJECTPATH/build
cmake ..
make -j8

# Copy required library to build folder
mkdir -p $PROJECTPATH/build/lib
cp $VCPKG_ROOT/installed/x64-linux/lib/libpistache.so.0 $PROJECTPATH/build/lib

# Copy Dockerfile
cp $PROJECTPATH/script/Dockerfile $PROJECTPATH/build
sudo docker build -t video-processing-service . --no-cache