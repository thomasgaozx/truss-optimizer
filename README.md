# Truss Optimizer

- [Truss Optimizer](#truss-optimizer)
  - [Problem Definition](#problem-definition)
  - [Cost Model](#cost-model)
    - [Simple Cost Function](#simple-cost-function)
  - [Constraint Model](#constraint-model)
    - [Failure Modes (TODO)](#failure-modes-todo)
      - [Euler Buckling Load Estimation](#euler-buckling-load-estimation)
  - [Computing Problem](#computing-problem)
    - [Input Specification](#input-specification)
    - [Output Specification](#output-specification)
  - [Appendix: Material Properties](#appendix-material-properties)
    - [Balsa wood Properties](#balsa-wood-properties)
    - [Dowel Pin Properties](#dowel-pin-properties)

## Problem Definition

$PV$ is the largest load to weight ratio. The main objective is to maximize $PV$. Since there are way too many factors to incorporate to simulate an actual load-to-weight ratio, a simplified truss and cost/weight model is required. The goal of this report is to find well-defined **cost function** (i.e. weight function) for any truss, and use the existing optimization algorithm to maximize the truss' **load to cost ratio**.

> **Cost Function** is now a synonym for **Weight Function**.

## Cost Model

Consider a truss, $T=(\mathcal J, \mathcal M)$ where $\mathcal J$ and $\mathcal M$ are the set of joints and members. Let $\mathcal J = \{j_1, j_2,\dots,j_r\}$, $\mathcal M=\{m_1, m_2,\dots,m_k\}$ where $r,k\in\mathbb Z^+$. Let $L(m_1)$ denotes the length of member $m_1$, and let $F(m_1)$ denotes the internal force of $m_1$.

### Simple Cost Function

Since a member can have variable thickness, i.e. more internal force in a member means thicker the member should be. I therefore assume that $F(m_i) \propto$ Volume/Weight of $m_i$.

Define function $C(T)$ as the cost function of the truss:

$$
C(T) = \sum_{i=1}^r F(M_i)\times L(M_i)
$$

This cost function is easy to implement, with the downside being it completely ignores the extra weight iccured by $\mathcal J$.

## Constraint Model

- Truss should be $40\pm 1$ cm long and $10\pm 0.5$ cm tall truss that can handle **the largest load-to-weight ratio**.
- The truss should be loaded at the middle of the bridge.
- A **minimum load-to-mass ratio of 100** is required, this is the lower threshold requirement.

### Failure Modes (TODO)

- Member in _tension_: **Rupture** and **Member Tearing**
- Member in _compression_: **Buckling** and **Bearing Stress**
- **Pin Shear** (bending, don't worry about it here)

#### Euler Buckling Load Estimation

> This is an over estimation

$$
P_{cr} = \frac{\pi^2EI}{(KL)^2}
$$

Where $P_{cr}$ is Euler's critical load, $E$ is modulus of elasticity of column material, $I$ is _minimum_ area moment of inertia of the cross section of the column, $L$ is unsupported length of column, $K$ is column effective length factor.

![column effective length factor](https://upload.wikimedia.org/wikipedia/commons/a/a2/ColumnEffectiveLength.png)

## Computing Problem

### Input Specification

Truss configuration should be entered as a textfile. To run the application, enter `truss-optimizer some_truss.txt` in the command prompt.

The first line consists of 3 inputs $P$, $Q$, and $R$. $P$ is the number of joints, $Q$ is the number of members, and $R$ is the number of loaded members.

The next $P$ lines are the $x, y$ coordinate of joints, each line contains 2 numbers and 2 characters `'T'`/`'F'` (separated by space). If the first character is `T`, it means its a free joint. if the second character is `T`, it means it's the highest point that should be used to maintain the dimension of the truss bridge.
Note that each joints entered are indexed from $0$ to $(P-1)$.

The next $Q$ lines are the member connections of joints, each line contains 2 numbers, and each number represents the index of a joint entered above.

The next $R$ lines consists of 2 numbers each, the joint number and the vertical load. Note that the vertical load in the up direction is positive and int he down direction is negative.

Example:

```txt
3 3 3
0 0 F F
20 10 T T
40 0 F F
0 1
1 2
0 2
0 1
1 -2
2 1
```

### Output Specification

Output the most optimized truss configuration to `optimized.txt` in the following format:

- First Line: `Score: $SCORE` where `$SCORE` is Cost divided by the Force applied.
- Next $P$ lines: `($X, $Y)` representing the coordinate of each joint.

On the console, print whatever.

## Appendix: Material Properties

### Balsa wood Properties

$$\rho = 123\pm 23\ kg/m^3$$
$$E=3.51\pm 1.05\ GPa$$
$$\sigma_{ut} =7.7 \pm 2.5\ MPa$$
$$\tau_{ut}=1.6\ MPa$$

### Dowel Pin Properties

$$\rho = 650\ kg/m^3$$
$$E=17\ GPa$$
$$\sigma_{ut} =171\ MPa$$
$$M_{max} = 368\ N.mm$$
$$\tau_{ut} = 23\ MPa$$
