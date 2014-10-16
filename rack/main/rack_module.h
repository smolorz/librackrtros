/**
 * @file
 * RACK - Robotics Application Construction Kit
 *
 * @note Copyright (C) 2005-2006 University of Hannover
 *                         Institute for Systems Engineering - RTS
 *                         Professor Bernardo Wagner
 *
 * @n
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
#ifndef __RACK_MODULE_H__
#define __RACK_MODULE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <unistd.h>
#include <limits.h>

#include <main/rack_gdos.h>
#include <main/rack_proxy.h>
#include <main/rack_mailbox.h>
#include <main/rack_mutex.h>
#include <main/rack_name.h>
#include <main/rack_task.h>
#include <main/rack_time.h>
#include <main/rack_bits.h>
#include <main/argopts.h>

using std::string;

/* The command task has been received the ON-Command.
 * Data task shall enable the Module. */
#define MODULE_TSTATE_ON                MSG_ON

/* The command task has been received the OFF-Command.
 *  Data task shall disable the Module. */
#define MODULE_TSTATE_OFF               MSG_OFF

/* Module is enabled - Data task is working */
#define MODULE_STATE_ENABLED            MSG_ENABLED

/* Module is disabled - Command task is waiting for ON-Command */
#define MODULE_STATE_DISABLED           MSG_DISABLED

/* Module is in error state */
#define MODULE_STATE_ERROR              MSG_ERROR

#define MODULE_STATE_ERROR_WAIT         -10
#define MODULE_STATE_ERROR_RETRY        -11

//
// save argument table and name of module
//
void save_argTab(arg_table_t* p_tab, const char* name);

extern arg_table_t module_argTab[];

//
// some additional functions
//

// module mailbox create flags
#define MBX_IN_KERNELSPACE              0x0001
#define MBX_IN_USERSPACE                0x0002
#define MBX_SLOT                        0x0004
#define MBX_FIFO                        0x0008

/**
 * The class RackModule is the higest class of RACK components. All
 * public or protected Module functions can be used by any component which
 * inherits data structures or functions of this class.
 *
 * The public or protected data structures and functions are described in this
 * section.
 *
 * @ingroup main_common
 */
class RackModule {

//
// init bits
//
    private:
        RackBits  moduleInitBits;       // internal rack_module init bits
    protected:
        RackBits  initBits;             // custom init bits

//
// command task
//
    protected:
        RackTask  cmdTask;
        int8_t    cmdTaskPrio;
        char      cmdTaskName[50];

        int       cmdTaskJoin();

    friend void   cmd_task_proc(void *arg);

//
// data task
//
    protected:
        RackTask  dataTask;
        int8_t    dataTaskPrio;
        char      dataTaskName[50];
        int       dataTaskErrorTimer;

        int       dataTaskJoin();

    friend void   data_task_proc(void *arg);
    friend void   notify(int8_t type, RackModule *p_mod);

    public:
        int8_t    getDataTaskPrio(void)
        {
            return dataTaskPrio;
        }

//
// common task values
//
    protected:
        int cpu;            // cpu to run the cmd and data tasks on
        int terminate;      // to stop the tasks
        int targetStatus;   // next module state
        int initializing;   // =1 if this module is still loading
        int errorTimeout;   // time [ms] to wait until restart of module

        /** Message info to reply a message */
        RackMessage replyMsgInfo;

        /** Module state */
        int status;

//
// mailboxes
//
    protected:
        uint32_t  mailboxBaseAdr;
        uint32_t  mailboxFreeAdr;

        int32_t   cmdMbxMsgSlots;
        uint32_t  cmdMbxMsgDataSize;
        uint32_t  cmdMbxFlags;

        void      mailboxList(void);
        uint32_t  mailbox_getNextFreeAdr(void);
        int       mailbox_putLastAdr(void);
        int       mailboxCreateCmdMbx(void);
        int       mailboxCreate(RackMailbox *p_mbx, uint32_t adr, int slots,
                                size_t data_size, uint32_t flags, int8_t prio);

        /** Command mailbox of the module */

        int           createMbx(RackMailbox *p_mbx, int slots, size_t data_size,
                                uint32_t flags);
        void          destroyMbx(RackMailbox *p_mbx);
        int           initCmdMbx(int slots, size_t data_size, uint32_t flags);
        int           createCmdMbx(void);
        RackMailbox   cmdMbx;
        RackMailbox*  getCmdMbx(void);
//
// Debugging
//
    protected:
        /** Debugging mailbox */
        RackGdos      *gdos;

        /** Debugging level */
        char          gdosLevel;

//
// Rack time
//
    public:
        /** RackTime instance of the module */
        RackTime rackTime;

//
// module values
//
private:
        uint32_t  name;          // module name (12345678) == cmdMbxAdr

    protected:
        uint32_t  systemId;      // system number
        uint32_t  instance;      // instance number

        rack_param_msg *paramMsg;

        int       parseArgTable(arg_table_t *argTable, rack_param_msg *paramMsg);

        int32_t   getInt32Param(const char* paramName);
        char*     getStringParam(const char* paramName);
        float     getFloatParam(const char* paramName);

        void      setInt32Param(const char* paramName, int32_t value);
        void      setStringParam(const char* paramName, char* value);
        void      setFloatParam(const char* paramName, float value);

    public:
        /** Get system of the module */
        uint32_t getSystem(void)
        {
            return systemId;
        }

        /** Get instance number of the module */
        uint32_t getInstance(void)
        {
            return instance;
        }

        /** Get the name of the module */
        uint32_t getName(void);

        RackModule(uint32_t classId,
                   uint64_t dataTaskErrorTime_ns,   // datatask error sleep time
                   int32_t  cmdMbxMsgSlots,         // command mailbox slots
                   uint32_t cmdMbxMsgDataSize,      // command mailbox data size
                   uint32_t cmdMbxFlags);           // command mailbox create flags

        virtual ~RackModule();

//
// virtual module functions
//
    public:
        /** The init function of the module */
        virtual int   moduleInit(void);

        /** The cleanup function of the module */
        virtual void  moduleCleanup(void);

    protected:
        /** The on function of the module */
        virtual int   moduleOn(void)
        {
            return 0;
        }

        /** The off function of the module */
        virtual void  moduleOff(void)
        {
        }

        /** The loop function of the module */
        virtual int   moduleLoop(void)
        {
            return 0;
        }

        /** The moduleCommand function of the module */
        virtual int   moduleCommand(RackMessage *msgInfo);

//
// signal handler shutdown function
//
    public:
        void moduleTerminate(void)
        {
            GDOS_DBG_INFO("Terminate ...\n");
            terminate = 1;
        }

        int isTerminated(void)
        {
            return terminate;
        }

//
// module start arguments
//
    public:

        static int getArgs(int argc, char *argv[], arg_table_t* p_tab,
                           const char *classname)
        {
            arg_descriptor_t module_argDesc[] =
            {
                { module_argTab}, {p_tab}, {NULL}
            };

            save_argTab(p_tab, classname);
            return argScan(argc, argv, module_argDesc, classname);
        }

//
// module run function
//
    protected:
        int startCmdTask(void);

    public:
        void run(void);

}; // class RackModule

//
// signal handler functions
//

void signal_handler(int sig);
int init_signal_handler(RackModule *p_mod);

#endif // __RACK_MODULE_H__
