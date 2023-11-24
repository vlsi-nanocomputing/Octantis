[![DOI](https://zenodo.org/badge/700281333.svg)](https://zenodo.org/doi/10.5281/zenodo.10017505)

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

After the building of the project, all the passes developed can be tested. 

Instructions for the usage of the project
-----------------------------------------------------------------------------------------------------

1. In order to provide an input C code to Octantis, execute the following commands:

    ```
    TESTNAME="name_of_c_code_file"
    LLVMDIR="path/to/build/bin"
    LIBDIR="path/to/build/lib"
    TESTDIR="path/to/c_code"
    
    clear
    clang -S -emit-llvm $TESTDIR/$TEST_NAME.c -o $TESTDIR/$TEST_NAME.ll
    $LLVMDIR/opt -load $LIBDIR/LLVMOctantis.so -octantisPass -debugMode=0 $TESTDIR/$TEST_NAME.ll
    ```

2. The debugMode parameter can be set to '1' to enable printing useful messages during the synthesis process.
3. The "Octantis.cfg" file must be put in the build directory.
4. After the synthesis process is finished, results regarding the final architecture and the related timing information is stored in two files named respectively "LiMArray.txt" and "FSM.txt".
5. Sample tests on Image Processing algorithms and associated results can be found in the folder "/Tests".
