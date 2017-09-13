/**
 * Copyright (C) 2017  Ardika Rommy Sanjaya
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

import com.ardikars.jxnet.util.Loaders;

import java.nio.ByteBuffer;
import java.util.List;

/**
 * @author Ardika Rommy Sanjaya
 * @since 1.0.0
 */
public final class Jxnet extends Core {

	public static final int OK = 0;
	
	private static boolean isLoaded = false;

	private static native void InitIDs(int hashCode);

	private Jxnet() {
		//prevent to create jxnet instance
	}

	public static void initIDs(Class clazz) {
		//InitIDs(clazz.getName().hashCode());
		System.out.println("Initializing " + clazz.getName() + " IDs.");
	}

	/**
	 * Get a list of all capture sources that are up and that we can open.
	 * Returns -1 on error, 0 otherwise.
	 * The list, as returned through "alldevsp", may be null if no interfaces
	 * were up and could be opened.
	 * @param alldevsp list of interface.
	 * @param errbuf error buffer.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native int PcapFindAllDevs(List<PcapIf> alldevsp, StringBuilder errbuf);

	/**
	 * Open a live capture from the network.
	 * @param source interface name.
	 * @param snaplen snapshot length (16 bit)
	 * @param promisc 1 to enable promiscuous mode, 0 otherwise.
	 * @param to_ms timeout.
	 * @param errbuf error buffer.
	 * @return null on error.
	 */
	public static native Pcap PcapOpenLive(String source, int snaplen, int promisc, int to_ms, StringBuilder errbuf);

	/**
	 * Collect a group of packets.
	 * @param pcap pcap object.
	 * @param cnt maximum iteration, -1 to infinite.
	 * @param callback callback funtion.
	 * @param user args
	 * @param <T> arps type.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native <T> int PcapLoop(Pcap pcap, int cnt, PcapHandler<T> callback, T user);

	/**
	 * Collect a group of packets.
	 * @param pcap pcap object.
	 * @param cnt maximum iteration, -1 to infinite.
	 * @param callback callback function.
	 * @param user arg.
	 * @param <T> arg type.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native <T> int PcapDispatch(Pcap pcap, int cnt, PcapHandler<T> callback, T user);

	/**
	 * Open a file to write packets.
	 * @param pcap pcap object.
	 * @param fname file name.
	 * @return null on error.
	 */
	public static native PcapDumper PcapDumpOpen(Pcap pcap, String fname);

	/**
	 * Save a packet to disk.
	 * @param pcap_dumper pcap dumper object.
	 * @param h pcap packet header.
	 * @param sp packet buffer.
	 */
	public static native void PcapDump(PcapDumper pcap_dumper, PcapPktHdr h, ByteBuffer sp);

	/**
	 * Open a savefile in the tcpdump/libpcap format to read packets.
	 * @param fname file name.
	 * @param errbuf error buffer.
	 * @return null on error.
	 */
	public static native Pcap PcapOpenOffline(String fname, StringBuilder errbuf);

	/**
	 * Compile a packet filter, converting an high level filtering expression
	 * (see Filtering expression syntax) in a program that can be interpreted
	 * by the kernel-level filtering engine.
	 * @param pcap pcap object.
	 * @param fp compiled bfp.
	 * @param str filter expression.
	 * @param optimize optimize (0/1).
	 * @param netmask netmask.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native int PcapCompile(Pcap pcap, BpfProgram fp, String str, int optimize, int netmask);

	/**
	 * Associate a filter to a capture.
	 * @param pcap pcap object.
	 * @param fp compiled bpf.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native int PcapSetFilter(Pcap pcap, BpfProgram fp);

	/**
	 * Send a raw packet.
	 * @param pcap pcap object.
	 * @param buf packet buffer.
	 * @param size size of packet buffer.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native int PcapSendPacket(Pcap pcap, ByteBuffer buf, int size);

	/**
	 * Return the next available packet.
	 * @param pcap pcap object.
	 * @param h packet header.
	 * @return next available packet.
	 */
	public static native ByteBuffer PcapNext(Pcap pcap, PcapPktHdr h);

