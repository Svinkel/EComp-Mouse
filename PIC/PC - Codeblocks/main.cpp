#include <iostream>

using namespace std;
/*********************************************************************
 *
 *                Example 02 - Run-time Linking
 *
 *********************************************************************
 * FileName:        console.cpp
 * Dependencies:    None
 * Compiler:        Borland C++ Builder 6
 * Company:         Copyright (C) 2004 by Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PICmicro® Microcontroller is intended and
 * supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rawin Rojvanit       11/19/04
 ********************************************************************/

//---------------------------------------------------------------------------

#pragma hdrstop

#include <stdio.h>
#include "windows.h"
#include "mpusbapi.h"                   // MPUSBAPI Header File

//---------------------------------------------------------------------------
#pragma argsused

// Global Vars
char vid_pid[]= "vid_04d8&pid_000c";    // Default Demo Application Firmware
char out_pipe[]= "\\MCHP_EP1";
char in_pipe[]= "\\MCHP_EP1";

DWORD temp;

HINSTANCE libHandle;
HANDLE myOutPipe;
HANDLE myInPipe;

//---------------------------------------------------------------------------
// Prototypes
void GetSummary(void);
void LoadDLL(void);
BYTE GetDataFromPIC(BYTE MY_COMMAND);
DWORD SendReceivePacket(BYTE *SendData, DWORD SendLength, BYTE *ReceiveData,
                    DWORD *ReceiveLength, UINT SendDelay, UINT ReceiveDelay);
void CheckInvalidHandle(void);

//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    BOOLEAN bQuit;
    DWORD selection;
    bQuit = false;
    POINT cursorPos;
    int pos[2];
    BYTE cursor[2];
    // Load DLL when it is necessary, i.e. on start-up!
    LoadDLL();

    // Always a good idea to initialize the handles
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;

    cursor[0]=0;
    cursor[1]=0;
    pos[0]=500;
    pos[1]=500;


    printf("Microchip Technology Inc., 2004\r\n");
    printf("===============================\r\n");
    while(!bQuit)
    {
        printf("Select an option\r\n");
        printf("[1] Get this application version\r\n");
        printf("[2] List boards present\r\n");
        printf("[3] Asks the PIC for the value in register\r\n");
        printf("[4] Change resolution\r\n");
        printf("[5] Read optical mouse sensor\r\n");
        printf("[6] Display image\r\n");
        printf("[7] Display image and read mouse\r\n>>");
        printf("[0] Quit\r\n>>");
        scanf("%d",&selection);

        switch(selection)
        {
            case 1:
                temp = MPUSBGetDLLVersion();
                printf("MPUSBAPI Version: %d.%d\r\n",HIWORD(temp),LOWORD(temp));
                break;
            case 2:
                GetSummary();
                break;
            case 3:
                cursor[0] = GetDataFromPIC(0x00);

                break;
            case 4:
                cursor[0] = GetDataFromPIC(0x0d);
                break;
            case 5:
                cursor[0] = GetDataFromPIC(0xFF);
                break;
            case 6:
                cursor[0] = GetDataFromPIC(0x0b);
                break;
            case 7:
                cursor[0] = GetDataFromPIC(0x56);
            case 0:
                bQuit = true;
                break;
            default:
                break;
        }// end switch

        fflush(stdin);printf("\r\n");
    }//end while

    // Always check to close all handles before exiting!
    if (myOutPipe != INVALID_HANDLE_VALUE) MPUSBClose(myOutPipe);
    if (myInPipe != INVALID_HANDLE_VALUE) MPUSBClose(myInPipe);
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;

    // Always check to close the library too.
    if (libHandle != NULL) FreeLibrary(libHandle);

    return 0;
}//end main

//---------------------------------------------------------------------------

