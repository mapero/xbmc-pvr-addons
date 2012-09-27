# DVBLink XBMC ADDON

WARNING: this is work in progress. Don't try to use this unless you know what you're doing.

## 3rd party dependencies
[libdvblinkremote] (https://github.com/zeroniak/dvblinkremote) - The link directs to a costumized version of libdvblinkremote that has partially been updated to DVBLink API v. 0.2. 
libdvblinkremote requires : [libcurl](http://curl.haxx.se/libcurl/) and [TinyXML-2](https://github.com/leethomason/tinyxml2). 

## How to compile & install

### Windows (Currently only option)

Requires Visual Studio 2010 (VS2010). The following guide has not been tested with VS2012!

#### Compiling 3rd party dependencies
This part guide is for building the dependencies in release mode.
If you want to develop / change anything in these please adjust the guide accordingly to get debug versions build.

libcurl :

 1. Download [libCurl v. 7.27.0](http://curl.haxx.se/download/curl-7.27.0.zip) and extract the file 
 2. Open file : curl-7.27.0/vc6curl.dsw in VS2010. VS2010 will convert it to a VS2010 Solution file.
 3. Change Build Configuration to "LIB Release"
 4. Open Properties for the libcurl project (Right click on project and select Properties)
 5. Change Configuration Properties->C/C++->Code Generation ->Runtime Library to "Multi-threaded (/MT)" and close Properties.
 6. Build libcurl project (Right click on project and click Build) - and it should say "Build succeded" in the bottom status bar of VS2010
 7. Copy the file "curl-7.27.0/lib/LIB-Release/libcurl.lib" to VS2010 std. lib folder (Usually C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib)
 8. Copy all files and folders in "curl-7.27.0/include" to VS2010 std. include folder (Usually C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\include)

TinyXML-2 :

 1. Download TinyXML-2 through github.
 2. Open file : tinyxml2/tinyxml2.sln in VS2010. 
 3. Change Build Configuration to "Release"
 4. Open Properties for the tinyxml2 project (Right click on project and select Properties)
 5. Change Configuration Properties->Configuration Type to "Static Lib (.lib)"
 6. Change Configuration Properties->C/C++->Code Generation->Runtime Library to "Multi-threaded (/MT)" and close Properties.
 7. Build tinyxml2 project (Right click on project and click Build) - and it should say "Build succeded" in the bottom status bar of VS2010
 8. Copy the file "tinyxml2/Win32/Release/tinyxml2.lib" to VS2010 std. lib folder (Usually C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib)
 9. Create a "tinyxml2" folder in VS2010 std. include folder (Usually C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\include)
 10. Copy the file "tinyxml2/tinyxml2.h" to the above folder (Fx. C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\include\tinyxml2)

libdvblinkremote :

 1. Download libdvblink through github. Use this git repo : https://github.com/zeroniak/dvblinkremote
 2. Open file : dvblinkremote/libdvblinkremote/project/VS2010Express/libdvblinkremote.sln in VS2010. 
 3. Change Build Configuration to "Release"
 4. Build libdvblinkremote project (Right click on project and click Build) - and it should say "Build succeded" in the bottom status bar of VS2010
 5. Copy the file "dvblinkremote/dvblinkremote/lib/libdvblinkremote.lib" to VS2010 std. lib folder (Usually C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\lib)
 6. Copy all files and folders in "dvblinkremote/dvblinkremote/include" to VS2010 std. include folder (Usually C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\include)

#### Compiling the addon

This part is missing :)

#### Install
The buildsystem does not yet generate .zip archives
Workaround: copy the dvblink addon to your XBMC installation by hand.
Example: addons\pvr.dvblink\addon\*.* => YOUR_XBMC_DIR\addons\pvr.dvblink\*.*

## More Information
For more information, please see [DVBLogic official forum](http://www.dvblogic.com/phpBB3/viewtopic.php?f=61&t=19703).

## License (MIT)

Copyright (c) 2012 Palle Ehmsen, [Barcode Madness] (http://www.barcodemadness.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.