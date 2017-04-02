package com.ardikars.test;

import com.ardikars.jxnet.Inet4Address;
import com.ardikars.jxnet.Jxnet;
import com.ardikars.jxnet.MacAddress;
import com.ardikars.jxnet.Pcap;
import com.ardikars.jxnet.packet.PacketHandler;
import com.ardikars.jxnet.packet.PacketHelper;
import com.ardikars.jxnet.packet.ethernet.Ethernet;
import com.ardikars.jxnet.packet.ip.IPv4;
import com.ardikars.jxnet.packet.tcp.TCP;

import java.nio.ByteBuffer;
import java.util.Map;

public class PacketLoop {

    public static void main(String[] args) {


        Ethernet ethernet = new Ethernet()
                .setDestinationMacAddress(MacAddress.DUMMY)
                .setSourceMacAddress(MacAddress.valueOf("AA:AA:AA:BB:BB:BB"));

        IPv4 iPv4 = new IPv4()
                .setSourceAddress(Inet4Address.valueOf("192.168.1.124"))
                .setDestinationAddress(Inet4Address.valueOf("192.168.1.156"))
                .setIdentification((short) 49967)
                .setTtl((byte) 42);
        TCP tcp = new TCP()
                .setSourcePort((short) 50)
                .setDestinationPort((short) 65)
                .setSequence(24)
                .setAcknowledge(0)
                .setDataOffset((byte) 0)
                .setFlags((short) 0)
                .setUrgentPointer((short) 0)
                .setWindowSize((short) 356);

        iPv4.setPacket(tcp);
        ethernet.setPacket(iPv4);



        StringBuilder errbuf = new StringBuilder();
        //Pcap pcap = Jxnet.PcapOpenOffline("/home/pi/Downloads/pcapfiles/arp_pcap.pcapng.cap", errbuf);
        Pcap pcap = Jxnet.PcapOpenLive("eth0", 1500, 1, 2000, errbuf);
        PacketHandler<String> callback = (arg, pktHdr, packets) -> {
            for (Map.Entry value : packets.entrySet()) {
                System.out.println(value);
            }
            System.out.println("===========================================================");
        };

        //PacketHelper.loop(pcap, 50, callback, null);

        byte[] data = ethernet.toBytes();
        ByteBuffer buf = ByteBuffer.allocateDirect(data.length);
        buf.put(data);
        Jxnet.PcapSendPacket(pcap, buf, buf.capacity());
        Jxnet.PcapClose(pcap);
    }
}
