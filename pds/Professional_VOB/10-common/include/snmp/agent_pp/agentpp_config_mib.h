/*_############################################################################
  _## 
  _##  agentpp_config_mib.h  
  _## 
  _##
  _##  AGENT++ API Version 3.5.23a
  _##  -----------------------------------------------
  _##  Copyright (C) 2000-2005 Frank Fock, Jochen Katz
  _##  
  _##  LICENSE AGREEMENT
  _##
  _##  WHEREAS,  Frank  Fock  and  Jochen  Katz  are  the  owners of valuable
  _##  intellectual  property rights relating to  the AGENT++ API and wish to
  _##  license AGENT++ subject to the  terms and conditions set forth  below;
  _##  and
  _##
  _##  WHEREAS, you ("Licensee") acknowledge  that Frank Fock and Jochen Katz
  _##  have the right  to grant licenses  to the intellectual property rights
  _##  relating to  AGENT++, and that you desire  to obtain a license  to use
  _##  AGENT++ subject to the terms and conditions set forth below;
  _##
  _##  Frank  Fock    and Jochen   Katz   grants  Licensee  a  non-exclusive,
  _##  non-transferable, royalty-free  license  to use   AGENT++ and  related
  _##  materials without  charge provided the Licensee  adheres to all of the
  _##  terms and conditions of this Agreement.
  _##
  _##  By downloading, using, or  copying  AGENT++  or any  portion  thereof,
  _##  Licensee  agrees to abide  by  the intellectual property  laws and all
  _##  other   applicable laws  of  Germany,  and  to all of   the  terms and
  _##  conditions  of this Agreement, and agrees  to take all necessary steps
  _##  to  ensure that the  terms and  conditions of  this Agreement are  not
  _##  violated  by any person  or entity under the  Licensee's control or in
  _##  the Licensee's service.
  _##
  _##  Licensee shall maintain  the  copyright and trademark  notices  on the
  _##  materials  within or otherwise  related   to AGENT++, and  not  alter,
  _##  erase, deface or overprint any such notice.
  _##
  _##  Except  as specifically   provided in  this  Agreement,   Licensee  is
  _##  expressly   prohibited  from  copying,   merging,  selling,   leasing,
  _##  assigning,  or  transferring  in  any manner,  AGENT++ or  any portion
  _##  thereof.
  _##
  _##  Licensee may copy materials   within or otherwise related   to AGENT++
  _##  that bear the author's copyright only  as required for backup purposes
  _##  or for use solely by the Licensee.
  _##
  _##  Licensee may  not distribute  in any  form  of electronic  or  printed
  _##  communication the  materials  within or  otherwise  related to AGENT++
  _##  that  bear the author's  copyright, including  but  not limited to the
  _##  source   code, documentation,  help  files, examples,  and benchmarks,
  _##  without prior written consent from the authors.  Send any requests for
  _##  limited distribution rights to fock@agentpp.com.
  _##
  _##  Licensee  hereby  grants  a  royalty-free  license  to  any  and   all 
  _##  derivatives  based  upon this software  code base,  that  may  be used
  _##  as a SNMP  agent development  environment or a  SNMP agent development 
  _##  tool.
  _##
  _##  Licensee may  modify  the sources  of AGENT++ for  the Licensee's  own
  _##  purposes.  Thus, Licensee  may  not  distribute  modified  sources  of
  _##  AGENT++ without prior written consent from the authors. 
  _##
  _##  The Licensee may distribute  binaries derived from or contained within
  _##  AGENT++ provided that:
  _##
  _##  1) The Binaries are  not integrated,  bundled,  combined, or otherwise
  _##     associated with a SNMP agent development environment or  SNMP agent
  _##     development tool; and
  _##
  _##  2) The Binaries are not a documented part of any distribution material. 
  _##
  _##
  _##  THIS  SOFTWARE  IS  PROVIDED ``AS  IS''  AND  ANY  EXPRESS OR  IMPLIED
  _##  WARRANTIES, INCLUDING, BUT NOT LIMITED  TO, THE IMPLIED WARRANTIES  OF
  _##  MERCHANTABILITY AND FITNESS FOR  A PARTICULAR PURPOSE  ARE DISCLAIMED.
  _##  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
  _##  INDIRECT,   INCIDENTAL,  SPECIAL, EXEMPLARY,  OR CONSEQUENTIAL DAMAGES
  _##  (INCLUDING,  BUT NOT LIMITED  TO,  PROCUREMENT OF SUBSTITUTE  GOODS OR
  _##  SERVICES; LOSS OF  USE,  DATA, OR PROFITS; OR  BUSINESS  INTERRUPTION)
  _##  HOWEVER CAUSED  AND ON ANY THEORY  OF  LIABILITY, WHETHER IN CONTRACT,
  _##  STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
  _##  IN  ANY WAY OUT OF  THE USE OF THIS  SOFTWARE,  EVEN IF ADVISED OF THE
  _##  POSSIBILITY OF SUCH DAMAGE. 
  _##
  _##
  _##  Stuttgart, Germany, Sat Jun 18 13:06:20 CEST 2005 
  _##  
  _##########################################################################*/
   

