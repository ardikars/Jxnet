package com.ardikars.jxnet;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

@RunWith(JUnit4.class)
public class ApplicationTest {



    @Test
    public void test() {
        assert true;
//        StringBuilder errbuf = new StringBuilder();
//        Application.run("Test", "0.1", Initializer.class,
//                new Pcap.Builder().pcapType(Pcap.PcapType.LIVE)
//                        .source("en0")
//                        .immediateMode(ImmediateMode.IMMEDIATE)
//                        .errbuf(errbuf),
//                new BpfProgram.Builder().bpfCompileMode(BpfProgram.BpfCompileMode.OPTIMIZE)
//                        .filter("tcp")
//                        .netmask(Inet4Address.valueOf("255.255.255.0").toInt()),
//                "Yoo"
//        );
//        Context context = Application.getApplicationContext();
//        context.pcapLoop(5, new PcapHandler<String>() {
//            @Override
//            public void nextPacket(String user, PcapPktHdr h, ByteBuffer byteBuffer) {
//                byte[] buffer = new byte[byteBuffer.capacity()];
//                byteBuffer.get(buffer, 0, buffer.length);
//                System.out.println("Header: " + h);
//                System.out.println("Buffer: \n" + Hexs.toPrettyHexDump(buffer));
//                System.out.println(user);
//            }
//        }, "***********************");
//        context.pcapClose();
    }

}