	/**
	 * Read a packet from an interface or from an offline capture.
	 * @param pcap pcap object.
	 * @param pkt_header packet header.
	 * @param pkt_data packet buffer.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native int PcapNextEx(Pcap pcap, PcapPktHdr pkt_header, ByteBuffer pkt_data);

	/**
	 * Close the files associated with pcap and deallocates resources.
	 * @param pcap pcap object.
	 */
	public static native void PcapClose(Pcap pcap);

	/**
	 * Flushes the output buffer to the ``savefile,'' so that any packets written
	 * with PcapDump() but not yet written to the ``savefile'' will be written. -1
	 * is returned on error, 0 on success.
	 * @param pcap_dumper pcap dumper object.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native int PcapDumpFlush(PcapDumper pcap_dumper);

	/**
	 * Closes a savefile.
	 * @param pcap_dumper pcap dumper object.
	 */
	public static native void PcapDumpClose(PcapDumper pcap_dumper);

	/**
	 * Return the link layer of an adapter.
	 * @param pcap pcap object.
	 * @return link layer of an adapter.
	 */
	public static native int PcapDataLink(Pcap pcap);

	/**
	 * Set the current data link type of the pcap descriptor to the type
	 * specified by dlt. -1 is returned on failure.
	 * @param pcap pcap object.
	 * @param dtl data link type.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native int PcapSetDataLink(Pcap pcap, int dtl);

	/**
	 * Set a flag that will force PcapDispatch() or PcapLoop() to return rather than looping.
	 * @param pcap pcap object.
	 */
	public static native void PcapBreakLoop(Pcap pcap); //

	/**
	 * Return the first valid device in the system.
	 * @param errbuf error buffer.
	 * @return first valid device in the system.
	 * PCAP_DEPRECATED(pcap_lookupdev, "use 'pcap_findalldevs' and use the first device")
	 */
	@Deprecated
	public static native String PcapLookupDev(StringBuilder errbuf); //

	/**
	 * Return the error text pertaining to the last pcap library error.
	 * @param pcap pcap object.
	 * @return error text pertaining to the last pcap library error.
	 */
	public static native String PcapGetErr(Pcap pcap);

	/**
	 * Returns a pointer to a string giving information about the version of the libpcap library being used;
	 * note that it contains more information than just a version number.
	 * @return libpcap version.
	 */
	public static native String PcapLibVersion();

	/**
	 * Returns true (1) if the current savefile uses a different byte order than the current system.
	 * @param pcap pcap object.
	 * @return true (1) if the current savefile uses a different byte order than the current system.
	 */
	public static native int PcapIsSwapped(Pcap pcap);

	/**
	 * Return the dimension of the packet portion (in bytes) that is delivered to the application.
	 * @param pcap pcap object
	 * @return dimension of the packet portion (in bytes) that is delivered to the application.
	 */
	public static native int PcapSnapshot(Pcap pcap);

	/**
	 * Provided in case StrError() isn't available.
	 * @param error error code.
	 * @return error message.
	 */
	public static native String PcapStrError(int error);

	/**
	 * Return the major version number of the pcap library used to write the savefile.
	 * @param pcap pcap object.
	 * @return major version number of the pcap library used to write the savefile.
	 */
	public static native int PcapMajorVersion(Pcap pcap);

	/**
	 * Return the minor version number of the pcap library used to write the savefile.
	 * @param pcap pcap object.
	 * @return minor version number of the pcap library used to write the savefile.
	 */
	public static native int PcapMinorVersion(Pcap pcap);

	/**
	 * Translates a data link type value to the corresponding data link type name.
	 * NULL is returned on failure.
	 * @param dtl link type.
	 * @return null is returned on failure.
	 */
	public static native String PcapDataLinkValToName(int dtl);

	/**
	 * Translates a data link type value to a short description of that data link type.
	 * NULL is returned on failure.
	 * @param dtl link type.
	 * @return is returned on failure.
	 */
	public static native String PcapDataLinkValToDescription(int dtl);

	/**
	 * Translates a data link type name, which is a DLT_ name with the DLT_ removed,
	 * to the corresponding data link type value. The translation is case-insensitive.
	 * -1 is returned on failure.
	 * @param name link type name.
	 * @return link type.
	 */
	public static native int PcapDataLinkNameToVal(String name);