//--AgentGen BEGIN=_BEGIN
//--AgentGen END

#ifndef _agentpp_config_mib_h
#define _agentpp_config_mib_h

#include <agent_pp/mib.h>
#include <agent_pp/mib_complex_entry.h>
#include <agent_pp/snmp_textual_conventions.h>
#include <agent_pp/notification_originator.h>


// Scalars
#define oidAgentppCfgLogLevelError        "1.3.6.1.4.1.4976.3.3.1.1.1.0"
#define oidAgentppCfgLogLevelWarning      "1.3.6.1.4.1.4976.3.3.1.1.2.0"
#define oidAgentppCfgLogLevelEvent        "1.3.6.1.4.1.4976.3.3.1.1.3.0"
#define oidAgentppCfgLogLevelInfo         "1.3.6.1.4.1.4976.3.3.1.1.4.0"
#define oidAgentppCfgLogLevelDebug        "1.3.6.1.4.1.4976.3.3.1.1.5.0"
#define oidAgentppCfgSecSrcAddrValidation   "1.3.6.1.4.1.4976.3.3.1.2.1.0"

// Columns
#define oidAgentppCfgStoragePath          "1.3.6.1.4.1.4976.3.3.1.3.1.1.2"
#define colAgentppCfgStoragePath          "2"
#define oidAgentppCfgStorageFormat        "1.3.6.1.4.1.4976.3.3.1.3.1.1.3"
#define colAgentppCfgStorageFormat        "3"
#define oidAgentppCfgStorageLastStore     "1.3.6.1.4.1.4976.3.3.1.3.1.1.4"
#define colAgentppCfgStorageLastStore     "4"
#define oidAgentppCfgStorageLastRestore   "1.3.6.1.4.1.4976.3.3.1.3.1.1.5"
#define colAgentppCfgStorageLastRestore   "5"
#define oidAgentppCfgStorageOperation     "1.3.6.1.4.1.4976.3.3.1.3.1.1.6"
#define colAgentppCfgStorageOperation     "6"
#define oidAgentppCfgStorageStorageType   "1.3.6.1.4.1.4976.3.3.1.3.1.1.7"
#define colAgentppCfgStorageStorageType   "7"
#define oidAgentppCfgStorageStatus        "1.3.6.1.4.1.4976.3.3.1.3.1.1.8"
#define colAgentppCfgStorageStatus        "8"


// Tables
#define oidAgentppCfgStorageEntry         "1.3.6.1.4.1.4976.3.3.1.3.1.1"
#define nAgentppCfgStoragePath           0
#define cAgentppCfgStoragePath           2
#define nAgentppCfgStorageFormat         1
#define cAgentppCfgStorageFormat         3
#define nAgentppCfgStorageLastStore      2
#define cAgentppCfgStorageLastStore      4
#define nAgentppCfgStorageLastRestore    3
#define cAgentppCfgStorageLastRestore    5
#define nAgentppCfgStorageOperation      4
#define cAgentppCfgStorageOperation      6
#define nAgentppCfgStorageStorageType    5
#define cAgentppCfgStorageStorageType    7
#define nAgentppCfgStorageStatus         6
#define cAgentppCfgStorageStatus         8

// Notifications

//--AgentGen BEGIN=_INCLUDE
#ifdef _SNMPv3
//--AgentGen END

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

//--AgentGen BEGIN=_UTIL_CLASSES

class AGENTPP_DECL agentppCfgStorageOperation;

#ifndef _NO_THREADS
class AGENTPP_DECL OperationTask: public Thread
{
 public:
    OperationTask(int op, agentppCfgStorageOperation* source) {
	operation = op;
	initiator = source;
    }
    virtual ~OperationTask() { }
    virtual void run();

