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

import com.ardikars.common.annotation.Immutable;
import com.ardikars.common.annotation.Incubating;
import com.ardikars.common.logging.Logger;
import com.ardikars.common.logging.LoggerFactory;
import com.ardikars.common.util.Callback;
import com.ardikars.jxnet.exception.BpfProgramCloseException;
import com.ardikars.jxnet.exception.DeviceNotFoundException;
import com.ardikars.jxnet.exception.NativeException;
import com.ardikars.jxnet.exception.PcapCloseException;
import com.ardikars.jxnet.exception.PcapDumperCloseException;
import com.ardikars.jxnet.exception.PlatformNotSupportedException;
import com.ardikars.jxnet.util.DefaultLibraryLoader;

import java.nio.ByteBuffer;
import java.util.List;

/**
 * This class is a function mapping for libpcap api.
 *
 * @author <a href="mailto:contact@ardikars.com">Ardika Rommy Sanjaya</a>
 * @since 1.0.0
 */
@Immutable
public final class Jxnet {

	private static final Logger LOGGER = LoggerFactory.getLogger(Jxnet.class);

	public static final int OK = 0;

	public static final int PCAP_ERRBUF_SIZE = 255;

	private static boolean loaded;

	private Jxnet() {
		super();
	}

	/**
	 * PcapFindAllDevs() Constructs a list of network devices that can be
	 * opened with PcapCreate() and PcapActivate() or with PcapOpenLive().
	 * (Note that there may be network devices that cannot be  opened by the
	 * process calling PcapFindAllDevs()).
	 * @param alldevsp list of PcapIf.
	 * @param errbuf error buffer.
	 * @return PcapFindAllDevs() returns 0 on success and -1 on failure; as indicated,
	 * finding  no devices is considered success, rather than failure,
	 * so 0 will be returned in that case. If -1 is returned, errbuf is
	 * filled in with an appropriate error message.
	 * @since 1.1.4
	 */
	public static native int PcapFindAllDevs(List<PcapIf> alldevsp, StringBuilder errbuf);

	/**
	 * Open a live capture handle.
	 * On Linux systems with 2.2 or later kernels, a device (source)
	 * argument of "any" or NULL can be used to capture packets from all interfaces.
	 * @param source interface name.
	 * @param snaplen specifies the snapshot length to be set on the handle (16 bit).
	 * @param promisc specifies if the interface is to be put into promiscuous mode.
	 * @param toMs specifies  the  packet  buffer  timeout  in  milliseconds.
	 * @param errbuf error buffer.
	 * @return PcapOpenLive() returns null on error. If null is returned, errbuf is filled in with an appropriate error message.
	 * @since 1.1.4
	 */
	public static native Pcap PcapOpenLive(String source, int snaplen, int promisc, int toMs, StringBuilder errbuf);

	/**
	 * Collect a group of packets.
	 * Callback argument already asyncronous.
	 * @param pcap pcap instance.
	 * @param cnt maximum iteration, -1 is infinite iteration.
	 * @param callback callback function.
	 * @param user args
	 * @param <T> args type.
	 * @return PcapLoop() returns 0 if cnt is exhausted or if, when reading from a
	 * @throws PcapCloseException pcap close exception.
	 * savefile, no more packets are available. It returns -1 if an error
	 * occurs or -2 if the loop terminated due to a call to PcapBreakLoop()
	 * before any packets were processed.  It does not return when live packet
	 * buffer timeouts occur; instead, it attempts to read more packets.
	 * @since 1.1.4
	 */
	public static native <T> int PcapLoop(Pcap pcap, int cnt, PcapHandler<T> callback, T user) throws PcapCloseException;

