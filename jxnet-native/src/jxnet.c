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

#include "../include/jxnet/com_ardikars_jxnet_Jxnet.h"
#if !defined(WIN32) && !defined(_WIN64)
#include "../include/jxnet/pcap/pcap-int.h"
#else
#include "../include/jxnet/pcap/pcap-int.h"
#include <winsock2.h>
#include <iphlpapi.h>
#endif

#include <pcap.h>
#include <string.h>

#include "ids.h"
#include "utils.h"
#include "preconditions.h"

#if !defined(WIN32) && !defined(_WIN64)
#include <sys/socket.h>
#endif

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapFindAllDevs
 * Signature: (Ljava/util/List;Ljava/lang/StringBuilder;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapFindAllDevs
		(JNIEnv *env, jclass jcls, jobject jlist_pcap_if, jobject jerrbuf) {

    UNUSED(jcls);

	if (CheckNotNull(env, jlist_pcap_if, NULL) == NULL) return -1;
	if (CheckNotNull(env, jerrbuf, NULL) == NULL) return -1;

	pcap_if_t *alldevsp = NULL;
	char errbuf[PCAP_ERRBUF_SIZE];
	int r = -1;
	errbuf[0] = '\0';

	r = pcap_findalldevs(&alldevsp, errbuf);

	if (r != 0 || alldevsp == NULL) {
		SetStringBuilder(env, jerrbuf, errbuf);
		return (jint) r;
	}

	jobject pcap_if = NULL;
	jobject pcap_addr = NULL;
	jobject list_pcap_addr = NULL;
	pcap_if_t *dev = alldevsp;
	pcap_addr_t *addr = NULL;

	while(dev != NULL) {

		pcap_if = NewObject(env, "com/ardikars/jxnet/PcapIf", "<init>", "()V");

		if (dev->name != NULL) {
			(*env)->SetObjectField(env, pcap_if, PcapIfNameFID,
								   (*env)->NewStringUTF(env, dev->name));
		} else {
			(*env)->SetObjectField(env, pcap_if, PcapIfNameFID, NULL);
		}

		if (dev->description != NULL) {
			(*env)->SetObjectField(env, pcap_if, PcapIfDescriptionFID,
								   (*env)->NewStringUTF(env, dev->description));
		} else {
			(*env)->SetObjectField(env, pcap_if, PcapIfDescriptionFID, NULL);
		}

		list_pcap_addr = (*env)->GetObjectField(env, pcap_if, PcapIfAddressesFID);
		addr = dev->addresses;

		while (addr != NULL) {

			pcap_addr = NewObject(env, "com/ardikars/jxnet/PcapAddr", "<init>", "()V");

			if (addr->addr != NULL) {
				(*env)->SetObjectField(env, pcap_addr, PcapAddrAddrFID,
									   NewSockAddr(env, addr->addr));
			} else {
				(*env)->SetObjectField(env, pcap_addr, PcapAddrAddrFID,
									   NewSockAddr(env, NULL));
			}

			if (addr->netmask != NULL) {
				(*env)->SetObjectField(env, pcap_addr, PcapAddrNetmaskFID,
									   NewSockAddr(env, addr->netmask));
			} else {
				(*env)->SetObjectField(env, pcap_addr, PcapAddrNetmaskFID,
									   NewSockAddr(env, NULL));
			}

			if (addr->broadaddr != NULL) {
				(*env)->SetObjectField(env, pcap_addr, PcapAddrBroadAddrFID,
									   NewSockAddr(env, addr->broadaddr));
			} else {
				(*env)->SetObjectField(env, pcap_addr, PcapAddrBroadAddrFID,
									   NewSockAddr(env, NULL));
			}

			if (addr->dstaddr != NULL) {
				(*env)->SetObjectField(env, pcap_addr, PcapAddrDstAddrFID,
									   NewSockAddr(env, addr->dstaddr));
			} else {
				(*env)->SetObjectField(env, pcap_addr, PcapAddrDstAddrFID,
									   NewSockAddr(env, NULL));
			}

			if ((*env)->CallBooleanMethod(env, list_pcap_addr, ListAddMID,
										  pcap_addr) == JNI_FALSE) {
				(*env)->DeleteLocalRef(env, pcap_addr);
				return (jint) -1;
			}

			(*env)->DeleteLocalRef(env, pcap_addr);
			addr = addr->next;

		}

		(*env)->SetIntField(env, pcap_if, PcapIfFlagsFID, (jint) dev->flags);

		if ((*env)->CallBooleanMethod(env, jlist_pcap_if, ListAddMID, pcap_if) == JNI_FALSE) {
			(*env)->DeleteLocalRef(env, pcap_if);
			return (jint) -1;
		}

		(*env)->DeleteLocalRef(env, pcap_if);
		(*env)->DeleteLocalRef(env, list_pcap_addr);
		dev = dev->next;
	}
	pcap_freealldevs(alldevsp);
	return (jint) r;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapOpenLive
 * Signature: (Ljava/lang/String;IIILjava/lang/StringBuilder;)Lcom/ardikars/jxnet/Pcap;
 */
JNIEXPORT jobject JNICALL Java_com_ardikars_jxnet_Jxnet_PcapOpenLive
		(JNIEnv *env, jclass jcls, jstring jsource, jint jsnaplen, jint jpromisc,
		 jint jto_ms, jobject jerrbuf) {

    UNUSED(jcls);

	if (CheckNotNull(env, jsource, NULL) == NULL) return NULL;
	if (!CheckArgument(env, (jsnaplen > 0 && jsnaplen <= MAXIMUM_SNAPLEN &&
							 (jpromisc == 0 || jpromisc == 1) && jto_ms > 0), NULL)) return NULL;
	if (CheckNotNull(env, jerrbuf, NULL) == NULL) return NULL;

	char errbuf[PCAP_ERRBUF_SIZE];
	errbuf[0] = '\0';
	const char *source = (*env)->GetStringUTFChars(env, jsource, 0);

	pcap_t *pcap = pcap_open_live(source, (int) jsnaplen, (int) jpromisc, (int) jto_ms, errbuf);
	(*env)->ReleaseStringUTFChars(env, jsource, source);

	if (pcap == NULL) {
		SetStringBuilder(env, jerrbuf, errbuf);
		return NULL;
	}
	return SetPcap(env, pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapLoop
 * Signature: (Lcom/ardikars/jxnet/Pcap;ILcom/ardikars/jxnet/PcapHandler;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapLoop
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jcnt, jobject jcallback, jobject juser) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jcallback, NULL) == NULL) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return -1;
	}

	pcap_user_data_t user_data;
	memset(&user_data, 0, sizeof(user_data));
	user_data.env = env;
	user_data.callback = jcallback;
	user_data.user = juser;
	user_data.PcapHandlerClass = (*env)->GetObjectClass(env, jcallback);
	user_data.PcapHandlerNextPacketMID = (*env)->GetMethodID(env,
															 user_data.PcapHandlerClass, "nextPacket",
															 "(Ljava/lang/Object;Lcom/ardikars/jxnet/PcapPktHdr;Ljava/nio/ByteBuffer;)V");

	return pcap_loop(pcap, (int) jcnt, pcap_callback, (u_char *) &user_data);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapLoop0
 * Signature: (Lcom/ardikars/jxnet/Pcap;ILcom/ardikars/jxnet/RawPcapHandler;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapLoop0
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jcnt, jobject jcallback, jobject juser) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jcallback, NULL) == NULL) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return -1;
	}

	pcap_user_data_t user_data;
	memset(&user_data, 0, sizeof(user_data));
	user_data.env = env;
	user_data.callback = jcallback;
	user_data.user = juser;
	user_data.PcapHandlerClass = (*env)->GetObjectClass(env, jcallback);
	user_data.PcapHandlerNextPacketMID = (*env)->GetMethodID(env,
															 user_data.PcapHandlerClass, "nextPacket",
															 "(Ljava/lang/Object;IIIJJ)V");

	return pcap_loop(pcap, (int) jcnt, pcap_callback0, (u_char *) &user_data);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDispatch
 * Signature: (Lcom/ardikars/jxnet/Pcap;ILcom/ardikars/jxnet/PcapHandler;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDispatch
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jcnt, jobject jcallback, jobject juser) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jcallback, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jcnt > 0), NULL)) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	pcap_user_data_t user_data;
	memset(&user_data, 0, sizeof(user_data));
	user_data.env = env;
	user_data.callback = jcallback;
	user_data.user = juser;
	user_data.PcapHandlerClass = (*env)->GetObjectClass(env, jcallback);
	user_data.PcapHandlerNextPacketMID = (*env)->GetMethodID(env,
															 user_data.PcapHandlerClass, "nextPacket", "(Ljava/lang/Object;Lcom/ardikars/jxnet/PcapPktHdr;Ljava/nio/ByteBuffer;)V");

	return pcap_dispatch(pcap, (int) jcnt, pcap_callback, (u_char *) &user_data);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDispatch0
 * Signature: (Lcom/ardikars/jxnet/Pcap;ILcom/ardikars/jxnet/RawPcapHandler;Ljava/lang/Object;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDispatch0
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jcnt, jobject jcallback, jobject juser) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jcallback, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jcnt > 0), NULL)) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	pcap_user_data_t user_data;
	memset(&user_data, 0, sizeof(user_data));
	user_data.env = env;
	user_data.callback = jcallback;
	user_data.user = juser;
	user_data.PcapHandlerClass = (*env)->GetObjectClass(env, jcallback);
	user_data.PcapHandlerNextPacketMID = (*env)->GetMethodID(env,
															 user_data.PcapHandlerClass, "nextPacket",
															  "(Ljava/lang/Object;IIIJJ)V");

	return pcap_dispatch(pcap, (int) jcnt, pcap_callback0, (u_char *) &user_data);
}


