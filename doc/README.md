# The Octantis Project
### An High-Level Explorer for Logic-in-Memory architectures.

Licence
-----------------------------------------------------------------------------------------------------
© Andrea Marchesin 2020 (andrea.marchesin@studenti.polito.it) for Politecnico di Torino

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
    `make -j #Nuber of available CPUs#`

-----------------------------------------------------------------------------------------------------

After the building of the project, all the passes developed can be tested. More detail will be released
in the future version of the project.

***

A brief introduction to the project
-----------------------------------------------------------------------------------------------------
Today, one of the problems the scientific community is called upon to tackle is the well-known *von Neumann bottleneck*. The latter involves two crucial elements in a digital electronic system, the CPU and the memory, which suffer from a limitation in bandwidth they exchange information with. The latency which characterizes the transmission of the data between these electronic devices predominantly depends on:
- The strict physical parameters which describe the behavior of the *interconnections* linking the memory and the processing unit.
- The diminished *performance of the memories *themselves compared to that of the processing unit.

Among the various solutions under study, in the recent years the VLSI Laboratory of Politecnico di Torino has proposed the concept of *Logic-In-Memory (LIM)*: a memory device which embeds simple computational elements between the different cells, overall arranging a distributed processing system. The key idea consists in reducing access to the memory for the CPU, implementing a precomputation of raw data within the same memory. The CPU is left only with the task of performing the more complex operations on that pre-processed information. Different architectures have been proposed during the years and since 2019 the research group decided to go beyond the realization of specific case of study. In this context, *DExIMA* was born as a “beta version” of a software tool able to characterize generic Logic-In-Memory architectures. The information which the program can provide refers to space occupation, maximum performance and static and dynamic power consumption. In order to do that, DExIMA relies on a detailed library of components which are constantly updated. Today attention is given on the definition of C-MOS components, but above C-MOS solutions are already starting to be taken into consideration, like *Nano Magnetic Logic* and *perpendicular Nano Magnetic Logic*. 

The thesis work has been inserted into this context with the introduction of *Octantis*, a software designed form scratch useful for the exploration of LIM architectures. By its nature it is a flexible solution to analyze an input algorithm described in standard C language and to identify which LIM architecture would implement it better. Only a subset of operations in the input C code is allowed and the algorithm so described must be accompanied by a configuration file, in which the LIM designer has to impose the constraints that Octantis considers to model the architecture in accordance with them. At the output of the program the DExIMA configuration files are provided, in order to enable the simulator engine to perform the related analysis. Octantis and DExIMA merge in a unique software of first approach in the design phase, which allows the designer to get an idea of what the benefits might be in the implementation of a specific algorithm through a Logic-In-Memory structure. The term of comparison is the execution of the same algorithm in a traditional microprocessor system.

The Octantis’ project considers the *LLVM compiler infrastructure* to realize the whole process of translation from C-code to DExIMA input files. The LLVM framework provides libraries useful to optimize and generate a code for a target architecture starting from an input source. It is distributed under an opensource license, which allows a free customization of each of its components. In the panorama of C compilers, the choice fell on LLVM, to the detriment of the tools provided by the competitor *GNU-GCC*, considering specially the flexibility of the former infrastructure, which can count on a more active community of developers and richer technical documentations.

![The ``binary system'': Octantis and DExIMA.](./LIM_Explorer.png)

The whole project has been designed so as to guarantee the *modularity* and the *maintenance* of the code. For what concerns the maintenance, a detailed documentation is provided together with Octantis in order to prepare the designer to work on it in a short time. The modularity is then guaranteed to allow the same developers to extend the capability of the tool during time.


At the state of art, many other features can be introduced. A future work goal could be the analysis of a generic C source code to define, directly when Octantis starts, which parts of the whole algorithm could benefit of a LIM implementation and which would not. In this way, only a portion of the source code would be mapped on a LIM architecture while the remaining part could be rearranged to be provided to a generic processor back-end. This new version of Octantis would be able to collect information on how a more complex system, composed by a LIM-Memory associated to a modern processor, would behave.
Moreover, considering the further developments on the DExIMA library, also new input operations could be mapped in a more efficient way. Therefore, it could be useful to extend the capabilities of Octantis itself.


These are few of the possible future works which could be realized around Octantis and, in general, the coupled *Octantis-DExIMA*.


In conclusion, the hope is to have realized an instrument capable of giving light to the many Logic-In-Memory researches that have been carried out so far by the researchers of the VLSI Laboratory. Octantis aims to be a *valid guide* during the exploratory phase of LIM architectures, a promising solution for how electronics could evolve in the coming years. 
***
