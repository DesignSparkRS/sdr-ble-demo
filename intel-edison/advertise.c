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

le_set_advertising_data_cp ble_hci_params_for_set_adv_data(const char * name, const char *dataUUID, const char *data)
{

	le_set_advertising_data_cp adv_data_cp;
	memset(&adv_data_cp, 0, sizeof(adv_data_cp));

	// Build simple advertisement data bundle according to:
	// - ​"Core Specification Supplement (CSS) v5" 
	// ( https://www.bluetooth.org/en-us/specification/adopted-specifications )

	size_t i = 0;
	adv_data_cp.data[i++] = 0x02; // Length.
	adv_data_cp.data[i++] = 0x01; // Flags field.
	adv_data_cp.data[i++] = 0x01; // LE Limited Discoverable Flag set

	int name_len = strlen(name);
	if (name_len != 0)
	{
		adv_data_cp.data[i++] = name_len + 1; // Length.
		adv_data_cp.data[i++] = 0x09; // Name field.
		memcpy(adv_data_cp.data + i, name, name_len);
		i += name_len;
	}

	int data_len = strlen(data);
	if (data_len != 0)
	{
		uint16_t service_uuid = (uint16_t)strtol(dataUUID, NULL, 16);
		adv_data_cp.data[i++] = data_len + 1 + 2; // Length.
		adv_data_cp.data[i++] = 0x16; // Service Data field.
		adv_data_cp.data[i++] = service_uuid & 0xff;
		adv_data_cp.data[i++] = service_uuid >> 8;
		memcpy(adv_data_cp.data + i, data, data_len);
		i += data_len;
	}

	adv_data_cp.length = i;

	return adv_data_cp;
}

int main(int argc, const char* argv[])
{
	if (argc != 4)
	{
		printf("Usage: advertise.exe <name> <16-bit service uuid in hex - EA06> <arbitrary service data string>\n");
		return -1;
	}
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
	
	le_set_advertising_data_cp adv_data_cp = ble_hci_params_for_set_adv_data(argv[1], argv[2], argv[3]);
	
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