	/**
	 * Switch between blocking and nonblocking mode.
	 * @param pcap pcap object.
	 * @param nonblock 1 to set non block.
	 * @param errbuf error buffer.
	 * @return -1 on error.
	 */
	public static native int PcapSetNonBlock(Pcap pcap, int nonblock, StringBuilder errbuf);

	/**
	 * Get the "non-blocking" state of an interface.
	 * @param pcap pcap object.
	 * @param errbuf error buffer.
	 * @return nonblocking is 1, blocking 0.
	 */
	public static native int PcapGetNonBlock(Pcap pcap, StringBuilder errbuf);

	/**
	 * Create a pcap_t structure without starting a capture.
	 * @param linktype link type.
	 * @param snaplen snapshot length.
	 * @return null on error.
	 */
	public static native Pcap PcapOpenDead(int linktype, int snaplen);

	/**
	 * Return the file position for a "savefile".
	 * @param pcap_dumper pcap dumper object.
	 * @return file position for a "savefile".
	 */
	public static native long PcapDumpFTell(PcapDumper pcap_dumper); //

	/**
	 * Free a filter.
	 * @param bpf_program compiled bpf.
	 */
	public static native void PcapFreeCode(BpfProgram bpf_program);

	/**
	 * Return the standard stream of an offline capture.
	 * @param pcap pcap object.
	 * @return null on error.
	 */
	//public static native File PcapFile(Pcap pcap);

	/**
	 * Return the standard I/O stream of the 'savefile' opened by PcapDumpOpen().
	 * @param pcap_dumper pcap dumper object.
	 * @return null on error.
	 */
	//public static native File PcapDumpFile(PcapDumper pcap_dumper);

	/**
	 *
 	 * @param pcap pcap object.
	 * @param f file object.
	 * @return null on error.
	 */
	//public static native PcapDumper PcapDumpFOpen(Pcap pcap, File f);

	/**
	 * Return statistics on current capture.
	 * @param pcap pcap object.
	 * @param pcap_stat pcap stat object.
	 * @return -1 on error
	 */
	public static native int PcapStats(Pcap pcap, PcapStat pcap_stat);

	/**
	 * Return the subnet and netmask of an interface.
	 * @param device interface name/
	 * @param netp netproto
	 * @param maskp netmask
	 * @param errbuf error buffer.
	 * @return -1 on error, 0 otherwise.
	 */
	public static native int PcapLookupNet(String device, Inet4Address netp, Inet4Address maskp, StringBuilder errbuf);

	/**
	 * Compile a packet filter without the need of opening an adapter.
	 * This function converts an high level filtering expression (see Filtering expression syntax)
	 * in a program that can be interpreted by the kernel-level filtering engine.
	 * @param snaplen_arg snapshot length.
	 * @param linktype_arg link type.
	 * @param program bpf.
	 * @param buf str.
	 * @param optimize optiomize (0/1).
	 * @param mask netmask.
	 * @return -1 on error.
	 */
	public static native int PcapCompileNoPcap(int snaplen_arg, int linktype_arg, BpfProgram program, String buf, int optimize, int mask);

	/**
	 * Print the text of the last pcap library error on stderr, prefixed by prefix.
	 * @param pcap pcap object.
	 * @param prefix prrfix.
	 */
	public static native void PcapPError(Pcap pcap, String prefix);

	/**
	 * Is used to create a packet capture handle to look at packets on the network.
	 * Source is a string that specifies the network device to open;
	 * on Linux systems with 2.2 or later kernels, a source argument of "any" or NULL
	 * can be used to capture packets from all interfaces.
	 *
	 * The returned handle must be activated with PcapActivate() before packets can be captured with it;
	 * options for the capture, such as promiscuous mode, can be set on the handle before activating it.
	 * @param source network device.
	 * @param errbuf errof buffer.
	 * @return returns a pcap_t * on success and NULL on failure. If NULL is returned, errbuf is filled in with an
	 * appropriate error message.
	 */
	public static native Pcap PcapCreate(String source, StringBuilder errbuf);

	/**
	 * Sets the snapshot length to be used on a capture handle when the handle is activated to snaplen.
	 * @param pcap pcap object.
	 * @param snaplen snaplen.
	 * @return 0 on success.
	 */
	public static native int PcapSetSnaplen(Pcap pcap, int snaplen);

