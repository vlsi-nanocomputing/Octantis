# The Octantis Project
### An High-Level Explorer for Logic-in-Memory architectures. :mag:

License
-----------------------------------------------------------------------------------------------------
© 2023 Andrea Marchesin, Alessio Naclerio (andrea.marchesin@polito.it, alessio.naclerio@polito.it) for Politecnico di Torino.

***

Instructions for the compilation of the project - Warning: It may require many Gb of free memory!
-----------------------------------------------------------------------------------------------------
 
1. Create a "build" folder, outside the Git repository
2. Enter the newly created folder and execute the following commands:
    ```
    cmake -G 'Unix Makefiles' ../The-Octantis-project/llvm -DLLVM_USE_LINKER=gold -DCMAKE_BUILD_TYPE=Release
    make
    ```

*NOTES: The serial compilation is extremely slow, so the following code is suggested:
    `make -j #Number of available CPUs#`

-----------------------------------------------------------------------------------------------------

After the building of the project, all the passes developed can be tested. More details will be released
in the future version of the project.