	/**
	 * Collect a group of packets.
	 * Callback argument already asyncronous.
	 * @param pcap pcap instance.
	 * @param cnt maximum iteration, -1 is infinite iteration.
	 * @param callback callback function.
	 * @param user args
	 * @param <T> args type.
	 * @return PcapLoop() returns 0 if cnt is exhausted or if, when reading from a
	 * @throws PcapCloseException pcap close exception.
	 * savefile, no more packets are available. It returns -1 if an error
	 * occurs or -2 if the loop terminated due to a call to PcapBreakLoop()
	 * before any packets were processed.  It does not return when live packet
	 * buffer timeouts occur; instead, it attempts to read more packets.
	 * @since 1.5.4
	 */
	@Incubating
	public static native <T> int PcapLoop0(Pcap pcap, int cnt, RawPcapHandler<T> callback, T user) throws PcapCloseException;

	/**
	 * Collect a group of packets.
	 * @param pcap pcap instance.
	 * @param cnt maximum iteration, -1 to infinite.
	 * @param callback callback function.
	 * @param user arg.
	 * @param <T> args type.
	 * @return PcapDispatch() returns the number of packets processed on success;
	 * @throws PcapCloseException pcap close exception.
	 * this can be 0 if no packets were read from a live capture (if, for
	 * example, they were discarded because they didn't pass the packet filter,
	 * or if, on platforms that support a packet buffer timeout that
	 * starts before any packets arrive, the timeout expires before any packets
	 * arrive, or if the file descriptor for the capture device is in non-blocking
	 * mode and no packets were available to be read) or if no more
	 * packets are available in a savefile. It returns -1 if an error
	 * occurs or -2 if the loop terminated due to a call to PcapBreakLoop()
	 * before any packets were processed. If your application uses
	 * PcapBreakLoop(), make sure that you explicitly check for -1 and -2,
	 * rather than just checking for a return value less then 0.
	 * @since 1.1.4
	 */
	public static native <T> int PcapDispatch(Pcap pcap, int cnt, PcapHandler<T> callback, T user) throws PcapCloseException;

	/**
	 * Collect a group of packets.
	 * @param pcap pcap instance.
	 * @param cnt maximum iteration, -1 to infinite.
	 * @param callback callback function.
	 * @param user arg.
	 * @param <T> args type.
	 * @return PcapDispatch() returns the number of packets processed on success;
	 * @throws PcapCloseException pcap close exception.
	 * this can be 0 if no packets were read from a live capture (if, for
	 * example, they were discarded because they didn't pass the packet filter,
	 * or if, on platforms that support a packet buffer timeout that
	 * starts before any packets arrive, the timeout expires before any packets
	 * arrive, or if the file descriptor for the capture device is in non-blocking
	 * mode and no packets were available to be read) or if no more
	 * packets are available in a savefile. It returns -1 if an error
	 * occurs or -2 if the loop terminated due to a call to PcapBreakLoop()
	 * before any packets were processed. If your application uses
	 * PcapBreakLoop(), make sure that you explicitly check for -1 and -2,
	 * rather than just checking for a return value less then 0.
	 * @since 1.5.4
	 */
	@Incubating
	public static native <T> int PcapDispatch0(Pcap pcap, int cnt, RawPcapHandler<T> callback, T user) throws PcapCloseException;

	/**
	 * Open a file to write packets.
	 * @param pcap pcap instance.
	 * @param fname fname specifies the name of the file to open. The file will have the same format
	 *                 as those used by tcpdump(1) and tcpslice(1). The name "-" is a synonym for stdout.
	 * @return null on error.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native PcapDumper PcapDumpOpen(Pcap pcap, String fname) throws PcapCloseException;

	/**
	 * Save a packet to disk.
	 * @param pcapDumper pcap dumper object.
	 * @param h pcap packet header.
	 * @param sp packet buffer.
	 * @throws PcapDumperCloseException pcap dumper close exception.
	 * @since 1.1.4
	 */
	public static native void PcapDump(PcapDumper pcapDumper, PcapPktHdr h, ByteBuffer sp) throws PcapDumperCloseException;

	/**
	 * Open a savefile in the tcpdump/libpcap format to read packets.
	 * @param fname file name.
	 * @param errbuf error buffer.
	 * @return null on error. If NULL is returned, errbuf is filled in with an appropriate error message.
	 * @since 1.1.4
	 */
	public static native Pcap PcapOpenOffline(String fname, StringBuilder errbuf);

