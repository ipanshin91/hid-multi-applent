#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "hidapi.h"

struct DeviceId
{
	unsigned int vid;
	unsigned int pid;
};

static const DeviceId SUPPORTED_DEVICES[] = {
	{0x825, 0x826},
	{0x825, 0x084},
};
static const size_t SUPPORTED_DEVICES_COUNT = sizeof(SUPPORTED_DEVICES) / sizeof(SUPPORTED_DEVICES[0]);

class RequestCommand
{
	public:
		uint cSize;
		char sHeader[24];
		char sPara[28];
		uint nSignature;
		uint nChecksum;
		
		RequestCommand() 
		{
			this->cSize = 60;
			memset(this->sHeader, 0, 24);
			memset(this->sPara, 0, 28);
			this->nSignature = 0x88805550;
			this->nChecksum = 0;
		}
		
		void setHeader(const char* str) 
		{
			strcpy(this->sHeader, str);
		}
		
		void setPara(const char* str) 
		{
			strcpy(this->sPara, str);
		}

		void setupChecksum()
		{
			unsigned char buff[64];
			unsigned int sum = 0;
			unsigned int indexBuff = 60;
			unsigned int index =0 ;
			
			this->getData(buff);
			
			for (; index<60; index++) {
				sum += (int)buff[index];
			}
			
			this->nChecksum = sum;
		};
	
		void getData(unsigned char* data)
		{
			unsigned char buff[64];
			memset(buff, 0, 64);
			unsigned int indexBuff = 0;
			
			buff[indexBuff++] = (unsigned char)(0xff & this->cSize);
			buff[indexBuff++] = (unsigned char)((0xff00 & this->cSize) >> 8);
			buff[indexBuff++] = (unsigned char)((0xff0000 & this->cSize) >> 16);
			buff[indexBuff++] = (unsigned char)((0xff000000 & this->cSize) >> 24);
			
			for (int hIndex = 0; hIndex<24; hIndex++) {
				buff[indexBuff++] = this->sHeader[hIndex];
				
			}
			
			for (int pIndex = 0; pIndex<28; pIndex++) {
				buff[indexBuff++] = this->sPara[pIndex];
				
			}
			
			buff[indexBuff++] = (unsigned char)(0xff & this->nSignature);
			buff[indexBuff++] = (unsigned char)((0xff00 & this->nSignature) >> 8);
			buff[indexBuff++] = (unsigned char)((0xff0000 & this->nSignature) >> 16);
			buff[indexBuff++] = (unsigned char)((0xff000000 & this->nSignature) >> 24);
			
			buff[indexBuff++] = (unsigned char)(0xff & this->nChecksum);
			buff[indexBuff++] = (unsigned char)((0xff00 & this->nChecksum) >> 8);
			buff[indexBuff++] = (unsigned char)((0xff0000 & this->nChecksum) >> 16);
			buff[indexBuff++] = (unsigned char)((0xff000000 & this->nChecksum) >> 24);

			memcpy(data, buff, sizeof(buff));
		};
};

class ATCommander
{
	public: 
		void init(const DeviceId* devices, size_t count)
		{
			this->devices = devices;
			this->count = count;
		}
		
		int cmd(const char* header, const char* para, unsigned char* data)
		{
			int result;
			unsigned char buff[64];
			RequestCommand request;
			request.setHeader(header);
			request.setPara(para);
			request.setupChecksum();
			memset(buff, 0, 64);

			int found = 0;
			for (size_t i = 0; i < this->count; i++) {
				hid_device_info *list = hid_enumerate(this->devices[i].vid, this->devices[i].pid);

				for (hid_device_info *dev = list; dev; dev = dev->next) {
					found++;
					request.getData(buff);

					hid_device *handle;
					handle = hid_open_path(dev->path);

					result = hid_write(handle, buff, 64);
					result = hid_read_timeout(handle, data, 64, 1000);

					hid_close(handle);

					printf("%s\n", data);
				}

				hid_free_enumeration(list);
			}

			hid_exit();

			if (!found)
			{
				printf("Error (%s): Devices not found\n", header);
				return 0;
			}

			return 1;
		}

	private:
		const DeviceId* devices;
		size_t count;
		
};

int main(int argc, char* argv[])
{
	if (argc < 3) {
		printf("Error (%s): Not enough arguments\n", argv[0]);
		return 0;
	}

	char *header = argv[1];
	char *para = argv[2];
	
	unsigned char buff[64];
	memset(buff, 0, 64);
	ATCommander atc;
	atc.init(SUPPORTED_DEVICES, SUPPORTED_DEVICES_COUNT);
	atc.cmd(header, para, buff);
	
	return 0;
}