 private:
    int		operation;
    agentppCfgStorageOperation* initiator;
};
#endif

//--AgentGen END

// Scalar Objects



/**
 *  agentppCfgSecSrcAddrValidation
 *
 * "Specifies whether SNMPv1/v2c source address
 * validation via the snmpTargetAddrExtTable and
 * the snmpCommunityTable is enabled or disabled.

 * If the value of this object is notAvailable(3), then at
 * least one of the necessary MIB modules are not
 * implemented for this agent instance and an attempt
 * to set this object's value to enabled(1) or disabled(2)
 * will result in a wrongValue error."
 */
 
class AGENTPP_DECL agentppCfgSecSrcAddrValidation: public MibLeaf
{
	
public:
	agentppCfgSecSrcAddrValidation();
	virtual ~agentppCfgSecSrcAddrValidation();

	static agentppCfgSecSrcAddrValidation* instance;
	
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual boolean    	value_ok(const Vbx&);
	enum labels {
		      e_enabled = 1,
		      e_disabled = 2,
		      e_notAvailable = 3	};

//--AgentGen BEGIN=agentppCfgSecSrcAddrValidation
//--AgentGen END
};


// Columnar Objects

// Tables

// Notifications
#ifdef _SNMPv3
#endif

// Group

class AGENTPP_DECL agentpp_config_mib: public MibGroup
{
  public:
	/**
	 * Default constructor without agentppCfgStorageEntry.
	 */
        agentpp_config_mib();
        virtual ~agentpp_config_mib() { }
//--AgentGen BEGIN=agentpp_config_mib
#ifndef _NO_THREADS
	/**
	 * Constructor with agentppCfgStorageEntry that allows to
	 * store and restore the agent persistent configuration at
	 * runtime.
	 */
        agentpp_config_mib(Mib*);
#endif
//--AgentGen END
};

//--AgentGen BEGIN=_CLASSES
class AGENTPP_DECL agentppCfgLogLevel: public MibLeaf
{
	
public:
	agentppCfgLogLevel(int, const Oidx&);
	virtual ~agentppCfgLogLevel();
	
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual void		get_request(Request*, int);
	virtual boolean    	value_ok(const Vbx&);
	virtual int        	commit_set_request(Request*, int);
	virtual int        	undo_set_request(Request*, int&);
	
 protected:
	int	logClass;
};
//--AgentGen END

// Columnar Objects



/**
 *  agentppCfgStoragePath
 *
 * "The path to the configuration on the agent's file
 * system. Depending on the persistent storage type,
 * the path can be a directory or a file."
 */
 
class AGENTPP_DECL agentppCfgStoragePath: public SnmpDisplayString
{
	
public:
	agentppCfgStoragePath(const Oidx&);
	virtual ~agentppCfgStoragePath();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual NS_SNMP OctetStr get_state();
	virtual void       	set_state(const NS_SNMP OctetStr&);
	virtual int        	prepare_set_request(Request*, int&);

//--AgentGen BEGIN=agentppCfgStoragePath
	virtual int        	commit_set_request(Request*, int);
//--AgentGen END
};




/**
 *  agentppCfgStorageFormat
 *
 * "The storage format specifies the format of the persistent
 * configuration storage associated with this row.
 * Currently only AGENT++'s BER encoded MIB object
 * serialization 'agentppBER(1)' is supported."
 */
 
class AGENTPP_DECL agentppCfgStorageFormat: public MibLeaf
{
	
public:
	agentppCfgStorageFormat(const Oidx&);
	virtual ~agentppCfgStorageFormat();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	prepare_set_request(Request*, int&);
	virtual boolean    	value_ok(const Vbx&);
	enum labels {
		      e_agentppBER = 1	};

//--AgentGen BEGIN=agentppCfgStorageFormat
//--AgentGen END
};




/**
 *  agentppCfgStorageOperation
 *
 * "The states 'idle(1)' and 'inProgress(2)'  can only be
 * read, whereas the states 'store(3)' and 'restore(4)' can
 * only be written.

 * Setting this object to 'store(3)' will save the agent's
 * configuration to the location identified by
 * agentppCfgStoragePath. Setting this object to
 * 'restore(4)' resets the agent to the configuration
 * read from location agentppCfgStoragePath.
 * While the two operations above are in progress,
 * this object returns 'inProgress(2)' on get requests.
 * Otherwise 'idle(1)' is returned on get requests.

 * While its state is 'inProgress' any set request returns
 * a 'resourceUnavailable(13)' error."
 */
 