	/**
	 * Compile a packet filter, converting an high level filtering expression
	 * (see Filtering expression syntax) in a program that can be interpreted
	 * by the kernel-level filtering engine.
	 * Note: Before libpcap-1.8.1 this function is not thread-safe @link https://github.com/the-tcpdump-group/libpcap/issues/75.
	 * @param pcap pcap instance.
	 * @param fp compiled bfp.
	 * @param str filter expression.
	 * @param optimize optimize (0/1).
	 * @param netmask netmask.
	 * @return -1 on error, 0 otherwise.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapCompile(Pcap pcap, BpfProgram fp, String str, int optimize, int netmask) throws PcapCloseException;

	/**
	 * Associate a filter to a capture.
	 * @param pcap pcap instance.
	 * @param fp compiled bpf.
	 * @return -1 on error, 0 otherwise.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapSetFilter(Pcap pcap, BpfProgram fp) throws PcapCloseException;

	/**
	 * Send a raw packet.
	 * @param pcap pcap instance.
	 * @param buf packet buffer.
	 * @param size size of packet buffer.
	 * @return -1 on error, 0 otherwise.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapSendPacket(Pcap pcap, ByteBuffer buf, int size) throws PcapCloseException;

	/**
	 * Return the next available packet.
	 * @param pcap pcap instance.
	 * @param h packet header.
	 * @return PcapNext() returns next available packet.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native ByteBuffer PcapNext(Pcap pcap, PcapPktHdr h) throws PcapCloseException;

	/**
	 * Read a packet from an interface or from an offline capture.
	 * @param pcap pcap instance.
	 * @param pktHeader packet header.
	 * @param pktData packet buffer.
	 * @return PcapNextEx() returns 1 if the packet was read without problems, 0 if
	 * packets are being read from a live capture and the packet buffer time-
	 * out expired, -1 if an error occurred while reading the packet, and -2
	 * if packets are being read from a savefile and there are no more
	 * packets to read from the savefile.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapNextEx(Pcap pcap, PcapPktHdr pktHeader, ByteBuffer pktData) throws PcapCloseException;

	/**
	 * Close the files associated with pcap and deallocates resources.
	 * @param pcap pcap instance.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native void PcapClose(Pcap pcap) throws PcapCloseException;

	/**
	 * Flushes the output buffer to the savefile, so that any packets written
	 * with PcapDump() but not yet written to the savefile will be written. -1
	 * is returned on error, 0 on success.
	 * @param pcapDumper pcap dumper object.
	 * @return -1 on error, 0 otherwise.
	 * @throws PcapDumperCloseException pcap dumper close exception.
	 * @since 1.1.4
	 */
	public static native int PcapDumpFlush(PcapDumper pcapDumper) throws PcapDumperCloseException;

	/**
	 * Closes a savefile.
	 * @param pcapDumper pcap dumper object.
	 * @throws PcapDumperCloseException pcap dumper close exception.
	 * @since 1.1.4
	 */
	public static native void PcapDumpClose(PcapDumper pcapDumper) throws PcapDumperCloseException;

	/**
	 * Return the link layer of an adapter on success.
	 * @param pcap pcap instance.
	 * @return link layer type of an adapter on success and
	 * PCAP_ERROR_NOT_ACTIVATED(-3) if called on a capture handle that has been
	 * created but not activated.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapDataLink(Pcap pcap) throws PcapCloseException;

	/**
	 * Set the current data link type of the pcap descriptor to the type
	 * specified by dlt. -1 is returned on failure.
	 * @param pcap pcap instance.
	 * @param dtl data link type.
	 * @return -1 on error, 0 otherwise.
	 * @throws PcapCloseException pcap close exception.
	 * @throws NativeException native exception (pcap handle is dead).
	 * @since 1.1.4
	 */
	public static native int PcapSetDataLink(Pcap pcap, int dtl) throws PcapCloseException, NativeException;

