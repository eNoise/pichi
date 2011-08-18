Pichi XMPP Bot
==============

Pichi is a jabber(xmpp) bot written in C++. It is based on the sulci abilities.

Build requirements
------------------

* cmake (http://www.cmake.org/)
* gcc-c++ >= 4.6.0 / gcc-g++ >= 4.6.0 / mingw-gcc-g++ >= 4.6.0
* boost >= 1.41.0, boost-regex >= 1.41.0, boost-program_options  >= 1.41.0
* libsqlite3 (http://www.sqlite.org/)
* pthread
* tinyxml (http://www.grinninglizard.com/tinyxml/) (optional)
* gloox (http://camaya.net/gloox/) (optional)
* libcurl (http://curl.haxx.se/)

Building (Linux)
----------------

    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release -DWITH_LUA=ON -DLOCAL_GLOOX=ON -DLOCAL_TINYXML=ON -DCMAKE_INSTALL_PREFIX='Directory to install' ../
    make
    make install

* -DLOCAL_GLOOX=ON - Enables built-in gloox library.
* -DLOCAL_TINYXML=ON - Enables built-in tinyxml library.
* -DLOCAL_LUA=ON - Enables built-in lia library.

Binaries (Windows) and Linux Packages
----------------
http://code.google.com/p/pichi/downloads/list - Here