class AGENTPP_DECL agentppCfgStorageOperation: public MibLeaf
{
friend class OperationTask;	
public:
	agentppCfgStorageOperation(const Oidx&);
	virtual ~agentppCfgStorageOperation();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	virtual boolean    	value_ok(const Vbx&);
	enum labels {
		      e_idle = 1,
		      e_inProgress = 2,
		      e_store = 3,
		      e_restore = 4	};

//--AgentGen BEGIN=agentppCfgStorageOperation
	virtual boolean		is_volatile() { return TRUE; }
#ifndef _NO_THREADS
 private:
	OperationTask*	operationTask; 
#endif
//--AgentGen END
};




/**
 *  agentppCfgStorageStorageType
 *
 * "The storage type of the entry in the AGENT++ persistent
 * storage table."
 */
 
class AGENTPP_DECL agentppCfgStorageStorageType: public StorageType
{
	
public:
	agentppCfgStorageStorageType(const Oidx&);
	virtual ~agentppCfgStorageStorageType();

	virtual MibEntryPtr	clone();
	virtual void       	get_request(Request*, int);
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	enum labels {
		      e_other = 1,
		      e_volatile = 2,
		      e_nonVolatile = 3,
		      e_permanent = 4,
		      e_readOnly = 5	};

//--AgentGen BEGIN=agentppCfgStorageStorageType
//--AgentGen END
};




/**
 *  agentppCfgStorageStatus
 *
 * "Control for creating and deleting entries.  Entries may
 * not be modified while active."
 */
 
class AGENTPP_DECL agentppCfgStorageStatus: public snmpRowStatus
{
	
public:
	agentppCfgStorageStatus(const Oidx&);
	virtual ~agentppCfgStorageStatus();

	virtual MibEntryPtr	clone();
	virtual long       	get_state();
	virtual void       	set_state(long);
	virtual int        	set(const Vbx&);
	virtual int        	prepare_set_request(Request*, int&);
	enum labels {
		      e_active = 1,
		      e_notInService = 2,
		      e_notReady = 3,
		      e_createAndGo = 4,
		      e_createAndWait = 5,
		      e_destroy = 6	};

//--AgentGen BEGIN=agentppCfgStorageStatus
//--AgentGen END
};


// Tables


/**
 *  agentppCfgStorageEntry
 *
 * "A row specifying the location and storage format
 * of an AGENT++ agent configuration."
 */
 
class AGENTPP_DECL agentppCfgStorageEntry: public StorageTable
{
	
public:
	agentppCfgStorageEntry();
	virtual ~agentppCfgStorageEntry();

	static agentppCfgStorageEntry* instance;
	
	virtual void        	get_request(Request*, int);
	virtual int         	prepare_set_request(Request*, int&);
	virtual int         	is_transition_ok(MibTable*, 
						 MibTableRow*, 
						 const Oidx&, int, int);
	virtual void		set_row(MibTableRow* r,
					const NS_SNMP OctetStr& p1,
					long p2,
					unsigned long p3,
					unsigned long p4,
					long p5,
					long p6,
					long p7);
//--AgentGen BEGIN=agentppCfgStorageEntry
	void			set_mib(Mib* m);
	Mib*			get_mib() { return mib; }

	/**
	 * Do not remove any rows when resetted, because a restore operation
	 * may need a row. 
	 */
	virtual void		reset() { }

	/**
	 * If set to TRUE, absolute and relative paths containing dots
	 * are not allowed. TRUE is the default value.
	 * @param securePaths
	 *    if TRUE, only secure paths are allowed for persistent storage.
	 */
	void			set_secure_paths(boolean b) { securePaths = b;}

	/**
	 * Returns TRUE if only secure paths can be configured via SNMP.
	 * @return
	 *    TRUE if only secure paths are allowed, FALSE if any path is
	 *    allowed.
	 */
	boolean			is_secure_paths() { return securePaths; }

 private:
	boolean			securePaths;
	Mib*			mib;
//--AgentGen END
};


#ifdef AGENTPP_NAMESPACE
}
#endif


//--AgentGen BEGIN=_END
#endif
//--AgentGen END

#endif
