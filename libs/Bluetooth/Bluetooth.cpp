//
// Created by insane on 17.01.19.
//

#include <iostream>
#include <boost/log/trivial.hpp>
#include "Bluetooth.h"


Bluetooth::Bluetooth(bc::BluetoothConfiguration cfg) {
    bluetoothConfiguration = std::move(cfg);
    this->setGPS(new GPS());
    setDoRun(true);
}

Bluetooth::Bluetooth(bc::BluetoothConfiguration cfg, GPS *gps) {
    bluetoothConfiguration = std::move(cfg);
    this->setGPS(gps);
    setDoRun(true);
}

void Bluetooth::run() {
    inquiry_info* ii = nullptr;
    int maxRsp, numRsp, devId, sck, len, flags, i;
    char addr[19] = {0};
    char name[248] = {0};
    devId = hci_open_dev(0);
    sck = hci_open_dev(devId);
    if(devId < 0 || sck < 0) this->setDoRun(false); // idk
    while(this->getDoRun()){
        len = 8;
        maxRsp = 255;
        flags = IREQ_CACHE_FLUSH;
        ii = (inquiry_info*) malloc(maxRsp * sizeof(inquiry_info));
        std::cout << "inquiring bluetooth devices" << std::endl;
        numRsp = hci_inquiry(devId, len, maxRsp, nullptr, &ii, flags);
        for (i = 0; i < numRsp; i++) {
            ba2str(&(ii+i)->bdaddr, addr);
            memset(name, 0, sizeof(name));
            if (hci_read_remote_name(sck, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0) strcpy(name, "[unknown]");
            std::cout << std::string("found device '") << name << std::string("' with address '") << std::string(addr) <<  "'" << std::endl;
        }
        usleep(500); // idk
    }
    std::cout << "bluetooth loop has finished running; cleaning up" << std::endl;
    free(ii);
    close(sck);
}