/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDumpOpen
 * Signature: (Lcom/ardikars/jxnet/Pcap;Ljava/lang/String;)Lcom/ardikars/jxnet/PcapDumper;
 */
JNIEXPORT jobject JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDumpOpen
		(JNIEnv *env, jclass jcls, jobject jpcap, jstring jfname) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return NULL;
	if (CheckNotNull(env, jfname, NULL) == NULL) return NULL;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return NULL;
	}

	const char *fname = (*env)->GetStringUTFChars(env, jfname, 0);
	pcap_dumper_t *pcap_dumper = pcap_dump_open(pcap, fname);
	(*env)->ReleaseStringUTFChars(env, jfname, fname);

	if (pcap_dumper == NULL) {
		ThrowNew(env, PCAP_DUMPER_CLOSE_EXCEPTION, pcap_geterr(pcap));
		return NULL;
	}
	return SetPcapDumper(env, pcap_dumper);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDump
 * Signature: (Lcom/ardikars/jxnet/PcapDumper;Lcom/ardikars/jxnet/PcapPktHdr;Ljava/nio/ByteBuffer;)V
 */
JNIEXPORT void JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDump
		(JNIEnv *env, jclass jcls, jobject jpcap_dumper, jobject jh, jobject jsp) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap_dumper, "") == NULL) return;
	if (CheckNotNull(env, jh, NULL) == NULL) return;
	if (CheckNotNull(env, jsp, NULL) == NULL) return;

	pcap_dumper_t *pcap_dumper = GetPcapDumper(env, jpcap_dumper);

	if (pcap_dumper == NULL) {
		return;
	}

	struct pcap_pkthdr hdr;
	hdr.ts.tv_sec = (int) (*env)->GetIntField(env, jh, PcapPktHdrTvSecFID);
	hdr.ts.tv_usec = (int) (*env)->GetLongField(env, jh, PcapPktHdrTvUsecFID);
	hdr.caplen = (int) (*env)->GetIntField(env, jh, PcapPktHdrCaplenFID);
	hdr.len = (int) (*env)->GetIntField(env, jh, PcapPktHdrLenFID);
	u_char *sp = (u_char *) (*env)->GetDirectBufferAddress(env, jsp);

	pcap_dump((u_char *) pcap_dumper, &hdr, sp);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapOpenOffline
 * Signature: (Ljava/lang/String;Ljava/lang/StringBuilder;)Lcom/ardikars/jxnet/Pcap;
 */