void GetSummary(void)
{
    HANDLE tempPipe = INVALID_HANDLE_VALUE;
    DWORD count = 0;
    DWORD max_count;

    max_count = MPUSBGetDeviceCount(vid_pid);

    printf("\r\n%d device(s) with %s currently attached\r\n",max_count,vid_pid);

    // Note:
    // The total number of devices using the generic driver could be
    // bigger than max_count. They could have different vid & pid numbers.
    // This means if max_count is 2, the valid instance index do not
    // necessary have to be '0' and '1'.
    //
    // Below is a sample code for searching for all valid instance indexes.
    // MAX_NUM_MPUSB_DEV is defined in _mpusbapi.h

    count = 0;
    for(int i = 0; i < MAX_NUM_MPUSB_DEV; i++)
    {
        tempPipe = MPUSBOpen(i,vid_pid,NULL,MP_READ,0);
        if(tempPipe != INVALID_HANDLE_VALUE)
        {
            printf("Instance Index # %d\r\n",i);
            MPUSBClose(tempPipe);
            count++;
        }
        if(count == max_count) break;
    }//end for
    printf("\r\n");
}//end GetSummary

//---------------------------------------------------------------------------

void LoadDLL(void)
{
    libHandle = NULL;
    libHandle = LoadLibrary("mpusbapi");
    if(libHandle == NULL)
    {
        printf("Error loading mpusbapi.dll\r\n");
    }
    else
    {
        MPUSBGetDLLVersion=(DWORD(*)(void))\
                    GetProcAddress(libHandle,"_MPUSBGetDLLVersion");
        MPUSBGetDeviceCount=(DWORD(*)(PCHAR))\
                    GetProcAddress(libHandle,"_MPUSBGetDeviceCount");
        MPUSBOpen=(HANDLE(*)(DWORD,PCHAR,PCHAR,DWORD,DWORD))\
                    GetProcAddress(libHandle,"_MPUSBOpen");
        MPUSBWrite=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))\
                    GetProcAddress(libHandle,"_MPUSBWrite");
        MPUSBRead=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))\
                    GetProcAddress(libHandle,"_MPUSBRead");
        MPUSBReadInt=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))\
                    GetProcAddress(libHandle,"_MPUSBReadInt");
        MPUSBClose=(BOOL(*)(HANDLE))GetProcAddress(libHandle,"_MPUSBClose");
        MPUSBSetConfiguration=(DWORD(*)(HANDLE,USHORT))\
                    GetProcAddress(libHandle,"_MPUSBSetConfiguration");
        MPUSBGetStringDescriptor = \
                    (DWORD(*)(HANDLE,UCHAR,USHORT,PVOID,DWORD,PDWORD))\
                    GetProcAddress(libHandle,"_MPUSBGetStringDescriptor");
        MPUSBGetConfigurationDescriptor = \
                   (DWORD(*)(HANDLE,UCHAR,PVOID,DWORD,PDWORD))\
                   GetProcAddress(libHandle,"_MPUSBGetConfigurationDescriptor");
        MPUSBGetDeviceDescriptor = (DWORD(*)(HANDLE,PVOID,DWORD,PDWORD))\
                   GetProcAddress(libHandle,"_MPUSBGetDeviceDescriptor");

        if((MPUSBGetDeviceCount == NULL) || (MPUSBOpen == NULL) ||
            (MPUSBWrite == NULL) || (MPUSBRead == NULL) ||
            (MPUSBClose == NULL) || (MPUSBGetDLLVersion == NULL) ||
            (MPUSBReadInt == NULL) || (MPUSBSetConfiguration == NULL) ||
            (MPUSBGetStringDescriptor == NULL) ||
            (MPUSBGetConfigurationDescriptor == NULL) ||
            (MPUSBGetDeviceDescriptor == NULL))
            printf("GetProcAddress Error\r\n");
    }//end if else
}//end LoadDLL

//---------------------------------------------------------------------------