	/**
	 * sets whether promiscuous mode should be set on a capture handle when the handle is activated.
	 * If promisc is non-zero, promiscuous mode will be set, otherwise it will not be set.
	 * @param pcap pcap object.
	 * @param promisc promisc.
	 * @return 0 on success.
	 */
	public static native int PcapSetPromisc(Pcap pcap, int promisc);

	/**
	 * Sets the packet buffer timeout that will be used on a capture handle when the handle is activated to to_ms, which is in units of milliseconds.
	 * @param pcap pcap object.
	 * @param timeout timeout.
	 * @return 0 on success.
	 */
	public static native int PcapSetTimeout(Pcap pcap, int timeout);

	/**
	 * Sets the buffer size that will be used on a capture handle when the handle is activated to buffer_size, which is in units of bytes.
	 * @param pcap pcap object.
	 * @param buffer_size buffer size.
	 * @return 0 on success.
	 */
	public static native int PcapSetBufferSize(Pcap pcap, int buffer_size);

	/**
	 * Checks whether monitor mode could be set on a capture handle when the handle is activated.
	 * @param pcap pcap object.
	 * @return 0 if monitor mode could not be set, 1 if monitor mode could be set, and a negative value on error.
	 */
	public static native int PcapCanSetRfMon(Pcap pcap);

	/**
	 * Sets whether monitor mode should be set on a capture handle when the handle is activated.
	 * If rfmon is non-zero, monitor mode will be set, otherwise it will not be set.
	 * @param pcap pcap object.
	 * @param rfmon 1 (true) or 0 (false).
	 * @return 0 on success.
	 */
	public static native int PcapSetRfMon(Pcap pcap, int rfmon);

	/**
	 * Sets whether immediate mode should be set on a capture handle when the handle is activated. If immediate_mode is non-zero,
	 * immediate mode will be set, otherwise it will not be set.
	 * @param pcap pcap object.
	 * @param immediate_mode immediate_mode.
	 * @return 0 on success.
	 */
	public static native int PcapSetImmediateMode(Pcap pcap, int immediate_mode);

	/**
	 * Is used to activate a packet capture handle to look at packets on the network,
	 * with the options that were set on the handle being in effect.
	 * @param pcap pcap object.
	 * @return 0 on success without warnings, a non-zero positive value on success with warnings, and a negative value on error.
	 */
	public static native int PcapActivate(Pcap pcap);

	/**
	 * used to specify a direction that packets will be
	 * captured.    d  is  one  of  the  constants  PCAP_D_IN,  PCAP_D_OUT  or
	 * PCAP_D_INOUT.  PCAP_D_IN will only  capture  packets  received  by  the
	 * device,  PCAP_D_OUT  will  only  capture packets sent by the device and
	 * PCAP_D_INOUT will capture packets received by or sent  by  the  device.
	 * PCAP_D_INOUT is the default setting if this function is not called.
	 *
	 * PcapSetdirection() isn't necessarily fully supported on all platforms;
	 * some platforms might return an error for all  values,  and  some  other
	 * platforms might not support PCAP_D_OUT.
	 *
	 * This operation is not supported if a ``savefile'' is being read.
	 * @param pcap pcap object.
	 * @param direction direction.
	 * @return returns  0 on success and -1 on failure (not supported by operating system).
	 */
	public static native int PcapSetDirection(Pcap pcap, PcapDirection direction);

	/**
	 * Set the time stamp precision returned in captures.
	 * @param pcap pcap.
	 * @param tstamp_precision time stamp precision.
	 * @return 0 on success if specified time stamp precision is expected to be supported
	 * by operating system.
	 */
	//public static native int PcapSetTStampPrecision(Pcap pcap, int tstamp_precision);

	/**
	 * Get the time stamp precision returned in captures.
	 * @param pcap pcap object.
	 * @return the precision of the time stamp returned in packet captures on the pcap descriptor.
	 */
	// public static native int PcapGetTStampPrecision(Pcap pcap);

	static {
		if (!isLoaded) {
			try {
				Loaders.loadLibrary();
				initIDs(List.class);
				initIDs(StringBuilder.class);
				initIDs(ByteBuffer.class);
				isLoaded = true;
			} catch (Exception e) {
				System.err.println(e.getMessage());
				isLoaded = false;
			}
		}
	}

}