JNIEXPORT jobject JNICALL Java_com_ardikars_jxnet_Jxnet_PcapOpenOffline
		(JNIEnv *env, jclass jcls, jstring jfname, jobject jerrbuf) {

    UNUSED(jcls);

	if (CheckNotNull(env, jfname, NULL) == NULL) return NULL;
	if (CheckNotNull(env, jerrbuf, NULL) == NULL) return NULL;

	char errbuf[PCAP_ERRBUF_SIZE];
	errbuf[0] = '\0';
	const char *fname = (*env)->GetStringUTFChars(env, jfname, 0);

	pcap_t *pcap = pcap_open_offline(fname, errbuf);
	(*env)->ReleaseStringUTFChars(env, jfname, fname);

	if (pcap == NULL) {
		SetStringBuilder(env, jerrbuf, errbuf);
		return NULL;
	}
	return SetPcap(env, pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapCompile
 * Signature: (Lcom/ardikars/jxnet/Pcap;Lcom/ardikars/jxnet/BpfProgram;Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapCompile
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jfp, jstring jstr, jint joptimize, jint jnetmask) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jfp, NULL) == NULL) return -1;
	if (CheckNotNull(env, jstr, NULL) == NULL) return -1;
	if (!CheckArgument(env, (joptimize == 0 || joptimize == 1), NULL)) return -1;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	struct bpf_program *fp = GetBpfProgram(env, jfp);

	if (fp == NULL) {
		return (jint) -1;
	}

	const char *str = (*env)->GetStringUTFChars(env, jstr, 0);

	int r = pcap_compile(pcap, fp, str, (int) joptimize, (bpf_u_int32) jnetmask);

	(*env)->ReleaseStringUTFChars(env, jstr, str);

	return r;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetFilter
 * Signature: (Lcom/ardikars/jxnet/Pcap;Lcom/ardikars/jxnet/BpfProgram;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetFilter
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jfp) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jfp, NULL) == NULL) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	struct bpf_program *fp = GetBpfProgram(env, jfp);

	if (fp == NULL) {
		return (jint) -1;
	}

	return (jint) pcap_setfilter(pcap, fp);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSendPacket
 * Signature: (Lcom/ardikars/jxnet/Pcap;Ljava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSendPacket
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jbuf, jint jsize) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jbuf, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jsize > 0), NULL)) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	const u_char *buf = (u_char *) (*env)->GetDirectBufferAddress(env, jbuf);

	if (buf == NULL) {
		ThrowNew(env, NULL_PTR_EXCEPTION, "Unable to retrive address from ByteBuffer");
		return (jint) -1;
	}

	return (jint) pcap_sendpacket(pcap, buf + (int) 0, (int) jsize);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapNext
 * Signature: (Lcom/ardikars/jxnet/Pcap;Lcom/ardikars/jxnet/PcapPktHdr;)Ljava/nio/ByteBuffer;
 */
JNIEXPORT jobject JNICALL Java_com_ardikars_jxnet_Jxnet_PcapNext
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jh) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return NULL;
	if (CheckNotNull(env, jh, NULL) == NULL) return NULL;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return NULL;
	}

	struct pcap_pkthdr pkt_header;

	const u_char *data = pcap_next(pcap, &pkt_header);

	if (data != NULL) {
		(*env)->SetIntField(env, jh, PcapPktHdrCaplenFID, (jint) pkt_header.caplen);
		(*env)->SetIntField(env, jh, PcapPktHdrLenFID, (jint) pkt_header.len);
		(*env)->SetIntField(env, jh, PcapPktHdrTvSecFID, (jint) pkt_header.ts.tv_sec);
		(*env)->SetLongField(env, jh, PcapPktHdrTvUsecFID, (jlong) pkt_header.ts.tv_usec);
		return (*env)->NewDirectByteBuffer(env, (void *) data, pkt_header.caplen);
	} else {
		return NULL;
	}
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapNextEx
 * Signature: (Lcom/ardikars/jxnet/Pcap;Lcom/ardikars/jxnet/PcapPktHdr;Ljava/nio/ByteBuffer;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapNextEx
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jpkt_header, jobject jpkt_data) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jpkt_header, NULL) == NULL) return -1;
	if (CheckNotNull(env, jpkt_data, NULL) == NULL) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return -1;
	}

	struct pcap_pkthdr *pkt_header;
	const u_char *data = NULL;

	int r = pcap_next_ex(pcap, &pkt_header, &data);

	if (data != NULL) {
		(*env)->CallObjectMethod(env, jpkt_data, ByteBufferClearMID);
		(*env)->CallObjectMethod(env, jpkt_data, ByteBufferPutMID,
								 (*env)->NewDirectByteBuffer(env, (void *) data, pkt_header->caplen));
		(*env)->SetIntField(env, jpkt_header, PcapPktHdrCaplenFID,
							(jint) pkt_header->caplen);
		(*env)->SetIntField(env, jpkt_header, PcapPktHdrLenFID,
							(jint) pkt_header->len);
		(*env)->SetIntField(env, jpkt_header, PcapPktHdrTvSecFID,
							(jint) pkt_header->ts.tv_sec);
		(*env)->SetLongField(env, jpkt_header, PcapPktHdrTvUsecFID,
							 (jlong) pkt_header->ts.tv_usec);
	}

	return r;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapClose
 * Signature: (Lcom/ardikars/jxnet/Pcap;)V
 */
JNIEXPORT void JNICALL Java_com_ardikars_jxnet_Jxnet_PcapClose
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return;
	}

	pcap_close(pcap);
	(*env)->SetLongField(env, jpcap, PcapAddressFID, (jlong) 0);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDumpFlush
 * Signature: (Lcom/ardikars/jxnet/PcapDumper;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDumpFlush
		(JNIEnv *env, jclass jcls, jobject jpcap_dumper) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap_dumper, NULL) == NULL) return -1;

    pcap_dumper_t *pcap_dumper = GetPcapDumper(env, jpcap_dumper);

    if (pcap_dumper == NULL) {
        return -1;
    }

	return (jint) pcap_dump_flush(pcap_dumper);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDumpClose
 * Signature: (Lcom/ardikars/jxnet/PcapDumper;)V
 */
