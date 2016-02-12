//
//  Intel Edison Playground
//  Copyright (c) 2015 Damian Kołakowski. All rights reserved.
//

#include <stdlib.h>
#include <errno.h>
#include <curses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

struct hci_request ble_hci_request(uint16_t ocf, int clen, void * status, void * cparam)
{
	struct hci_request rq;
	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = ocf;
	rq.cparam = cparam;
	rq.clen = clen;
	rq.rparam = status;
	rq.rlen = 1;
	return rq;
}

le_set_advertising_data_cp ble_hci_params_for_set_adv_data(char * name)
{
	int name_len = strlen(name);

	le_set_advertising_data_cp adv_data_cp;
	memset(&adv_data_cp, 0, sizeof(adv_data_cp));

	// Build simple advertisement data bundle according to:
	// - ​"Core Specification Supplement (CSS) v5" 
	// ( https://www.bluetooth.org/en-us/specification/adopted-specifications )

	adv_data_cp.data[0] = 0x02; // Length.
	adv_data_cp.data[1] = 0x01; // Flags field.
	adv_data_cp.data[2] = 0x01; // LE Limited Discoverable Flag set

	adv_data_cp.data[3] = name_len + 1; // Length.
	adv_data_cp.data[4] = 0x09; // Name field.
	memcpy(adv_data_cp.data + 5, name, name_len);

	adv_data_cp.length = strlen(adv_data_cp.data);

	return adv_data_cp;
}

int main()
{
	int ret, status;

	// Get HCI device.

	const int device = hci_open_dev(hci_get_route(NULL));
	if ( device < 0 ) { 
		perror("Failed to open HC device.");
		return 0; 
	}

	// Set BLE advertisement parameters.
	
	le_set_advertising_parameters_cp adv_params_cp;
	memset(&adv_params_cp, 0, sizeof(adv_params_cp));
	adv_params_cp.min_interval = htobs(0x0800);
	adv_params_cp.max_interval = htobs(0x0800);
	adv_params_cp.chan_map = 7;
	
	struct hci_request adv_params_rq = ble_hci_request(
		OCF_LE_SET_ADVERTISING_PARAMETERS,
		LE_SET_ADVERTISING_PARAMETERS_CP_SIZE, &status, &adv_params_cp);
	
	ret = hci_send_req(device, &adv_params_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to set advertisement parameters data.");
		return 0;
	}

	// Set BLE advertisement data.
	
	le_set_advertising_data_cp adv_data_cp = ble_hci_params_for_set_adv_data("Intel Edison");
	
	struct hci_request adv_data_rq = ble_hci_request(
		OCF_LE_SET_ADVERTISING_DATA,
		LE_SET_ADVERTISING_DATA_CP_SIZE, &status, &adv_data_cp);

	ret = hci_send_req(device, &adv_data_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to set advertising data.");
		return 0;
	}

	// Enable advertising.

	le_set_advertise_enable_cp advertise_cp;
	memset(&advertise_cp, 0, sizeof(advertise_cp));
	advertise_cp.enable = 0x01;

	struct hci_request enable_adv_rq = ble_hci_request(
		OCF_LE_SET_ADVERTISE_ENABLE,
		LE_SET_ADVERTISE_ENABLE_CP_SIZE, &status, &advertise_cp);

	ret = hci_send_req(device, &enable_adv_rq, 1000);
	if ( ret < 0 ) {
		hci_close_dev(device);
		perror("Failed to enable advertising.");
		return 0;
	}

	hci_close_dev(device);
	
	return 0;
}