	/**
	 * Set a flag that will force PcapDispatch() or PcapLoop() to return rather than looping.
	 * @param pcap pcap instance.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native void PcapBreakLoop(Pcap pcap) throws PcapCloseException;

	/**
	 * Return the first valid device in the system.
	 * @param errbuf error buffer.
	 * @return first valid device in the system. PCAP_DEPRECATED(pcap_lookupdev, "use 'pcap_findalldevs' and use the first device")
	 * @since 1.1.4
	 * @deprecated use 'pcap_findalldevs' and use the first device.
	 */
	@Deprecated
	public static native String PcapLookupDev(StringBuilder errbuf);

	/**
	 * Return the error text pertaining to the last pcap library error.
	 * @param pcap pcap instance.
	 * @return error text pertaining to the last pcap library error.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native String PcapGetErr(Pcap pcap) throws PcapCloseException;

	/**
	 * Returns a string giving information about the version of the libpcap library being used;
	 * note that it contains more information than just a version number.
	 * @return libpcap version.
	 * @since 1.1.4
	 */
	public static native String PcapLibVersion();

	/**
	 * Returns true (1) if the current savefile uses a different byte order than the current system.
	 * @param pcap pcap instance.
	 * @return PcapIsSwapped() returns true (1) or false (0) on success and
	 * PCAP_ERROR_NOT_ACTIVATED(-3) if called on a capture handle that has been
	 * created but not activated.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapIsSwapped(Pcap pcap) throws PcapCloseException;

	/**
	 * Return the dimension of the packet portion (in bytes) that is delivered to the application.
	 * @param pcap pcap instance
	 * @return PcapSnapshot() returns the snapshot length on success and
	 * PCAP_ERROR_NOT_ACTIVATED(-3) if called on a capture handle that has been created but not activated.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapSnapshot(Pcap pcap) throws PcapCloseException;

	/**
	 * Provided in case StrError() isn't available. It returns an error message string corresponding to error.
	 * @param error error code.
	 * @return error message.
	 * @since 1.1.4
	 */
	public static native String PcapStrError(int error);

	/**
	 * Return the major version number of the pcap library used to write the savefile.
	 * @param pcap pcap instance.
	 * @return major version number of the pcap library used to write the savefile.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapMajorVersion(Pcap pcap) throws PcapCloseException;

	/**
	 * Return the minor version number of the pcap library used to write the savefile.
	 * @param pcap pcap instance.
	 * @return minor version number of the pcap library used to write the savefile.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapMinorVersion(Pcap pcap) throws PcapCloseException;

	/**
	 * Translates a data link type value to the corresponding data link type name.
	 * NULL is returned on failure.
	 * @param dtl link type.
	 * @return null is returned if the type value does not correspond to a known datalink value.
	 * @since 1.1.4
	 */
	public static native String PcapDataLinkValToName(int dtl);

	/**
	 * Translates a data link type value to a short description of that data link type.
	 * NULL is returned on failure.
	 * @param dtl link type.
	 * @return null is returned if the type value does not correspond to a known datalink value.
	 * @since 1.1.4
	 */
	public static native String PcapDataLinkValToDescription(int dtl);

	/**
	 * Translates a data link type name, which is a DLT_ name with the DLT_ removed,
	 * to the corresponding data link type value. The translation is case-insensitive.
	 * -1 is returned on failure.
	 * @param name link type name.
	 * @return PcapDataLinkNameToVal() returns the type value on success and -1 if the name is not a known type name.
	 * @since 1.1.4
	 */
	public static native int PcapDataLinkNameToVal(String name);

	/**
	 * Switch between blocking and nonblocking mode.
	 * @param pcap pcap instance.
	 * @param nonblock 1 to set non block.
	 * @param errbuf error buffer.
	 * @return returns the current non-blocking state of the capture descriptor;
	 * it always returns 0 on savefiles. If there is an error, -1 is returned and
	 * errbuf is filled in with an appropriate error message.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapSetNonBlock(Pcap pcap, int nonblock, StringBuilder errbuf) throws PcapCloseException;

	/**
	 * Get the "non-blocking" state of an interface.
	 * @param pcap pcap instance.
	 * @param errbuf error buffer.
	 * @return PcapGetNonBlock() returns the current non-blocking state of the
	 * capture  descriptor; it always returns 0 on savefiles.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapGetNonBlock(Pcap pcap, StringBuilder errbuf) throws PcapCloseException;

	/**
	 * Create a pcap instance without starting a capture.
	 * @param linktype link type.
	 * @param snaplen snapshot length.
	 * @return null on error.
	 * @since 1.1.4
	 */
	public static native Pcap PcapOpenDead(int linktype, int snaplen);

