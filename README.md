# install

```
git clone https://github.com/ligang1109/cppbox.git

cd cppbox
mkdir mybuild
cd mybuild

cmake ../ -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
            -DCMAKE_BUILD_TYPE=Debug(this opt is for test) \
            -DGTEST_DIR=(/usr/local/googletest) 
            
make
make install
```