# [SCALE](http://www.github.com/danpage/scale): software-oriented material

<!--- -------------------------------------------------------------------- --->

## Concept

Although compelling for a variety of reasons, leaning about
[side-channel](http://en.wikipedia.org/wiki/Side-channel_attack)
(and fault) attack techniques using real targets (in real scenarios) *can*
present a range of challenges.  For example, doing so may

- require equipment (e.g., an oscilloscope) which is hard to scale beyond
  a small number of users (e.g., one researcher vs. a cohort of students),
- require special-case (e.g., a specific micro-controller) equipment that,
  independently from the attack, presents a steep(er) learning curve,
- demand configuration (e.g., an isolated network) that is awkward to work
  with (e.g., due to space or requirements for privileged access),
- add complicating factors (e.g., low signal-to-noise ratio) that present
  too high a challenge at an introductory level

and thus, in combination, detract from a focus on the fundamentals.  As a 
result, and in *certain* cases, use of simulated targets can represent an 
attractive alternative.  Although more contrived, they provide a simpler, 
controlled environment in which to explore a particular technique; having 
done so, one can *then* confidently apply any lessons learned to the real 
targets.

This repo. captures a suite of 
software-based,
[CTF](http://en.wikipedia.org/wiki/Capture_the_flag#Computer_security)-like
challenges along these lines;
It has a strict remit, namely educational use (e.g., as a lab. exercise 
or assignment), which to some extent determines goals, design decisions, 
and, ultimately, the content.
Each challenge fits into a fairly general model, in the sense it relates 
to a scenario depicted as follows:

```
      indirect input
     ----------------------+
                           |
                           v
        direct input  +---------+
     ---------------->|         |---+
E                     |    D  +-+   | computation
     <----------------|       |k|<--+
        direct output +-------+-+
                           |
                           |
     <---------------------+
      indirect output
```

The basic idea is that

- the attack target `D` houses some (potentially security-critical) data
  `k` (e.g., some key material),

- an adversary `E` interacts with `D` by providing

  -   direct input  (e.g., some plaintext),
    and
  - indirect input  (e.g., a glitched voltage or clock signal),

  which `D` uses via some computation to produce

  -   direct output (e.g., a ciphertext),
    and
  - indirect output (e.g., side-channel leakage such as execution time),

  such that

- by adaptively selecting inputs and using the resulting outputs, `E` is
  challenged to mount an attack (e.g., to recover `k`) on the underlying
  cryptography and/or how `D` implements it.

<!--- -------------------------------------------------------------------- --->

## Quickstart

- Install any pre-requisites, e.g., support for
  [Git Large File Storage (LFS)](http://git-lfs.github.com/)
  (without this, some content will appear as a set of pointers to data vs. the data itself).

- Clone the repo.

  ```sh
  git clone https://github.com/danpage/scale-sw.git ./scale-sw
  cd ./scale-sw
  git submodule update --init --recursive
  source ./bin/conf.sh
  ```

- Create and populate a suitable Python
  [virtual environment](https://docs.python.org/3/library/venv.html)
  based on 
  [`${REPO_HOME}/requirements.txt`](./requirements.txt) 
  by executing
   
  ```sh
  make venv
  ```
   
  then activate it by executing
   
  ```sh
  source ${REPO_HOME}/build/venv/bin/activate
  ```

- Build the material:

  - execute

    ```sh
    make --directory="${REPO_HOME}/src/scale" build-step1
    ```

  - execute

    ```sh
    make --directory="${REPO_HOME}/src/scale" build-step2
    ```

- Make use of the material in, 
  e.g., the archive

  ```sh
  ${REPO_HOME}/build/alice.tar.gz
  ```

<!--- -------------------------------------------------------------------- --->
