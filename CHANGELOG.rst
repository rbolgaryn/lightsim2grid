Change Log
===========

[TODO]
--------
- improve speed by not performing internal checks 
  (keep check for boundaries and all for python API instead) [see `TODO DEBUG MODE` in c++ code]
- improve speed
- code parrallelism directly in the `Computer` and `SecurityAnalysisCPP` classes
- code `helm` powerflow method
- possibility to read CGMES files
- possibility to read XIIDM files
- interface with gridpack (to enforce q limits for example)
- maybe have a look at suitesparse "sliplu" tools ?
- easier building (get rid of the "make" part)
- code NR with dense matrices

[0.6.1.post1] 2022-02-02
-------------------------
- [FIXED] support for python3.7 (and add proper tests in CI)

[0.6.1] 2022-02-01
--------------------
- [BREAKING] the behaviour of the `newton_pf` function is not 
  consistent with pandapower default concerning distributed slack.
- [FIXED] an issue in the distributed slack case spotted by pandapower team 
  thanks to them (see https://github.com/e2nIEE/pandapower/pull/1455)
- [IMPROVED] lightsim2grid will now use the single slack algorithm if the 
  grids counts only one slack bus (performance increase)

[0.6.0] 2021-12-17
-------------------
- [BREAKING] change the interface of the `newton_pf` function to reflect pandapower change in their
  latest version (arguments `ref` has been added). You can still use the old `newton_pf` function, with the
  old signature by importing `newtonpf_old` instead or explicitly importing the new one by importing `newtonpf_new`
- [BREAKING] `SecurityAnalysis` now also returns the active flows when calling `security_analysis.get_flows()`
- [BREAKING] change the file names (python side) to be compliant with pep 8. You can no longer
  do things like `from lightsim2grid.LightSimBackend import LightSimBackend` change it to
  `from lightsim2grid import LightSimBackend` (preferred method)
- [BREAKING] change the file names (python side) to be compliant with pep 8. You can no longer
  do things like `from lightsim2grid.initGridModel import init` change it to
  `from lightsim2grid.gridmodel import init` (preferred method) (same for `GridModel` class)
- [FIXED] a bug that lead to the wrong computation of the dc powerflow in case of `sn_mva != 1.` and phase shifters.
- [FIXED] bug preventing to use the NICSLU linear solver in the `GridModel`
- [FIXED] compilation warnings on clang (missing virtual destructor, unused variables, etc.)
- [FIXED] a bug in the `SecurityAnalysisCPP`: when it diverges for some contingencies, the others were not simulated properly.
- [FIXED] `LightSimBackend` now contains members for `shunts` and `***_theta` as it does for the other quantities. This improves the consistency, but most importantly
  fixes some bugs when used in earlier grid2op versions
- [ADDED] possibility to compute the active flows using the `BaseMultiplePower` 
- [ADDED] possibility to change linear solver used when performing a DC solver
- [ADDED] possibility to make powerflow with distributed slack bus (only for newton raphson at the moment)
- [ADDED] access (read only) to the element of a lightsim2grid grid with the `get_XXX` (*eg* `get_loads()`) methods (see documentation)
- [ADDED] direct access to the solver used in the grid model python side
- [ADDED] unittest in circleci.
- [ADDED] all kind of solvers based on different linear solvers (Eigen sparse LU, KLU or NICSLU) for Newton Raphson and
  DC approximation (9 solvers in total)
- [IMPROVED] use of `steady_clock` to retrieve the ellapse time c++ side
- [IMPROVED] refactoring of the c++ part to use template mecanism instead of inheritance for the
  Newton Raphson and DC solvers.
- [IMPROVED] `GridModel` now contains two different solvers, one for AC powerflow and one for DC powerflow.
- [IMPROVED] error message in the solver are now embedded in an Enum instead of being integers, for better readibility.
- [IMPROVED] error message when the powerflow diverge (error are read from c++ now)

[0.5.5] 2021-11-10
-------------------
- [ADDED] possibility to perform dc powerflow
- [ADDED] a class to compute flows on whole time series when the Ybus does not change (see `TimeSerie`)
- [ADDED] a class to compute flows on multiple contingencies, when Sbus does not change (see `SecurityAnalysis`).
- [IMPROVED] running speed of Newton Raphson solvers with better filling of sparse matrices
- [IMPROVED] upgrade to SuiteSparse `v5.10.1`
- [IMPROVED] upgrade to eigen `3.4.0` (stable release)
- [IMPROVED] clean the compilation warnings on microsoft windows (force the conversion from
  `Eigen::EigenBase<Derived>::Index` to `int` using `static_cast`)
- [IMPROVED] add the proper optimization flag for windows (`/O2` instead of `-03` on linux / macos)
- [IMPROVED] high performance gain when topology is not changed between steps (gain obtained by 
  reusing the previous Ybus)

[0.5.4] 2021-08-20
------------------
- [FIXED] a bug for static generator (wrong signed convention were used in some part of the c++ code). This has
  no impact at all for provided grid2op environments.
- [FIXED] An issue where the backend could get "stuck" in a wrong state because of the way the Vinit was computed (see
  `Issue 30 <https://github.com/BDonnot/lightsim2grid/issues/30>`_)
- [ADDED] experimental support for the `NICSLU` linear solver (requires a proper license and library, see
  https://github.com/chenxm1986/nicslu for more information. Support does not include multi threaded at the moment.
- [IMPROVED] minor performance improvements for the solvers based on Newton Raphson (faster filling of the Jacobian
  matrix after the first iteration)

[0.5.3] 2021-08-11
-------------------
- [FIXED] minor issues in the benchmark (some time measurments were wrong)
- [ADDED] lightsim2grid package now can be distributed on pypi
- [ADDED] compilation of SuiteSparse using cmake
- [ADDED] compatibility with the KLU linear solver on windows based systems.
- [IMPROVED] the package should now be available on pypi

[0.5.2] 2021-07-26
-------------------
- [FIXED] `GridModel` now properly throw "out_of_range" exception when trying to change the bus of non existing
  elements
- [FIXED] wrong units were displayed for the iterators for lines and transformers.
- [ADDED] now able to retrieve the powerlines parameters python side.
- [IMPROVED] more explicit error messages when the building of the `Ybus` matrix fails.
- [IMPROVED] now the solver is not reset when using the `backend._grid.check_solution`
- [IMPROVED] upgrade SuiteSparse to version `v5.10.1`
- [IMPROVED] upgrade eigen to version `3.4-rc1`

[0.5.1] 2021-04-09
-------------------
- [FIXED] yet another compilation issue with clang (see
  `Issue 22 <https://github.com/BDonnot/lightsim2grid/issues/22>`_)
- [ADDED] circleci to check compilation for gcc
- [ADDED] circleci to check compilation for clang
- [ADDED] circleci to check compilation for msvc
- [ADDED] function to read the voltage angle from the backend
- [ADDED] compatibility with grid2op 1.5.0 (up to an issue with the storage units)

[0.5.0] 2021-03-01
-------------------
- [FIXED] a compilation issue on macos
- [FIXED] a compilation issue on windows (missing import of vector in `DataConverter.h`)
- [FIXED] an import issue (with `lightsim2grid.SolverType`)
- [FIXED] a bug that lead to the wrong computation of the ratio of the trafo when the tap on hv side.
- [FIXED] wrong timing was measured in the "solver powerflow time" of pandapower in the benchmarks
- [FIXED] a broken handling of shunt modification (wrong bus was assigned)
- [FIXED] an issue in `LightSimBackend.copy` that prevent the copied environment from being reset.
- [FIXED] errors are now raised when pandapower grid cannot be converted in lightsim2grid (*eg.* when
  unsupported elements are present)
- [ADDED] a variant of the Gauss Seidel method which does the update in a "synchronous" fashion
- [ADDED] a function that, given a complex vector is able to check kicchoff's law violation.
- [ADDED] Support for phase shifter (modeled as trafo with an extra parameter `shift`)
- [ADDED] Experimental support for `sn_mva` pandapower parameter.
- [UPDATED] github issue template
- [IMPROVED] warnings are issued when some of the pandapowergrid attributes have been automatically replaced
  when converting to / from pandapower

[0.4.0] - 2020-10-26
---------------------
- [ADDED] the Gauss Seidel method for AC powerflow is now available
- [ADDED] possibility to change easily the solver types from python side

[0.3.0] - 2020-10-06
-------------------------
- [ADDED] Support for pickle for the lightsim Backend.
- [ADDED] LightSim should now be compatible with windows (implementation of a powerflow mode without
  using the SuiteSparse KLU linear solver but rather the Eigen SparseLU one)
- [ADDED] start of the documentation.

[0.2.4] - 2020-08-20
--------------------
- [FIXED] issue for copying environment

[0.2.3] - 2020-08-03
--------------------
- [UPDATED] consistent behaviour between grid2op.PandaPowerBackend and LightSimBackend for action that
  set the bus of only one extremity of a powerline.
- [ADDED] compatibility with grid2op 1.2.0

[0.2.2] - 2020-06-25
---------------------
- [UPDATED] removing the `-march=native` that causes some difficulty for some compilers
- [ADDED] compatibility with grid2op 1.0.0

[0.2.1] - 2020-06-xx
--------------------
- [FIXED] update of the `topo_vect` attribute in class `LightSimBackend` when reset.
- [ADDED] a github issue template

[0.2.0] - 2020-06-15
--------------------
- [ADDED] the changelog
- [FIXED] the import of files when elements where not in service
- [FIXED] a bad catch of a divergence in the solver
- [IMPROVED] the speed to apply the actions
- [FIXED] tests for the backend in grid2op and here are not identical without (too much) duplicates