	/**
	 * Return the file position for a savefile.
	 * @param pcapDumper pcap dumper object.
	 * @return file position for a savefile.
	 * @throws PcapDumperCloseException pcap dumper close exception.
	 * @since 1.1.4
	 */
	public static native long PcapDumpFTell(PcapDumper pcapDumper) throws PcapDumperCloseException;

	/**
	 * Free a filter.
	 * @param bpfProgram compiled bpf.
	 * @throws BpfProgramCloseException bpf program close exception.
	 * @since 1.1.4
	 */
	public static native void PcapFreeCode(BpfProgram bpfProgram) throws BpfProgramCloseException;

	/**
	 * Return statistics on current capture.
	 * @param pcap pcap instance.
	 * @param pcapStat pcap stat object.
	 * @return PcapStats() returns 0 on success and returns -1 if there is an error
	 * or if pcap handle doesn't support packet statistics.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapStats(Pcap pcap, PcapStat pcapStat) throws PcapCloseException;

	/**
	 * Compile a packet filter without the need of opening an adapter.
	 * This function converts an high level filtering expression (see Filtering expression syntax)
	 * in a program that can be interpreted by the kernel-level filtering engine.
	 * @param snaplenArg snapshot length.
	 * @param linktypeArg link type.
	 * @param program bpf.
	 * @param buf str.
	 * @param optimize optiomize (0/1).
	 * @param mask netmask.
	 * @return -1 on error.
	 * @throws BpfProgramCloseException bpf program close exception.
	 * @since 1.1.4
	 */
	public static native int PcapCompileNoPcap(int snaplenArg, int linktypeArg, BpfProgram program, String buf, int optimize, int mask)
			throws BpfProgramCloseException;

	/**
	 * Print the text of the last pcap library error on stderr, prefixed by prefix.
	 * @param pcap pcap instance.
	 * @param prefix prefix.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native void PcapPError(Pcap pcap, String prefix) throws PcapCloseException;

	/**
	 * Is used to create a packet capture handle to look at packets on the network.
	 * Source is a string that specifies the network device to open;
	 * on Linux systems with 2.2 or later kernels, a source argument of "any" or NULL
	 * can be used to capture packets from all interfaces.
	 *
	 * <p>The returned handle must be activated with PcapActivate() before packets can be captured with it;
	 * options for the capture, such as promiscuous mode, can be set on the handle before activating it.</p>
	 * @param source network device.
	 * @param errbuf errof buffer.
	 * @return returns a pcap instance on success and NULL on failure. If NULL is returned, errbuf is filled in with an appropriate error message.
	 * @since 1.1.4
	 */
	public static native Pcap PcapCreate(String source, StringBuilder errbuf);

