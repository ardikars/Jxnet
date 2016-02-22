package com.ardikars.jxpcap;

import java.util.ArrayList;
import java.util.List;

public class JxpcapIf {
	
	private native static void initIDs();
	
	public final static int PCAP_IF_LOOPBACK = 1;
	
	private JxpcapIf next;
	
	private String name;
	
	private String description;
	
	List<JxpcapAddr> addresses = new ArrayList<JxpcapAddr>();
	
	JxpcapNaddr naddresses = new JxpcapNaddr();
	
	private int flags;
	
	public String getName() {
		return name;
	}
	
	public String getDescription() {
		return description;
	}
	
	public List<JxpcapAddr> getAddresses() {
		return addresses;
	}
	
	public JxpcapNaddr getNaddresses() {
		return naddresses;
	}
		
	public int getFlags() {
		return flags;
	}
	
	static {
		initIDs();
		new JxpcapNaddr();
		new JxpcapAddr();
		/*try {
			Class.forName("com.ardikars.jxpcap.JxpcapAddr");
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}*/
	}
}