BYTE GetDataFromPIC(BYTE MY_COMMAND)
{
    // First we need to open data pipes...
    DWORD selection;
    selection = 0; // Assumes only one board is connected to PC through USB and it has index 0
    fflush(stdin);
    int pos[2];
    BYTE cursor[2];

    pos[0]=pos[1]=500;

    myOutPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_WRITE,0);
    myInPipe = MPUSBOpen(selection,vid_pid,out_pipe,MP_READ,0);
    if(myOutPipe == INVALID_HANDLE_VALUE || myInPipe == INVALID_HANDLE_VALUE)
    {
        printf("Failed to open data pipes.\r\n");
        return 0;
    }//end if

    HDC hdcScreen;
    HDC MemDCExercising;
    HBITMAP bm;
    HBRUSH hBrush;
    HPEN hPen;

    int d=30;
    int x=10;
    int y=10;
    int i,j;
    int a=0;

    // This Computer Electronics Course Demo has a simple communication protocol.
    // To read the status of switches S2 and S3 one has to send the MY_COMMAND command
    // which is defined as 0x00, follows by the length of the
    // expected result, in this case is 2 bytes, S2 status, and S3 status.
    // i.e. <MY_COMMAND><0x02>
    //
    // The response packet from the board has the following format:
    // <MY_COMMAND><0x02><S2><S3>

    // The receive buffer size must be equal to or larger than the maximum
    // endpoint size it is communicating with. In this case, 64 bytes.

    BYTE send_buf[64],receive_buf[64];
    DWORD RecvLength=4;

    send_buf[0] = MY_COMMAND;      // Command
    send_buf[1] = 0x02;              // Expected length of the result
    POINT LPPOINT;

    switch(MY_COMMAND) {
        case 0xFF:
            while(1){
                send_buf[1] = 0x04;
                send_buf[0] = 0x55;
                if(SendReceivePacket(send_buf,2,receive_buf,&RecvLength,1000,1000) == 1) {
                    cursor[0] = (INT8) receive_buf[2];
                    cursor[1] = (INT8) receive_buf[3];
                    //printf("valor no registo X: %d\n", (INT8)cursor[0]);
                    //printf("valor no registo Y: %d\n", (INT8)cursor[1]);
                }else
                    exit(-1);

                GetCursorPos(&LPPOINT);
                SetCursorPos( LPPOINT.x - (INT8)cursor[0], LPPOINT.y + (INT8)cursor[1]);
                //printf("Cursor movement\n");
            }
        break;
        //*******************************************************//
        case 0x0b:

            hdcScreen = GetDC( NULL );
            MemDCExercising = CreateCompatibleDC(hdcScreen);
            bm = CreateCompatibleBitmap(hdcScreen, 15*d,15*d); // tamanho rectangulo
            SelectObject(MemDCExercising, bm);
            hBrush = CreateSolidBrush(RGB(200,200,100));
            SelectObject(MemDCExercising, hBrush);
            hPen = CreatePen(PS_NULL,1,RGB(200,50,200));
            SelectObject(MemDCExercising, hPen);
            BitBlt(hdcScreen, 0, 0, 15*d,15*d, MemDCExercising, 0, 0, SRCCOPY);

            while(1) {
                send_buf[0] = 0x06;
                send_buf[1] = 4;
                RecvLength=4;
                if(SendReceivePacket(send_buf,2,receive_buf,&RecvLength,1000,1000) == 1){
                            //printf("init 0: %d\n", receive_buf[0]);
                }else {
                    perror("\r\n Falha ao inicializar a camara,\n");
                    exit(-1);
                }
                int contador=0;
                y=14;
                j=2;
                RecvLength=47;
                send_buf[0] = MY_COMMAND;
                send_buf[1] = 47;
                for(i=0;i<5;i++){
                    if(SendReceivePacket(send_buf,47,receive_buf,&RecvLength,1000,1000) == 1) {
                            for(j=2;j<47;j++){
                                if(y == -1){
                                    y = 14;
                                    contador++;
                                }
                                cursor[0] = ((INT8)receive_buf[j] & 127)*2;
                                //printf("#%d --- valor %d no registo X: %d\n",a, i+1, (INT8)cursor[0]);
                                hBrush = CreateSolidBrush(RGB(cursor[0],cursor[0],cursor[0]));
                                SelectObject(MemDCExercising, hBrush);
                                hPen = CreatePen(PS_NULL,1,RGB(cursor[0],cursor[0],cursor[0]));
                                SelectObject(MemDCExercising, hPen);
                                Rectangle(MemDCExercising, contador*d,y*d,(contador+1)*d,(y+1)*d);
                                DeleteObject(hBrush);
                                DeleteObject(hPen);
                                y--;
                            }
                    }else {
                        perror("\r\n Falha ao ler da camara,\n");
                        exit(-1);
                    }
                }
                BitBlt(hdcScreen, 0, 0, 15*d,15*d, MemDCExercising, 0, 0, SRCCOPY);
                a++;

            /*RecvLength=4;
            send_buf[0] = 0x06;
            if(SendReceivePacket(send_buf,2,receive_buf,&RecvLength,1000,1000) == 1){
                        //printf("init 0: %d\n", receive_buf[0]);
            }else {
                perror("\r\n Falha ao fazer reset a camara,\n");
                exit(-1);
            }*/
                memset(receive_buf,0,sizeof(BYTE)*64);
            }
        break;
        //*******************************************************//
         case 0x56:
            hdcScreen = GetDC( NULL );
            MemDCExercising = CreateCompatibleDC(hdcScreen);
            bm = CreateCompatibleBitmap(hdcScreen, 15*d,15*d); // tamanho rectangulo
            SelectObject(MemDCExercising, bm);
            //hBrush = CreateSolidBrush(RGB(200,200,100));
            //SelectObject(MemDCExercising, hBrush);
            //hPen = CreatePen(PS_NULL,1,RGB(200,50,200));
            //SelectObject(MemDCExercising, hPen);
            BitBlt(hdcScreen, 0, 0, 15*d,15*d, MemDCExercising, 0, 0, SRCCOPY);

            while(1) {
                send_buf[0] = 0x06;
                RecvLength=4;
                if(SendReceivePacket(send_buf,2,receive_buf,&RecvLength,1000,1000) == 1){
                            //printf("init 0: %d\n", receive_buf[0]);
                }else {
                    perror("\r\n Falha ao inicializar a camara,\n");
                    exit(-1);
                }
                int contador=0;
                y=14;
                j=2;
                RecvLength=49;
                send_buf[0] = MY_COMMAND;

                for(i=0;i<5;i++){
                    if(SendReceivePacket(send_buf,49,receive_buf,&RecvLength,1000,1000) == 1) {
                        cursor[0] = (INT8) receive_buf[2];
                        cursor[1] = (INT8) receive_buf[3];

                        GetCursorPos(&LPPOINT);
                        SetCursorPos( LPPOINT.x - (INT8)cursor[0], LPPOINT.y + (INT8)cursor[1]);

                        for(j=4;j<49;j++){
                            if(y == -1){
                                y = 14;
                                contador++;
                            }
                            cursor[0] = ((INT8)receive_buf[j] & 127)*2;
                            //printf("#%d --- valor %d no registo X: %d\n",a, i+1, (INT8)cursor[0]);
                            hBrush = CreateSolidBrush(RGB(cursor[0],cursor[0],cursor[0]));
                            SelectObject(MemDCExercising, hBrush);
                            hPen = CreatePen(PS_SOLID,1,RGB(cursor[0],cursor[0],cursor[0]));
                            SelectObject(MemDCExercising, hPen);
                            Rectangle(MemDCExercising, contador*d,y*d,(contador+1)*d,(y+1)*d);
                            DeleteObject(hBrush);
                            DeleteObject(hPen);
                            y--;
                        }
                    }else {
                        perror("\r\n Falha ao ler da camara,\n");
                        exit(-1);
                    }
                }
                BitBlt(hdcScreen, 0, 0, 15*d,15*d, MemDCExercising, 0, 0, SRCCOPY);
                a++;
                memset(receive_buf,0,sizeof(BYTE)*64);

            /*RecvLength=4;
            send_buf[0] = 0x06;
            if(SendReceivePacket(send_buf,2,receive_buf,&RecvLength,1000,1000) == 1){
                        //printf("init 0: %d\n", receive_buf[0]);
            }else {
                perror("\r\n Falha ao fazer reset a camara,\n");
                exit(-1);
            }*/
            }
        break;
        //*******************************************************//
        case 0x0d:
            RecvLength=3;
            if(SendReceivePacket(send_buf,2,receive_buf,&RecvLength,1000,1000) == 1) {
                        if (receive_buf[2]==1) printf("\n Resolucao alterada com sucesso.\n");
                }
                else
                    printf("USB Operation Failed\r\n");
        break;
        //*******************************************************//

        default:
            if(SendReceivePacket(send_buf,2,receive_buf,&RecvLength,1000,1000) == 1) {
                    printf("valor no registo: %d\n", (INT8)receive_buf[2]);

                    /*if(RecvLength == 4 && receive_buf[0] == MY_COMMAND &&
                        receive_buf[1] == 0x02)
                    {
                        if(receive_buf[2]==1 && receive_buf[3]==1) printf("\nS2 and S3 pressed\r\n");
                        else if(receive_buf[2]==1 && receive_buf[3]==0) printf("\nS2 pressed\r\n");
                        else if(receive_buf[2]==0 && receive_buf[3]==1) printf("\nS3 pressed\r\n");
                        else printf("\nno switch is pressed\r\n");
                    }*/
            }
            else
                printf("USB Operation Failed\r\n");
        break;
        //*******************************************************//
    }

    // Let's close the data pipes since we have nothing left to do..
    MPUSBClose(myOutPipe);
    MPUSBClose(myInPipe);
    myOutPipe = myInPipe = INVALID_HANDLE_VALUE;
    return receive_buf[2];

}//end GetUSBDemoFWVersion

