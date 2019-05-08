/**
 * Copyright (C) 2015-2018 Jxnet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package com.ardikars.jxnet;

import com.ardikars.jxnet.exception.ExceptionTest;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;

@RunWith(Suite.class)
@Suite.SuiteClasses({
        JxnetTest.class,
        FreakTest.class,
        ImmediateModeTest.class,
        PcapTimeStampPrecisionTest.class,
        PromiscuousModeTest.class,
        RadioFrequencyMonitorModeTest.class,
        SockAddrTest.class,
        PcapStatTest.class,
        PcapPktHdrTest.class,
        ExceptionTest.class,
        PcapCodeTest.class,
        DatalinkTypeTest.class,
        BpfProgramTest.class,
        PcapTest.class
})
public class SuiteTest {

    @Test
    public void LibraryTest() {
        assert true;
    }

}
