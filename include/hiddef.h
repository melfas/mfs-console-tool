#include <stdint.h>

#define HID_DATA_LENGTH_MAXIMUM							512

#define HID_CUSTOM_PACKET_SIZE							64				//HID_PACKET_LENGTH
#define HID_CUSTOM_PACKET_DATA_SIZE						58				//HID_DEFAULT_PACKET_DATA_SIZE
#define HID_CUSTOM_PACKET_HEADER_SIZE					6				//HID_CUSTOM_PACKET_HEADER_SIZE

//----------------------------
// Report ID
//----------------------------
#define HID_REPORT_ID_CUSTOM_OUT						0x09
#define HID_REPORT_ID_CUSTOM_IN							0x0A

//----------------------------
// Read-Write bit
//----------------------------
#define HID_RW_READ										0x00
#define HID_RW_WRITE									0x01

//----------------------------
// Extended Packet ID
//----------------------------
#define HID_EXT_ID_NORMAL								0x00
#define HID_EXT_ID_ADDITIONAL_WRITE_AND_READ			0x01

typedef unsigned char uint8_t;
typedef unsigned char bool_t;
typedef unsigned short uint16_t;

#pragma pack(1)
typedef struct HID_CUSTOM_PACKET_
{
	uint8_t		ReportId;

	uint8_t		ReadWrite	:1;
	uint8_t		Last		:1;
	uint8_t		Reserved	:2;
	uint8_t		ExtId		:2;
	uint8_t		RouteIdx	:2;

	uint16_t	Address;
	uint16_t	Length;
	uint8_t		Data[HID_CUSTOM_PACKET_DATA_SIZE];

}HID_CUSTOM_PACKET;

#pragma pack()