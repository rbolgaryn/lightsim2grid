# Copyright (c) 2020, RTE (https://www.rte-france.com)
# See AUTHORS.txt
# This Source Code Form is subject to the terms of the Mozilla Public License, version 2.0.
# If a copy of the Mozilla Public License, version 2.0 was not distributed with this file,
# you can obtain one at http://mozilla.org/MPL/2.0/.
# SPDX-License-Identifier: MPL-2.0
# This file is part of LightSim2grid, LightSim2grid implements a c++ backend targeting the Grid2Op platform.
import unittest
import warnings

from grid2op.tests.helper_path_test import PATH_DATA_TEST_PP, PATH_DATA_TEST

from grid2op.tests.helper_path_test import HelperTests
from grid2op.tests.BaseRedispTest import BaseTestRedispatch, BaseTestRedispatchChangeNothingEnvironment
from grid2op.tests.BaseRedispTest import BaseTestRedispTooLowHigh, BaseTestDispatchRampingIllegalETC
from grid2op.tests.BaseRedispTest import BaseTestLoadingAcceptAlmostZeroSumRedisp

from lightsim2grid.lightSimBackend import LightSimBackend

PATH_DATA_TEST_INIT = PATH_DATA_TEST
PATH_DATA_TEST = PATH_DATA_TEST_PP


class TestRedispatch(HelperTests, BaseTestRedispatch):
    def setUp(self):
        # TODO find something more elegant
        BaseTestRedispatch.setUp(self)

    def tearDown(self):
        # TODO find something more elegant
        BaseTestRedispatch.tearDown(self)

    def make_backend(self, detailed_infos_for_cascading_failures=False):
        with warnings.catch_warnings():
            warnings.filterwarnings("ignore")
            bk = LightSimBackend(detailed_infos_for_cascading_failures=detailed_infos_for_cascading_failures)
        return bk

    def get_path(self):
        return PATH_DATA_TEST_PP

    def get_casefile(self):
        return "test_case14.json"


class TestRedispatchChangeNothingEnvironment(HelperTests, BaseTestRedispatchChangeNothingEnvironment):
    def setUp(self):
        # TODO find something more elegant
        BaseTestRedispatchChangeNothingEnvironment.setUp(self)

    def tearDown(self):
        # TODO find something more elegant
        BaseTestRedispatchChangeNothingEnvironment.tearDown(self)

    def make_backend(self, detailed_infos_for_cascading_failures=False):
        with warnings.catch_warnings():
            warnings.filterwarnings("ignore")
            bk = LightSimBackend(detailed_infos_for_cascading_failures=detailed_infos_for_cascading_failures)
        return bk

    def get_path(self):
        return PATH_DATA_TEST_PP

    def get_casefile(self):
        return "test_case14.json"


class TestRedispTooLowHigh(HelperTests, BaseTestRedispTooLowHigh):
    def setUp(self):
        # TODO find something more elegant
        BaseTestRedispTooLowHigh.setUp(self)

    def tearDown(self):
        # TODO find something more elegant
        BaseTestRedispTooLowHigh.tearDown(self)

    def make_backend(self, detailed_infos_for_cascading_failures=False):
        with warnings.catch_warnings():
            warnings.filterwarnings("ignore")
            bk = LightSimBackend(detailed_infos_for_cascading_failures=detailed_infos_for_cascading_failures)
        return bk


class TestDispatchRampingIllegalETC(HelperTests, BaseTestDispatchRampingIllegalETC):
    def setUp(self):
        # TODO find something more elegant
        BaseTestDispatchRampingIllegalETC.setUp(self)

    def tearDown(self):
        # TODO find something more elegant
        BaseTestDispatchRampingIllegalETC.tearDown(self)

    def make_backend(self, detailed_infos_for_cascading_failures=False):
        with warnings.catch_warnings():
            warnings.filterwarnings("ignore")
            bk = LightSimBackend(detailed_infos_for_cascading_failures=detailed_infos_for_cascading_failures)
        return bk


class TestLoadingAcceptAlmostZeroSumRedisp(HelperTests, BaseTestLoadingAcceptAlmostZeroSumRedisp):
    def setUp(self):
        # TODO find something more elegant
        BaseTestLoadingAcceptAlmostZeroSumRedisp.setUp(self)

    def tearDown(self):
        # TODO find something more elegant
        BaseTestLoadingAcceptAlmostZeroSumRedisp.tearDown(self)

    def make_backend(self, detailed_infos_for_cascading_failures=False):
        with warnings.catch_warnings():
            warnings.filterwarnings("ignore")
            bk = LightSimBackend(detailed_infos_for_cascading_failures=detailed_infos_for_cascading_failures)
        return bk


if __name__ == "__main__":
    unittest.main()
