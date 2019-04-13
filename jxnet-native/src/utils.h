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

#include <jni.h>
#include <pcap.h>
#include <stdint.h>

#define CLASS_NOT_FOUND_EXCEPTION "java/lang/ClassNotFoundException"
#define NO_SUCH_METHOD_EXCEPTION "java/lang/NoSuchMethodException"
#define NO_SUCH_FIELD_EXCEPTION "java/lang/NoSuchFieldException"
#define NULL_PTR_EXCEPTION "java/lang/NullPointerException"
#define NATIVE_EXCEPTION "com/ardikars/jxnet/exception/NativeException"
#define DEVICE_NOT_FOUND_EXCEPTION "com/ardikars/jxnet/exception/DeviceNotFoundException"
#define PCAP_CLOSE_EXCEPTION "com/ardikars/jxnet/exception/PcapCloseException"
#define BPF_PROGRAM_CLOSE_EXCEPTION "com/ardikars/jxnet/exception/BpfProgramCloseException"
#define PCAP_DUMPER_CLOSE_EXCEPTION "com/ardikars/jxnet/exception/PcapDumperCloseException"
#define PLATFORM_NOT_SUPPORTED_EXCEPTION "com/ardikars/jxnet/exception/PlatformNotSupportedException"
#define ILLEGAL_STATE_EXCEPTION "java/lang/IllegalStateException"
#define ILLEGAL_ARGUMENT_EXCEPTION "java/lang/IllegalArgumentException"

typedef struct pcap_user_data_t {
    JNIEnv *env;
    jobject callback;
    jobject user;
    jclass PcapHandlerClass;
    jmethodID PcapHandlerNextPacketMID;
} pcap_user_data_t;

typedef struct arp_user_data_t {
    JNIEnv *env;
    jobject callback;
    jobject user;
    jclass ArpHandlerClass;
    jmethodID ArpHandlerNextArpEntryMID;
} arp_user_data_t;

void swap_order_uint32(uint32_t *value);

jlong PointerToJlong(void *pointer);

void *JlongToPointer(jlong address);

void ThrowNew(JNIEnv *env, const char *class_name, const char *message);

void SetStringBuilder(JNIEnv *env, jobject obj, const char *str);

jobject NewObject(JNIEnv *env, const char *class_name, const char *name, const char *signature);

jbyteArray NewByteAddr(JNIEnv *env, struct sockaddr *addr);

jobject NewSockAddr(JNIEnv *env, struct sockaddr *addr);

jobject SetPcap(JNIEnv *env, pcap_t *pcap);

jobject SetDeadPcap(JNIEnv *env, pcap_t *pcap);

pcap_t *GetPcap(JNIEnv *env, jobject jpcap);

pcap_t *GetNotDeadPcap(JNIEnv *env, jobject jpcap);

jobject SetPcapDumper(JNIEnv *env, pcap_dumper_t *pcap_dumper);

pcap_dumper_t *GetPcapDumper(JNIEnv *env, jobject jpcap_dumper);

jobject SetBpfProgram(JNIEnv *env, jobject obj, struct bpf_program *fp);

struct bpf_program *GetBpfProgram(JNIEnv *env, jobject jbpf_program);

void pcap_callback(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);

void pcap_callback0(u_char *user, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