JNIEXPORT void JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDumpClose
		(JNIEnv *env, jclass jcls, jobject jpcap_dumper) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap_dumper, NULL) == NULL) return;

    pcap_dumper_t *pcap_dumper = GetPcapDumper(env, jpcap_dumper);

	if (pcap_dumper == NULL) {
		return;
	}

	pcap_dump_close(pcap_dumper);

	(*env)->SetLongField(env, jpcap_dumper, PcapDumperAddressFID, (jlong) 0);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDataLink
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDataLink
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	return (jint) pcap_datalink(pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetDataLink
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetDataLink
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jdtl) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jdtl > -1), NULL)) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	return (jint) pcap_set_datalink(pcap, (int) jdtl);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapBreakLoop
 * Signature: (Lcom/ardikars/jxnet/Pcap;)V
 */
JNIEXPORT void JNICALL Java_com_ardikars_jxnet_Jxnet_PcapBreakLoop
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return;
	}

	pcap_breakloop(pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapLookupDev
 * Signature: (Ljava/lang/StringBuilder;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ardikars_jxnet_Jxnet_PcapLookupDev
		(JNIEnv *env, jclass jcls, jobject jerrbuf) {

    UNUSED(jcls);

	if (CheckNotNull(env, jerrbuf, NULL) == NULL) return NULL;

	char errbuf[PCAP_ERRBUF_SIZE];
	errbuf[0] = '\0';

	const char *r = pcap_lookupdev(errbuf);

	if (r == NULL) {
		SetStringBuilder(env, jerrbuf, errbuf);
	}

#if defined(WIN32) || defined(_WIN64)
	int size=WideCharToMultiByte(0, 0, (const WCHAR*) r, -1, NULL, 0, NULL, NULL);
	char device[size + 1];
	WideCharToMultiByte(0, 0, (const WCHAR*) r, -1, device, size, NULL, NULL);
	return (*env)->NewStringUTF(env, device);
#endif
	return (*env)->NewStringUTF(env, r);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapGetErr
 * Signature: (Lcom/ardikars/jxnet/Pcap;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ardikars_jxnet_Jxnet_PcapGetErr
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return NULL;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return NULL;
	}

	return (jstring) (*env)->NewStringUTF(env, pcap_geterr(pcap));
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapLibVersion
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ardikars_jxnet_Jxnet_PcapLibVersion
		(JNIEnv *env, jclass jcls) {

    UNUSED(jcls);

	return (*env)->NewStringUTF(env, pcap_lib_version());
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapIsSwapped
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapIsSwapped
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	return (jint) pcap_is_swapped(pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSnapshot
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSnapshot
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	return (jint) pcap_snapshot(pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapStrError
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ardikars_jxnet_Jxnet_PcapStrError
		(JNIEnv *env, jclass jcls, jint jerror) {

    UNUSED(jcls);

	return (*env)->NewStringUTF(env, pcap_strerror((int) jerror));
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapMajorVersion
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapMajorVersion
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	return (jint) pcap_major_version(pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapMinorVersion
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapMinorVersion
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;

	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	return (jint) pcap_minor_version(pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDataLinkValToName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDataLinkValToName
		(JNIEnv *env, jclass jcls, jint jdtl) {

    UNUSED(jcls);

	if (!CheckArgument(env, (jdtl > -1), NULL)) return NULL;

	return (*env)->NewStringUTF(env, (char *) pcap_datalink_val_to_name((jint) jdtl));
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDataLinkValToDescription
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDataLinkValToDescription
		(JNIEnv *env, jclass jcls, jint jdtl) {

    UNUSED(jcls);

	if (!CheckArgument(env, (jdtl > -1), NULL)) return NULL;

	return (*env)->NewStringUTF(env, (char *) pcap_datalink_val_to_description((jint) jdtl));
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDataLinkNameToVal
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDataLinkNameToVal
		(JNIEnv *env, jclass jcls, jstring jname) {

    UNUSED(jcls);

	if (CheckNotNull(env, jname, NULL) == NULL) return -1;

	const char *name = (*env)->GetStringUTFChars(env, jname, 0);

	int r = pcap_datalink_name_to_val(name);
	(*env)->ReleaseStringUTFChars(env, jname, name);

	return r;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetNonBlock
 * Signature: (Lcom/ardikars/jxnet/Pcap;ILjava/lang/StringBuilder;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetNonBlock
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jnonblock, jobject jerrbuf) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1L;
	if (!CheckArgument(env, (jnonblock == 0 || jnonblock == 1),
					   "1 to enable non blocking, 0 otherwise.")) return -1;
	if (CheckNotNull(env, jerrbuf, NULL) == NULL) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	char errbuf[PCAP_ERRBUF_SIZE];
	errbuf[0] = '\0';

	int r = pcap_setnonblock(pcap, (int) jnonblock, errbuf);

	SetStringBuilder(env, jerrbuf, errbuf);

	return (jint) r;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapGetNonBlock
 * Signature: (Lcom/ardikars/jxnet/Pcap;Ljava/lang/StringBuilder;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapGetNonBlock
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jerrbuf) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jerrbuf, NULL) == NULL) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	char errbuf[PCAP_ERRBUF_SIZE];
	errbuf[0] = '\0';

	int r = pcap_getnonblock(pcap, errbuf);

	return (jint) r;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapOpenDead
 * Signature: (II)Lcom/ardikars/jxnet/Pcap;
 */
JNIEXPORT jobject JNICALL Java_com_ardikars_jxnet_Jxnet_PcapOpenDead
		(JNIEnv *env, jclass jcls, jint jlinktype, jint jsnaplen) {

    UNUSED(jcls);

	if (!CheckArgument(env, (jlinktype > -1), NULL)) return NULL;
	if (!CheckArgument(env, (jsnaplen > 0 && jsnaplen <= MAXIMUM_SNAPLEN), NULL)) return NULL;

	pcap_t *pcap = pcap_open_dead((int) jlinktype, (int) jsnaplen);

	return SetDeadPcap(env, pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDumpFTell
 * Signature: (Lcom/ardikars/jxnet/PcapDumper;)J
 */
JNIEXPORT jlong JNICALL Java_com_ardikars_jxnet_Jxnet_PcapDumpFTell
		(JNIEnv *env, jclass jcls, jobject jpcap_dumper) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap_dumper, NULL) == NULL) return (jlong) -1;

    pcap_dumper_t *pcap_dumper = GetPcapDumper(env, jpcap_dumper);

	if (pcap_dumper == NULL) {
		return -1;
	}

	return (jlong) pcap_dump_ftell(pcap_dumper);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapFreeCode
 * Signature: (Lcom/ardikars/jxnet/BpfProgram;)V
 */
JNIEXPORT void JNICALL Java_com_ardikars_jxnet_Jxnet_PcapFreeCode
		(JNIEnv *env, jclass jcls, jobject jfp) {

    UNUSED(jcls);

	if (CheckNotNull(env, jfp, NULL) == NULL) return;

	struct bpf_program *fp = GetBpfProgram(env, jfp);

	if (fp == NULL) {
		return;
	}

	pcap_freecode(fp);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapDumpFile
 * Signature: (Lcom/ardikars/jxnet/PcapDumper;)Lcom/ardikars/jxnet/File;
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapStats
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jpcap_stat) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jpcap_stat, NULL) == NULL) return -1;

    pcap_t *pcap = GetNotDeadPcap(env, jpcap);

    if (pcap == NULL) {
        return -1;
    }

	struct pcap_stat stats;
	memset(&stats, 0, sizeof(struct pcap_stat));

	int r = pcap_stats(pcap, &stats);

	if (r == 0) {
		(*env)->SetLongField(env, jpcap_stat, PcapStatPsRecvFID, (jlong) stats.ps_recv);
		(*env)->SetLongField(env, jpcap_stat, PcapStatPsDropFID, (jlong) stats.ps_drop);
		(*env)->SetLongField(env, jpcap_stat, PcapStatPsIfDropFID, (jlong) stats.ps_ifdrop);
	}

	return r;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapCompileNoPcap
 * Signature: (IILcom/ardikars/jxnet/BpfProgram;Ljava/lang/String;II)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapCompileNoPcap
		(JNIEnv *env, jclass jcls, jint jsnaplen_arg, jint jlinktype_arg, jobject jprogram,
		 jstring jbuf, jint joptimize, jint jmask) {

    UNUSED(jcls);

	if (!CheckArgument(env, (jsnaplen_arg > 0 && jsnaplen_arg <= MAXIMUM_SNAPLEN), NULL)) return -1;
	if (!CheckArgument(env, (jlinktype_arg > -1), NULL)) return -1;
	if (CheckNotNull(env, jprogram, NULL) == NULL) return -1;
	if (CheckNotNull(env, jbuf, NULL) == NULL) return -1;
	if (!CheckArgument(env, (joptimize == 0 || joptimize == 1), NULL)) return -1;

	struct bpf_program *program = GetBpfProgram(env, jprogram);

	if (program == NULL) {
		return (jint) -1;
	}

	const char *buf = (*env)->GetStringUTFChars(env, jbuf, 0);

	int r = pcap_compile_nopcap((int)jsnaplen_arg, (int) jlinktype_arg, program, buf,
								(int) joptimize, (bpf_u_int32) jmask);

	(*env)->ReleaseStringUTFChars(env, jbuf, buf);

	return r;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapPError
 * Signature: (Lcom/ardikars/jxnet/Pcap;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_ardikars_jxnet_Jxnet_PcapPError
		(JNIEnv *env, jclass jcls, jobject jpcap, jstring jprefix) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return;
	if (CheckNotNull(env, jprefix, NULL) == NULL) return;

    pcap_t *pcap = GetPcap(env, jpcap);

    if (pcap == NULL) {
        return;
    }

	const char *prefix =  (*env)->GetStringUTFChars(env, jprefix, 0);

	pcap_perror(pcap, (char *) prefix);
	(*env)->ReleaseStringUTFChars(env, jprefix, prefix);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapCreate
 * Signature: (Ljava/lang/String;Ljava/lang/StringBuilder;)Lcom/ardikars/jxnet/Pcap;
 */
JNIEXPORT jobject JNICALL Java_com_ardikars_jxnet_Jxnet_PcapCreate
		(JNIEnv *env, jclass jcls, jstring jsource, jobject jerrbuf) {

    UNUSED(jcls);

	if (CheckNotNull(env, jsource, NULL) == NULL) return NULL;
	if (CheckNotNull(env, jerrbuf, NULL) == NULL) return NULL;

	char errbuf[PCAP_ERRBUF_SIZE];
	errbuf[0] = '\0';
	const char *source = (*env)->GetStringUTFChars(env, jsource, 0);

	pcap_t *pcap = (pcap_t *) pcap_create(source, errbuf);
	(*env)->ReleaseStringUTFChars(env, jsource, source);

	if (pcap == NULL) {
		SetStringBuilder(env, jerrbuf, errbuf);
		return NULL;
	}
	return SetPcap(env, pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetSnaplen
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetSnaplen
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jsnaplen) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jsnaplen > 0 && jsnaplen <= MAXIMUM_SNAPLEN), NULL)) return -1;

	pcap_t *pcap = GetPcap(env, jpcap);

	if (pcap == NULL) {
	    return -1;
	}

	return pcap_set_snaplen(pcap, jsnaplen);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetPromisc
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetPromisc
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jpromisc) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jpromisc == 0 || jpromisc == 1), NULL)) return -1;

	pcap_t *pcap = GetPcap(env, jpcap);

	if (pcap == NULL) {
	    return -1;
	}

	return pcap_set_promisc(pcap, jpromisc);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetTimeout
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetTimeout
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jtimeout) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jtimeout > 0), NULL)) return -1;

	pcap_t *pcap = GetPcap(env, jpcap);

	if (pcap == NULL) {
	    return -1;
	}

	return pcap_set_timeout(pcap, jtimeout);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetBufferSize
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetBufferSize
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jbuffer_size) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jbuffer_size > 0), NULL)) return -1;

	pcap_t *pcap = GetPcap(env, jpcap);

	if (pcap == NULL) {
	    return -1;
	}

	return pcap_set_buffer_size(pcap, jbuffer_size);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapCanSetRfMon
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapCanSetRfMon
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;

	pcap_t *pcap = GetNotDeadPcap(env, jpcap);

	if (pcap == NULL) {
	    return -1;
	}

	return pcap_can_set_rfmon(pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetRfMon
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetRfMon
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jrfmon) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jrfmon == 0 || jrfmon == 1), NULL)) return -1;

	pcap_t *pcap = GetPcap(env, jpcap);

	if (pcap == NULL) {
	    return -1;
	}

	return pcap_set_rfmon(pcap, jrfmon);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetImmediateMode
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetImmediateMode
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jimmediate) {

    UNUSED(jcls);

	if (!CheckArgument(env, (jimmediate == 0 || jimmediate == 1), NULL)) return -1;
	pcap_t *pcap = GetPcap(env, jpcap);
	if (pcap == NULL) {
	    return -1;
	}
#if defined(WIN32) || defined(_WIN64)
	/*
	 * pcap_setmintocopy() changes the minimum amount of data in the kernel buffer that causes
	 * a read from the application to return (unless the timeout expires). If the value of size is large,
	 * the kernel is forced to wait the arrival of several packets before copying the data to the user.
	 * This guarantees a low number of system calls, i.e. low processor usage, and is a good setting
	 * for applications like packet-sniffers and protocol analyzers. Vice versa, in presence of
	 * a small value for this variable, the kernel will copy the packets as soon as the application is ready to receive them.
	 * This is useful for real time applications that need the best responsiveness from the kernel. pcap_open_live()
	 * sets a default mintocopy value of 16000 bytes. 
	 */
	if (jimmediate == 1) {
		return pcap_setmintocopy(pcap, 0);
	}
	return 0; // sets to a default mintocopy value (16000 bytes).
#else
	return pcap_set_immediate_mode(pcap, jimmediate);
#endif
	return -1;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapActivate
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapActivate
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;

	pcap_t *pcap = GetPcap(env, jpcap);

	if (pcap == NULL) {
	    return -1;
	}

	return pcap_activate(pcap);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetDirection
 * Signature: (Lcom/ardikars/jxnet/Pcap;Lcom/ardikars/jxnet/PcapDirection;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetDirection
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jdirection) {

    UNUSED(jcls);

#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return -1;
#else

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jdirection, NULL) == NULL) return -1;

    pcap_t *pcap = GetNotDeadPcap(env, jpcap);

    if (pcap == NULL) {
        return -1;
    }

	jstring direction = (jstring) (*env)->CallObjectMethod(env, jdirection, PcapDirectionNameMID);
	const char *enumName = (*env)->GetStringUTFChars(env, direction, 0);


	int ret;

	if (strncmp(enumName, "PCAP_D_INOUT", 12) == 0) {
		ret = pcap_setdirection(pcap, PCAP_D_INOUT);
	} else if (strncmp(enumName, "PCAP_D_OUT", 10) == 0) {
		ret = pcap_setdirection(pcap, PCAP_D_OUT);
	} else if (strncmp(enumName, "PCAP_D_IN", 9) == 0) {
		ret = pcap_setdirection(pcap, PCAP_D_IN);
	} else {
		ret = -1;
	}

	(*env)->ReleaseStringUTFChars(env, direction, enumName);
	(*env)->DeleteLocalRef(env, direction);
	return ret;
