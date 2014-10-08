DDS Example for TechEn
==============


To build the example
------------------
```
  make -f make/Makefile.ARCH
```

where ARCH is your local architecture (there are various makefiles)

To run the example run the clamp side of the communication
```
    objs/ARCH/main clamp 
```

And to run the controller side of the communication
```
    objs/ARCH/main controller
```

Ensure you run from the root of the project containing USER_QOS_PROFILES.xml.  RTI DDS will automatically use these default profiles found in the working directory.
