
<!-- README.md is generated from README.Rmd. Please edit that file -->
copertura
=========

[![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/github/njtierney/copertura?branch=master&svg=true)](https://ci.appveyor.com/project/njtierney/copertura)[![Travis-CI Build Status](https://travis-ci.org/njtierney/copertura.svg?branch=master)](https://travis-ci.org/njtierney/copertura)

The goal of copertura is to make it easy to solve the Maximal Location Coverage Problem. Currently it uses the `lp` solver from the `lpsolve` package.

Why copertura?
==============

It is named "copertura" as this means "coverage" in Italian, and the research problem that created the need for this package is in Ticino, the Italian speaking canton of Switzerland.

At this stage I'm strongly considering a renaming - perhaps to `macor` - **ma**ximum **co**verage in **r**.

How to Install
==============

``` r

# install.packages("devtools")
devtools::install_github("njtierney/copertura")
```

Example Usage
-------------

``` r

library(copertura)

# generate the A matrix with a function

# courtesy of http://www.quantumforest.com/2012/08/m-x-n-matrix-with-randomly-assigned-01/
rb_mat <- function(r,c,prob = 0.3) matrix(rbinom(r*c,1,prob),r,c)

my_A <- rb_mat(r = 20, # 20 cases
               c = 100, # 100 AED locations
               prob = 0.3) # 30% probability of success

# top corner
my_A[1:5,1:5]
#>      [,1] [,2] [,3] [,4] [,5]
#> [1,]    0    1    0    0    1
#> [2,]    0    0    1    0    0
#> [3,]    0    0    0    0    0
#> [4,]    1    0    0    1    0
#> [5,]    0    0    1    0    0

# bottom corner
my_A[16:20,96:100]
#>      [,1] [,2] [,3] [,4] [,5]
#> [1,]    0    1    1    1    0
#> [2,]    0    0    0    1    1
#> [3,]    1    0    1    0    0
#> [4,]    0    0    1    0    1
#> [5,]    0    0    1    0    1

my_soln <- max_coverage(A = my_A,
                        num_aed = 5) # five AEDs
# print the solution.
my_soln$solution
#>   [1] 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
#>  [36] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
#>  [71] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1
#> [106] 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
```

interpretation
==============

The first I (number of columns of A) entries of the solution tell you which AEDs are used

The next J (number of rows of A) entries of the solution tell you which OHCA are covered.

``` r

# which AEDs are to be used
my_soln$solution[1:ncol(my_A)]
#>   [1] 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
#>  [36] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
#>  [71] 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0

# which OHCA are covered
my_soln$solution[c(ncol(my_A)+1):c(ncol(my_A) + nrow(my_A))]
#>  [1] 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
```

I will be adding functions soon which help you match up the AEDs and the OHCAs from the analysis.

Speed
=====

At the moment it doesn't seem like `max_coverage` is that fast, but I'm hoping to provide more scalable methods by formulating the model using the fantastic [`ompr`](https://github.com/dirkschumacher/ompr), which will give users the capability to select the solver they want to use. I feel that this solution is best because it means that there is still an open source solver, but people can also use something proprietary like "gurobi" or "CPLEX".

Known Issues
============

There may also be identified bugs, please keep this in mind!

Future Work
===========

In the future we will include a set of functions to allow the user to keep their work within a dataframe and specify the potential locations and the cases that require coverage.