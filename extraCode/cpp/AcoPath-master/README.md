# AcoPath: Ant Colony Optimization algorithm for the shortest path problem

```python
Copyright (C) 2014-2021 by Constantine Kyriakopoulos, zfox@users.sourceforge.net
Version: 0.9.1
License: GNU GPL Version 3
```


## About the project

The shortest path problem is solved by many methods including heuristics that offer lower complexity in expense of accuracy. There are many use-cases where the lower accuracy is acceptable in return of lower consumption of computing resources or the ability to adapt to a constantly changing operating environment.

The basic idea of the Ant System (AS) [1, 2] is that virtual ants are exploited for finding paths with a specific property, e.g., short distance between physical nodes, in the same way nature guides physical ants. A special chemical substance is being deposited upon their path which raises the probability for other ants to follow it during subsequent traversals. When this substance concentrates in high levels on a path, all subsequent ants have higher probability to follow it and increment it as well. Evaporation takes place on paths that are less traversed. Usually, the path with the highest pheromone concentration is the shortest path. The AS emulates this nature's behaviour with satisfying results solving computational problems. When the number of virtual ants and iterations are high enough, the right paths are usually found under polynomial complexity.

This is a heuristic method, i.e., optimal results are not always feasible. According to topology's resources like the node and edge numbers, the proper numbers of iterations and virtual ants must be used. Large numbers lead to paths with higher probability of being optimal but more computational resources are consumed.


## Prerequisites to build

There are only two requirements, i.e., the Boost Library and the availability of C++20 or C++17 standard. Boost is utilised for parsing the JSON representation of the topology. There is also the option to insert edges using the <em>AdaptiveSystem</em> interface, so in this case the JSON dependence can be commented out. Tested with Clang 12 and libc++ from the LLVM project. Build with 'mkdir build && cd build; cmake -DCMAKE_BUILD_TYPE=Release ../ && make' from the main source directory.



## Usage

Create a new instance of <em>AntSystem</em> in your code passing as arguments the JSON topology file and the numbers of iterations and ants (default values are also provided but shortest paths aren't returned under all topology sizes). Next, execute the method <em>path(src, dest)</em> where <em>src</em> is the source node and <em>dest</em> the destination to reach. This returns the valid path which ants converge to.


## Related work

```python
[1] Dorigo, M., Birattari, M. and Stutzle, T., 2006. Ant colony optimization. IEEE computational intelligence magazine, 1(4), pp. 28-39.
[2] Dorigo, M. and Stützle, T., 2019. Ant colony optimization: overview and recent advances. In Handbook of metaheuristics, pp. 311-351. Springer, Cham.
```


## Changelog

<pre>
0.9.X    2020-XX-XX    XX
0.9.1    2020-11-21    Extended the interface for inserting single edges
0.9      2020-11-12    Initial public release
</pre>