//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
//
// A typical application would send a command to the target device and expect
// a response.
// SendReceivePacket is a wrapper function that facilitates the
// send command / read response paradigm
//
// SendData - pointer to data to be sent
// SendLength - length of data to be sent
// ReceiveData - Points to the buffer that receives the data read from the call
// ReceiveLength - Points to the number of bytes read
// SendDelay - time-out value for MPUSBWrite operation in milliseconds
// ReceiveDelay - time-out value for MPUSBRead operation in milliseconds
//

DWORD SendReceivePacket(BYTE *SendData, DWORD SendLength, BYTE *ReceiveData,
                    DWORD *ReceiveLength, UINT SendDelay, UINT ReceiveDelay)
{
    DWORD SentDataLength;
    DWORD ExpectedReceiveLength = *ReceiveLength;

    if(myOutPipe != INVALID_HANDLE_VALUE && myInPipe != INVALID_HANDLE_VALUE)
    {
        if(MPUSBWrite(myOutPipe,SendData,SendLength,&SentDataLength,SendDelay))
        {

            if(MPUSBRead(myInPipe,ReceiveData, ExpectedReceiveLength,
                        ReceiveLength,ReceiveDelay))
            {
                if(*ReceiveLength == ExpectedReceiveLength)
                {
                    return 1;   // Success!
                }
                else if(*ReceiveLength < ExpectedReceiveLength)
                {
                    return 2;   // Partially failed, incorrect receive length
                }//end if else
            }
            else
                CheckInvalidHandle();
        }
        else
            CheckInvalidHandle();
    }//end if

    return 0;  // Operation Failed
}//end SendReceivePacket

//---------------------------------------------------------------------------

void CheckInvalidHandle(void)
{
    if(GetLastError() == ERROR_INVALID_HANDLE)
    {
        // Most likely cause of the error is the board was disconnected.
        MPUSBClose(myOutPipe);
        MPUSBClose(myInPipe);
        myOutPipe = myInPipe = INVALID_HANDLE_VALUE;
    }//end if
    else
        printf("Error Code %x \r\n",GetLastError());
}//end CheckInvalidHandle

//---------------------------------------------------------------------------
