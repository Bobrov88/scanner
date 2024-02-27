#ifndef __P_FWDL_INTERFACE_H__
#define __P_FWDL_INTERFACE_H__

#define LINUX

#ifndef LINUX
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif


	typedef enum
	{
		FAIL = -6,//The upgrade failed
		FAIL_STOPUPDATE = -5,//Aborted in the middle of the upgrade
		RECONNECTDEVICE = -4,//The device needs to be reconnected
		FAIL_CMDNORETURN  = -3,//Send but no return
		FAIL_NONEEDUPDATE = -2,//Same as the current firmware of the device, the device does not need to be upgraded
		FAIL_TIMEOUT = -1,//The firmware upgrade failed and exited after a timeout
		SUCCESS = 0,//==0 The firmware upgrade is successful
		FWVERSIONGET,//==1 Send a firmware version query
		FWVERSIONOK,//==2 The firmware version query is complete
		FWUPDATENOTIFY,//==3 Send a firmware upgrade notification
		FWUPDATENOTIFYOK,//==4 The firmware upgrade notification is complete
		FWHEADNOTIFY,//==5 Sends a firmware header notification
		FWHEADNOTIFYOK,//==6 The firmware header information is notified
		FWDBAGSEND,//==7 Send a firmware upgrade file packet
		FWDBAGSENDOK,//==8 The firmware upgrade file packet has been sent
		FWUPDATEENDNOTIFY,//==9 Send a firmware end notification
		FWUPDATEENDNOTIFYOK,//==10 The firmware sending end notification is complete
	}DownloadState;


	/*
	*The callback function declaration used to send
	*pSendData Callback function pointer
	*psendbuf Data pointers
	*sendlen The length of the data sent
	*/
	typedef int(*pSendData)(char *psendbuf, int sendlen);

	/*
	*The callback function declaration used to receive
	*pReceiveData Callback function pointer
	*precbuf Data pointers
	*reclen The length of the data received
	*/
	typedef int(*pReceiveData)(char* precbuf, int reclen);

	/*
	 * The firmware content parsing function is called when the firmware is loaded
	 *params
	 *pinfwpath:Enter the firmware name, including the path where the firmware is located
	 *ret
	 * ==0:The firmware was parsed successfully
	 * < 0:Failed to parse the firmware
	 */
	DLL_API int firmware_parse_pro(char* pinfwpath);

	/*
	 * Start the download function and call it when the download starts
	 *params
	 *pSend:The pointer to the serial port sending function
	 *pReceive:The pointer to the serial port receiving function
	 iseraseconfig:Whether to erase the existing configurations of the app
	 *ret
	 * ==0:The startup was successful
	 * < 0:Failed to start
	 */
	DLL_API int firmware_download_start(pSendData pSend, pReceiveData pReceive, bool iseraseconfig);

	/*
	 * Get the download status function
	 *params
	 * poutpercent:Download progress
	 * poutdlstate:Download status
	 */
	DLL_API void get_download_state(double *poutpercent, DownloadState *poutdlstate);

	/*
	 *Aborted in the middle of the upgrade 
	 */
	DLL_API void stop_download();

#ifdef __cplusplus
}
#endif

#endif // __P_FWDL_INTERFACE_H__