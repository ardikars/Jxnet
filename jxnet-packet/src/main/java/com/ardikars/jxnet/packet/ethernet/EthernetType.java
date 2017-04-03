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

package com.ardikars.jxnet.packet.ethernet;

import com.ardikars.jxnet.util.NamedNumber;

import java.util.HashMap;
import java.util.Map;

/**
 * @author Ardika Rommy Sanjaya
 * @since 1.1.0
 */
public final class EthernetType extends NamedNumber<Short, EthernetType> {

    /**
     * MTU Size (1500)
     */
    public static final int IEEE802_3_MAX_LENGTH = 1500;

    /**
     * IPv4: 0x0800
     */
    public static final EthernetType IPV4
            = new EthernetType((short) 0x0800, "IPv4");

    /**
     * ARP: 0x0806
     */
    public static final EthernetType ARP
            = new EthernetType((short) 0x0806, "ARP");

    /**
     * IEEE 802.1Q VLAN-tagged frames: 0x8100
     */
    public static final EthernetType DOT1Q_VLAN_TAGGED_FRAMES
            = new EthernetType((short) 0x8100, "IEEE 802.1Q VLAN-tagged frames");

    /**
     * RARP: 0x8035
     */
    public static final EthernetType RARP
            = new EthernetType((short) 0x8035, "RARP");

    /**
     * Appletalk: 0x809b
     */
    public static final EthernetType APPLETALK
            = new EthernetType((short) 0x809b, "Appletalk");

    /**
     * IPv6: 0x86dd
     */
    public static final EthernetType IPV6
            = new EthernetType((short) 0x86dd, "IPv6");

    /**
     * PPP: 0x880b
     */
    public static final EthernetType PPP
            = new EthernetType((short) 0x880b, "PPP");

    /**
     * MPLS: 0x8847
     */
    public static final EthernetType MPLS
            = new EthernetType((short) 0x8847, "MPLS");

    /**
     * PPPoED Discovery Stage: 0x8863
     */
    public static final EthernetType PPPOE_DISCOVERY_STAGE
            = new EthernetType((short) 0x8863, "PPPoED Discovery Stage");

    /**
     * PPPoED Session Stage: 0x8864
     */
    public static final EthernetType PPPOE_SESSION_STAGE
            = new EthernetType((short) 0x8864, "PPPoED Session Stage");

    public static final EthernetType UNKNOWN
            = new EthernetType((short) -1, "Unknown");

    private static final Map<Short, EthernetType> registry
            = new HashMap<Short, EthernetType>();

    static {
        registry.put(IPV4.getValue(), IPV4);
        registry.put(ARP.getValue(), ARP);
        registry.put(DOT1Q_VLAN_TAGGED_FRAMES.getValue(), DOT1Q_VLAN_TAGGED_FRAMES);
        registry.put(RARP.getValue(), RARP);
        registry.put(APPLETALK.getValue(), APPLETALK);
        registry.put(IPV6.getValue(), IPV6);
        registry.put(PPP.getValue(), PPP);
        registry.put(MPLS.getValue(), MPLS);
        registry.put(PPPOE_DISCOVERY_STAGE.getValue(), PPPOE_DISCOVERY_STAGE);
        registry.put(PPPOE_SESSION_STAGE.getValue(), PPPOE_SESSION_STAGE);
    }

    /**
     * @param value value
     * @param name  name
     */
    public EthernetType(Short value, String name) {
        super(value, name);
    }

    /**
     * @param value value
     * @return a EthernetType object.
     */
    public static EthernetType getInstance(final Short value) {
        if (registry.containsKey(value)) {
            return registry.get(value);
        } else if ((value & 0xFFFF) <= IEEE802_3_MAX_LENGTH) {
            return new EthernetType(value, "-");
        } else {
            return UNKNOWN;
        }
    }

    /**
     * @param type type
     * @return a EthernetType object.
     */
    public static EthernetType register(EthernetType type) {
        return registry.put(type.getValue(), type);
    }

    @Override
    public String toString() {
        if ((super.getValue() & 0xFFFF) <= IEEE802_3_MAX_LENGTH) {
            StringBuilder sb = new StringBuilder(70);
            return sb.append("[Value: ")
                    .append(super.getValue() & 0xffff)
                    .append(", Name: ")
                    .append(super.getName())
                    .append("]")
                    .toString();
        } else {
            return super.toString();
        }
    }

}
