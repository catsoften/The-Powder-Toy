# Circuit Solving Method

## 0. Identify circuits

## 1. Identify circuit nodes

First we take the circuit we identified and apply the Zhang-Suen algorithm for image thinning to 
create a skeleton of the circuit. Next, we identify nodes by the following criteria, which is documented in a comment in simulation/circuits/circuits.cpp:

```
Count surrounding disjoint connections, if > 2 then it must be a junction, ie:
 #YY
   X##
   #
The pixel marked x has 3 connections going into it, even though it has 4 surrounding pixels, as the surrounding pixels marked with Y are touching, so are only counted once.
By convention, the code below ignores directly adjacent pixels (up, down, left, right) if a diagonal
touching that pixel is already filled; this allows for more diagonal connections to be considered.
 
Nodes cannot be adjacent, so if another node is found, then terminate
 ```

![Skeleton of circuit](https://i.imgur.com/a4JVGq4.png)

At the end of this step, we have something similar to the above diagram (skeleton in red, nodes circled in white).

## 2. Solve circuit

## 3. Translate solution to Simulation

## 4. 