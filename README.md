# [SCALE](http://www.github.com/danpage/scale): software-oriented offshoot

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

The SCALE software-oriented offshoot represents a suite of 
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

## Content

#### Prepare pre-requisites

Some content is stored using
[Git Large File Storage (LFS)](http://git-lfs.github.com/):
it makes sense to install this first, otherwise that content appears
as a set of pointers to data vs. the data itself.

#### Prepare     repository

This is actually a 
[submodule](http://www.git-scm.com/docs/git-submodule)
of the
[SCALE](http://www.github.com/danpage/scale)
repository: you can *either*

1. follow the instructions to prepare the SCALE repository, then
   use

   ```
   cd ${SCALE}/sw ; export SCALE_SW="${PWD}"
   ```

   to fix the working directory,
   *or*

2. treat it as a standalone repository,
   using

   ```
   git clone http://www.github.com/danpage/scale-sw.git
   ```
   
   to clone the repository, then
   
   ```
   cd scale-sw ; export SCALE_SW="${PWD}"
   ```

   to fix the working directory.

Note that, either way, the associated path is denoted by 
`${SCALE_SW}` 
from here on.  

#### Prepare pre-requisites

There are several classes of dependency, some instances of which only
relate to specific challenges (so can be ignored if those challenges 
will not be generated).  You can use

```
make check-deps
```

to (somewhat imperfectly) check the existence of all instances, which 
include

1. some      standard dependencies, e.g.,

   - [make](http://www.gnu.org/software/make)
   - [Doxygen](http://www.doxygen.org)
   - [GCC](http://www.gnu.org/software/gcc)
   - [M4](http://www.gnu.org/software/m4)
   - [GMP](http://www.gmplib.org)
   - [OpenSSL](http://www.openssl.org)
   - [LaTeX](http://www.latex-project.org)
     - [algorithm2e](http://www.ctan.org/pkg/algorithm2e)
     - [amslatex](http://www.ctan.org/pkg/amslatex)
     - [biblatex](http://www.ctan.org/pkg/biblatex)
     - [fancyhdr](http://www.ctan.org/pkg/fancyhdr)
     - [fontenc](http://www.ctan.org/pkg/fontenc)
     - [geometry](http://www.ctan.org/pkg/geometry)
     - [hyperref](http://www.ctan.org/pkg/hyperref)
     - [listings](http://www.ctan.org/pkg/listings)
     - [PGF](http://www.ctan.org/pkg/pgf)
     - [siunitx](http://www.ctan.org/pkg/siunitx)
     - [standalone](http://www.ctan.org/pkg/standalone)
     - [titlesec](http://www.ctan.org/pkg/titlesec)
     - [xargs](http://www.ctan.org/pkg/xargs)
     - [xstring](http://www.ctan.org/pkg/xstring)
   - [Python](http://www.python.org)
     - [PyCrypto](http://pypi.python.org/pypi/pycrypto)
     - [braceexpand](http://pypi.python.org/pypi/braceexpand)

   which are likely to be installed already, or can be installed easily
   using whatever package manager is appropriate; 
   under
   [Ubuntu](http://www.ubuntu.com)-based
   Linux, for example, 
   most of the above can be installed 
   by issuing the commands

   ```
   sudo apt-get install make doxygen gcc m4 libgmp-dev openssl libssl-dev texlive-full python python-dev python-pip
   sudo pip install pycrypto braceexpand
   ```

   although using `tlmgr` to selectively install the LaTeX packages may
   be preferable to installing *everything* via `texlive-full`,

2. some less standard dependencies, e.g.,
 
   - [AS31](http://www.pjrc.com/tech/8051)
 
   which are more likely to require manual compilation and installation
   (e.g., from source),

3. some  non-standard dependencies that can't be linked against as is, 
   e.g., because they must be patched or configured in a specific way:
   this can be managed as part of the build process, as outlined below.

   As an example, the `des_cache` challenge uses the cache simulator 
   [DineroIV](http://pages.cs.wisc.edu/~markhill/DineroIV),
   plus a 3rd-party 
   [DES](http://www.schneier.com/sccd/DES-OUTE.ZIP)
   implementation.  The former doesn't *really* support installation of 
   the library, so is a candidate for SCALE to manage; the latter needs
   to be patched (to invoke functions in the former) before use, so 
   again demands special treatment.

#### Configure content

Everything is configured centrally via 
`${SCALE_SW}/scale.conf`.
This file has some quite extensive documentation inline, so the syntax
and parameters aren't expanded on further here: most have sane default 
values if all you want to do is generate a set of challenges to use.

#### Build     content

Each challenge includes

- a clear, precise description of the scenario and goals,
- a simulated, executable attack target 
  parameterised on a per-user basis (e.g., with different key material),
  and
- a set of exam-style analysis questions for use as an accompaniment to
  any implementation effort,
  again selected on a per-user basis.
   
You can build this material as follows:
either

1. 1. - `make build-share`
         will build   the shared    material into `${SCALE_SW}/src/share/*/build`,
      - `make build-conf`
         will build   the configuration      into `${SCALE_SW}/build`,

      then

   2. - `make build-doc`
         will build   the documentation      into `${SCALE_SW}/build/doc`,
      - `make build-deps`
         will download, verify (against a pre-computed hash), then build
         each dependency,
      - `make build-challenge`
         will build   the challenge material into `${SCALE_SW}/src/challenge/*/build`.
      - `make build-dist`
         will collate the challenge material into `${SCALE_SW}/build`,

   *or*

2. `make build-step1`
   then, separately,
   `make build-step2`
   (i.e.. *not* an all-in-one `make build-step1 build-step2`),
   will perform *all* of the above automatically, step-by-step and in 
   the correct order: this is the recommended approach.

#### Use       content

Once generated, using the material amounts to understanding and then
developing an attack implementation for each challenge.
Consider an example wherein

```
export CHALLENGE="${SCALE_SW}/src/challenge/rsa_timing"
```

denotes the path to a particular challenge implementation,
in this case `rsa_timing`,
and

```
export USER="alice"
```

a user for whom material has been generated:

1. The associated challenge description can be viewed via

   ```
   evince ${CHALLENGE}/build/${USER}.pdf
   ```
   
2. An example 
   [Python](http://www.python.org)
   script
   can interact with the associated attack target: executing
   
   ```
   python ${SCALE_SW}/src/example/example.py ${CHALLENGE}/build/${USER}.D ${CHALLENGE}/build/${USER}.conf
   ```
   
   will
   
   - execute the attack target,
     namely `${USER}.D`,
   - load content from the configuration file, 
     i.e., 
     load
     `N` (an RSA modulus)
     and
     `e` (an RSA public exponent)
     from `${USER}.conf`,
   - generate a random `c` st. `0 <= c < N`,
   - send    input to    the attack target,
     i.e.,
     send
     `c` (an RSA ciphertext),
     then
   - receive output from the attack target,
     i.e.,
     receive
     `t` (an execution time) 
     and 
     `m` (an RSA plaintext),
   - dump configuration, inputs, and outputs.
   
   Using a similar approach, the goal is to recover an unknown
     `d` (an RSA private modulus)
   embedded in the attack target and used to compute `m` from `N` and `c`.

#### Clean     content

You can clean up as follows:
either

1. 1. - `make clean-doc`
        will clean   the documentation      from `${SCALE_SW}/build/doc`,
      - `make clean-deps`
        will clean
        each dependency,
      - `make clean-challenge`
        will clean   the challenge material from `${SCALE_SW}/src/challenge/*/build`,
      - `make clean-dist`
        will collate the challenge material from `${SCALE_SW}/build`,

      then

   2. - `make clean-share`
        will clean   the shared    material from `${SCALE_SW}/src/share/*/build`,
      - `make clean-conf`
         will clean   the configuration     from `${SCALE_SW}/build`,

   *or*

2. `make clean-step1`
   then, separately,
   `make clean-step2`
   (i.e.. *not* an all-in-one `make clean-step1 clean-step2`),
   will perform *all* of the above automatically, step-by-step and in 
   the correct order: this is the recommended approach.

<!--- -------------------------------------------------------------------- --->

## FAQs

- **"Is there anything else similar I could look at?"**
  Yes: there are plenty of great resources available, some offering more
  direct alternatives to SCALE and some of more tangential interest.
  Beyond the archive of CTF-like resources aggregated at
  [CTFtime](http://www.ctftime.org),
  and in no particular order, see 

  - http://www.dpacontest.org
  - http://cryptopals.com
  - http://www.riscure.com/challenge
  - http://microcorruption.com
  - http://exploit-exercises.com
  - http://challenges.re
  - http://crackmes.one
  - http://id0-rsa.pub
  - http://www.cis.syr.edu/~wedu/seed
  - http://overthewire.org/wargames
  - http://www.stockfighter.io
  - http://flaws.cloud
  - http://ropemporium.com

  plus CTF-like challenges from CHES, from

  - [2015](http://www.cryptoexperts.com/ches2015/challenge.html)
  - [2016](http://ctf.newae.com)
  - [2017](http://whibox-contest.github.io)

- **"The documentation sucks"**
  Yes it does; sorry about that.
  The main issue is that I have *very* limited experience with
  [Doxygen](http://www.doxygen.org):
  hopefully the documentation quality will improve once I learn how best
  to use it.

- **"The build process sucks"**
  Yes it does; sorry about that.
  In some more detail, there are actually several issues that need to be
  solved:

  - The build process has only really been tested on a
    [Ubuntu](http//www.ubuntu.com)-based
    Linux platform.  There doesn't *seem* an inherent reason it couldn't 
    work on a
    [Homebrew](http://www.brew.sh)-based
    MacOS platform, for example, but I don't have one to test it with.

  - One of the main wrinkles is that the configuration is
    a) generated from `scale.conf` by `Makefile`,
       *and*
    b) used by `Makefile` to control which challenges processed.
    This yields a somewhat circular problem, and is why most of the build
    process steps must be applied in the stated order.  Say, for example,
    the configuration was removed 
    (via `make clean-conf`) 
    *before* the challenges 
    (via `make clean-challenge`):
    this means `Makefile` would not know *which* challenges to remove, so
    fails.

- **"Do you have a solution for challenge X?"**
  Yes.

- **"I've solved challenge X: look, I've put the code on GitHub!"**
  Great!  Although the wider issue can be
  [thorny](http://www.wired.com/2015/02/university-bans-github-homework-changes-mind),
  obviously *I've* got no problem with you doing that.  Playing devils 
  advocate, however, one reason not to do so is that your solution *could* 
  cause problems for someone else using SCALE to set assessed coursework 
  assignments: in simple terms, they'd probably argue it'd make it harder 
  to tell how much of a submitted solution is original work (and, more 
  importantly, whether or not the fundamentals were fully understood).

  So if you *really* want to demonstrate that you have a solution (e.g.,
  for your CV), an alternative might be to just send *us* the code: I'll
  think about making a fancy looking
  [Udacity](http://www.udacity.com)-style
  certificate of completion or something.

- **"I can solve X by doing Y to executable attack target"**
  SCALE is really just for learning about side-channel and fault attacks.
  Although there are obviously strategies that may yield the same result 
  as the attack required, using them arguable misses the point (at least 
  a bit).  For example, some executable attack targets need associated
  material embedded in them, so although reverse engineering is a useful 
  skill it is *obvious* that applying it can and will work.

- **"Challenge X is nice, but I'd like to run my attack across a network 
     rather than locally."**
  I did consider doing this directly, i.e., using interaction between a
  networked client and server to representing the adversary and attack
  target, but worried any the extra challenge (stemming from a need for
  network programming) *may* put some people off.  So I opted for the 
  simplest form of interaction possible, using the standard `stdin` and
  `stdout` streams.  

  That said, 
  [`ncat`](http://nmap.org/ncat) (or similar) 
  makes "upgrading" to a networked alternative both possible and fairly
  easy.  Imagine you have an executable attack target called `${USER}.D`.
  By executing

  `ncat --listen ${PORT} --keep-open --exec ${USER}.D`
 
  on the server, `ncat` will listen on port `${PORT}` and then execute 
  `${USER}.D` when a connection is made: since it will connect `stdin` 
  and `stdout` of the executed process to the resulting socket, this
  basically allows `${USER}.D` to communicate over port `${PORT}` like
  a bespoke server.  For example, you can interact with it manually by 
  executing

  `ncat ${SERVER} ${PORT}`

  or connect an executable adversary, again via the network, in much 
  the same way.

<!--- -------------------------------------------------------------------- --->

## TODOs

- There are *always* improvements and additions possible in terms of the
  set of challenges offered:

  - The 
    [8051](http://en.wikipedia.org/wiki/Intel_MCS-51)
    simulator used by the `aes_power` challenge is somewhat of a hack;
    it *works*, but isn't too elegant.  It'd be nice to swap this for
    a 
    [HDL-based](http://en.wikipedia.org/wiki/Hardware_description_language)
    alternative, which might also offer a nice link to mounting the
    attack on a real target (e.g., the same implementation on a
    [SASEBO](http://satoh.cs.uec.ac.jp/SASEBO/en)).

  - It'd be nice to include a
    [Bellcore](http://link.springer.com/chapter/10.1007/3-540-69053-0_4)
    fault attack on RSA.

- Some challenges describe an attack target with a specific, say 64-bit
  word size.  If the platform SCALE is built and used on is *different*
  from this, behaviour of the executable target device may also differ:
  if GMP uses a 32-bit word size, for example, there will likely be an
  unintended difference in behaviour.  

  One could view this as a special-case of a more general problem; one
  way to solve it is to shift to a model whereby *everything* executes
  on top of a processor emulator, such as
  [Unicorn](http://www.unicorn-engine.org).
  Modulo the performance degradation, this *seems* an attractive step
  in the longer term: the challenge is having a consistent, structured
  way to do it.

- Currently there is no 
  [test suite](http://en.wikipedia.org/wiki/Software_testing)
  for the challenges, and so no
  [regression testing](http://en.wikipedia.org/wiki/Regression_testing).
  Clearly it makes sense to resolve this, but doing so has some subtle
  challenges: for instance, as FAQs explain, I don't *really* want to 
  distribute sample solutions (which would form an ideal test).

- A set of parameter definitions is currently provided to GCC using the 
  command line.  This is already quite long, and may need to get longer 
  still: at some point this will be a problem, and so it will be become
  preferable to generate then use the `@file` feature instead.

- I've considered solving the issue of dependencies etc. by building a
  custom 
  [LiveCD](http://en.wikipedia.org/wiki/Live_CD) 
  ISO or a 
  [Docker](http://www.docker.com) 
  container or something similar.  So far this hasn't happened, partly 
  because I didn't know how; it seems an interesting question to look
  into in the longer term.

<!--- -------------------------------------------------------------------- --->
