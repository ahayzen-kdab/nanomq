/**
 * Created by Alvin on 2020/7/25.
 */

#ifndef NANOMQ_PUB_HANDLER_H
#define NANOMQ_PUB_HANDLER_H

#include "broker.h"
#include <nng/mqtt/packet.h>
#include <nng/nng.h>
#include <nng/protocol/mqtt/mqtt.h>

typedef uint32_t variable_integer;

// MQTT Fixed header
struct fixed_header {
	// flag_bits
	uint8_t retain : 1;
	uint8_t qos : 2;
	uint8_t dup : 1;
	// packet_types
	mqtt_control_packet_types packet_type : 4;
	// remaining length
	uint32_t remain_len;
};

// MQTT Variable header
union variable_header {
	struct {
		uint16_t           packet_id;
		struct mqtt_string topic_name;
		property           properties;
		uint32_t           prop_len;
	} publish;

	struct {
		uint16_t    packet_id;
		reason_code reason_code;
		property    properties;
		uint32_t    prop_len;
	} pub_arrc, puback, pubrec, pubrel, pubcomp;
};

struct mqtt_payload {
	uint8_t *data;
	uint32_t len;
};

struct pub_packet_struct {
	struct fixed_header   fixed_header;
	union variable_header var_header;
	struct mqtt_payload   payload;
};

struct pipe_info {
	mqtt_control_packet_types cmd;

	uint8_t    qos;
	uint32_t   index;
	uint32_t   pipe;
	nano_work *work;
	uint8_t    retain;
	uint32_t * sub_id_p;
};

struct pipe_content {
	uint32_t       total;
	uint32_t       current_index;
	uint32_t *     pipes; // queue of nng_pipes
	mqtt_msg_info *msg_infos;
};

bool        encode_pub_message(nng_msg *dest_msg, const nano_work *work,
           mqtt_control_packet_types cmd, mqtt_msg_info *);
reason_code decode_pub_message(nano_work *work);
void        foreach_client(
           void **cli_ctx_list, nano_work *pub_work, struct pipe_content *pipe_ct);
void free_pub_packet(struct pub_packet_struct *pub_packet);
void free_msg_infos(mqtt_msg_info *msg_infos);
void init_pipe_content(struct pipe_content *pipe_ct);
void handle_pub(nano_work *work, struct pipe_content *pipe_ct);
struct pub_packet_struct *copy_pub_packet(
    struct pub_packet_struct *src_pub_packet);
void init_pub_packet_property(struct pub_packet_struct *pub_packet);

#endif // NNG_PUB_HANDLER_H
