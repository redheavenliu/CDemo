#ifndef INCLUDE_STANET_QMI_H
#define INCLUDE_STANET_QMI_H

#include "stanet_type.h"

void qmi_session_callback(
	int session_id,
	sta_msg_id_enum msg_id,
	sta_session_err_enum err,
	void *data,
	int len
);


void stanet_qmi_init
(
	void
);



#endif /* INCLUDE_STANET_QMI_H */