#endif
	return -1;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetTStampPrecision
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetTStampPrecision
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jtstamp_precision) {

    UNUSED(jcls);

#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return -1;
#else

    pcap_t *pcap = GetPcap(env, jpcap);

    if (pcap == NULL) {
        return -1;
    }

	return pcap_set_tstamp_precision(pcap, jtstamp_precision);
#endif
    return -1;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetTStampType
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetTStampType
		(JNIEnv *env, jclass jcls, jobject jpcap, jint jtype) {

    UNUSED(jcls);

#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return -1;
#else

    pcap_t *pcap = GetPcap(env, jpcap);

    if (pcap == NULL) {
        return -1;
    }

	return pcap_set_tstamp_type(pcap, jtype);
#endif
    return -1;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapGetTStampPrecision
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapGetTStampPrecision
		(JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return -1;
#else

    pcap_t *pcap = GetPcap(env, jpcap);

    if (pcap == NULL) {
        return -1;
    }

	return pcap_get_tstamp_precision(pcap);
#endif
    return -1;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapListDataLinks
 * Signature: (Lcom/ardikars/jxnet/Pcap;Ljava/util/List;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapListDataLinks
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jdtl_buffer) {

    UNUSED(jcls);

#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return -1;
#else

    pcap_t *pcap = GetPcap(env, jpcap);

    if (pcap == NULL) {
        return -1;
    }

	int *dtl_buffer;
	int count = pcap_list_datalinks(pcap, &dtl_buffer);
	int i;
	for (i=0; i<count; i++) {
		jclass jclazz = (*env)->FindClass(env, "java/lang/Integer");
		jobject jinteger = (*env)->NewObject(env, jclazz, (*env)->GetMethodID(env, jclazz, "<init>", "(I)V"), dtl_buffer[i]);
		if ((*env)->CallBooleanMethod(env, jdtl_buffer, ListAddMID, jinteger) == JNI_FALSE) {
			(*env)->DeleteLocalRef(env, jinteger);
			return (jint) -1;
		}
	}
	pcap_free_datalinks(dtl_buffer);
	return count;
#endif
    return -1;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapListTStampTypes
 * Signature: (Lcom/ardikars/jxnet/Pcap;Ljava/util/List;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapListTStampTypes
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jtstamp_typesp) {

    UNUSED(jcls);

#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return -1;
#else

    pcap_t *pcap = GetPcap(env, jpcap);

    if (pcap == NULL) {
        return -1;
    }

	int *list_tstamp_type;
	int count = pcap_list_tstamp_types(pcap, &list_tstamp_type);
	int i;
	for (i=0; i<count; i++) {
		jclass jclazz = (*env)->FindClass(env, "java/lang/Integer");
		jobject jinteger = (*env)->NewObject(env, jclazz, (*env)->GetMethodID(env, jclazz, "<init>", "(I)V"), list_tstamp_type[i]);
		if ((*env)->CallBooleanMethod(env, jtstamp_typesp, ListAddMID, jinteger) == JNI_FALSE) {
			(*env)->DeleteLocalRef(env, jinteger);
			return (jint) -1;
		}
	}
	pcap_free_tstamp_types(list_tstamp_type);
	return count;
#endif
    return -1;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapTStampTypeNameToVal
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapTStampTypeNameToVal
		(JNIEnv *env, jclass jcls, jstring jname) {

    UNUSED(jcls);

	if (CheckNotNull(env, jname, NULL) == NULL) return -1;
#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return -1;
#else
	const char *name = (*env)->GetStringUTFChars(env, jname, 0);

	int r = pcap_tstamp_type_name_to_val(name);
	(*env)->ReleaseStringUTFChars(env, jname, name);

	return r;
#endif
    return -1;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapTStampTypeValToName
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ardikars_jxnet_Jxnet_PcapTStampTypeValToName
		(JNIEnv *env, jclass jcls, jint jtstamp_type) {

    UNUSED(jcls);

	if (!CheckArgument(env, (jtstamp_type > -1), NULL)) return NULL;
#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return NULL;
#else
	return (*env)->NewStringUTF(env, (char *) pcap_tstamp_type_val_to_name((jint) jtstamp_type));
#endif
    return NULL;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapTStampTypeValToDescription
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ardikars_jxnet_Jxnet_PcapTStampTypeValToDescription
		(JNIEnv *env, jclass jcls, jint jtstamp_type) {

    UNUSED(jcls);

	if (!CheckArgument(env, (jtstamp_type > -1), NULL)) return NULL;
#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return NULL;
#else
	return (*env)->NewStringUTF(env, (char *) pcap_tstamp_type_val_to_description((jint) jtstamp_type));
#endif
    return NULL;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapStatusToStr
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_ardikars_jxnet_Jxnet_PcapStatusToStr
		(JNIEnv *env, jclass jcls, jint jerrnum) {

    UNUSED(jcls);

#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return NULL;
#else
	return (*env)->NewStringUTF(env, (char *) pcap_statustostr((jint) jerrnum));
#endif
    return NULL;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapOpenDeadWithTStampPrecision
 * Signature: (III)Lcom/ardikars/jxnet/Pcap;
 */
JNIEXPORT jobject JNICALL Java_com_ardikars_jxnet_Jxnet_PcapOpenDeadWithTStampPrecision
		(JNIEnv *env, jclass jcls, jint jlinktype, jint jsnaplen, jint jprecision) {

    UNUSED(jcls);

	if (!CheckArgument(env, (jlinktype > -1), NULL)) return NULL;
	if (!CheckArgument(env, (jsnaplen > 0 && jsnaplen <= MAXIMUM_SNAPLEN), NULL)) return NULL;
	if (!CheckArgument(env, (jprecision >= 0 || jprecision <= 1), NULL)) return NULL;
#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return NULL;
#else
	pcap_t *pcap = pcap_open_dead_with_tstamp_precision(jlinktype, jsnaplen, jprecision);
	return SetDeadPcap(env, pcap);
#endif
    return NULL;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapOfflineFilter
 * Signature: (Lcom/ardikars/jxnet/BpfProgram;Lcom/ardikars/jxnet/PcapPktHdr;Ljava/nio/ByteBuffer;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapOfflineFilter
		(JNIEnv *env, jclass jcls, jobject jfp, jobject jh, jobject jpkt) {

    UNUSED(jcls);

	if (CheckNotNull(env, jfp, NULL) == NULL) return -1;
	if (CheckNotNull(env, jh, NULL) == NULL) return -1;
	if (CheckNotNull(env, jpkt, NULL) == NULL) return -1;

	struct bpf_program *fp = GetBpfProgram(env, jfp);

	if (fp == NULL) {
		return (jint) -1;
	}

	struct pcap_pkthdr hdr;
	hdr.ts.tv_sec = (int) (*env)->GetIntField(env, jh, PcapPktHdrTvSecFID);
	hdr.ts.tv_usec = (int) (*env)->GetLongField(env, jh, PcapPktHdrTvUsecFID);
	hdr.caplen = (int) (*env)->GetIntField(env, jh, PcapPktHdrCaplenFID);
	hdr.len = (int) (*env)->GetIntField(env, jh, PcapPktHdrLenFID);

	u_char *sp = (u_char *) (*env)->GetDirectBufferAddress(env, jpkt);

	return (jint) pcap_offline_filter(fp, &hdr, sp);
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapOpenOfflineWithTStampPrecision
 * Signature: (Ljava/lang/String;ILjava/lang/StringBuilder;)Lcom/ardikars/jxnet/Pcap;
 */
JNIEXPORT jobject JNICALL Java_com_ardikars_jxnet_Jxnet_PcapOpenOfflineWithTStampPrecision
		(JNIEnv *env, jclass jcls, jstring jfname, jint jtstamp_precision, jobject jerrbuf) {

    UNUSED(jcls);

	if (CheckNotNull(env, jfname, NULL) == NULL) return NULL;
	if (!CheckArgument(env, (jtstamp_precision >= 0 || jtstamp_precision <= 1), NULL)) return NULL;
	if (CheckNotNull(env, jerrbuf, NULL) == NULL) return NULL;

#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return NULL;
#else
	char errbuf[PCAP_ERRBUF_SIZE];
	errbuf[0] = '\0';
	const char *fname = (*env)->GetStringUTFChars(env, jfname, 0);

	pcap_t *pcap = pcap_open_offline_with_tstamp_precision(fname, jtstamp_precision, errbuf);
	(*env)->ReleaseStringUTFChars(env, jfname, fname);

	if (pcap == NULL) {
		SetStringBuilder(env, jerrbuf, errbuf);
		return NULL;
	}
	return SetPcap(env, pcap);
#endif
    return NULL;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapInject
 * Signature: (Lcom/ardikars/jxnet/Pcap;Ljava/nio/ByteBuffer;I)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapInject
		(JNIEnv *env, jclass jcls, jobject jpcap, jobject jbuf, jint jsize) {

    UNUSED(jcls);

	if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
	if (CheckNotNull(env, jbuf, NULL) == NULL) return -1;
	if (!CheckArgument(env, (jsize > 0), NULL)) return -1;

#if defined(WIN32) || defined(_WIN64)
	ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, NULL);
	return -1;
#else
	pcap_t *pcap = GetNotDeadPcap(env, jpcap); // Exception already thrown

	if (pcap == NULL) {
		return (jint) -1;
	}

	const u_char *buf = (u_char *) (*env)->GetDirectBufferAddress(env, jbuf);

	if (buf == NULL) {
		ThrowNew(env, NULL_PTR_EXCEPTION, "Unable to retrive address from ByteBuffer");
		return (jint) -1;
	}

	return (jint) pcap_inject(pcap, buf + (int) 0, (int) jsize);
#endif
    return -1;
}

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapCheckActivated
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapCheckActivated
  (JNIEnv *env, jclass jcls, jobject jpcap) {

    UNUSED(jcls);

    if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;

   	pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown

   	if (pcap == NULL) {
   		return (jint) -1;
   	}

   	//return (jint) pcap_check_activated(pcap);
	if (pcap->activated) {
		return (-1);
	}
	return (0);
  }

// libpcap 1.8.1
/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetProtocol
 * Signature: (Lcom/ardikars/jxnet/Pcap;I)I
 */
//JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetProtocol
//  (JNIEnv *env, jclass jclazz, jobject jpcap, jint jprotocol) {
//
//    if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
//    if (!CheckArgument(env, (jprotocol > 0), NULL)) return -1;
//
//    // Including SetPcapIDs().
//    pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown
//
//    if (pcap == NULL) {
//        return (jint) -1;
//    }
//
//    return (jint) pcap_set_protocol(pcap, jprotocol);
//  }

// libpcap 1.8.1
/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapBufSize
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
//JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapBufSize
//  (JNIEnv *env, jclass jclazz, jobject jpcap) {
//
//    if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
//
//    // Including SetPcapIDs().
//    pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown
//
//    if (pcap == NULL) {
//        return (jint) -1;
//    }
//
//    return (jint) pcap_bufsize(pcap);
//
//  }

// libpcap 1.8.1
/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    PcapSetNonBlockUnactivated
 * Signature: (Lcom/ardikars/jxnet/Pcap;)I
 */
//JNIEXPORT jint JNICALL Java_com_ardikars_jxnet_Jxnet_PcapSetNonBlockUnactivated
//  (JNIEnv *env, jclass jclazz, jobject jpcap) {
//
//    if (CheckNotNull(env, jpcap, NULL) == NULL) return -1;
//
//    // Including SetPcapIDs().
//    pcap_t *pcap = GetPcap(env, jpcap); // Exception already thrown
//
//    if (pcap == NULL) {
//        return (jint) -1;
//    }
//
//    return (jint) pcap_setnonblock_unactivated(pcap);
//
//  }


/**
 * Non pcap functions
 */

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    FindHardwareAddress
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_ardikars_jxnet_Jxnet_FindHardwareAddress
  (JNIEnv *env, jclass jcls, jstring jnic_name) {

    UNUSED(jcls);

#if !defined(WIN32) && !defined(_WIN64)
    UNUSED(jnic_name);
    ThrowNew(env, PLATFORM_NOT_SUPPORTED_EXCEPTION, "FindHardwareAddress() only supported on windows.");
    return NULL;
#else
    if (CheckNotNull(env, jnic_name, NULL) == NULL) return NULL;

	jbyteArray hw_addr = NULL;
    const char *buf = (*env)->GetStringUTFChars(env, jnic_name, 0);

	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	ULONG ulOutBufLen = (ULONG) sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		(*env)->ReleaseStringUTFChars(env, jnic_name, buf);
		ThrowNew(env, NATIVE_EXCEPTION, "Error allocating memory needed to call GetAdaptersinfo");
		return NULL;
	}
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			(*env)->ReleaseStringUTFChars(env, jnic_name, buf);
			ThrowNew(env, NATIVE_EXCEPTION, "Error allocating memory needed to call GetAdaptersinfo");
			return NULL;
		}
	}
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			const char *p1 = strchr(buf, '{');
			const char *p2 = strchr(pAdapter->AdapterName,  '{');
			if (p1 == NULL || p2 == NULL) {
				p1 = buf;
				p2 = pAdapter->AdapterName;
			}
			if ((strcmp(p1, p2) == 0)) {
				hw_addr = (*env)->NewByteArray(env, (jsize) pAdapter->AddressLength);
				(*env)->SetByteArrayRegion(env, hw_addr, 0 , pAdapter->AddressLength, (jbyte *) pAdapter->Address);
				break;
			}
			pAdapter = pAdapter->Next;
        }
	} else {
		(*env)->ReleaseStringUTFChars(env, jnic_name, buf);
		ThrowNew(env, DEVICE_NOT_FOUND_EXCEPTION, "GetAdaptersInfo failed");
		return NULL;
	}
	if (pAdapterInfo)
		free(pAdapterInfo);

	(*env)->ReleaseStringUTFChars(env, jnic_name, buf);
	return hw_addr;
#endif
  }

/*
 * Class:     com_ardikars_jxnet_Jxnet
 * Method:    initIDs
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_ardikars_jxnet_Jxnet_initIDs
  (JNIEnv *env, jclass jcls) {
    UNUSED(jcls);
    SetStringBuilderIDs(env);
    SetListIDs(env);
    SetPcapIfIDs(env);
    SetPcapAddrIDs(env);
    SetSockAddrIDs(env);
    SetPcapIDs(env);
    SetBpfProgramIDs(env);
  }