	/**
	 * Sets the snapshot length to be used on a capture handle when the handle is activated to snaplen.
	 * @param pcap pcap instance.
	 * @param snaplen snaplen.
	 * @return returns 0 on success or PCAP_ERROR_ACTIVATED(-4) if called on a capture handle that has been activated.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapSetSnaplen(Pcap pcap, int snaplen) throws PcapCloseException;

	/**
	 * Sets whether promiscuous mode should be set on a capture handle when the handle is activated.
	 * If promisc is non-zero, promiscuous mode will be set, otherwise it will not be set.
	 * @param pcap pcap instance.
	 * @param promisc promisc.
	 * @return pcapSetPromisc() returns 0 on success or PCAP_ERROR_ACTIVATED(-3) if
	 * called on a capture handle that has been activated.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapSetPromisc(Pcap pcap, int promisc) throws PcapCloseException;

	/**
	 * Sets the packet buffer timeout that will be used on a capture handle when the handle is activated to to_ms,
	 * which is in units of milliseconds.
	 * @param pcap pcap instance.
	 * @param timeout timeout.
	 * @return PcapSetTimeout() returns 0 on success or PCAP_ERROR_ACTIVATED(-3) if
	 * called on a capture handle that has been activated.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapSetTimeout(Pcap pcap, int timeout) throws PcapCloseException;

	/**
	 * Sets the buffer size that will be used on a capture handle when the handle is activated to buffer_size, which is in units of bytes.
	 * @param pcap pcap instance.
	 * @param bufferSize buffer size.
	 * @return PcapSetBufferSize() returns 0 on success or PCAP_ERROR_ACTIVATED(-3) if
	 * called on a capture handle that has been activated.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapSetBufferSize(Pcap pcap, int bufferSize) throws PcapCloseException;

	/**
	 * Checks whether monitor mode could be set on a capture handle when the handle is activated.
	 * @param pcap pcap instance.
	 * @return PcapCanSetRfMon() returns 0 if monitor mode could not be set, 1 if
	 * monitor mode could be set, and a negative value on error. A negative
	 * return value indicates what error condition occurred. The possible
	 * error values are:
	 * PCAP_ERROR_NO_SUCH_DEVICE(-5): The capture source specified when the handle was created doesn' exist.
	 * PCAP_ERROR_PERM_DENIED(-8): The  process  doesn't  have  permission to check whether monitor mode could be supported.
	 * PCAP_ERROR_ACTIVATED(-4): The capture handle has already been activated.
	 * PCAP_ERROR(-1): Generic error.
	 * @throws PcapCloseException pcap close exception.
	 * @throws NativeException native exception (pcap handle is dead).
	 * @since 1.1.4
	 */
	public static native int PcapCanSetRfMon(Pcap pcap) throws PcapCloseException, NativeException;

	/**
	 * Sets whether monitor mode should be set on a capture handle when the handle is activated.
	 * If rfmon is non-zero, monitor mode will be set, otherwise it will not be set.
	 * @param pcap pcap instance.
	 * @param rfmon 1 (true) or 0 (false).
	 * @return PcapSetRfMon() returns 0 on success or PCAP_ERROR_ACTIVATED(-4) if called
	 * on a capture handle that has been activated.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapSetRfMon(Pcap pcap, int rfmon) throws PcapCloseException;

	/**
	 * Sets whether immediate mode should be set on a capture handle when the handle is activated. If immediate_mode is non-zero,
	 * immediate mode will be set, otherwise it will not be set.
	 * @param pcap pcap instance.
	 * @param immediateMode immediate_mode.
	 * @return PcapSetImmediateMode() returns 0 on success or  PCAP_ERROR_ACTIVATED(-4)
	 * if called on a capture handle that has been activated.
	 * @throws PcapCloseException pcap close exception.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 * @since 1.1.4
	 */
	public static native int PcapSetImmediateMode(Pcap pcap, int immediateMode) throws PcapCloseException, PlatformNotSupportedException;

	/**
	 * Is used to activate a packet capture handle to look at packets on the network,
	 * with the options that were set on the handle being in effect.
	 * @param pcap pcap instance.
	 * @return 0 on success without warnings, a non-zero positive value on success with warnings, and a negative value on error.
	 * A non-zero  return  value  indicates  what  warning  or  error  condition occurred.
	 * The possible warning values are:
	 * PCAP_WARNING_PROMISC_NOTSUP(2): Promiscuous mode was requested, but the capture source doesn't support promiscuous mode.
	 * PCAP_WARNING_TSTAMP_TYPE_NOTSUP(3): The time stamp type specified in a previous PcapSetTStampType() call isn't supported
	 * by the capture source (the time stamp type is left as the default).
	 * PCAP_WARNING(1): Generic waring.
	 * The possible error values are:
	 * PCAP_ERROR_ACTIVATED(-4): The handle has already been activated.
	 * PCAP_ERROR_NO_SUCH_DEVICE(-5): The capture source specified when the handle was created doesn't exist.
	 * PCAP_ERROR_PERM_DENIED(-8): The process doesn't have permission to open the capture source.
	 * PCAP_ERROR_PROMISC_PERM_DENIED(-11): The process has permission to open the capture source but doesn't have
	 * spermission to put it into promiscuous mode.
	 * PCAP_ERROR_RFMON_NOTSUP(-6): Monitor mode was specified but the capture source doesn't support monitor mode.
	 * PCAP_ERROR_IFACE_NOT_UP(-9): The capture source device is not up.
	 * PCAP_ERROR(-1): Generic error.
	 * @throws PcapCloseException pcap close exception.
	 * @since 1.1.4
	 */
	public static native int PcapActivate(Pcap pcap) throws PcapCloseException;

