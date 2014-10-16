/*
 * RACK - Robotics Application Construction Kit
 * Copyright (C) 2005-2006 University of Hannover
 *                         Institute for Systems Engineering - RTS
 *                         Professor Bernardo Wagner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * Authors
 *      Joerg Langenberg <joerg.langenberg@gmx.net>
 *      Oliver Wulf <oliver.wulf@web.de>
 *
 */
#ifndef __RACK_DATA_MODULE_H__
#define __RACK_DATA_MODULE_H__

#include <main/rack_module.h>

#include <math.h>

//######################################################################
//# class DataBufferEntry
//######################################################################

class DataBufferEntry {
    public:
        void*       pData;
        uint32_t    dataSize;

        // Konstruktor
        DataBufferEntry()
        {
            pData      = NULL;
            dataSize    = 0;
        }

        // Destruktor
        ~DataBufferEntry()
        {
            pData = NULL;
        };
};

//######################################################################
//# class ListenerEntry
//######################################################################

class ListenerEntry {
    public:
        uint32_t        reduction;
        RackMessage     msgInfo;
        uint32_t        getNextData;

        // Konstruktor
        ListenerEntry()
        {
            reduction = 0;
            getNextData = 0;
        };

        // Destruktor
        ~ListenerEntry()
        { };
};

/**
 *
 * @ingroup main_common
 */
class RackDataModule : public RackModule
{
    private:
        RackBits            dataModuleInitBits;   // internal rack_data_module init bits

    protected:
        uint32_t            dataBufferMaxDataSize;  // per slot !!!
        uint32_t            index;
        uint32_t            globalDataCount;
        uint32_t            listenerNum;

        DataBufferEntry*    dataBuffer;
        ListenerEntry*      listener;

        RackMutex           bufferMtx;
        char                bufferMtxName[30];

        RackMutex           listenerMtx;
        char                listenerMtxName[30];

        uint32_t            dataBufferMaxEntries;
        uint32_t            dataBufferMaxListener;
        int16_t             dataBufferSendType;
        RackMailbox*        dataBufferSendMbx;
        rack_time_t         dataBufferPeriodTime;
        rack_time_t         dataBufferSleepTime;

        rack_time_t         getRecordingTime(void *pData);
        int                 getDataBufferIndex(rack_time_t time);
        virtual int         sendDataReply(rack_time_t time, RackMessage *msgInfo);

        int                 addListener(rack_time_t periodTime, uint32_t getNextData, uint32_t destMbxAdr,
                                        RackMessage *msgInfo);
        void                removeListener(uint32_t destMbxAdr);
        void                removeAllListener(void);
        rack_time_t         getListenerPeriodTime(uint32_t dataMbx);

        void                setDataBufferMaxDataSize(uint32_t max_size);
        uint32_t            getDataBufferMaxDataSize(void);

        friend void         cmd_task_proc(void* arg);

  public:

    RackDataModule(uint32_t class_id,           // class ID
               uint64_t dataTaskErrorTime_ns,   // datatask error sleep time
               int32_t  cmdMbxMsgSlots,         // command mailbox slots
               uint32_t cmdMbxMsgDataSize,      // command mailbox data size
               uint32_t cmdMbxFlags,            // command mailbox create flags
               uint32_t maxDataBufferEntries,   // max buffer entries
               uint32_t maxDataBufferListener); // data buffer listener

    ~RackDataModule();

    void*     getDataBufferWorkSpace(void);
    void      putDataBufferWorkSpace(uint32_t datalength);

    void      sleepDataBufferPeriodTime(void);

    //
    // virtual module functions
    //

    int   moduleInit(void);
    void  moduleCleanup(void);

    int   moduleOn(void);
    void  moduleOff(void);

    int   moduleLoop(void) { return 0; };
    int   moduleCommand(RackMessage *msgInfo);
};

#endif // __RACK_DATA_MODULE_H_
