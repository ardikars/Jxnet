/**
 * Copyright (C) 2017-2018  Ardika Rommy Sanjaya <contact@ardikars.com>
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

import com.ardikars.jxnet.exception.NativeException;
import com.ardikars.jxnet.util.Validate;

/**
 * @author Ardika Rommy Sanjaya
 * @since 1.0.0
 */
public final class BpfProgram implements PointerHandler {

	/**
	 * Bpf compile mode
	 */
	public enum BpfCompileMode {

		OPTIMIZE(1), NON_OPTIMIZE(0);

		private final int value;

		BpfCompileMode(final int value) {
			this.value = value;
		}

		public int getValue() {
			return this.value;
		}

	}
	
	private native void initBpfProgram();

	private long address;

	/**
	 * Create instance of BpfProgram and initialize it.
	 */
	public BpfProgram() {
		this.initBpfProgram();
	}

	/**
	 * Create instance of BpfProgram and initialize it.
	 * @param builder filter builder.
	 * @return BpfProgram (filter handle).
	 */
	public static BpfProgram newInstance(Builder builder) {
		return builder.build();
	}

	/**
	 * Bpf program builder.
	 * @return BpfProgram instance.
	 */
	public static Builder builder() {
		return new Builder();
	}

	/**
	 * Get Bpf Program pointer address.
	 * @return pointer address.
	 */
	@Override
	public long getAddress() {
		synchronized (this) {
			return this.address;
		}
	}

	/**
	 * Check bpf handle.
	 * @return true if closed.
	 */
	public boolean isClosed() {
		if (this.getAddress() == 0) {
			return true;
		}
		return false;
	}

	@Override
	public boolean equals(final Object o) {
		if (this == o) {
			return true;
		}
		if (o == null || getClass() != o.getClass()) {
			return false;
		}

		final BpfProgram that = (BpfProgram) o;

		return this.getAddress() == that.getAddress();
	}

	@Override
	public int hashCode() {
		return (int) (this.getAddress() ^ (this.getAddress() >>> 32));
	}

	@Override
	protected Object clone() throws CloneNotSupportedException {
		BpfProgram bpfProgram = (BpfProgram) super.clone();
		return bpfProgram;
	}

	@Override
	public String toString() {
		final StringBuilder sb = new StringBuilder("BpfProgram{");
		sb.append("address=").append(this.getAddress());
		sb.append('}');
		return sb.toString();
	}

	/**
	 * Builder class for BpfProgram.
	 */
	public static final class Builder implements com.ardikars.jxnet.common.Builder<BpfProgram> {

		private Pcap pcap;
		private String filter;
		private Inet4Address netmask;
		private BpfCompileMode bpfCompileMode;

		/**
		 * Pcap handle.
		 * @param pcap pcap handle.
		 * @return Builder.
		 */
		public Builder pcap(final Pcap pcap) {
			this.pcap = pcap;
			return this;
		}

		/**
		 * Filter pattern.
		 * @param filter filter pattern.
		 * @return Builder.
		 */
		public Builder filter(final String filter) {
			this.filter = filter;
			return this;
		}

		/**
		 * Inet4Address netmask.
		 * @param netmask netmask.
		 * @return Builder.
		 */
		public Builder netmask(final Inet4Address netmask) {
			this.netmask = netmask;
			return this;
		}

		/**
		 * Bpf compile mode.
		 * @param bpfCompileMode bpf compile mode.
		 * @return Builder.
		 */
		public Builder bpfCompileMode(final BpfCompileMode bpfCompileMode) {
			this.bpfCompileMode = bpfCompileMode;
			return this;
		}

		@Override
		public BpfProgram build() {
			Validate.nullPointer(pcap);
			Validate.illegalArgument(!pcap.isClosed(), new IllegalArgumentException("Pcap handle is closed."));
			Validate.illegalArgument(filter != null && !filter.equals(""));
			Validate.nullPointer(netmask);
			Validate.nullPointer(bpfCompileMode);

			BpfProgram bpfProgram = new BpfProgram();
			if (Jxnet.PcapCompile(pcap, bpfProgram, filter, bpfCompileMode.getValue(), netmask.toInt()) != Jxnet.OK) {
				throw new NativeException();
			}
			if (Jxnet.PcapSetFilter(pcap, bpfProgram) != Jxnet.OK) {
				throw new NativeException();
			}
			return bpfProgram;
		}

	}

	static {
		try {
			Class.forName("com.ardikars.jxnet.Jxnet");
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
	}

}