	/**
	 * Used to specify a direction that packets will be
	 * captured. Direction is one of the constants PCAP_D_IN, PCAP_D_OUT or
	 * PCAP_D_INOUT. PCAP_D_IN will only capture packets received by the
	 * device, PCAP_D_OUT will only capture packets sent by the device and
	 * PCAP_D_INOUT will capture packets received by or sent by the device.
	 * PCAP_D_INOUT is the default setting if this function is not called.
	 *
	 * <p>PcapSetdirection() isn't necessarily fully supported on all platforms;
	 * some platforms might return an error for all  values,  and  some  other
	 * platforms might not support PCAP_D_OUT.</p>
	 *
	 * <p>This operation is not supported if a savefile is being read.</p>
	 * @param pcap pcap instance.
	 * @param direction direction.
	 * @return returns  0 on success and -1 on failure (not supported by operating system).
	 * @throws PcapCloseException pcap close exception.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 * @since 1.1.4
	 */
	public static native int PcapSetDirection(Pcap pcap, PcapDirection direction) throws PcapCloseException, PlatformNotSupportedException;

	/**
	 * Set the time stamp precision returned in captures.
	 * @param pcap pcap.
	 * @param tstampPrecision time stamp precision.
	 * @return 0 on success if specified time stamp precision is expected to be supported by operating system.
	 * @throws PcapCloseException pcap close exception.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native int PcapSetTStampPrecision(Pcap pcap, int tstampPrecision) throws PcapCloseException, PlatformNotSupportedException;

	/**
	 * Set the time stamp type returned in captures.
	 * @param pcap pcap.
	 * @param type time stamp type.
	 * @return 0 on success if specified time type is expected to be supported by operating system.
	 * @throws PcapCloseException pcap close exception.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native int PcapSetTStampType(Pcap pcap, int type) throws PcapCloseException, PlatformNotSupportedException;

	/**
	 * Get the time stamp precision returned in captures.
	 * @param pcap pcap instance.
	 * @return the precision of the time stamp precision returned in packet captures on the pcap descriptor.
	 * @throws PcapCloseException pcap close exception.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native int PcapGetTStampPrecision(Pcap pcap) throws PcapCloseException, PlatformNotSupportedException;

	/**
	 * Get list of datalinks.
	 * @param pcap pcap instance.
	 * @param dtlBuffer datalinks.
	 * @return list of datalinks.
	 * @throws PcapCloseException pcap close exception.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native int PcapListDataLinks(Pcap pcap, List<Integer> dtlBuffer) throws PcapCloseException, PlatformNotSupportedException;

	/**
	 * Get link of time stamp types.
	 * @param pcap pcap instance.
	 * @param tstampTypesp time stamp types.
	 * @return time stamp types.
	 * @throws PcapCloseException pcap close exception.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native int PcapListTStampTypes(Pcap pcap, List<Integer> tstampTypesp) throws PcapCloseException, PlatformNotSupportedException;

	/**
	 * Translates a time stamp type name to the corresponding time stamp type value. The translation is case-insensitive.
	 * @param name time stamp name.
	 * @return returns time stamp type value on success and PCAP_ERROR on failure.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native int PcapTStampTypeNameToVal(String name) throws PlatformNotSupportedException;

	/**
	 * Translates a time stamp type value to the corresponding time stamp type name.
	 * @param tstampType time stamp type.
	 * @return NULL is returned on failure.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native String PcapTStampTypeValToName(int tstampType) throws PlatformNotSupportedException;

	/**
	 * Translates a time stamp type value to a short description of that time stamp type.
	 * @param tstampType time stamp type.
	 * @return NULL is returned on failure.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native String PcapTStampTypeValToDescription(int tstampType) throws PlatformNotSupportedException;

	/**
	 * Converts a status code value returned by a libpcap routine to an error string.
	 * @param errnum statuc code number.
	 * @return status string.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native String PcapStatusToStr(int errnum) throws PlatformNotSupportedException;

	/**
	 * Used for creating a Pcap instance to use when calling the other functions.
	 * It is typically used when just using for compiling BPF  code;
	 * it can also be used if using pcap_dump_open(), pcap_dump(),
	 * and PcapDumpClose() to write a savefile if there is  no  Pcap  that
	 * supplies the packets to be written.
	 * @param linktype specifies the link-layer type
	 * @param snaplen specifies the snapshot length.
	 * @param precision specifies the time stamp precision.
	 * @return null on fail.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native Pcap PcapOpenDeadWithTStampPrecision(int linktype, int snaplen, int precision) throws PlatformNotSupportedException;

	/**
	 * Given a BPF program, a PcapPktHdr structure for a packet, and the raw
	 * data for the packet, check whether the packet passes the filter.
	 * Returns the return value of the filter program, which will be zero if
	 * the packet doesn't pass and non-zero if the packet does pass.
	 * @param fp bpfProgram
	 * @param h pktHdr
	 * @param pkt buffer.
	 * @return 0 on success.
	 */
	public static native int PcapOfflineFilter(BpfProgram fp, PcapPktHdr h, ByteBuffer pkt);

