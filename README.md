DDS Example for TechEn
==============

ICE_IC01
------------------
Join the RTI Infrastructure Community ICE_IC01 to download RTI libraries for DDS.

[http://www.rti.com/downloads/rti-dds.html](http://www.rti.com/downloads/rti-dds.html)


NDDSHOME
-------------
Ensure that the NDDSHOME environment variable refers to the location of the RTI DDS target package
```
  export NDDSHOME=/path/to/ndds
```

Build
------------------
To build the example
```
  make -f make/Makefile.ARCH
```

where ARCH is your local architecture (there are various makefiles in make/)

To run the example run the clamp side of the communication
```
    objs/ARCH/main clamp 
```

And to run the controller side of the communication
```
    objs/ARCH/main controller
```

Make sure you run from the root of the project containing USER_QOS_PROFILES.xml.  RTI DDS will automatically use these default profiles found in the working directory.
