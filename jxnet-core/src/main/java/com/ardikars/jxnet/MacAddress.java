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

import com.ardikars.jxnet.exception.DeviceNotFoundException;
import com.ardikars.jxnet.util.Validate;

import java.util.Arrays;
import java.util.regex.Pattern;

/**
 * This class represents an Media Access Control (MAC) address.
 * @author Ardika Rommy Sanjaya
 * @since 1.0.0
 */
public final class MacAddress implements Cloneable {

	/**
	 * MAC Address Length.
	 */
	public static final int MAC_ADDRESS_LENGTH = 6;

	/**
	 * Zero MAC Address (00:00:00:00:00:00).
	 */
	public static final MacAddress ZERO = valueOf("00:00:00:00:00:00");

	/**
	 * Dummy MAC Address (de:ad:be:ef:c0:fe).
	 */
	public static final MacAddress DUMMY = valueOf("de:ad:be:ef:c0:fe");

	/**
	 * Broadcast MAC Address (ff:ff:ff:ff:ff:ff).
	 */
	public static final MacAddress BROADCAST = valueOf("ff:ff:ff:ff:ff:ff");

	/**
	 * Multicast Address.
	 */
	public static final MacAddress IPV4_MULTICAST = valueOf("01:00:5e:00:00:00");

	public static final MacAddress IPV4_MULTICAST_MASK = valueOf("ff:ff:ff:80:00:00");
	
	private byte[] address = new byte[MAC_ADDRESS_LENGTH];

	private MacAddress() {

	}
	
	private MacAddress(final byte[] address) {
		Validate.nullPointer(address);
		Validate.illegalArgument(address.length == MAC_ADDRESS_LENGTH);
		System.arraycopy(address, 0, this.address, 0, address.length);
	}

	/**
	 * Create MacAddress instance from NIC name.
	 * @param nicName NIC name.
	 * @return MacAddress instance.
	 * @throws DeviceNotFoundException device not found exception.
	 */
	public static native MacAddress fromNicName(final String nicName) throws DeviceNotFoundException;

	/**
	 * Create MacAddress instance.
	 * @param address string MAC Address.
	 * @return returns MacAddress instance.
	 */
	public static MacAddress valueOf(String address) {
		address = Validate.nullPointer(address, "00:00:00:00:00:00");
		final String[] elements = address.split(":|-");
		Validate.illegalArgument(elements.length == MAC_ADDRESS_LENGTH);
		final byte[] b = new byte[MAC_ADDRESS_LENGTH];
		for (int i = 0; i < MAC_ADDRESS_LENGTH; i++) {
			final String element = elements[i];
			b[i] = (byte) Integer.parseInt(element, 16);
		}
		return new MacAddress(b);
	}

	/**
	 * Create MacAddress instance.
	 * @param address bytes MAC Address.
	 * @return returns MacAddress instance.
	 */
	public static MacAddress valueOf(final byte[] address) {
		return new MacAddress(address);
	}

	/**
	 * Create MacAddress instance.
	 * @param address long MAC Address.
	 * @return returns MacAddress instance.
	 */
	public static MacAddress valueOf(final long address) {
		final byte[] bytes = new byte[] {
				(byte) (address >> 40 & 0xff),
				(byte) (address >> 32 & 0xff),
				(byte) (address >> 24 & 0xff),
				(byte) (address >> 16 & 0xff),
				(byte) (address >> 8 & 0xff),
				(byte) (address >> 0 & 0xff)};
		return new MacAddress(bytes);
	}

	/**
	 * Validate Mac Address.
	 * @param address string address.
	 * @return returns true is valid, false otherwise.
	 */
	public static boolean isValidAddress(final String address) {
		Validate.nullPointer(address);
		return Pattern.matches("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$", address);
	}

	/**
	 * Change value of MacAddress.
	 * @param macAddress MacAddress.
	 */
	public void update(final MacAddress macAddress) {
		Validate.nullPointer(macAddress);
		this.address = macAddress.toBytes();
	}

	/**
	 * Getting length of MAC Address.
	 * @return returns MAC Address length.
	 */
	public int length() {
		return this.address.length;
	}

	/**
	 * Getting MAC Address as bytes.
	 * @return returns MAC Address.
	 */
	public byte[] toBytes() {
		return Arrays.copyOf(this.address, this.address.length);
	}

	/**
	 * Getting MAC Address as long.
	 * @return returns MAC Address.
	 */
	public long toLong() {
		long addr = 0;
		for (int i = 0; i < MAC_ADDRESS_LENGTH; i++) {
			final long tmp = (this.address[i] & 0xffL) << (5 - i) * 8;
			addr |= tmp;
		}
		return addr;
	}

	/**
	 * Returns true if broadcast MAC Address.
	 * @return returns true if broadcast MAC Address, false otherwise.
	 */
	public boolean isBroadcast() {
		for (final byte b : this.address) {
			if (b != -1) {
				return false;
			}
		}
		return true;
	}

	/**
	 * Returns true if multicast MAC Address.
	 * @return returns true if multicast MAC Address, false otherwise.
	 */
	public boolean isMulticast() {
		if (this.isBroadcast()) {
			return false;
		}
		return (this.address[0] & 0x01) != 0;
	}

	@Override
	public boolean equals(final Object o) {
		if (this == o) {
			return true;
		}
		if (o == null || getClass() != o.getClass()) {
			return false;
		}

		final MacAddress that = (MacAddress) o;

		return Arrays.equals(this.address, that.address);
	}

	@Override
	public int hashCode() {
		return Arrays.hashCode(this.address);
	}

	@Override
	protected Object clone() throws CloneNotSupportedException {
		MacAddress macAddress = (MacAddress) super.clone();
		return macAddress;
	}

	@Override
	public String toString() {
		final StringBuilder sb = new StringBuilder();
		for (final byte b : this.address) {
			if (sb.length() > 0) {
				sb.append(':');
			}
			final String hex = Integer.toHexString(b & 0xff);
			sb.append(hex.length() == 1 ? "0" + hex : hex);
		}
		return sb.toString();
	}

	static {
		try {
			Class.forName("com.ardikars.jxnet.Jxnet");
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
		}
	}

}
