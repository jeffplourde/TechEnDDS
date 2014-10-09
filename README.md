DDS Example for TechEn
==============

Join the RTI Infrastructure Community ICE_IC01
------------------
[http://www.rti.com/downloads/rti-dds.html](http://www.rti.com/downloads/rti-dds.html)


Ensure that NDDSHOME refers to the location of the RTI DDS target package
-------------
```
  export NDDSHOME=/path/to/ndds
```

To build the example
------------------
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

Ensure you run from the root of the project containing USER_QOS_PROFILES.xml.  RTI DDS will automatically use these default profiles found in the working directory.