	/**
	 * Called to open a savefile for reading pcap file.
	 * @param fname specifies the file name.
	 * @param tstampPrecision specifies the time stamp precision.
	 * @param errbuf specifies errror buffer.
	 * @return pcap instance.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native Pcap PcapOpenOfflineWithTStampPrecision(String fname, int tstampPrecision, StringBuilder errbuf)
			throws PlatformNotSupportedException;

	/**
	 * Sends a raw packet through the network interface; buf points to the data of the packet,
	 * including the link-layer header, and size is the number of bytes in the packet.
	 * @param pcap pcap instance.
	 * @param buf packet buffer.
	 * @param size packet size.
	 * @return PcapInject returns the number of bytes written on success and -1 on failure.
	 * @throws PcapCloseException pcap close exception.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 */
	public static native int PcapInject(Pcap pcap, ByteBuffer buf, int size) throws PcapCloseException, PlatformNotSupportedException;

	/**
	 * Check if pcap handle is alredy activated or no.
	 * @param pcap pcap instance.
	 * @return returns 0 if pcap handle not active yet.
	 */
	public static native int PcapCheckActivated(Pcap pcap);


	/**
	 * Non pcap functions
	 */

	/**
	 * Find hardware address; Only for windows.
	 * @param source source / adapter name.
	 * @return returns bytes address.
	 * @throws PlatformNotSupportedException platform not supported exception.
	 * @throws DeviceNotFoundException device not found exception.
	 */
	public static native byte[] FindHardwareAddress(String source) throws PlatformNotSupportedException, DeviceNotFoundException;

	/**
	 * Initialize default IDs.
	 */
	private static native void initIDs();

	static {
		if (!loaded) {
			new DefaultLibraryLoader().load(new Callback<Void>() {

				@Override
				public void onSuccess(Void value) {
					initIDs();
					loaded = true;
				}

				@Override
				public void onFailure(Throwable throwable) {
					LOGGER.error("Jxnet Native library failed to load load: {}.", throwable);
				}

			}, new Class<?>[] {
					PcapIf.class, PcapAddr.class, SockAddr.class, Pcap.class, BpfProgram.class,
					List.class, StringBuilder.class
			});
		}
	}

}
